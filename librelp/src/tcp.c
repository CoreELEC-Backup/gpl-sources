/* This implements the relp mapping onto TCP.
 *
 * Copyright 2008-2018 by Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of librelp.
 *
 * Note: gnutls_certificate_set_verify_function is problematic, as it
 *       is not available in old GnuTLS versions, but rather important
 *       for verifying certificates correctly.
 *
 * Librelp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Librelp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Librelp.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A copy of the GPL can be found in the file "COPYING" in this distribution.
 *
 * If the terms of the GPL are unsuitable for your needs, you may obtain
 * a commercial license from Adiscon. Contact sales@adiscon.com for further
 * details.
 *
 * ALL CONTRIBUTORS PLEASE NOTE that by sending contributions, you assign
 * your copyright to Adiscon GmbH, Germany. This is necessary to permit the
 * dual-licensing set forth here. Our apologies for this inconvenience, but
 * we sincerely believe that the dual-licensing model helps us provide great
 * free software while at the same time obtaining some funding for further
 * development.
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <assert.h>
#include "relp.h"
#include "relpsrv.h"
#include "relpclt.h"
#include "relpsess.h"
#include "tcp.h"
#ifdef ENABLE_TLS
#	include <gnutls/gnutls.h>
#	include <gnutls/x509.h>
#	if GNUTLS_VERSION_NUMBER <= 0x020b00
#		include <gcrypt.h>
		GCRY_THREAD_OPTION_PTHREAD_IMPL;
#	endif
	static int called_gnutls_global_init = 0;
#endif
#ifdef ENABLE_TLS_OPENSSL
#	include <openssl/ssl.h>
#	include <openssl/x509v3.h>
#	include <openssl/err.h>
#	include <openssl/engine.h>
/* OpenSSL API differences */
#	if OPENSSL_VERSION_NUMBER >= 0x10100000L
#		define RSYSLOG_X509_NAME_oneline(X509CERT) X509_get_subject_name(X509CERT)
#		define RSYSLOG_BIO_method_name(SSLBIO) BIO_method_name(SSLBIO)
#		define RSYSLOG_BIO_number_read(SSLBIO) BIO_number_read(SSLBIO)
#		define RSYSLOG_BIO_number_written(SSLBIO) BIO_number_written(SSLBIO)
#	else
#		define RSYSLOG_X509_NAME_oneline(X509CERT) (X509CERT != NULL ? X509CERT->cert_info->subject : NULL)
#		define RSYSLOG_BIO_method_name(SSLBIO) SSLBIO->method->name
#		define RSYSLOG_BIO_number_read(SSLBIO) SSLBIO->num
#		define RSYSLOG_BIO_number_written(SSLBIO) SSLBIO->num
#	endif
#endif /* #ifdef ENABLE_TLS_OPENSSL */


#ifndef SOL_TCP
#  define SOL_TCP (getprotobyname("tcp")->p_proto)
#endif

/*  AIXPORT : MSG_DONTWAIT not supported */
#if defined(_AIX) && !defined(MSG_DONTWAIT)
#define MSG_DONTWAIT    MSG_NONBLOCK
#endif


#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
/* forward definitions */
#ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION
static int relpTcpVerifyCertificateCallback(gnutls_session_t session);
#endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION */
#if defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION) || defined(ENABLE_TLS_OPENSSL)
static void relpTcpChkOnePeerName(relpTcp_t *const pThis, char *peername, int *pbFoundPositiveMatch);
static int relpTcpAddToCertNamesBuffer(relpTcp_t *const pThis, char *const buf,
	const size_t buflen, int *p_currIdx, const char *const certName);

#endif /* defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION) || defined(ENABLE_TLS_OPENSSL) */

static relpRetVal relpTcpPermittedPeerWildcardCompile(tcpPermittedPeerEntry_t *pEtry);

/* helper to free permittedPeer structure */
static inline void
relpTcpFreePermittedPeers(relpTcp_t *const pThis)
{
	int i;
	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i)
		free(pThis->permittedPeers.peer[i].name);
	pThis->permittedPeers.nmemb = 0;
	if(pThis->permittedPeers.peer != NULL)
		free(pThis->permittedPeers.peer);
}

/* helper to call onAuthErr if set */
static inline void
callOnAuthErr(relpTcp_t *const pThis, char *authdata, char *emsg, relpRetVal ecode)
{
	pThis->pEngine->dbgprint("librelp: auth error: authdata:'%s', ecode %d, "
		"emsg '%s'\n", authdata, ecode, emsg);
	if(pThis->pEngine->onAuthErr != NULL) {
		pThis->pEngine->onAuthErr(pThis->pUsr, authdata, emsg, ecode);
	}
}

#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */

#ifdef ENABLE_TLS_OPENSSL
/*--------------------------------------MT OpenSSL helpers ------------------------------------------*/
static MUTEX_TYPE *mutex_buf = NULL;

void locking_function(int mode, int n,
	__attribute__((unused)) const char * file, __attribute__((unused)) int line)
{
	if (mode & CRYPTO_LOCK)
		MUTEX_LOCK(mutex_buf[n]);
	else
		MUTEX_UNLOCK(mutex_buf[n]);
}

unsigned long id_function(void)
{
	return ((unsigned long)THREAD_ID);
}


struct CRYPTO_dynlock_value * dyn_create_function(
	__attribute__((unused)) const char *file, __attribute__((unused)) int line)
{
	struct CRYPTO_dynlock_value *value;
	value = (struct CRYPTO_dynlock_value *)malloc(sizeof(struct CRYPTO_dynlock_value));
	if (!value)
		return NULL;

	MUTEX_SETUP(value->mutex);
	return value;
}

void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l,
	__attribute__((unused)) const char *file, __attribute__((unused)) int line)
{
	if (mode & CRYPTO_LOCK)
		MUTEX_LOCK(l->mutex);
	else
		MUTEX_UNLOCK(l->mutex);
}

void dyn_destroy_function(struct CRYPTO_dynlock_value *l,
	__attribute__((unused)) const char *file, __attribute__((unused)) int line)
{
	MUTEX_CLEANUP(l->mutex);
	free(l);
}

/* set up support functions for openssl multi-threading. This must
 * be done at library initialisation. If the function fails,
 * processing can not continue normally. On failure, 0 is
 * returned, on success 1.
 */
int opensslh_THREAD_setup(void)
{
	int i;
	mutex_buf = (MUTEX_TYPE *)malloc(CRYPTO_num_locks( ) * sizeof(MUTEX_TYPE));
	if (mutex_buf == NULL)
		return 0;
	for (i = 0; i < CRYPTO_num_locks( ); i++)
		MUTEX_SETUP(mutex_buf[i]);

	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
	/* The following three CRYPTO_... functions are the OpenSSL functions
	for registering the callbacks we implemented above */
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

//	DBGPRINTF("openssl: multithread setup finished\n");
	return 1;
}

/* shut down openssl - do this only when you are totally done
 * with openssl.
 */
int opensslh_THREAD_cleanup(void)
{
	int i;
	if (!mutex_buf)
		return 0;

	CRYPTO_set_id_callback(NULL);
	CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_dynlock_create_callback(NULL);
	CRYPTO_set_dynlock_lock_callback(NULL);
	CRYPTO_set_dynlock_destroy_callback(NULL);

	for (i = 0; i < CRYPTO_num_locks( ); i++)
		MUTEX_CLEANUP(mutex_buf[i]);

	free(mutex_buf);
	mutex_buf = NULL;

//	DBGPRINTF("openssl: multithread cleanup finished\n");
	return 1;
}
/*--------------------------------------MT OpenSSL helpers ------------------------------------------*/
/* OpenSSL Helper functions */
/* OpenSSL implementation of TLS funtions.
 * alorbach, 2018-06-11
 */

long BIO_debug_callback(BIO *bio, int cmd, const char __attribute__((unused)) *argp,
			int argi, long __attribute__((unused)) argl, long ret)
{
	long r = 1;

	relpTcp_t* pThis = (relpTcp_t*) (void *) BIO_get_callback_arg(bio);

	if (BIO_CB_RETURN & cmd)
	r = ret;

	switch (cmd) {
	case BIO_CB_FREE:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: Free - %s\n",
			(void *)bio, RSYSLOG_BIO_method_name(bio));
		break;
	/* Disabled due API changes for OpenSSL 1.1.0+ */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	case BIO_CB_READ:
	if (bio->method->type & BIO_TYPE_DESCRIPTOR)
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: read(%d,%lu) - %s fd=%d\n",
			(void *)bio,
			RSYSLOG_BIO_number_read(bio), (unsigned long)argi,
			RSYSLOG_BIO_method_name(bio), RSYSLOG_BIO_number_read(bio));
	else
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: read(%d,%lu) - %s\n",
			(void *)bio,
			RSYSLOG_BIO_number_read(bio), (unsigned long)argi, RSYSLOG_BIO_method_name(bio));
	break;
	case BIO_CB_WRITE:
	if (bio->method->type & BIO_TYPE_DESCRIPTOR)
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: write(%d,%lu) - %s fd=%d\n",
			(void *)bio,
			RSYSLOG_BIO_number_written(bio), (unsigned long)argi,
			RSYSLOG_BIO_method_name(bio), RSYSLOG_BIO_number_written(bio));
	else
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: write(%d,%lu) - %s\n",
			(void *)bio,
			RSYSLOG_BIO_number_written(bio), (unsigned long)argi, RSYSLOG_BIO_method_name(bio));
	break;
#else
	case BIO_CB_READ:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: read %s\n",
			(void *)bio,
			RSYSLOG_BIO_method_name(bio));
	break;
	case BIO_CB_WRITE:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: write %s\n",
			(void *)bio,
			RSYSLOG_BIO_method_name(bio));
	break;
#endif
	case BIO_CB_PUTS:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: puts() - %s\n",
			(void *)bio,
			RSYSLOG_BIO_method_name(bio));
		break;
	case BIO_CB_GETS:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: gets(%lu) - %s\n",
			(void *)bio,
			(unsigned long)argi,
			RSYSLOG_BIO_method_name(bio));
		break;
	case BIO_CB_CTRL:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: ctrl(%lu) - %s\n",
			(void *)bio,
			(unsigned long)argi,
			RSYSLOG_BIO_method_name(bio));
		break;
	case BIO_CB_RETURN | BIO_CB_READ:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: read return %ld\n",
			(void *)bio,
			ret);
		break;
	case BIO_CB_RETURN | BIO_CB_WRITE:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: write return %ld\n",
			(void *)bio,
			ret);
		break;
	case BIO_CB_RETURN | BIO_CB_GETS:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: gets return %ld\n",
			(void *)bio,
			ret);
		break;
	case BIO_CB_RETURN | BIO_CB_PUTS:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: puts return %ld\n",
			(void *)bio,
			ret);
		break;
	case BIO_CB_RETURN | BIO_CB_CTRL:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: ctrl return %ld\n",
			(void *)bio,
			ret);
		break;
	default:
		pThis->pEngine->dbgprint("openssl debugmsg: BIO[%p]: bio callback - unknown type (%d)\n",
			(void *)bio,
			cmd);
		break;
	}

	return (r);
}

void relpTcpLastSSLErrorMsg(int ret, relpTcp_t *pThis, const char* pszCallSource)
{
	unsigned long un_error = 0;
	char psz[256];
	long iMyRet = SSL_get_error(pThis->ssl, ret);

	/* Check which kind of error we have */
	pThis->pEngine->dbgprint("relpTcpLastSSLErrorMsg: openssl error '%s' with error code=%ld\n",
		pszCallSource, iMyRet);
	if(iMyRet == SSL_ERROR_SSL) {
		/* Loop through errors */
		while ((un_error = ERR_get_error()) != 0){
			ERR_error_string_n(un_error, psz, 256);
			pThis->pEngine->dbgprint("relpTcpLastSSLErrorMsg: Errorstack: %s\n", psz);
		}

	} else if(iMyRet == SSL_ERROR_SYSCALL){
		iMyRet = ERR_get_error();
		if(ret == 0) {
			iMyRet = SSL_get_error(pThis->ssl, iMyRet);
			if(iMyRet == 0) {
				*psz = '\0';
			} else {
				ERR_error_string_n(iMyRet, psz, 256);
			}
			pThis->pEngine->dbgprint("relpTcpLastSSLErrorMsg: SysErr: %s\n", psz);
		} else {
			/* Loop through errors */
			while ((un_error = ERR_get_error()) != 0){
				ERR_error_string_n(un_error, psz, 256);
				pThis->pEngine->dbgprint("relpTcpLastSSLErrorMsg: Errorstack: %s\n", psz);
			}
		}
	} else {
		pThis->pEngine->dbgprint("relpTcpLastSSLErrorMsg: Unknown SSL Error in '%s' (%d), SSL_get_error: %ld\n",
			pszCallSource, ret, iMyRet);
	}
}

/* Convert a fingerprint to printable data. The  conversion is carried out
 * according IETF I-D syslog-transport-tls-12. The fingerprint string is
 */
static relpRetVal
GenFingerprintStr(unsigned char *pFingerprint, size_t sizeFingerprint,
	char * fpBuf,const size_t bufLen)
{
	size_t iSrc, iDst;
	ENTER_RELPFUNC;

	if (bufLen <= 5) {
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	strncpy(fpBuf,"SHA1", bufLen);
	iDst=4;

	for(iSrc = 0; iSrc < sizeFingerprint && iSrc < bufLen; ++iSrc, iDst += 3) {
		sprintf(fpBuf+iDst, ":%2.2X", (unsigned char) pFingerprint[iSrc]);
	}

finalize_it:
	LEAVE_RELPFUNC;
}

/* Check the peer's ID in fingerprint auth mode. */
static int
relpTcpChkPeerFingerprint(relpTcp_t *const pThis, X509* cert)
{
	int i;
	unsigned int n;
//	char fingerprint[126];
	unsigned char fingerprint[20 /*EVP_MAX_MD_SIZE**/];
	char fpPrintable[256];

	size_t size;
	int bFoundPositiveMatch;
	const EVP_MD *fdig = EVP_sha1();
	ENTER_RELPFUNC;

	/* obtain the SHA1 fingerprint */
	size = sizeof(fingerprint);
	if (!X509_digest(cert,fdig,fingerprint,&n)) {
		pThis->pEngine->dbgprint("relpTcpChkPeerFingerprint: error X509cert is not valid!\n");
		ABORT_FINALIZE(RELP_RET_ERR_TLS);
	}

	GenFingerprintStr(fingerprint, size, fpPrintable,sizeof(fpPrintable));
	pThis->pEngine->dbgprint("relpTcpChkPeerFingerprint: peer's certificate SHA1 fingerprint: %s\n", fpPrintable);

	/* now search through the permitted peers to see if we can find a permitted one */
	bFoundPositiveMatch = 0;
	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i)
	{
		pThis->pEngine->dbgprint("relpTcpChkPeerFingerprint: checking peer '%s','%s'\n",
			fpPrintable, pThis->permittedPeers.peer[i].name);
		if(!strcmp(fpPrintable, pThis->permittedPeers.peer[i].name)) {
			pThis->pEngine->dbgprint("relpTcpChkPeerFingerprint: peer's certificate MATCH found: %s\n",
				pThis->permittedPeers.peer[i].name);
			bFoundPositiveMatch = 1;
			break;
		}
	}

	if(!bFoundPositiveMatch) {
		pThis->pEngine->dbgprint("relpTcpChkPeerFingerprint: invalid peer fingerprint, "
			"not permitted to talk to it\n");
		callOnAuthErr(pThis, fpPrintable, "non-permited fingerprint", RELP_RET_AUTH_ERR_FP);
		ABORT_FINALIZE(RELP_RET_AUTH_ERR_FP);
	}
finalize_it:
	LEAVE_RELPFUNC;
}

/* Check the peer's ID in name auth mode. */
static int
relpTcpChkPeerName(relpTcp_t *const pThis, X509* cert)
{
	unsigned char lnBuf[256];
	int bFoundPositiveMatch = 0;
	char *x509name = NULL;
	char allNames[32*1024]; /* for error-reporting */
	int iAllNames = 0;

	/* Helpers for altName */
	int iLoop;
	int san_names_nb = -1;
	GENERAL_NAMES *san_names = NULL;
	GENERAL_NAME *gnDnsName;
	int gtype;
	ASN1_STRING *asDnsName;
	const char *szAltName;

	ENTER_RELPFUNC;

	bFoundPositiveMatch = 0;

	/* Obtain Namex509 name from subject */
	x509name = X509_NAME_oneline(RSYSLOG_X509_NAME_oneline(cert), NULL, 0);

	pThis->pEngine->dbgprint("relpTcpChkPeerName: checking - peername '%s'\n", x509name);
	snprintf((char*)lnBuf, sizeof(lnBuf), "name: %s; ", x509name);
//	CHKRet(osslChkOnePeerName(pThis, cert, (unsigned char*)x509name, &bFoundPositiveMatch));
	relpTcpChkOnePeerName(pThis, x509name, &bFoundPositiveMatch);

	if(!bFoundPositiveMatch) {
		/* Try to extract altname within the SAN extension from the certificate */
		san_names = X509_get_ext_d2i((X509 *) cert, NID_subject_alt_name, NULL, NULL);
		if (san_names == NULL) {
			pThis->pEngine->dbgprint("relpTcpChkPeerName: X509_get_ext_d2i returned no ALTNAMES\n");
		} else {
			/* Loop through each name within the extension */
			san_names_nb = sk_GENERAL_NAME_num(san_names);
			for (iLoop=0; iLoop<san_names_nb; iLoop++) {
				/* Get DNSName from san_names */
				gnDnsName = sk_GENERAL_NAME_value(san_names, iLoop);
				asDnsName = GENERAL_NAME_get0_value(gnDnsName, &gtype);

#				if OPENSSL_VERSION_NUMBER >= 0x10100000L
				szAltName = (const char *)ASN1_STRING_get0_data(asDnsName);
#				else
				szAltName = (const char *)ASN1_STRING_data(asDnsName);
#				endif

				pThis->pEngine->dbgprint("relpTcpChkPeerName: checking - altName: '%s'\n", szAltName);

				/* Add to Names Buffer first */
				if (relpTcpAddToCertNamesBuffer(pThis, allNames, sizeof(allNames),
						&iAllNames, (char *)szAltName) != 0)
					ABORT_FINALIZE(RELP_RET_AUTH_CERT_INVL);

				/* Perform PeerName check on AltName */
				relpTcpChkOnePeerName(pThis, (char *)szAltName, &bFoundPositiveMatch);

				/* Stop if match was found */
				if (bFoundPositiveMatch)
					break;
			}
		}
	}

	if(!bFoundPositiveMatch) {
		pThis->pEngine->dbgprint("relpTcpChkPeerName: invalid peername, not permitted to talk to it\n");
		callOnAuthErr(pThis, allNames, "no permited name found", RELP_RET_AUTH_ERR_NAME);
		ABORT_FINALIZE(RELP_RET_AUTH_ERR_NAME);
	} else {
		pThis->pEngine->dbgprint("relpTcpChkPeerName: permitted to talk!\n");
	}

finalize_it:
	/* Free mem */
	if (x509name != NULL){
		OPENSSL_free(x509name);
	}
	if (san_names != NULL){
		GENERAL_NAMES_free(san_names);
	}

	LEAVE_RELPFUNC;
}

int verify_callback(int status, X509_STORE_CTX *store)
{
	char szdbgdata1[256];
	char szdbgdata2[256];
	char szdberrmsg[1024];

	/* Get current SSL object in order to obtain relpTcp_t obj */
	SSL* ssl = X509_STORE_CTX_get_ex_data(store, SSL_get_ex_data_X509_STORE_CTX_idx());
	relpTcp_t *pThis = (relpTcp_t *) SSL_get_ex_data(ssl, 0);
	assert(pThis != NULL);

	X509 *cert = X509_STORE_CTX_get_current_cert(store);
	int depth = X509_STORE_CTX_get_error_depth(store);
	int err = X509_STORE_CTX_get_error(store);

	/* Already failed ? */
	if(status == 0) {
		pThis->pEngine->dbgprint("verify_callback: certificate validation failed!\n");
		X509_NAME_oneline(X509_get_issuer_name(cert), szdbgdata1, sizeof(szdbgdata1));
		X509_NAME_oneline(RSYSLOG_X509_NAME_oneline(cert), szdbgdata2, sizeof(szdbgdata2));

		/* Log Warning only on EXPIRED */
		if (err == X509_V_OK || err == X509_V_ERR_CERT_HAS_EXPIRED) {
			snprintf(szdberrmsg, sizeof(szdberrmsg),
				"Certificate expired in verify_callback at depth: %d \n\t"
				"issuer  = %s\n\t"
				"subject = %s\n\t"
				"err %d:%s\n",
				depth, szdbgdata1, szdbgdata2, err, X509_verify_cert_error_string(err));
			pThis->pEngine->dbgprint("verify_callback: %s", szdberrmsg);

			callOnAuthErr(pThis, (char*)X509_verify_cert_error_string(err),
				szdberrmsg, RELP_RET_AUTH_CERT_INVL);

			/* Set Status to OK ?!*/
			status = 1;
		} else {
			snprintf(szdberrmsg, sizeof(szdberrmsg),
				"Certificate error in verify_callback at depth: %d \n\t"
				"issuer  = %s\n\t"
				"subject = %s\n\t"
				"err %d:%s\n",
				depth, szdbgdata1, szdbgdata2, err, X509_verify_cert_error_string(err));
			pThis->pEngine->dbgprint("verify_callback: %s", szdberrmsg);

			callOnAuthErr(pThis, (char*)X509_verify_cert_error_string(err),
				szdberrmsg, RELP_RET_AUTH_CERT_INVL);
		}
	} else {
		pThis->pEngine->dbgprint("verify_callback: certificate validation success!\n");
	}
	return status;
}
#endif /* #ifdef ENABLE_TLS_OPENSSL */

/** Construct a RELP tcp instance
 * This is the first thing that a caller must do before calling any
 * RELP function. The relp tcp must only destructed after all RELP
 * operations have been finished. Parameter pParent contains a pointer
 * to the "parent" client or server object, depending on connType.
 */
relpRetVal
relpTcpConstruct(relpTcp_t **ppThis, relpEngine_t *const pEngine,
	const int connType,
	void *const pParent)
{
	relpTcp_t *pThis;

	ENTER_RELPFUNC;
	assert(ppThis != NULL);
	if((pThis = calloc(1, sizeof(relpTcp_t))) == NULL) {
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	RELP_CORE_CONSTRUCTOR(pThis, Tcp);
	if(connType == RELP_SRV_CONN) {
		pThis->pSrv = (relpSrv_t*) pParent;
	} else {
		pThis->pClt = (relpClt_t*) pParent;
	}
	pThis->sock = -1;
	pThis->pEngine = pEngine;
	pThis->iSessMax = 500;	/* default max nbr of sessions - TODO: make configurable -- rgerhards, 2008-03-17*/
	pThis->bTLSActive = 0;
	pThis->dhBits = DEFAULT_DH_BITS;
	pThis->pristring = NULL;
	pThis->authmode = eRelpAuthMode_None;
	pThis->caCertFile = NULL;
	pThis->ownCertFile = NULL;
	pThis->privKeyFile = NULL;
	pThis->pUsr = NULL;
	pThis->permittedPeers.nmemb = 0;
	pThis->permittedPeers.peer = NULL;

	*ppThis = pThis;

finalize_it:
	LEAVE_RELPFUNC;
}


/** Destruct a RELP tcp instance
 */
relpRetVal
relpTcpDestruct(relpTcp_t **ppThis)
{
	relpTcp_t *pThis;
	int i;
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	int sslRet;
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */
#if defined(ENABLE_TLS_OPENSSL)
	int sslErr;
#endif /* #ifdef ENABLE_TLS_OPENSSL */

	ENTER_RELPFUNC;
	assert(ppThis != NULL);
	pThis = *ppThis;
	RELPOBJ_assert(pThis, Tcp);

	if(pThis->sock != -1) {
		close(pThis->sock);
		pThis->sock = -1;
	}

	if(pThis->socks != NULL) {
		/* if we have some sockets at this stage, we need to close them */
		for(i = 1 ; i <= pThis->socks[0] ; ++i)
			close(pThis->socks[i]);
		free(pThis->socks);
	}

#ifdef ENABLE_TLS
	if(pThis->bTLSActive) {
		sslRet = gnutls_bye(pThis->session, GNUTLS_SHUT_RDWR);
		while(sslRet == GNUTLS_E_INTERRUPTED || sslRet == GNUTLS_E_AGAIN) {
			sslRet = gnutls_bye(pThis->session, GNUTLS_SHUT_RDWR);
		}
		gnutls_deinit(pThis->session);
	}
#endif /* #ifdef ENABLE_TLS */

#ifdef ENABLE_TLS_OPENSSL
	if(	pThis->bTLSActive &&
		pThis->ssl != NULL) {

		/* Try shutdown */
		pThis->pEngine->dbgprint("relpTcpDestruct: try shutdown #1 for [%p]\n", (void *) pThis->ssl);
		sslRet = SSL_shutdown(pThis->ssl);
		if (sslRet <= 0) {
			sslErr = SSL_get_error(pThis->ssl, sslRet);
			pThis->pEngine->dbgprint("relpTcpDestruct: shutdown failed with err = %d, "
				"forcing ssl shutdown!\n", sslErr);

			/* ignore those SSL Errors on shutdown */
			if(	sslErr != SSL_ERROR_SYSCALL &&
					sslErr != SSL_ERROR_ZERO_RETURN &&
					sslErr != SSL_ERROR_WANT_READ &&
					sslErr != SSL_ERROR_WANT_WRITE) {
				/* Output Warning only */
				relpTcpLastSSLErrorMsg(sslRet, pThis, "relpTcpDestruct");
			}

			pThis->pEngine->dbgprint("relpTcpDestruct: session closed (un)successfully \n");
		} else {
			pThis->pEngine->dbgprint("relpTcpDestruct: session closed successfully \n");
		}

		/* Session closed */
		pThis->bTLSActive = 0;
		SSL_free(pThis->ssl);
		pThis->ssl = NULL;
	}
#endif /* #ifdef ENABLE_TLS_OPENSSL */

#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	relpTcpFreePermittedPeers(pThis);
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */

	if (pThis->pRemHostIP != NULL)
		free(pThis->pRemHostIP);
	if (pThis->pRemHostName != NULL)
		free(pThis->pRemHostName);
	if (pThis->pristring != NULL)
		free(pThis->pristring);
	if (pThis->caCertFile != NULL)
		free(pThis->caCertFile);
	if (pThis->ownCertFile != NULL)
		free(pThis->ownCertFile);
	if (pThis->privKeyFile != NULL)
		free(pThis->privKeyFile);

	/* done with de-init work, now free tcp object itself */
	free(pThis);
	*ppThis = NULL;

	LEAVE_RELPFUNC;
}


/* helper to call onErr if set */
static void
callOnErr(const relpTcp_t *__restrict__ const pThis,
	char *__restrict__ const emsg,
	const relpRetVal ecode)
{
	char objinfo[1024];
	pThis->pEngine->dbgprint("librelp: generic error: ecode %d, "
		"emsg '%s'\n", ecode, emsg);
	if(pThis->pEngine->onErr != NULL) {
		if(pThis->pSrv == NULL) { /* client */
			snprintf(objinfo, sizeof(objinfo), "conn to srvr %s:%s",
				 pThis->pClt->pSess->srvAddr,
				 pThis->pClt->pSess->srvPort);
		} else if(pThis->pRemHostIP == NULL) { /* server listener */
			snprintf(objinfo, sizeof(objinfo), "lstn %s",
				 pThis->pSrv->pLstnPort);
		} else { /* server connection to client */
			snprintf(objinfo, sizeof(objinfo), "lstn %s: conn to clt %s/%s",
				 pThis->pSrv->pLstnPort, pThis->pRemHostIP,
				 pThis->pRemHostName);
		}
		objinfo[sizeof(objinfo)-1] = '\0';
		pThis->pEngine->onErr(pThis->pUsr, objinfo, emsg, ecode);
	}
}


#ifdef ENABLE_TLS
/* helper to call an error code handler if gnutls failed. If there is a failure,
 * an error message is pulled form gnutls and the error message properly
 * populated.
 * Returns 1 if an error was detected, 0 otherwise. This can be used as a
 * shortcut for error handling (safes doing it twice).
 */
static int
chkGnutlsCode(relpTcp_t *const pThis, char *emsg, relpRetVal ecode, const int gnuRet)
{
	char msgbuf[4096];
	int r;

	if(gnuRet == GNUTLS_E_SUCCESS) {
		r = 0;
	} else {
		r = 1;
		snprintf(msgbuf, sizeof(msgbuf), "%s [gnutls error %d: %s]",
			 emsg, gnuRet, gnutls_strerror(gnuRet));
		msgbuf[sizeof(msgbuf)-1] = '\0';
		callOnErr(pThis, msgbuf, ecode);
	}
	return r;
}
#endif /* #ifdef ENABLE_TLS */

/* abort a tcp connection. This is much like relpTcpDestruct(), but tries
 * to discard any unsent data. -- rgerhards, 2008-03-24
 */
relpRetVal
relpTcpAbortDestruct(relpTcp_t **ppThis)
{
	struct linger ling;

	ENTER_RELPFUNC;
	assert(ppThis != NULL);
	RELPOBJ_assert((*ppThis), Tcp);

	if((*ppThis)->sock != -1) {
		ling.l_onoff = 1;
		ling.l_linger = 0;
		if(setsockopt((*ppThis)->sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0 ) {
			(*ppThis)->pEngine->dbgprint("could not set SO_LINGER, errno %d\n", errno);
		}
	}

	iRet = relpTcpDestruct(ppThis);

	LEAVE_RELPFUNC;
}


#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN
#	define SALEN(sa) ((sa)->sa_len)
#else
static inline size_t SALEN(struct sockaddr *sa) {
	switch (sa->sa_family) {
	case AF_INET:  return (sizeof(struct sockaddr_in));
	case AF_INET6: return (sizeof(struct sockaddr_in6));
	default:       return 0;
	}
}
#endif


/* we may later change the criteria, thus we encapsulate it
 * into a function.
 */
static inline int8_t
isAnonAuth(relpTcp_t *const pThis)
{
	return pThis->ownCertFile == NULL;
}

/* Set pRemHost based on the address provided. This is to be called upon accept()ing
 * a connection request. It must be provided by the socket we received the
 * message on as well as a NI_MAXHOST size large character buffer for the FQDN.
 * Please see http://www.hmug.org/man/3/getnameinfo.php (under Caveats)
 * for some explanation of the code found below. If we detect a malicious
 * hostname, we return RELP_RET_MALICIOUS_HNAME and let the caller decide
 * on how to deal with that.
 * rgerhards, 2008-03-31
 */
static relpRetVal
relpTcpSetRemHost(relpTcp_t *const pThis, struct sockaddr *pAddr)
{
	relpEngine_t *pEngine;
	int error;
	unsigned char szIP[NI_MAXHOST] = "";
	unsigned char szHname[NI_MAXHOST+64] = ""; /* 64 extra bytes for message text */
	struct addrinfo hints, *res;
	size_t len;

	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pEngine = pThis->pEngine;
	assert(pAddr != NULL);

	error = getnameinfo(pAddr, SALEN(pAddr), (char*)szIP, sizeof(szIP), NULL, 0, NI_NUMERICHOST);
	if(error) {
		pThis->pEngine->dbgprint("Malformed from address %s\n", gai_strerror(error));
		strcpy((char*)szHname, "???");
		strcpy((char*)szIP, "???");
		ABORT_FINALIZE(RELP_RET_INVALID_HNAME);
	}

	if(pEngine->bEnableDns) {
		error = getnameinfo(pAddr, SALEN(pAddr), (char*)szHname, sizeof(szHname), NULL, 0, NI_NAMEREQD);
		if(error == 0) {
			memset (&hints, 0, sizeof (struct addrinfo));
			hints.ai_flags = AI_NUMERICHOST;
			hints.ai_socktype = SOCK_STREAM;
			/* we now do a lookup once again. This one should fail,
			 * because we should not have obtained a non-numeric address. If
			 * we got a numeric one, someone messed with DNS!
			 */
			if(getaddrinfo((char*)szHname, NULL, &hints, &res) == 0) {
				freeaddrinfo (res);
				/* OK, we know we have evil, so let's indicate this to our caller */
				snprintf((char*)szHname, sizeof(szHname), "[MALICIOUS:IP=%s]", szIP);
				pEngine->dbgprint("Malicious PTR record, IP = \"%s\" HOST = \"%s\"", szIP, szHname);
				iRet = RELP_RET_MALICIOUS_HNAME;
			}
		} else {
			strcpy((char*)szHname, (char*)szIP);
		}
	} else {
		strcpy((char*)szHname, (char*)szIP);
	}

	/* We now have the names, so now let's allocate memory and store them permanently.
	 * (side note: we may hold on to these values for quite a while, thus we trim their
	 * memory consumption)
	 */
	len = strlen((char*)szIP) + 1; /* +1 for \0 byte */
	if((pThis->pRemHostIP = malloc(len)) == NULL)
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	memcpy(pThis->pRemHostIP, szIP, len);

	len = strlen((char*)szHname) + 1; /* +1 for \0 byte */
	if((pThis->pRemHostName = malloc(len)) == NULL) {
		free(pThis->pRemHostIP); /* prevent leak */
		pThis->pRemHostIP = NULL;
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
	memcpy(pThis->pRemHostName, szHname, len);

finalize_it:
	LEAVE_RELPFUNC;
}

/* this copies a *complete* permitted peers structure into the
 * tcp object.
 */
relpRetVal
relpTcpSetPermittedPeers(relpTcp_t __attribute__((unused)) *pThis,
	relpPermittedPeers_t __attribute__((unused)) *pPeers)
{
	ENTER_RELPFUNC;
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	int i;
	relpTcpFreePermittedPeers(pThis);
	if(pPeers->nmemb != 0) {
		if((pThis->permittedPeers.peer =
			malloc(sizeof(tcpPermittedPeerEntry_t) * pPeers->nmemb)) == NULL) {
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
		}
		for(i = 0 ; i < pPeers->nmemb ; ++i) {
			if((pThis->permittedPeers.peer[i].name = strdup(pPeers->name[i])) == NULL) {
				ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
			}
			pThis->permittedPeers.peer[i].wildcardRoot = NULL;
			pThis->permittedPeers.peer[i].wildcardLast = NULL;
			CHKRet(relpTcpPermittedPeerWildcardCompile(&(pThis->permittedPeers.peer[i])));
		}
	}
	pThis->permittedPeers.nmemb = pPeers->nmemb;
#else
	ABORT_FINALIZE(RELP_RET_ERR_NO_TLS);
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */
finalize_it:
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetUsrPtr(relpTcp_t *const pThis, void *pUsr)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->pUsr = pUsr;
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetAuthMode(relpTcp_t *const pThis, relpAuthMode_t authmode)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->authmode = authmode;
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetConnTimeout(relpTcp_t *const pThis, const int connTimeout)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->connTimeout = connTimeout;
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetGnuTLSPriString(relpTcp_t *const pThis, char *pristr)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	free(pThis->pristring);
	if(pristr == NULL) {
		pThis->pristring = NULL;
	} else {
		if((pThis->pristring = strdup(pristr)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetCACert(relpTcp_t *const pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	free(pThis->caCertFile);
	if(cert == NULL) {
		pThis->caCertFile = NULL;
	} else {
		if((pThis->caCertFile = strdup(cert)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetOwnCert(relpTcp_t *const pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	free(pThis->ownCertFile);
	if(cert == NULL) {
		pThis->ownCertFile = NULL;
	} else {
		if((pThis->ownCertFile = strdup(cert)) == NULL)
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
finalize_it:
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetPrivKey(relpTcp_t *const pThis, char *cert)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	free(pThis->privKeyFile);
	if(cert == NULL) {
		pThis->privKeyFile = NULL;
	} else {
#if		defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION) || defined(ENABLE_TLS_OPENSSL)
			if((pThis->privKeyFile = strdup(cert)) == NULL)
				ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
#		else
			ABORT_FINALIZE(RELP_RET_ERR_NO_TLS_AUTH);
#		endif
	}
finalize_it:
	LEAVE_RELPFUNC;
}


/* Enable TLS mode. */
relpRetVal
relpTcpEnableTLS(relpTcp_t __attribute__((unused)) *pThis)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	pThis->bEnableTLS = 1;
#else
	iRet = RELP_RET_ERR_NO_TLS;
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpEnableTLSZip(relpTcp_t __attribute__((unused)) *pThis)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
#if defined(ENABLE_TLS)
/* || defined(ENABLE_TLS_OPENSSL)*/
	pThis->bEnableTLSZip = 1;
#else
	iRet = RELP_RET_ERR_NO_TLS;
#endif /* #ifdef ENABLE_TLS */
	LEAVE_RELPFUNC;
}

relpRetVal
relpTcpSetDHBits(relpTcp_t *const pThis, const int bits)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->dhBits = bits;
	LEAVE_RELPFUNC;
}

#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
/* set TLS priority string, common code both for client and server */
static relpRetVal
relpTcpTLSSetPrio(relpTcp_t *const pThis)
{
#if defined(ENABLE_TLS)
	int r;
#endif
	char pristringBuf[4096];
	char *pristring;
	ENTER_RELPFUNC;
	/* Compute priority string (in simple cases where the user does not care...) */
	if(pThis->pristring == NULL) {
#if defined(ENABLE_TLS)
		if(pThis->bEnableTLSZip) {
			strncpy(pristringBuf, "NORMAL:+ANON-DH:+COMP-ALL", sizeof(pristringBuf));
		} else {
			strncpy(pristringBuf, "NORMAL:+ANON-DH:+COMP-NULL", sizeof(pristringBuf));
		}
#endif /* defined(ENABLE_TLS)*/
#if defined(ENABLE_TLS_OPENSSL)
	if (pThis->authmode == eRelpAuthMode_None)
		strncpy(pristringBuf, "ALL:+COMPLEMENTOFDEFAULT:+ADH:+ECDH:+aNULL" /* :+aNULL:+eNULL */,
			sizeof(pristringBuf));
	else
		strncpy(pristringBuf, "DEFAULT", sizeof(pristringBuf));

#endif /* defined(ENABLE_TLS_OPENSSL)*/
		pristringBuf[sizeof(pristringBuf)-1] = '\0';
		pristring = pristringBuf;
	} else {
		pristring = pThis->pristring;
	}

#if defined(ENABLE_TLS)
	r = gnutls_priority_set_direct(pThis->session, pristring, NULL);
	if(r == GNUTLS_E_INVALID_REQUEST) {
		ABORT_FINALIZE(RELP_RET_INVLD_TLS_PRIO);
	} else if(r != GNUTLS_E_SUCCESS) {
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}
#endif /* defined(ENABLE_TLS)*/
#if defined(ENABLE_TLS_OPENSSL)
	if ( SSL_set_cipher_list(pThis->ssl, pristring) == 0 ){
		pThis->pEngine->dbgprint("relpTcpTLSSetPrio: Error setting ciphers '%s'\n", pristring);
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}
#endif /* defined(ENABLE_TLS_OPENSSL)*/

finalize_it:
	pThis->pEngine->dbgprint("relpTcpTLSSetPrio: Setting ciphers '%s' iRet=%d\n", pristring, iRet);

#if defined(ENABLE_TLS)
	if(iRet != RELP_RET_OK)
		chkGnutlsCode(pThis, "Failed to set GnuTLS priority", iRet, r);
#endif /* defined(ENABLE_TLS)*/
	LEAVE_RELPFUNC;
}
#endif /* defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)*/

#ifdef ENABLE_TLS
#ifndef _AIX
#pragma GCC diagnostic push
/* per https://lists.gnupg.org/pipermail/gnutls-help/2004-August/000154.html This is expected */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif
static relpRetVal
relpTcpAcceptConnReqInitTLS(relpTcp_t *const pThis, relpSrv_t *const pSrv)
{
	int r;
	ENTER_RELPFUNC;

	r = gnutls_init(&pThis->session, GNUTLS_SERVER);
	if(chkGnutlsCode(pThis, "Failed to initialize GnuTLS", RELP_RET_ERR_TLS_SETUP, r)) {
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}

	if(pSrv->pTcp->pristring != NULL)
		pThis->pristring = strdup(pSrv->pTcp->pristring);
	pThis->authmode = pSrv->pTcp->authmode;
	pThis->pUsr = pSrv->pUsr;
	CHKRet(relpTcpTLSSetPrio(pThis));
	gnutls_session_set_ptr(pThis->session, pThis);

	if(isAnonAuth(pSrv->pTcp)) {
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_ANON, pSrv->pTcp->anoncredSrv);
		if(chkGnutlsCode(pThis, "Failed setting anonymous credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	} else { /* cert-based auth */
		if(pSrv->pTcp->caCertFile == NULL) {
			gnutls_certificate_send_x509_rdn_sequence(pThis->session, 0);
		}
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_CERTIFICATE, pSrv->pTcp->xcred);
		if(chkGnutlsCode(pThis, "Failed setting certificate credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	}
	gnutls_dh_set_prime_bits(pThis->session, pThis->dhBits);
	gnutls_certificate_server_set_request(pThis->session, GNUTLS_CERT_REQUEST);

	gnutls_transport_set_ptr(pThis->session, (gnutls_transport_ptr_t) pThis->sock);
	r = gnutls_handshake(pThis->session);
	if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
		pThis->pEngine->dbgprint("librelp: gnutls_handshake retry necessary (this is OK and expected)\n");
		pThis->rtryOp = relpTCP_RETRY_handshake;
	} else if(r != GNUTLS_E_SUCCESS) {
		chkGnutlsCode(pThis, "TLS handshake failed", RELP_RET_ERR_TLS_HANDS, r);
		ABORT_FINALIZE(RELP_RET_ERR_TLS_HANDS);
	}

	pThis->bTLSActive = 1;

finalize_it:
	LEAVE_RELPFUNC;
}
#ifndef _AIX
#pragma GCC diagnostic pop
#endif
#endif /* #ifdef ENABLE_TLS */

#ifdef ENABLE_TLS_OPENSSL
#ifndef _AIX
#pragma GCC diagnostic push
/* per https://lists.gnupg.org/pipermail/gnutls-help/2004-August/000154.html This is expected */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

/* global init OpenSSL
 */
static relpRetVal
relpTcpInitTLS(relpTcp_t *const pThis)
{
	int iErr = 0;
	ENTER_RELPFUNC;
	pThis->pEngine->dbgprint("relpTcpInitTLS: Init OpenSSL library\n");

	/* Setup OpenSSL library */
	if((opensslh_THREAD_setup() == 0) || !SSL_library_init()) {
		pThis->pEngine->dbgprint("relpTcpInitTLS: Error OpenSSL initialization failed\n");
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	/* Load readable error strings */
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();

	/* Create main CTX Object */
	ctx = SSL_CTX_new(SSLv23_method());

	/* Set CTX Options */
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);		/* Disable insecure SSLv2 Protocol */
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3);		/* Disable insecure SSLv3 Protocol */
	SSL_CTX_sess_set_cache_size(ctx,1024);			/* TODO: make configurable? */

	#if OPENSSL_VERSION_NUMBER >= 0x10002000L
	/* Enable Support for automatic EC temporary key parameter selection. */
	SSL_CTX_set_ecdh_auto(ctx, 1);
	#else
	pThis->pEngine->dbgprint("relpTcpInitTLS: openssl to old, cannot use SSL_CTX_set_ecdh_auto."
		"Using SSL_CTX_set_tmp_ecdh with NID_X9_62_prime256v1/() instead.\n");
	SSL_CTX_set_tmp_ecdh(ctx, EC_KEY_new_by_curve_name(NID_X9_62_prime256v1));

	#endif

	SSL_CTX_set_timeout(ctx, 30);	/* Default Session Timeout, TODO: Make configureable */
	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

	/* After CA Cert Init, set default VERIFY Options for OpenSSL CTX - and CALLBACK */
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, verify_callback);

	/* Init CA Certificate first */
	if(	pThis->caCertFile != NULL ) {
		if (SSL_CTX_load_verify_locations(ctx, pThis->caCertFile, NULL) != 1) {
			pThis->pEngine->dbgprint("relpTcpInitTLS: Error, CA certificate could not be accessed."
					" Is the file at the right path? And do we have the permissions?\n");
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		} else
			pThis->pEngine->dbgprint("relpTcpInitTLS: Successfully initialized CA certificate\n");
	} else
		pThis->pEngine->dbgprint("relpTcpInitTLS: CA certificate MISSING\n");

	called_openssl_global_init = 1;
finalize_it:
	LEAVE_RELPFUNC;
}

static void
relpTcpExitTLS(void)
{
	SSL_CTX_free(ctx);
	ENGINE_cleanup();
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
}

/* Perform additional certificate name checks
 */
relpRetVal
relpTcpChkPeerAuth(relpTcp_t *const pThis)
{
	X509* certpeer;
	int r;
	ENTER_RELPFUNC;

	/* Get peer certificate from SSL */
	certpeer = SSL_get_peer_certificate(pThis->ssl);
	if ( certpeer == NULL ) {
		if(pThis->authmode == eRelpAuthMode_None ) {
			pThis->pEngine->dbgprint("relpTcpChkPeerAuth: peer certificate for [%p] invalid, "
				"but allowed in anon auth mode\n", (void *) pThis);
		} else
			ABORT_FINALIZE(RELP_RET_AUTH_NO_CERT);
	} else {
		/* verify client cert at first **/
		r = SSL_get_verify_result(pThis->ssl);
		if (r != X509_V_OK) {
			if (r == X509_V_ERR_CERT_HAS_EXPIRED) {
				callOnAuthErr(pThis, (char*) X509_verify_cert_error_string(r),
					"certificate validation failed, certificate expired!",
					RELP_RET_AUTH_CERT_INVL);
				ABORT_FINALIZE(RELP_RET_AUTH_CERT_INVL);
			} else {
				callOnAuthErr(pThis, (char*) X509_verify_cert_error_string(r),
					"certificate validation failed",
					RELP_RET_AUTH_CERT_INVL);
				ABORT_FINALIZE(RELP_RET_AUTH_CERT_INVL);
			}
		}

		/* Now check for auth modes */
		if(pThis->authmode == eRelpAuthMode_Name ) {
			CHKRet(relpTcpChkPeerName(pThis, certpeer));
			pThis->pEngine->dbgprint("relpTcpChkPeerAuth: name mode - success\n");
		} else if(pThis->authmode == eRelpAuthMode_Fingerprint) {
			CHKRet(relpTcpChkPeerFingerprint(pThis, certpeer));
			pThis->pEngine->dbgprint("relpTcpChkPeerAuth: fingerprint mode - success\n");
		} else {
			pThis->pEngine->dbgprint("relpTcpChkPeerAuth: anon mode - success\n");
		}
	}

finalize_it:
	LEAVE_RELPFUNC;
}


/* Perform all necessary checks after Handshake
 */
relpRetVal
relpTcpPostHandshakeCheck(relpTcp_t *const pThis)
{
	ENTER_RELPFUNC;
	char szDbg[255];
	const SSL_CIPHER* sslCipher;

	/* Some extra output for debugging openssl */
	if (SSL_get_shared_ciphers(pThis->ssl,szDbg, sizeof szDbg) != NULL)
		pThis->pEngine->dbgprint("relpTcpPostHandshakeCheck: Debug Shared ciphers = %s\n", szDbg);
	sslCipher = (const SSL_CIPHER*) SSL_get_current_cipher(pThis->ssl);
	if (sslCipher != NULL)
		pThis->pEngine->dbgprint("relpTcpPostHandshakeCheck: Debug Version: %s Name: %s\n",
			SSL_CIPHER_get_version(sslCipher), SSL_CIPHER_get_name(sslCipher));

	FINALIZE;

finalize_it:
	LEAVE_RELPFUNC;
}

/* Perform all necessary checks after Handshake
 */
relpRetVal
relpTcpSslInitCerts(relpTcp_t *const pThis, char *ownCertFile, char *privKeyFile)
{
	ENTER_RELPFUNC;

	if(	ownCertFile!= NULL ) {
		if (SSL_use_certificate_file(pThis->ssl, ownCertFile, SSL_FILETYPE_PEM) != 1) {
			pThis->pEngine->dbgprint("relpTcpSslInitCerts: error, Certificate file could not be "
				"accessed. Is the file at the right path? And do we have the "
				"permissions?");
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		} else
			pThis->pEngine->dbgprint("relpTcpSslInitCerts: Successfully initialized certificate file\n");
	} else
		pThis->pEngine->dbgprint("relpTcpSslInitCerts: certificate file MISSING\n");

	if(	privKeyFile!= NULL ) {
		if (SSL_use_PrivateKey_file(pThis->ssl, privKeyFile, SSL_FILETYPE_PEM) != 1) {
			pThis->pEngine->dbgprint("relpTcpSslInitCerts: Error, Key file could not be accessed. "
				"Is the file at the right path? And do we have the permissions?");
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		} else
			pThis->pEngine->dbgprint("relpTcpSslInitCerts: Successfully initialized key file\n");
	} else
		pThis->pEngine->dbgprint("relpTcpSslInitCerts: key file MISSING\n");
	FINALIZE;

finalize_it:
	LEAVE_RELPFUNC;
}

/* return direction in which retry must be done. We maps openssl retry state to
 * "0 if trying to read data, 1 if trying to write data."
 */
int
relpTcpGetRtryDirection(relpTcp_t *const pThis)
{
	if (pThis->rtryOp == relpTCP_RETRY_send)
		return 1; /* try to write data */
	else
		return 0; /* try to read data = default */
}

/* Perform all necessary actions for Handshake
 */
relpRetVal
relpTcpRtryHandshake(relpTcp_t *const pThis)
{
	int res, resErr;
	ENTER_RELPFUNC;
	pThis->pEngine->dbgprint("relpTcpRtryHandshake: Starting TLS Handshake for ssl[%p]\n", (void *)pThis->ssl);

	if (pThis->sslState == osslServer) {
		/* Handle Server SSL Object */
		if((res = SSL_accept(pThis->ssl)) <= 0) {
			/* Obtain SSL Error code */
			resErr = SSL_get_error(pThis->ssl, res);
			if(	resErr == SSL_ERROR_WANT_READ ||
				resErr == SSL_ERROR_WANT_WRITE) {
				pThis->rtryOp = relpTCP_RETRY_handshake;
//				pThis->rtryOsslErr = resErr; /* Store SSL ErrorCode into*/
				pThis->pEngine->dbgprint("relpTcpRtryHandshake: Server handshake does not "
					"complete immediately - setting to retry (this is OK and normal)\n");
				FINALIZE;
			} else if(resErr == SSL_ERROR_SYSCALL) {
				pThis->pEngine->dbgprint("relpTcpRtryHandshake: Server handshake failed with "
					"SSL_ERROR_SYSCALL - Aborting handshake.\n");
				relpTcpLastSSLErrorMsg(res, pThis, "relpTcpRtryHandshake Server");
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			} else {
				relpTcpLastSSLErrorMsg(res, pThis, "relpTcpRtryHandshake Server");
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
		} else
			pThis->pEngine->dbgprint("relpTcpRtryHandshake: Server handshake finished for ssl[%p]\n",
				(void *)pThis->ssl);
	} else {
		/* Handle Client SSL Object */
		if((res = SSL_do_handshake(pThis->ssl)) <= 0) {
			/* Obtain SSL Error code */
			resErr = SSL_get_error(pThis->ssl, res);
			if(	resErr == SSL_ERROR_WANT_READ ||
				resErr == SSL_ERROR_WANT_WRITE) {
				pThis->rtryOp = relpTCP_RETRY_handshake;
//				pThis->rtryOsslErr = resErr; /* Store SSL ErrorCode into*/
				pThis->pEngine->dbgprint("relpTcpRtryHandshake: Client handshake does not complete "
					"immediately - setting to retry (this is OK and normal)\n");
				FINALIZE;
			} else if(resErr == SSL_ERROR_SYSCALL) {
				pThis->pEngine->dbgprint("relpTcpRtryHandshake: Client handshake failed with "
					"SSL_ERROR_SYSCALL - Aborting handshake.\n");
				relpTcpLastSSLErrorMsg(res, pThis, "relpTcpRtryHandshake Client");
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP /*RS_RET_RETRY*/);
			} else {
				relpTcpLastSSLErrorMsg(res, pThis, "relpTcpRtryHandshake Client");
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
		} else
			pThis->pEngine->dbgprint("relpTcpRtryHandshake: Client handshake finished for ssl[%p]\n",
				(void *)pThis->ssl);
	}

	/* Handshake finished */
	pThis->rtryOp = relpTCP_RETRY_none;

	/* Do post handshake stuff */
	CHKRet(relpTcpPostHandshakeCheck(pThis));

	/* Now check authorization */
	CHKRet(relpTcpChkPeerAuth(pThis));

finalize_it:
	LEAVE_RELPFUNC;
}

static relpRetVal
relpTcpAcceptConnReqInitTLS(relpTcp_t *const pThis, relpSrv_t *const pSrv)
{
	BIO *client = NULL;
	ENTER_RELPFUNC;

	pThis->pEngine->dbgprint("relpTcpAcceptConnReqInitTLS: : Accepting connection for [%p] ... \n", (void *)pThis);

	if(!(pThis->ssl = SSL_new(ctx))) {
		relpTcpLastSSLErrorMsg(0, pThis, "relpTcpAcceptConnReqInitTLS");
	}
	pThis->authmode = pSrv->pTcp->authmode;
	pThis->pUsr = pSrv->pUsr;

	if(!isAnonAuth(pThis->pSrv->pTcp)) {
		CHKRet(relpTcpSslInitCerts(pThis, pThis->pSrv->ownCertFile, pThis->pSrv->privKey));
	} else
		pThis->authmode = eRelpAuthMode_None;

	CHKRet(relpTcpTLSSetPrio(pThis));
	SSL_set_ex_data(pThis->ssl, 0, pThis);

	if (pThis->authmode != eRelpAuthMode_None) {
		pThis->pEngine->dbgprint("relpTcpAcceptConnReqInitTLS: enable certificate checking\n");
		/* Enable certificate valid checking */
		SSL_set_verify(pThis->ssl, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_callback);
		SSL_set_verify_depth(pThis->ssl, 4);
	} else {
		SSL_set_verify(pThis->ssl, SSL_VERIFY_NONE, verify_callback);
	}

	/* Create BIO from ptcp socket! */
	client = BIO_new_socket(pThis->sock, BIO_CLOSE /*BIO_NOCLOSE*/);
	pThis->pEngine->dbgprint("relpTcpAcceptConnReqInitTLS: Init client BIO[%p] done\n", (void *)client);

	/* Set debug Callback for client BIO as well! */
	BIO_set_callback(client, BIO_debug_callback);
	BIO_set_callback_arg(client, (char *)pThis);

/* TODO: still needed? Set to NON blocking ! */
BIO_set_nbio( client, 1 );

	SSL_set_bio(pThis->ssl, client, client);
	SSL_set_accept_state(pThis->ssl); /* sets ssl to work in server mode. */

	pThis->bTLSActive = 1;
	pThis->sslState = osslServer; /*set Server state */

	/* We now do the handshake */
	CHKRet(relpTcpRtryHandshake(pThis));

finalize_it:
	/* Accept appears to be done here */
	pThis->pEngine->dbgprint("relpTcpAcceptConnReqInitTLS: END iRet = %d, pThis=[%p], pThis->rtryCall=%d\n",
		iRet, (void *) pThis, pThis->rtryOp);
	if(iRet != RELP_RET_OK) {
		if (pThis->ssl != NULL) {
			/* Client BIO is freed within SSL_free() */
			SSL_free(pThis->ssl);
			pThis->ssl = NULL;
		}
	}

	LEAVE_RELPFUNC;
}


/* Wrapper function for openssl lib client init */
static relpRetVal
relpTcpConnectTLSInit(relpTcp_t *const pThis)
{
	int sockflags;
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit openssl\n");

	/* SSL Objects */
	BIO *conn = NULL;

	/* We expect a non blocking socket to establish a tls session */
	if((sockflags = fcntl(pThis->sock, F_GETFL)) != -1) {
		sockflags &= ~O_NONBLOCK;
		sockflags = fcntl(pThis->sock, F_SETFL, sockflags);
	}

	if(sockflags == -1) {
		pThis->pEngine->dbgprint("error %d unsetting fcntl(O_NONBLOCK) on relp socket\n", errno);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	if(!called_openssl_global_init) {
		/* Init OpenSSL lib */
		CHKRet(relpTcpInitTLS(pThis));
	}

	/* Create BIO from ptcp socket! */
	conn = BIO_new_socket(pThis->sock, BIO_CLOSE /*BIO_NOCLOSE*/);
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit: Init conn BIO[%p] done\n", (void *)conn);

	/* Set debug Callback for client BIO as well! */
	BIO_set_callback(conn, BIO_debug_callback);
	BIO_set_callback_arg(conn, (char *)pThis);

/* TODO: still needed? Set to NON blocking ! */
BIO_set_nbio( conn, 1 );

	/*if we reach this point we are in tls mode */
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit: TLS Mode\n");
	if(!(pThis->ssl = SSL_new(ctx))) {
		relpTcpLastSSLErrorMsg(0, pThis, "relpTcpConnectTLSInit");
/*		errmsg.LogError(0, RS_RET_NO_ERRCODE, "Error creating an SSL context"); */
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	/* Load certificate data into SSL object */
	if(!isAnonAuth(pThis)) {
		pThis->pEngine->dbgprint("relpTcpConnectTLSInit: Init Client Certs \n");
		CHKRet(relpTcpSslInitCerts(pThis, pThis->ownCertFile, pThis->privKeyFile));
	} else
		pThis->authmode = eRelpAuthMode_None;

	CHKRet(relpTcpTLSSetPrio(pThis));
	SSL_set_ex_data(pThis->ssl, 0, (void*)pThis);

	SSL_set_bio(pThis->ssl, conn, conn);
	SSL_set_connect_state(pThis->ssl); /*sets ssl to work in client mode.*/
	pThis->sslState = osslClient; /*set client state */

	/* Perform the TLS handshake */
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit: try handshake for [%p]\n", (void *)pThis);
	CHKRet(relpTcpRtryHandshake(pThis));

	/* set the socket to non-blocking IO (we do this on the recv() for non-TLS */
	if((sockflags = fcntl(pThis->sock, F_GETFL)) != -1) {
		sockflags |= O_NONBLOCK;
		/* SETFL could fail too, so get it caught by the subsequent
		 * error check.  */
		if(fcntl(pThis->sock, F_SETFL, sockflags) == -1) {
			callOnErr(pThis, "error setting socket to non-blocking",
				RELP_RET_ERR_TLS_SETUP);
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	}

finalize_it:
	/* Connect appears to be done here */
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit: END iRet = %d, pThis=[%p], pNsd->rtryOp=%d\n",
		iRet, (void *) pThis, pThis->rtryOp);
	if(iRet != RELP_RET_OK)	{
		if (conn != NULL) {
			BIO_free(conn);
		}
	}
	LEAVE_RELPFUNC;
}

/* Wrapper function for openssl lib server init */
static relpRetVal
relpTcpLstnInitTLS(relpTcp_t *const pThis)
{
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	if(!called_openssl_global_init) {
		CHKRet(relpTcpInitTLS(pThis));
	}
	pThis->pEngine->dbgprint("relpTcpLstnInitTLS openssl init done \n");

finalize_it:
	LEAVE_RELPFUNC;
}


#ifndef _AIX
#pragma GCC diagnostic pop
#endif
#endif /* #ifdef ENABLE_TLS_OPENSSL */

/* Enable KEEPALIVE handling on the socket.  */
static void
EnableKeepAlive(const relpTcp_t *__restrict__ const pThis,
	const relpSrv_t *__restrict__ const pSrv,
	const int sock)
{
	int ret;
	int optval;
	socklen_t optlen;

	optval = 1;
	optlen = sizeof(optval);
	ret = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
	if(ret < 0) {
		pThis->pEngine->dbgprint("librelp: EnableKeepAlive socket call "
					"returns error %d\n", ret);
		goto done;
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveProbes > 0) {
		optval = pSrv->iKeepAliveProbes;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPCNT, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive probes - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveTime > 0) {
		optval = pSrv->iKeepAliveTime;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive time - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

#	if defined(TCP_KEEPCNT)
	if(pSrv->iKeepAliveIntvl > 0) {
		optval = pSrv->iKeepAliveIntvl;
		optlen = sizeof(optval);
		ret = setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &optval, optlen);
	} else {
		ret = 0;
	}
#	else
	ret = -1;
#	endif
	if(ret < 0) {
		callOnErr(pThis, "librelp cannot set keepalive intvl - ignored",
			  RELP_RET_WRN_NO_KEEPALIVE);
	}

	// pThis->pEngine->dbgprint("KEEPALIVE enabled for socket %d\n", sock);

done:
	return;
}

/* a portable way to put the current thread asleep. Note that
 * using the sleep() API family may result in the whole process
 * to be put asleep on some platforms.
 */
static void
doSleep(int iSeconds, const int iuSeconds)
{
	struct timeval tvSelectTimeout;
	tvSelectTimeout.tv_sec = iSeconds;
	tvSelectTimeout.tv_usec = iuSeconds; /* micro seconds */
	select(0, NULL, NULL, NULL, &tvSelectTimeout);
}

/* accept an incoming connection request, sock provides the socket on which we can
 * accept the new session.
 * rgerhards, 2008-03-17
 */
relpRetVal
relpTcpAcceptConnReq(relpTcp_t **ppThis, const int sock, relpSrv_t *const pSrv)
{
	relpTcp_t *pThis = NULL;
	int sockflags;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	int iNewSock = -1;
	relpEngine_t *pEngine = pSrv->pEngine;

	ENTER_RELPFUNC;
	assert(ppThis != NULL);

	iNewSock = accept(sock, (struct sockaddr*) &addr, &addrlen);
	if(iNewSock < 0) {
		char errStr[1024];
		_relpEngine_strerror_r(errno, errStr, sizeof(errStr));
		pSrv->pEngine->dbgprint("error during accept, sleeping 20ms: %s\n", errStr);
		doSleep(0, 20000);
		pSrv->pEngine->dbgprint("END SLEEP\n");
		ABORT_FINALIZE(RELP_RET_ACCEPT_ERR);
	}

	/* construct our object so that we can use it... */
	CHKRet(relpTcpConstruct(&pThis, pEngine, RELP_SRV_CONN, pSrv));

	if(pSrv->bKeepAlive)
		EnableKeepAlive(pThis, pSrv, iNewSock);

	/* TODO: obtain hostname, normalize (callback?), save it */
	CHKRet(relpTcpSetRemHost(pThis, (struct sockaddr*) &addr));
	pThis->pEngine->dbgprint("remote host is '%s', ip '%s'\n", pThis->pRemHostName, pThis->pRemHostIP);

	/* set the new socket to non-blocking IO */
	if((sockflags = fcntl(iNewSock, F_GETFL)) != -1) {
		sockflags |= O_NONBLOCK;
		/* SETFL could fail too, so get it caught by the subsequent
		 * error check.
		 */
		sockflags = fcntl(iNewSock, F_SETFL, sockflags);
	}
	if(sockflags == -1) {
		pThis->pEngine->dbgprint("error %d setting fcntl(O_NONBLOCK) on relp socket %d", errno, iNewSock);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	pThis->sock = iNewSock;
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	if(pSrv->pTcp->bEnableTLS) {
		pThis->bEnableTLS = 1;
		pThis->pSrv = pSrv;
		CHKRet(relpTcpSetPermittedPeers(pThis, &(pSrv->permittedPeers)));
		CHKRet(relpTcpAcceptConnReqInitTLS(pThis, pSrv));
	}
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL */

	*ppThis = pThis;

finalize_it:
	if(iRet != RELP_RET_OK) {
		if(pThis != NULL)
			relpTcpDestruct(&pThis);
		/* the close may be redundant, but that doesn't hurt... */
		if(iNewSock >= 0)
			close(iNewSock);
	}

	LEAVE_RELPFUNC;
}

#if defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION) || defined(ENABLE_TLS_OPENSSL)
/* check a peer against a wildcard entry. This is a more lengthy
 * operation.
 */
static void
relpTcpChkOnePeerWildcard(tcpPermittedPeerWildcardComp_t *pRoot,
	const char *peername,
	int *pbFoundPositiveMatch,
	relpEngine_t *const pEngine)
{
	tcpPermittedPeerWildcardComp_t *pWildcard;
	const char *pC;
	const char *pStart; /* start of current domain component */
	int iWildcard, iName; /* work indexes for backward comparisons */

	*pbFoundPositiveMatch = 0;
	pWildcard = pRoot;
	pC = peername;
	while(*pC != '\0') {
		if(pWildcard == NULL) {
			/* we have more domain components than we have wildcards --> no match */
			goto done;
		}
		pStart = pC;
		while(*pC != '\0' && *pC != '.') {
			++pC;
		}

		/* got the component, now do the match */
		switch(pWildcard->wildcardType) {
			case tcpPEER_WILDCARD_NONE:
				if(   pWildcard->lenDomainPart != pC - pStart
				   || strncmp((char*)pStart, (char*)pWildcard->pszDomainPart, pC - pStart)) {
					goto done;
				}
				break;
			case tcpPEER_WILDCARD_AT_START:
				/* we need to do the backwards-matching manually */
				if(pWildcard->lenDomainPart > pC - pStart) {
					goto done;
				}
				iName = (size_t) (pC - pStart) - pWildcard->lenDomainPart;
				iWildcard = 0;
				while(iWildcard < pWildcard->lenDomainPart) {
					if(pWildcard->pszDomainPart[iWildcard] != pStart[iName]) {
						goto done;
					}
					++iName;
					++iWildcard;
				}
				break;
			case tcpPEER_WILDCARD_AT_END:
				if(   pWildcard->lenDomainPart > pC - pStart
				   || strncmp((char*)pStart, (char*)pWildcard->pszDomainPart,
					pWildcard->lenDomainPart)) {
					goto done;
				}
				break;
			case tcpPEER_WILDCARD_MATCH_ALL:
				/* everything is OK, just continue */
				break;
			case tcpPEER_WILDCARD_EMPTY_COMPONENT:
				if(pC - pStart > 0) {
				   	/* if it is not empty, it is no match... */
					goto done;
				}
				break;
			default:assert(0); /* make sure we die when debugging */
				relpEngineCallOnGenericErr(pEngine,
					"librelp", RELP_RET_ERR_INTERNAL,
					"invalid wildcardType %d in %s:%d",
					pWildcard->wildcardType, __FILE__, __LINE__);
				break;
		}
		pWildcard =  pWildcard->pNext; /* we processed this entry */

		/* skip '.' if we had it and so prepare for next iteration */
		if(*pC == '.')
			++pC;
	}

	/* we need to adjust for a border case, that is if the last component is
	 * empty. That happens frequently if the domain root (e.g. "example.com.")
	 * is properly given.
	 */
	if(pWildcard != NULL && pWildcard->wildcardType == tcpPEER_WILDCARD_EMPTY_COMPONENT)
		pWildcard = pWildcard->pNext;

	if(pWildcard != NULL) {
		/* we have more domain components than in the name to be
		 * checked. So this is no match.
		 */
		goto done;
	}
	*pbFoundPositiveMatch = 1;
done:	return;
}

/* Perform a match on ONE peer name obtained from the certificate. This name
 * is checked against the set of configured credentials. *pbFoundPositiveMatch is
 * set to 1 if the ID matches. *pbFoundPositiveMatch must have been initialized
 * to 0 by the caller (this is a performance enhancement as we expect to be
 * called multiple times).
 */
static void
relpTcpChkOnePeerName(relpTcp_t *const pThis, char *peername, int *pbFoundPositiveMatch)
{
	int i;

	for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i) {
		pThis->pEngine->dbgprint("relpTcpChkOnePeerName: compare peername '%s' with permitted name '%s'\n",
			peername, pThis->permittedPeers.peer[i].name);

		if(pThis->permittedPeers.peer[i].wildcardRoot == NULL) {
			/* simple string, only, no wildcards */
			if(!strcmp(peername, pThis->permittedPeers.peer[i].name)) {
				*pbFoundPositiveMatch = 1;
				break;
			}
		} else {
			relpTcpChkOnePeerWildcard(pThis->permittedPeers.peer[i].wildcardRoot,
			        peername, pbFoundPositiveMatch, pThis->pEngine);
			if (*pbFoundPositiveMatch)
				break;
		}
	}
}

/* helper to consistently add names to error message buffer */
static int
relpTcpAddToCertNamesBuffer(relpTcp_t *const pThis,
	char *const buf,
	const size_t buflen,
	int *p_currIdx,
	const char *const certName)
{
	int r = 0;
	assert(buf != NULL);
	assert(p_currIdx != NULL);
	const int currIdx = *p_currIdx;
	const int n = snprintf(buf + currIdx, buflen - currIdx,
		"DNSname: %s; ", certName);
	if(n < 0 || n >= (int) (buflen - currIdx)) {
		callOnAuthErr(pThis, "", "certificate validation failed, names "
			"inside certifcate are way to long (> 32KiB)",
			RELP_RET_AUTH_CERT_INVL);
		r = RELP_RET_PARAM_ERROR;
	} else {
		*p_currIdx += n;
	}
	return r;
}
#endif /* defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION) || defined(ENABLE_TLS_OPENSSL) */


#if defined(ENABLE_TLS)
#ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION

/* Glue to use the right type of function depending of the version */
#if GNUTLS_VERSION_NUMBER < 0x030202
	//gnutls_mac_algorithm_t and gnutls_digest_algorithm_t are aligned
	//So we can use the result to get the fingerprint without trouble
	typedef  gnutls_mac_algorithm_t digest_id_t;
	digest_id_t digest_get_id(const char * name){return gnutls_mac_get_id (name);}
	const char* digest_get_name(digest_id_t id){return gnutls_mac_get_name (id);}
#	define UNK_DIGEST GNUTLS_MAC_UNKNOWN

#else
	typedef  gnutls_digest_algorithm_t digest_id_t;
	digest_id_t digest_get_id(const char * name){return gnutls_digest_get_id (name);}
	const char* digest_get_name(digest_id_t id){return gnutls_digest_get_name (id);}
#	define UNK_DIGEST GNUTLS_DIG_UNKNOWN
#endif

/* Convert a fingerprint to printable data. The function must be provided a
 * sufficiently large buffer. 1024 bytes shall always do (for sha512 and future).
 * To the size of the hexadecimal fingerprint, we must add the name of the fingerprint, and the separators.
 * Warn : if the size of the buffer isn't sufficient, then the data is truncated.
 */

static void
GenFingerprintStr(const char *pFingerprint,const int sizeFingerprint,
		char * fpBuf,const size_t bufLen,
		const digest_id_t type,relpEngine_t * pEngine)
{
	int iSrc, iDst;

	size_t sizeTotal=0,sizeDigest=0;
	//statically assigned char*, no free.
	const char* digestType=digest_get_name(type);
	if (NULL==digestType)
	{
		if (pEngine!=NULL)
			pEngine->dbgprint("warn : the signature type %d is unknown\n",type);
		digestType="0000";
	}
	sizeDigest=strlen(digestType);
	//digestname + 3 char by byte (:xx) + last '\0'
	sizeTotal=sizeDigest+(sizeFingerprint*3)+1;
	if (sizeTotal <bufLen)
	{
		strncpy(fpBuf,digestType,sizeDigest);
		iDst=sizeDigest;
		for(iSrc = 0; iSrc < sizeFingerprint ; ++iSrc, iDst += 3) {
			sprintf(fpBuf+iDst, ":%2.2X", (unsigned char) pFingerprint[iSrc]);
		}
	}else if(bufLen>=1){
		if (pEngine!=NULL)
			pEngine->dbgprint("warn: buffer overflow for %s signature\n",digestType);
		fpBuf[0]='\0';//an empty string
	}else{
		if (pEngine!=NULL)
			pEngine->dbgprint("warn: buffer empty, unable to print the signature\n");
	}
}

#define MAX_DIGEST_PEER 10
static size_t ListDigestPeer(digest_id_t* listSigPeer,
		tcpPermittedPeers_t* listPeers,relpEngine_t* pEngine)
{
	int i;
	int maxDigest=0;
	//No signature name of more than 32 bytes.
	//Most take 4. Some are slightly longer (SHA3_256, ...)
	char digest[32];
	if (NULL==listPeers || listPeers->nmemb<=0 )
	{
		if (pEngine!=NULL) pEngine->dbgprint("warn: no PermittedPeer listed\n");
		return 0;
	}
	for (i=0; i<listPeers->nmemb;++i)
	{
		if ((listPeers->peer[i].name)!=NULL)
		{
			char*eow=strchr(listPeers->peer[i].name,':');//The first separator
			if (eow!=NULL)
			{
				int sizeDigest=(int)(eow-(listPeers->peer[i].name));
				//31= sizeof(digest)-1;
				sizeDigest = sizeDigest > 31 ? 31: sizeDigest;
				strncpy(digest,listPeers->peer[i].name,sizeDigest);
				digest[sizeDigest]='\0';
				digest_id_t actualDigest=digest_get_id(digest);
				if (actualDigest!=UNK_DIGEST)
				{
					int alreadyExist=0;
					int j;
					for (j=0;j<maxDigest && alreadyExist==0 && j <MAX_DIGEST_PEER;++j)
					{
						if (listSigPeer[j]==actualDigest)
							alreadyExist=1;
					}
					if (maxDigest<MAX_DIGEST_PEER && alreadyExist==0)
					{
						if (pEngine!=NULL)
							pEngine->dbgprint("DDDD: adding digest %s\n",
									digest);
						listSigPeer[maxDigest++]=actualDigest;
					}
				}
			}
		}
	}
	return maxDigest;
}
/* Check the peer's ID in fingerprint auth mode. */
static int
relpTcpChkPeerFingerprint(relpTcp_t *const pThis, gnutls_x509_crt_t cert)
{
	int r = 0;
	int i;
	char fingerprint[126];
	char fpPrintable[256];
	digest_id_t listSigPeer[MAX_DIGEST_PEER];
	size_t maxDigest,k;
	size_t size;
	int8_t found;


	/* List which digest we have in our permittedPeer list. We verify only the first MAX_DIGEST_PEER.*/
	maxDigest=ListDigestPeer(listSigPeer,&(pThis->permittedPeers),pThis->pEngine);

	/* obtain the SHA1 fingerprint */
	found = 0;
	for(k=0; k<maxDigest && found==0;++k)
	{
		digest_id_t digest=listSigPeer[k];
		size = sizeof(fingerprint);
		r = gnutls_x509_crt_get_fingerprint(cert, (gnutls_digest_algorithm_t) digest, fingerprint, &size);
		if(chkGnutlsCode(pThis, "Failed to obtain fingerprint from certificate", RELP_RET_ERR_TLS, r)) {
			r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
		}

		GenFingerprintStr(fingerprint, (int) size,
			(char*)fpPrintable,sizeof(fpPrintable),digest,pThis->pEngine);
		pThis->pEngine->dbgprint("peer's certificate %s fingerprint: %s\n",
			digest_get_name(digest), fpPrintable);

		/* now search through the permitted peers to see if we can find a permitted one */
		pThis->pEngine->dbgprint("n peers %d\n", pThis->permittedPeers.nmemb);
		for(i = 0 ; i < pThis->permittedPeers.nmemb ; ++i) {
		pThis->pEngine->dbgprint("checking peer '%s','%s'\n",
			fpPrintable, pThis->permittedPeers.peer[i].name);
			if(!strcmp(fpPrintable, pThis->permittedPeers.peer[i].name)) {
				found = 1;
				break;
			}
		}
	}
	if(!found) {
		r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
	}
done:
	if(r != 0) {
		callOnAuthErr(pThis, fpPrintable, "non-permited fingerprint", RELP_RET_AUTH_ERR_FP);
	}
	return r;
}
#endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION */

#if defined(HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION)

/* Obtain the CN from the DN field and hand it back to the caller
 * (which is responsible for destructing it). We try to follow
 * RFC2253 as far as it makes sense for our use-case. This function
 * is considered a compromise providing good-enough correctness while
 * limiting code size and complexity. If a problem occurs, we may enhance
 * this function. A (pointer to a) certificate must be caller-provided.
 * The buffer for the name (namebuf) must also be caller-provided. A
 * size of 1024 is most probably sufficien. The
 * function returns 0 if all went well, something else otherwise.
 * Note that non-0 is also returned if no CN is found.
 */
static int
relpTcpGetCN(relpTcp_t *const pThis, gnutls_x509_crt_t cert, char *namebuf, const int lenNamebuf)
{
	int r;
	int gnuRet;
	int i,j;
	int bFound;
	size_t size;
	char szDN[1024]; /* this should really be large enough for any non-malicious case... */

	size = sizeof(szDN);
	gnuRet = gnutls_x509_crt_get_dn(cert, (char*)szDN, &size);
	if(chkGnutlsCode(pThis, "Failed to obtain DN from certificate", RELP_RET_ERR_TLS, gnuRet)) {
		r = 1; goto done;
	}

	/* now search for the CN part */
	i = 0;
	bFound = 0;
	while(!bFound && szDN[i] != '\0') {
		/* note that we do not overrun our string due to boolean shortcut
		 * operations. If we have '\0', the if does not match and evaluation
		 * stops. Order of checks is obviously important!
		 */
		if(szDN[i] == 'C' && szDN[i+1] == 'N' && szDN[i+2] == '=') {
			bFound = 1;
			i += 2;
		}
		i++;

	}

	if(!bFound) {
		r = 1; goto done;
	}

	/* we found a common name, now extract it */
	j = 0;
	while(szDN[i] != '\0' && szDN[i] != ',' && j < lenNamebuf-1) {
		if(szDN[i] == '\\') {
			/* hex escapes are not implemented */
			r = 2; goto done;
		} else {
			namebuf[j++] = szDN[i];
		}
		++i; /* char processed */
	}
	namebuf[j] = '\0';

	/* we got it - we ignore the rest of the DN string (if any). So we may
	 * not detect if it contains more than one CN
	 */
	r = 0;

done:
	return r;
}


/* Check the peer's ID in name auth mode. */
static int
relpTcpChkPeerName(relpTcp_t *const pThis, gnutls_x509_crt_t cert)
{
	int ret;
	unsigned int status = 0;
	char cnBuf[1024]; /* this is sufficient for the DNSNAME... */
	char szAltName[1024]; /* this is sufficient for the DNSNAME... */
	int iAltName;
	char allNames[32*1024]; /* for error-reporting */
	int iAllNames;
	size_t szAltNameLen;
	int bFoundPositiveMatch;
	int gnuRet;
	ENTER_RELPFUNC;

	ret = gnutls_certificate_verify_peers2(pThis->session, &status);
	if(ret < 0) {
		callOnAuthErr(pThis, "", "certificate validation failed",
			RELP_RET_AUTH_CERT_INVL);
		ABORT_FINALIZE(GNUTLS_E_CERTIFICATE_ERROR);
	}
	if(status != 0) { /* Certificate is not trusted */
		callOnAuthErr(pThis, "", "certificate validation failed",
			RELP_RET_AUTH_CERT_INVL);
		ABORT_FINALIZE(GNUTLS_E_CERTIFICATE_ERROR);
	}

	bFoundPositiveMatch = 0;
	iAllNames = 0;

	/* first search through the dNSName subject alt names */
	iAltName = 0;
	while(!bFoundPositiveMatch) { /* loop broken below */
		szAltNameLen = sizeof(szAltName);
		gnuRet = gnutls_x509_crt_get_subject_alt_name(cert, iAltName,
				szAltName, &szAltNameLen, NULL);
		if(gnuRet < 0)
			break;
		else if(gnuRet == GNUTLS_SAN_DNSNAME) {
			pThis->pEngine->dbgprint("relpTcpChkPeerName: subject alt dnsName: '%s'\n", szAltName);
			if (relpTcpAddToCertNamesBuffer(pThis, allNames, sizeof(allNames),
					&iAllNames, szAltName) != 0) {
				ABORT_FINALIZE(GNUTLS_E_CERTIFICATE_ERROR);
			}
			relpTcpChkOnePeerName(pThis, szAltName, &bFoundPositiveMatch);
			/* do NOT break, because there may be multiple dNSName's! */
		}
		++iAltName;
	}

	if(!bFoundPositiveMatch) {
		/* if we did not succeed so far, we try the CN part of the DN... */
		if(relpTcpGetCN(pThis, cert, cnBuf, sizeof(cnBuf)) == 0) {
			pThis->pEngine->dbgprint("relpTcpChkPeerName: relpTcp now checking auth for CN '%s'\n", cnBuf);
			if (relpTcpAddToCertNamesBuffer(pThis, allNames, sizeof(allNames), &iAllNames, cnBuf)) {
				ABORT_FINALIZE(GNUTLS_E_CERTIFICATE_ERROR);
			}
			relpTcpChkOnePeerName(pThis, cnBuf, &bFoundPositiveMatch);
		}
	}

	if(!bFoundPositiveMatch) {
		callOnAuthErr(pThis, allNames, "no permited name found", RELP_RET_AUTH_ERR_NAME);
		ABORT_FINALIZE(GNUTLS_E_CERTIFICATE_ERROR);
	}
finalize_it:

	LEAVE_RELPFUNC;
}

/* This function will verify the peer's certificate, and check
 * if the hostname matches, as well as the activation, expiration dates.
 */
static int
relpTcpVerifyCertificateCallback(gnutls_session_t session)
{
	int r = 0;
	const gnutls_datum_t *cert_list;
	unsigned int list_size = 0;
	gnutls_x509_crt_t cert;
	int bMustDeinitCert = 0;

	relpTcp_t *const pThis = (relpTcp_t*) gnutls_session_get_ptr(session);

	/* This function only works for X.509 certificates.  */
	if(gnutls_certificate_type_get(session) != GNUTLS_CRT_X509) {
		r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
	}

	cert_list = gnutls_certificate_get_peers(pThis->session, &list_size);

	if(list_size < 1) {
		callOnAuthErr(pThis, "", "peer did not provide a certificate",
			      RELP_RET_AUTH_NO_CERT);
		r = GNUTLS_E_CERTIFICATE_ERROR; goto done;
	}

	/* If we reach this point, we have at least one valid certificate.
	 * We always use only the first certificate. As of GnuTLS documentation, the
	 * first certificate always contains the remote peer's own certificate. All other
	 * certificates are issuer's certificates (up the chain). We are only interested
	 * in the first certificate, which is our peer. -- rgerhards, 2008-05-08
	 */
	gnutls_x509_crt_init(&cert);
	bMustDeinitCert = 1; /* indicate cert is initialized and must be freed on exit */
	gnutls_x509_crt_import(cert, &cert_list[0], GNUTLS_X509_FMT_DER);
	if(pThis->authmode == eRelpAuthMode_Fingerprint) {
		r = relpTcpChkPeerFingerprint(pThis, cert);
	} else {
		r = relpTcpChkPeerName(pThis, cert);
	}
	if(r != 0) goto done;

	/* notify gnutls to continue handshake normally */
	r = 0;

done:
	if(bMustDeinitCert)
		gnutls_x509_crt_deinit(cert);
	return r;
}
#endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION */

#if 0 /* enable if needed for debugging */
static void logFunction(int level, const char *msg)
{
	fprintf(stdout, "DDDD: GnuTLS log msg, level %d: %s", level, msg);
	fflush(stdout);
}
#endif

/* initialize the listener for TLS use */
static relpRetVal
relpTcpLstnInitTLS(relpTcp_t *const pThis)
{
	int r;
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	#if GNUTLS_VERSION_NUMBER <= 0x020b00
	/* gcry_control must be called first, so that the thread system is correctly set up */
	gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
	#endif
	gnutls_global_init();
	/* uncomment for (very intense) debug help
	 * gnutls_global_set_log_function(logFunction);
	 * gnutls_global_set_log_level(10); // 0 (no) to 9 (most), 10 everything
	 */

	if(isAnonAuth(pThis)) {
		r = gnutls_dh_params_init(&pThis->dh_params);
		if(chkGnutlsCode(pThis, "Failed to initialize dh_params", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		r = gnutls_dh_params_generate2(pThis->dh_params, pThis->dhBits);
		if(chkGnutlsCode(pThis, "Failed to generate dh_params", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		r = gnutls_anon_allocate_server_credentials(&pThis->anoncredSrv);
		if(chkGnutlsCode(pThis, "Failed to allocate server credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		gnutls_anon_set_server_dh_params(pThis->anoncredSrv, pThis->dh_params);
	} else {
#		ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION
		r = gnutls_certificate_allocate_credentials(&pThis->xcred);
		if(chkGnutlsCode(pThis, "Failed to allocate certificate credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->caCertFile != NULL) {
			r = gnutls_certificate_set_x509_trust_file(pThis->xcred,
				pThis->caCertFile, GNUTLS_X509_FMT_PEM);
			if(r < 0) {
				chkGnutlsCode(pThis, "Failed to set certificate trust files",
								RELP_RET_ERR_TLS_SETUP, r);
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
			pThis->pEngine->dbgprint("librelp: obtained %d certificates from %s\n", r, pThis->caCertFile);
		}
		r = gnutls_certificate_set_x509_key_file (pThis->xcred,
			pThis->ownCertFile, pThis->privKeyFile, GNUTLS_X509_FMT_PEM);
		if(chkGnutlsCode(pThis, "Failed to set certificate key files", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->authmode == eRelpAuthMode_None)
			pThis->authmode = eRelpAuthMode_Fingerprint;
		gnutls_certificate_set_verify_function(pThis->xcred, relpTcpVerifyCertificateCallback);
#		else /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
		ABORT_FINALIZE(RELP_RET_ERR_NO_TLS_AUTH);
#		endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
	}
finalize_it:
	LEAVE_RELPFUNC;
}
// !!!!!!!!!!! #endif /* #if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL) */
#endif /* defined(ENABLE_TLS)*/


/* add a wildcard entry to this permitted peer. Entries are always
 * added at the tail of the list. pszStr and lenStr identify the wildcard
 * entry to be added. Note that the string is NOT \0 terminated, so
 * we must rely on lenStr for when it is finished.
 * rgerhards, 2008-05-27
 */
static relpRetVal
AddPermittedPeerWildcard(tcpPermittedPeerEntry_t *pEtry, char* pszStr, const int lenStr)
{
	tcpPermittedPeerWildcardComp_t *pNew = NULL;
	int iSrc;
	int iDst;
	ENTER_RELPFUNC;

	if((pNew = calloc(1, sizeof(tcpPermittedPeerWildcardComp_t))) == NULL) {
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	if(lenStr == 0) {
		pNew->wildcardType = tcpPEER_WILDCARD_EMPTY_COMPONENT;
		FINALIZE;
	} else {
		/* alloc memory for the domain component. We may waste a byte or
		 * two, but that's ok.
		 */
		if((pNew->pszDomainPart = malloc(lenStr +1 )) == NULL) {
			ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
		}
	}

	if(pszStr[0] == '*') {
		pNew->wildcardType = tcpPEER_WILDCARD_AT_START;
		iSrc = 1; /* skip '*' */
	} else {
		iSrc = 0;
	}

	for(iDst = 0 ; iSrc < lenStr && pszStr[iSrc] != '*' ; ++iSrc, ++iDst)  {
		pNew->pszDomainPart[iDst] = pszStr[iSrc];
	}

	if(iSrc < lenStr) {
		if(iSrc + 1 == lenStr && pszStr[iSrc] == '*') {
			if(pNew->wildcardType == tcpPEER_WILDCARD_AT_START) {
				ABORT_FINALIZE(RELP_RET_INVLD_WILDCARD);
			} else {
				pNew->wildcardType = tcpPEER_WILDCARD_AT_END;
			}
		} else {
			/* we have an invalid wildcard, something follows the asterisk! */
			ABORT_FINALIZE(RELP_RET_INVLD_WILDCARD);
		}
	}

	if(lenStr == 1 && pNew->wildcardType == tcpPEER_WILDCARD_AT_START) {
		pNew->wildcardType = tcpPEER_WILDCARD_MATCH_ALL;
	}

	/* if we reach this point, we had a valid wildcard. We now need to
	 * properly terminate the domain component string.
	 */
	pNew->pszDomainPart[iDst] = '\0';
	pNew->lenDomainPart = (int16_t) strlen((char*)pNew->pszDomainPart);

finalize_it:
	if(iRet != RELP_RET_OK) {
		if(pNew != NULL) {
			if(pNew->pszDomainPart != NULL)
				free(pNew->pszDomainPart);
			free(pNew);
		}
	} else {
		/* add the element to linked list */
		if(pEtry->wildcardRoot == NULL) {
			pEtry->wildcardRoot = pNew;
			pEtry->wildcardLast = pNew;
		} else {
			pEtry->wildcardLast->pNext = pNew;
		}
		pEtry->wildcardLast = pNew;
	}
	LEAVE_RELPFUNC;
}

#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
/* Compile a wildcard - must not yet be compiled */
static relpRetVal
relpTcpPermittedPeerWildcardCompile(tcpPermittedPeerEntry_t *pEtry)
{
	char *pC;
	char *pStart;
	ENTER_RELPFUNC;

	/* first check if we have a wildcard */
	for(pC = pEtry->name ; *pC != '\0' && *pC != '*' ; ++pC)
		/*EMPTY, just skip*/;

	if(*pC == '\0') { /* no wildcard found, we are done */
		FINALIZE;
	}

	/* if we reach this point, the string contains wildcards. So let's
	 * compile the structure. To do so, we must parse from dot to dot
	 * and create a wildcard entry for each domain component we find.
	 * We must also flag problems if we have an asterisk in the middle
	 * of the text (it is supported at the start or end only).
	 */
	pC = pEtry->name;
	while(*pC) {
		pStart = pC;
		/* find end of domain component */
		for( ; *pC != '\0' && *pC != '.' ; ++pC)
			/*EMPTY, just skip*/;
		CHKRet(AddPermittedPeerWildcard(pEtry, pStart, pC - pStart));
		/* now check if we have an empty component at end of string */
		if(*pC == '.' && *(pC + 1) == '\0') {
			/* pStart is a dummy, it is not used if length is 0 */
			CHKRet(AddPermittedPeerWildcard(pEtry, pStart, 0));
		}
		if(*pC != '\0')
			++pC;
	}

finalize_it:
	LEAVE_RELPFUNC;
}

#endif /* defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL) */


/* initialize the tcp socket for a listner
 * pLstnPort is a pointer to a port name. NULL is not permitted.
 * gerhards, 2008-03-17
 * pLstnAddr is a pointer to a bind address. NULL is permitted.
 * perlei, 2018-04-19
 */
relpRetVal
relpTcpLstnInit(relpTcp_t *const pThis, unsigned char *pLstnPort, unsigned char *pLstnAddr, int ai_family)
{
	struct addrinfo hints, *res = NULL, *r;
	int error, maxs, *s, on = 1;
	int sockflags;
	unsigned char *pLstnPt;

	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	pLstnPt = pLstnPort;
	assert(pLstnPt != NULL);

	pThis->pEngine->dbgprint("creating relp tcp listen socket on port %s\n", pLstnPt);

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = ai_family;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo((char*)pLstnAddr, (char*) pLstnPt, &hints, &res);
	if(error) {
		pThis->pEngine->dbgprint("error %d querying port '%s'\n", error, pLstnPt);
		ABORT_FINALIZE(RELP_RET_INVALID_PORT);
	}

	/* Count max number of sockets we may open */
	for(maxs = 0, r = res; r != NULL ; r = r->ai_next, maxs++)
		/* EMPTY */;
	pThis->socks = malloc((maxs+1) * sizeof(int));
	if (pThis->socks == NULL) {
		pThis->pEngine->dbgprint("couldn't allocate memory for TCP listen sockets, "
			"suspending RELP message reception.\n");
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	*pThis->socks = 0;   /* num of sockets counter at start of array */
	s = pThis->socks + 1;
	for(r = res; r != NULL ; r = r->ai_next) {
		*s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (*s < 0) {
			if(!(r->ai_family == PF_INET6 && errno == EAFNOSUPPORT))
				pThis->pEngine->dbgprint("creating relp tcp listen socket\n");
				/* it is debatable if PF_INET with EAFNOSUPPORT should
				 * also be ignored...
				 */
			continue;
		}

#ifdef IPV6_V6ONLY
		if (r->ai_family == AF_INET6) {
			int iOn = 1;
			if (setsockopt(*s, IPPROTO_IPV6, IPV6_V6ONLY,
				(char *)&iOn, sizeof (iOn)) < 0) {
				close(*s);
				*s = -1;
				continue;
			}
		}
#endif
		if(setsockopt(*s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0 ) {
			pThis->pEngine->dbgprint("error %d setting relp/tcp socket option\n", errno);
			close(*s);
			*s = -1;
			continue;
		}

		/* We use non-blocking IO! */
		if((sockflags = fcntl(*s, F_GETFL)) != -1) {
			sockflags |= O_NONBLOCK;
			/* SETFL could fail too, so get it caught by the subsequent
			 * error check.
			 */
			sockflags = fcntl(*s, F_SETFL, sockflags);
		}
		if(sockflags == -1) {
			pThis->pEngine->dbgprint("error %d setting fcntl(O_NONBLOCK) on relp socket\n", errno);
			close(*s);
			*s = -1;
			continue;
		}

#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
		if(pThis->bEnableTLS) {
			CHKRet(relpTcpLstnInitTLS(pThis));
		}
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL*/

	        if( (bind(*s, r->ai_addr, r->ai_addrlen) < 0)
#ifndef IPV6_V6ONLY
		     && (errno != EADDRINUSE)
#endif
	           ) {
			char msgbuf[4096];
			snprintf(msgbuf, sizeof(msgbuf), "error while binding relp tcp socket "
				 "on port '%s'", pLstnPort);
			msgbuf[sizeof(msgbuf)-1] = '\0';
			callOnErr(pThis, msgbuf, errno);
			close(*s);
			*s = -1;
			continue;
		}

		if(listen(*s,pThis->iSessMax / 10 + 5) < 0) {
			/* If the listen fails, it most probably fails because we ask
			 * for a too-large backlog. So in this case we first set back
			 * to a fixed, reasonable, limit that should work. Only if
			 * that fails, too, we give up.
			 */
			pThis->pEngine->dbgprint("listen with a backlog of %d failed - retrying with default of 32.\n",
				pThis->iSessMax / 10 + 5);
			if(listen(*s, 32) < 0) {
				pThis->pEngine->dbgprint("relp listen error %d, suspending\n", errno);
	                	close(*s);
				*s = -1;
				continue;
			}
		}

		(*pThis->socks)++;
		s++;
	}

	if(*pThis->socks != maxs)
		pThis->pEngine->dbgprint("We could initialize %d RELP TCP listen sockets out of %d we received "
		 	"- this may or may not be an error indication.\n", *pThis->socks, maxs);

	if(*pThis->socks == 0) {
		pThis->pEngine->dbgprint("No RELP TCP listen socket could successfully be initialized, "
			 "message reception via RELP disabled.\n");
		/* free(pThis->socks);
		*	DONE IN relpTcpDestruct!
		*/
		ABORT_FINALIZE(RELP_RET_COULD_NOT_BIND);
	}

finalize_it:
	if(res != NULL)
		freeaddrinfo(res);

	LEAVE_RELPFUNC;
}

/* receive data from a tcp socket
 * The lenBuf parameter must contain the max buffer size on entry and contains
 * the number of octets read (or -1 in case of error) on exit. This function
 * never blocks, not even when called on a blocking socket. That is important
 * for client sockets, which are set to block during send, but should not
 * block when trying to read data. If *pLenBuf is -1, an error occured and
 * errno holds the exact error cause.
 * rgerhards, 2008-03-17
 */
relpRetVal
relpTcpRcv(relpTcp_t *const pThis, relpOctet_t *pRcvBuf, ssize_t *pLenBuf)
{
	ENTER_RELPFUNC;
	int lenRcvd;
#if defined(ENABLE_TLS_OPENSSL)
	int err;
#endif
	RELPOBJ_assert(pThis, Tcp);

#if defined(ENABLE_TLS)
	if(pThis->bEnableTLS) {
		lenRcvd = gnutls_record_recv(pThis->session, pRcvBuf, *pLenBuf);
		if(lenRcvd == GNUTLS_E_INTERRUPTED || lenRcvd == GNUTLS_E_AGAIN) {
			pThis->pEngine->dbgprint("librelp: gnutls_record_recv must be retried\n");
			pThis->rtryOp = relpTCP_RETRY_recv;
		} else {
			if(lenRcvd < 0)
				chkGnutlsCode(pThis, "TLS record reception failed", RELP_RET_IO_ERR, lenRcvd);
			pThis->rtryOp = relpTCP_RETRY_none;
		}
		*pLenBuf = (lenRcvd < 0) ? -1 : lenRcvd;
	} else {
#elif defined(ENABLE_TLS_OPENSSL)
	if(pThis->bEnableTLS) {
		lenRcvd = SSL_read(pThis->ssl, pRcvBuf, *pLenBuf);
		if(lenRcvd > 0) {
			pThis->pEngine->dbgprint("relpTcpRcv: SSL_read SUCCESS\n");
			*pLenBuf = lenRcvd;
		} else {
			*pLenBuf = -1;

			err = SSL_get_error(pThis->ssl, lenRcvd);
			if(	err == SSL_ERROR_ZERO_RETURN ) {
				pThis->pEngine->dbgprint("relpTcpRcv: SSL_ERROR_ZERO_RETURN received, "
					"connection may closed already\n");
				pThis->rtryOp = relpTCP_RETRY_none;
				ABORT_FINALIZE(RELP_RET_IO_ERR);
			}
			else if(err != SSL_ERROR_WANT_READ &&
				err != SSL_ERROR_WANT_WRITE) {
				/* Output error and abort */
				relpTcpLastSSLErrorMsg(lenRcvd, pThis, "relpTcpRcv");
				pThis->rtryOp = relpTCP_RETRY_none;
				ABORT_FINALIZE(RELP_RET_IO_ERR);
			} else {
				pThis->pEngine->dbgprint("relpTcpRcv: SSL_get_error = %d, setting RETRY \n", err);
				pThis->rtryOp =  relpTCP_RETRY_recv;
			}
		}
	} else {
#endif /* #ifdef ENABLE_TLS */
		*pLenBuf = lenRcvd = recv(pThis->sock, pRcvBuf, *pLenBuf, MSG_DONTWAIT);
		pThis->pEngine->dbgprint("relpTcpRcv: read %zd bytes from sock %d\n",
			*pLenBuf, pThis->sock);
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	}

finalize_it:
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL*/
	pThis->pEngine->dbgprint("relpTcpRcv return. relptcp [%p], iRet %d, lenRcvd %d, pLenBuf %zd\n",
		(void *) pThis, iRet, lenRcvd, *pLenBuf);
	LEAVE_RELPFUNC;
}


/* helper for CORK option manipulation. As this is not portable, the
 * helper abstracts it. Note that it is a null-operation if no
 * such option is available on the platform in question.
 */
static void
setCORKopt(const relpTcp_t *const pThis, const int onOff)
{
#if defined(TCP_CORK)
	if(setsockopt(pThis->sock, SOL_TCP, TCP_CORK, &onOff, sizeof (onOff)) == -1) {
		pThis->pEngine->dbgprint("relpTcp: setsockopt() TCP_CORK failed\n");
	}
#elif defined(TCP_NOPUSH)
	if(setsockopt(pThis->sock, IPPROTO_TCP, TCP_NOPUSH, &onOff, sizeof (onOff)) == -1) {
		pThis->pEngine->dbgprint("relpTcp: setsockopt() TCP_NOPUSH failed\n");
	}
#endif
}
/* this function is called to hint librelp that a "burst" of data is to be
 * sent. librelp can than try to optimize it's handling. Right now, this
 * means we turn on the CORK option and will turn it off when we are
 * hinted that the burst is over.
 * The function is intentionally void as it must operate in a way that
 * does not interfere with normal operations.
 */
void
relpTcpHintBurstBegin(relpTcp_t *const pThis)
{
	setCORKopt(pThis, 1);
}
/* this is the counterpart to relpTcpHintBurstBegin -- see there for doc */
void
relpTcpHintBurstEnd(relpTcp_t *const pThis)
{
	setCORKopt(pThis, 0);
}

/* send a buffer via TCP.
 * On entry, pLenBuf contains the number of octets to
 * write. On exit, it contains the number of octets actually written.
 * If this number is lower than on entry, only a partial buffer has
 * been written.
 * rgerhards, 2008-03-19
 */
relpRetVal
relpTcpSend(relpTcp_t *const pThis, relpOctet_t *pBuf, ssize_t *pLenBuf)
{
	ssize_t written;
#if defined(ENABLE_TLS_OPENSSL)
	int err;
#endif
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

#if defined(ENABLE_TLS)
	if(pThis->bEnableTLS) {
		written = gnutls_record_send(pThis->session, pBuf, *pLenBuf);
		pThis->pEngine->dbgprint("relpTcpSend: TLS send returned %d\n", (int) written);
		if(written == GNUTLS_E_AGAIN || written == GNUTLS_E_INTERRUPTED) {
			pThis->rtryOp = relpTCP_RETRY_send;
			written = 0;
		} else {
			pThis->rtryOp = relpTCP_RETRY_none;
			if(written < 1) {
				chkGnutlsCode(pThis, "TLS record write failed", RELP_RET_IO_ERR, written);
				ABORT_FINALIZE(RELP_RET_IO_ERR);
			}
		}
	} else {
#elif defined(ENABLE_TLS_OPENSSL)
	if(pThis->bEnableTLS) {
		written = SSL_write(pThis->ssl, pBuf, *pLenBuf);
		if(written > 0) {
			pThis->pEngine->dbgprint("relpTcpSend: SSL_write SUCCESS\n");
		} else {
			err = SSL_get_error(pThis->ssl, written);
			if(	err == SSL_ERROR_ZERO_RETURN ) {
				pThis->pEngine->dbgprint("relpTcpSend: SSL_ERROR_ZERO_RETURN received, "
					"retry next time\n");
				pThis->rtryOp = relpTCP_RETRY_send;
				written = 0;
			}
			else if(err != SSL_ERROR_WANT_READ &&
				err != SSL_ERROR_WANT_WRITE) {
				/* Output error and abort */
				relpTcpLastSSLErrorMsg( (int)written, pThis, "relpTcpSend");
				ABORT_FINALIZE(RELP_RET_IO_ERR);
			} else {
				/* Check for SSL Shutdown */
				if (SSL_get_shutdown(pThis->ssl) == SSL_RECEIVED_SHUTDOWN) {
					pThis->pEngine->dbgprint("relpTcpSend: received SSL_RECEIVED_SHUTDOWN!\n");
					ABORT_FINALIZE(RELP_RET_IO_ERR);
				}
			}
		}
	} else {
#endif /* defined(ENABLE_TLS) | defined(ENABLE_TLS_OPENSSL */
		written = send(pThis->sock, pBuf, *pLenBuf, 0);
		const int errno_save = errno;
		pThis->pEngine->dbgprint("relpTcpSend: sock %d, lenbuf %zd, send returned %d [errno %d]\n",
			(int)pThis->sock, *pLenBuf, (int) written, errno_save);
		if(written == -1) {
			switch(errno_save) {
				case EAGAIN:
				case EINTR:
					/* this is fine, just retry... */
					written = 0;
					break;
				default:
					ABORT_FINALIZE(RELP_RET_IO_ERR);
					break;
			}
		}
#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	}
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL*/

	*pLenBuf = written;
finalize_it:
	LEAVE_RELPFUNC;
}

#ifdef ENABLE_TLS
#ifndef _AIX
#pragma GCC diagnostic push /* we need to disable a warning below */
/* per https://lists.gnupg.org/pipermail/gnutls-help/2004-August/000154.html This is expected */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif
/* this is only called for client-initiated sessions */
static relpRetVal
relpTcpConnectTLSInit(relpTcp_t *const pThis)
{
	int r;
	int sockflags;
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pThis->pEngine->dbgprint("relpTcpConnectTLSInit gnutls\n");

	/* We expect a non blocking socket to establish a tls session */
	if((sockflags = fcntl(pThis->sock, F_GETFL)) != -1) {
		sockflags &= ~O_NONBLOCK;
		sockflags = fcntl(pThis->sock, F_SETFL, sockflags);
	}

	if(sockflags == -1) {
		pThis->pEngine->dbgprint("error %d unsetting fcntl(O_NONBLOCK) on relp socket", errno);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	if(!called_gnutls_global_init) {
		#if GNUTLS_VERSION_NUMBER <= 0x020b00
		/* gcry_control must be called first, so that the thread system is correctly set up */
		gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
		#endif
		gnutls_global_init();
		/* uncomment for (very intense) debug help
		 * gnutls_global_set_log_function(logFunction);
		 * gnutls_global_set_log_level(10); // 0 (no) to 9 (most), 10 everything
		 */
		pThis->pEngine->dbgprint("DDDD: gnutls_global_init() called\n");
		called_gnutls_global_init = 1;
	}
	r = gnutls_init(&pThis->session, GNUTLS_CLIENT);
	if(chkGnutlsCode(pThis, "Failed to initialize GnuTLS", RELP_RET_ERR_TLS_SETUP, r)) {
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}

	gnutls_session_set_ptr(pThis->session, pThis);
	CHKRet(relpTcpTLSSetPrio(pThis));

	if(isAnonAuth(pThis)) {
		r = gnutls_anon_allocate_client_credentials(&pThis->anoncred);
		if(chkGnutlsCode(pThis, "Failed to allocate client credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		/* put the anonymous credentials to the current session */
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_ANON, pThis->anoncred);
		if(chkGnutlsCode(pThis, "Failed to set credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	} else {
#ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION
		r = gnutls_certificate_allocate_credentials(&pThis->xcred);
		if(chkGnutlsCode(pThis, "Failed to allocate certificate credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->caCertFile != NULL) {
			r = gnutls_certificate_set_x509_trust_file(pThis->xcred,
				pThis->caCertFile, GNUTLS_X509_FMT_PEM);
			if(r < 0) {
				chkGnutlsCode(pThis, "Failed to set certificate trust file", RELP_RET_ERR_TLS_SETUP, r);
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
			pThis->pEngine->dbgprint("librelp: obtained %d certificates from %s\n", r, pThis->caCertFile);
		}
		if(pThis->ownCertFile != NULL) {
			r = gnutls_certificate_set_x509_key_file (pThis->xcred,
				pThis->ownCertFile, pThis->privKeyFile, GNUTLS_X509_FMT_PEM);
			if(chkGnutlsCode(pThis, "Failed to set certificate key file", RELP_RET_ERR_TLS_SETUP, r)) {
				ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
			}
		}
		r = gnutls_credentials_set(pThis->session, GNUTLS_CRD_CERTIFICATE, pThis->xcred);
		if(chkGnutlsCode(pThis, "Failed to set credentials", RELP_RET_ERR_TLS_SETUP, r)) {
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
		if(pThis->authmode == eRelpAuthMode_None)
			pThis->authmode = eRelpAuthMode_Fingerprint;
		gnutls_certificate_set_verify_function(pThis->xcred, relpTcpVerifyCertificateCallback);
#		else /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
		ABORT_FINALIZE(RELP_RET_ERR_NO_TLS_AUTH);
#		endif /* #ifdef HAVE_GNUTLS_CERTIFICATE_SET_VERIFY_FUNCTION   */
	}

	gnutls_transport_set_ptr(pThis->session, (gnutls_transport_ptr_t) pThis->sock);
	//gnutls_handshake_set_timeout(pThis->session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

	/* Perform the TLS handshake */
	do {
		r = gnutls_handshake(pThis->session);
		pThis->pEngine->dbgprint("DDDD: gnutls_handshake: %d: %s\n", r, gnutls_strerror(r));
		if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
			pThis->pEngine->dbgprint("librelp: gnutls_handshake must be retried\n");
			pThis->rtryOp = relpTCP_RETRY_handshake;
		} else if(r != GNUTLS_E_SUCCESS) {
			chkGnutlsCode(pThis, "TLS handshake failed", RELP_RET_ERR_TLS_SETUP, r);
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	}
	while(0);
	//while (r < 0 && gnutls_error_is_fatal(r) == 0);

	/* set the socket to non-blocking IO (we do this on the recv() for non-TLS */
	if((sockflags = fcntl(pThis->sock, F_GETFL)) != -1) {
		sockflags |= O_NONBLOCK;
		/* SETFL could fail too, so get it caught by the subsequent
		 * error check.  */
		if(fcntl(pThis->sock, F_SETFL, sockflags) == -1) {
			callOnErr(pThis, "error setting socket to non-blocking",
				RELP_RET_ERR_TLS_SETUP);
			ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
		}
	}
finalize_it:
	LEAVE_RELPFUNC;
}
#ifndef _AIX
#pragma GCC diagnostic pop
#endif
#endif /* #ifdef ENABLE_TLS */

/* open a connection to a remote host (server).
 * This is only use for client initiated connections.
 * rgerhards, 2008-03-19
 */
relpRetVal
relpTcpConnect(relpTcp_t *const pThis,
	const int family,
	unsigned char *port,
	unsigned char *host,
	unsigned char *clientIP)
{
	struct addrinfo *res = NULL;
	struct addrinfo hints;
	struct addrinfo *reslocal = NULL;
	struct pollfd pfd;
	char errmsg[1024];
	int so_error;
	socklen_t len = sizeof so_error;
	int r;

	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	assert(port != NULL);
	assert(host != NULL);
	assert(pThis->sock == -1);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo((char*)host, (char*)port, &hints, &res) != 0) {
		pThis->pEngine->dbgprint("error %d in getaddrinfo\n", errno);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	if((pThis->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

	if(clientIP != NULL) {
		if(getaddrinfo((char*)clientIP, (char*)NULL, &hints, &reslocal) != 0) {
			pThis->pEngine->dbgprint("error %d in getaddrinfo of clientIP\n", errno);
			ABORT_FINALIZE(RELP_RET_IO_ERR);
		}
		if(bind(pThis->sock, reslocal->ai_addr, reslocal->ai_addrlen) != 0) {
			ABORT_FINALIZE(RELP_RET_IO_ERR);
		}
	}

	if(fcntl(pThis->sock, F_SETFL, O_NONBLOCK) == -1) {
		callOnErr(pThis, "error setting socket to non-blocking",
			RELP_RET_IO_ERR);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}
	if(connect(pThis->sock, res->ai_addr, res->ai_addrlen) == -1) {
		if(errno != EINPROGRESS) {
			char errStr[1024];
			_relpEngine_strerror_r(errno, errStr, sizeof(errStr));
			snprintf(errmsg, sizeof(errmsg), "error connecting: '%s'", errStr);
			callOnErr(pThis, errmsg, RELP_RET_IO_ERR);
			ABORT_FINALIZE(RELP_RET_IO_ERR);
		}
	}

	pfd.fd = pThis->sock;
	pfd.events = POLLOUT;

	if (poll(&pfd, 1, pThis->connTimeout * 1000) != 1) {
		pThis->pEngine->dbgprint("connection timed out after %d seconds\n", pThis->connTimeout);
		ABORT_FINALIZE(RELP_RET_TIMED_OUT);
	}

	r = getsockopt(pThis->sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
	if (r == -1 || so_error != 0) {
		pThis->pEngine->dbgprint("socket has an error %d\n", so_error);
		ABORT_FINALIZE(RELP_RET_IO_ERR);
	}

#if defined(ENABLE_TLS) || defined(ENABLE_TLS_OPENSSL)
	if(pThis->bEnableTLS) {
		CHKRet(relpTcpConnectTLSInit(pThis));
		pThis->bTLSActive = 1;
	}
#endif /* #ifdef ENABLE_TLS | ENABLE_TLS_OPENSSL*/

finalize_it:
	if(res != NULL)
		freeaddrinfo(res);
	if(reslocal != NULL)
		freeaddrinfo(reslocal);

	if(iRet != RELP_RET_OK) {
		if(pThis->sock != -1) {
			close(pThis->sock);
			pThis->sock = -1;
		}
	}

	LEAVE_RELPFUNC;
}

#ifdef ENABLE_TLS
/* return direction in which retry must be done. We return the original
 * gnutls code, which means:
 * "0 if trying to read data, 1 if trying to write data."
 */
int
relpTcpGetRtryDirection(relpTcp_t *const pThis)
{
	return gnutls_record_get_direction(pThis->session);
}

relpRetVal
relpTcpRtryHandshake(relpTcp_t *const pThis)
{
	int r;
	ENTER_RELPFUNC;
	r = gnutls_handshake(pThis->session);
	if(r < 0) {
		pThis->pEngine->dbgprint("librelp: state %d during retry handshake: %s\n", r, gnutls_strerror(r));
	}
	if(r == GNUTLS_E_INTERRUPTED || r == GNUTLS_E_AGAIN) {
		; /* nothing to do, just keep our status... */
	} else if(r == 0) {
		pThis->rtryOp = relpTCP_RETRY_none;
	} else {
		chkGnutlsCode(pThis, "TLS handshake failed", RELP_RET_ERR_TLS_SETUP, r);
		ABORT_FINALIZE(RELP_RET_ERR_TLS_SETUP);
	}

finalize_it:
	LEAVE_RELPFUNC;
}
#endif /* #ifdef ENABLE_TLS */


/* wait until a socket is writable again. This is primarily for use in client cases.
 * It does NOT take care of our regular event loop, and must not be used by parts
 * of the code that are driven by this loop. Returns 0 if a timeout occured and 1
 * otherwise.
 */
int
relpTcpWaitWriteable(relpTcp_t *const pThis, struct timespec *const tTimeout)
{
	int r;
	struct timespec tCurr; /* current time */
	struct pollfd pfd;

	clock_gettime(CLOCK_REALTIME, &tCurr);
	const int timeout =   (tTimeout->tv_sec - tCurr.tv_sec) * 1000
			    + (tTimeout->tv_nsec - tCurr.tv_nsec) / 1000000000;
	if(timeout < 0) {
		r = 0; goto done;
	}

	pThis->pEngine->dbgprint("librelp: telpTcpWaitWritable doing poll() "
		"on fd %d, timoeut %d\n", pThis->sock, timeout);

	pfd.fd = pThis->sock;
	pfd.events = POLLOUT;
	r = poll(&pfd, 1, timeout);
done:	return r;
}
