/////////////////////////////////////////////////////////////////////////////
//
// @file ddci.cpp @brief Digital Devices Common Interface plugin for VDR.
//
// Copyright (c) 2013 - 2014 by Jasmin Jessich.  All Rights Reserved.
//
// Contributor(s):
//
// License: GPLv2
//
// This file is part of vdr_plugin_ddci2.
//
// vdr_plugin_ddci2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// vdr_plugin_ddci2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with vdr_plugin_ddci2.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////////

#include "ddci2.h"
#include "ddciadapter.h"
#include "logging.h"

#include <vdr/plugin.h>
#include <vdr/device.h>

#include <getopt.h>
#include <string.h>

static const char *VERSION = "1.0.5";
static const char *DESCRIPTION = "External Digital Devices CI-Adapter";

static const char *DEV_DVB_CIDEVS[] = { "ci", "sec" };
static const int DEV_DVB_CIDEVS_NUM = ARRAY_SIZE(DEV_DVB_CIDEVS);

static const int SLEEP_TMO_DEF = 100;    // in ms
static const int SLEEP_TMO_MAX = 1000;

static const int BUF_NUM_DEF = 1500;
static const int BUF_NUM_MAX = 10000;

int LogLevel;
int LogDbgMask;
int cfgIgnAct;      // 1: active flag in DdCiCamSlot is ignored
int cfgBufSz;       // in 188 byte packages
int cfgClrSct;
int cfgSleepTmo;    // in ms

// the struct to be used in the vecter of found DD CI devices
class DdCiName
{
public:
	int adapter;     //< .../adapterX number
	int ci;          //< .../......../ciY ci number
	cString ciName;  //< CI device name (see DEV_DVB_CIDEVS)

	DdCiName( int Adapter, int Ci, const char *CiName );

	cString CiName( void ) const;
	cString FullCiName( void ) const;
	cString CaName( void ) const;
	cString FullCaName( void ) const;

	// required for DdCiNameList::Sort
	static int Compare(const void *a, const void *b);
};

class DdCiNameList : public cVector<DdCiName *> {
public:
	DdCiNameList( int Allocated = 10 ): cVector<DdCiName *>(Allocated) {}
	virtual ~DdCiNameList();
	void Sort()	{ cVector<DdCiName *>::Sort(DdCiName::Compare); }
	virtual void Clear(void);
	void Print(void);
};

/**
 * This class implements the interface to the CAM device.
 */
class PluginDdci: public cPlugin
{
private:
	DdCiAdapter *adapters[ MAXDEVICES ];
	DdCiNameList dd_ci_names;

	void Cleanup();

	/* add the given file to dd_ci_names, if it is a ci device */
	bool AddDdCiDev( struct dirent *f, int adapter, const char *adapter_name, const char *ci_name );
	/* add all found ci devices with names from DEV_DVB_CIDEVS to dd_ci_names */
	bool AddDdCiDevs( struct dirent *f, int adapter, const char *adapter_name );
	bool FindDdCi();  // fill dd_ci_names; returns TRUE, if one or more ci devices are found
	DdCiName *GetDdCi( void ); // get next ci device from dd_ci_names (caller needs to free the pointer!)

public:
	PluginDdci();
	virtual ~PluginDdci();

	/* see file plugin.h in the VDR include directory for the description of
	 * the following functions
	 */
	virtual const char *Version();
	virtual const char *Description();
	virtual const char *CommandLineHelp();
	virtual bool ProcessArgs( int argc, char *argv[] );

	// virtual bool Initialize();  	  // currently not used
	virtual bool Start();
	virtual void Stop();
	// virtual void Housekeeping();   // currently not used
	// virtual void MainThreadHook(); // currently not used
};

//------------------------------------------------------------------------

static inline bool DirentIsName( struct dirent *d, const char *name )
{
	return strstr( d->d_name, name ) == d->d_name;
}

//------------------------------------------------------------------------

static inline int DirentGetNameNum( struct dirent *d, int offset )
{
	return strtol( d->d_name + offset, NULL, 10 );
}

//------------------------------------------------------------------------

static inline cString CxDevName( const char *name, int adapter, int ci )
{
	return cString::sprintf( "%s/%s%d/%s%d", DEV_DVB_BASE, DEV_DVB_ADAPTER, adapter, name, ci );
}

//------------------------------------------------------------------------

static inline cString CxName( const char *name, int ci )
{
	return cString::sprintf( "%s%d", name, ci );
}

//------------------------------------------------------------------------

static int CxDevOpen( const char *name, int mode )
{
	LOG_FUNCTION_ENTER;

	int fd = open( name, mode );
	if (fd < 0)
		L_ERR_LINE( "Couldn't open %s with mode 0x%x: %m", name, mode );

	LOG_FUNCTION_EXIT;

	return fd;
}

//------------------------------------------------------------------------

DdCiNameList::~DdCiNameList()
{
	Clear();
}

//------------------------------------------------------------------------

void DdCiNameList::Clear(void)
{
	// first delete all stored objects
	for (int i = 0; i < Size(); i++)
		delete At( i );

	// and now clear them all
	cVector<DdCiName *>::Clear();
}

//------------------------------------------------------------------------

void DdCiNameList::Print(void)
{
	for (int i = 0; i < Size(); i++) {
		DdCiName *dd_ci_name = At( i );

		L_DBG_M( LDM_D, "DdCiNameList [%d]:'%s'", i, *dd_ci_name->FullCiName() );
	}
}

//------------------------------------------------------------------------

DdCiName::DdCiName( int Adapter, int Ci, const char *CiName )
: adapter( Adapter )
, ci( Ci )
, ciName( CiName )
{
}

//------------------------------------------------------------------------

cString DdCiName::CiName( void ) const
{
	return ::CxName( ciName, ci );
}

//------------------------------------------------------------------------

cString DdCiName::FullCiName( void ) const
{
	return CxDevName( ciName, adapter, ci );
}

//------------------------------------------------------------------------

cString DdCiName::CaName( void ) const
{
	return ::CxName( DEV_DVB_CA, ci );
}

//------------------------------------------------------------------------

cString DdCiName::FullCaName( void ) const
{
	return CxDevName( DEV_DVB_CA, adapter, ci );
}

//------------------------------------------------------------------------

int DdCiName::Compare(const void *a, const void *b)
{
	const DdCiName *ddci_a = *(static_cast< const DdCiName * const *>(a));
	const DdCiName *ddci_b = *(static_cast< const DdCiName * const *>(b));

	return strcmp( ddci_a->FullCiName(), ddci_b->FullCiName());
}

//------------------------------------------------------------------------

void PluginDdci::Cleanup()
{
	LOG_FUNCTION_ENTER;

	for (int i = 0; i < MAXDEVICES; i++) {
		delete adapters[ i ];
		adapters[ i ] = NULL;
	}

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

bool PluginDdci::AddDdCiDev( struct dirent *f, int adapter, const char *adapter_name, const char *ci_name )
{
	bool ret = false;

	if (DirentIsName( f, ci_name )) {
		int ci = DirentGetNameNum( f, strlen( ci_name ) );

		// there must be no frontend device!
		cReadDir adapterdir2( AddDirectory( DEV_DVB_BASE, adapter_name ) );
		struct dirent *f2;
		while ((f2 = adapterdir2.Next()) != NULL) {
			if (DirentIsName( f2, DEV_DVB_FRONTEND )) {
				return false;  // frontend found -> ignore this adapter
			}
		}

		DdCiName *dd_ci_name = new DdCiName( adapter, ci, ci_name );

		L_DBG_M( LDM_D, "found DD CI adapter '%s'", *dd_ci_name->FullCiName() );

		dd_ci_names.Append( dd_ci_name );
		ret = true;
	}

	return ret;
}

//------------------------------------------------------------------------

bool PluginDdci::AddDdCiDevs( struct dirent *f, int adapter, const char *adapter_name )
{
	int ret = false;

	for (int cidev = 0; cidev < DEV_DVB_CIDEVS_NUM; ++cidev)
		ret |= AddDdCiDev( f, adapter, adapter_name, DEV_DVB_CIDEVS[cidev] );

	return ret;
}

//------------------------------------------------------------------------

bool PluginDdci::FindDdCi()
{
	LOG_FUNCTION_ENTER;

	cReadDir dvbdir( DEV_DVB_BASE );
	if (dvbdir.Ok()) {
		dirent *a;
		while ((a = dvbdir.Next()) != NULL) {
			if (DirentIsName( a, DEV_DVB_ADAPTER )) {
				int adapter = DirentGetNameNum( a, strlen( DEV_DVB_ADAPTER ) );
				cReadDir adapterdir( AddDirectory( DEV_DVB_BASE, a->d_name ) );
				if (adapterdir.Ok()) {
					struct dirent *f;
					while ((f = adapterdir.Next()) != NULL)
						AddDdCiDevs( f, adapter, a->d_name);
				}
			}
		}
	}

	int found = dd_ci_names.Size();
	if (found > 0) {
		dd_ci_names.Sort();
		L_INF( "found %d DD CI adapter%s", found, found > 1 ? "s" : "" );
	} else
		L_INF( "no DD CI adapter found" );

	LOG_FUNCTION_EXIT;

	return found > 0;
}

//------------------------------------------------------------------------

DdCiName *PluginDdci::GetDdCi( void )
{
	LOG_FUNCTION_ENTER;

	DdCiName *ret = 0;

	// get always the first object until the list is empty
	if ( dd_ci_names.Size() ) {
		ret = dd_ci_names.At( 0 );
		dd_ci_names.Remove( 0 );
	}

	LOG_FUNCTION_EXIT;

	return ret;
}

//------------------------------------------------------------------------

PluginDdci::PluginDdci()
{
	LOG_FUNCTION_ENTER;

	memset( adapters, 0x00, sizeof(adapters) );
	LogLevel = LL_DEFAULT;
	LogDbgMask = 0;
	cfgIgnAct = 0;
	cfgBufSz = BUF_NUM_DEF;
	cfgClrSct = 0;
	cfgSleepTmo = SLEEP_TMO_DEF;

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

PluginDdci::~PluginDdci()
{
	LOG_FUNCTION_ENTER;

	Cleanup();

	LOG_FUNCTION_EXIT;
}

//------------------------------------------------------------------------

const char *PluginDdci::Version()
{
	return VERSION;
}

//------------------------------------------------------------------------

const char *PluginDdci::Description()
{
	return DESCRIPTION;
}

//------------------------------------------------------------------------

const char *PluginDdci::CommandLineHelp()
{
	static const char *txt =
	  "  -A        --ignact       ignore active flag; speeds up channel switching to\n"
	  "                           decryted channels\n"
	  "  -b        --bufsz        CAM receive/send buffer size in packets a 188 bytes\n"
	  "                           default: 1500, max: 10000\n"
	  "  -c        --clrsct       clear the scambling control bit before the\n"
	  "                           packet is send to VDR\n"
	  "  -l        --loglevel     0/1/2/3 log nothing/error/info/debug\n"
	  "  -d        --debugmask    Bitmask to enable special debug logging\n"
	  "                           0x0001 ... all what the developer thought\n"
	  "                                      should be logged in debug default\n"
	  "                           0x0002 ... file access during init\n"
	  "                           0x0400 ... CAM data read/write access (heavy\n"
	  "                                      logging)\n"
	  "                           0x0800 ... Scrambling control\n"
	  "                           0x1000 ... CAM buffer statistic (quite much\n"
	  "                                      logging)\n"
	  "  -t        --sleeptimer   CAM receive/send/deliver thread sleep timer in ms\n"
	  "                           default: 100, max: 1000\n"
	  ;

	return txt;
}

//------------------------------------------------------------------------

bool PluginDdci::ProcessArgs( int argc, char *argv[] )
{
	static struct option long_options[] = {
		{ "ignact", required_argument, NULL, 'A' },
		{ "bufsz", required_argument, NULL, 'b' },
		{ "clrsct", no_argument, NULL, 'c' },
		{ "debugmask", required_argument, NULL, 'd' },
		{ "loglevel", required_argument, NULL, 'l' },
		{ "sleeptimer", required_argument, NULL, 't' },
		{ NULL, no_argument, NULL, 0 }
	};

	int c, ll, logm;

	while ((c = getopt_long( argc, argv, "Ab:cd:l:t:", long_options, NULL )) != -1) {
		const char * err_txt;

		switch (c) {
		case 'A':
			cfgIgnAct = 1;
			ll = 1; // no error
			break;
		case 'b':
			err_txt = "Invalid Buffer number entered";
			ll = sscanf( optarg, "%u", &cfgBufSz );
			if (cfgBufSz > BUF_NUM_MAX)
				ll = 0;   // to enter error handling
			break;
		case 'c':
			cfgClrSct = 1;
			ll = 1; // no error
			break;
		case 'd':
			logm = 0;
			err_txt = "Invalid Debug Mask entered";
			ll = sscanf( optarg, "0x%4x", &logm );
			LogDbgMask |= logm;
			break;
		case 'l':
			err_txt = "Invalid Loglevel entered";
			ll = sscanf( optarg, "%u", &LogLevel );
			if (LogLevel > LOG_L_MAX)
				ll = 0;   // to enter error handling
			break;
		case 't':
			err_txt = "Invalid Sleep timer value entered";
			ll = sscanf( optarg, "%u", &cfgSleepTmo );
			if (cfgSleepTmo > SLEEP_TMO_MAX)
				ll = 0;   // to enter error handling
			break;
		default:
			ll = 0;
			err_txt = "Unknown option found";
			break;;
		}

		if ( ll <= 0 ) {
			fprintf( stderr, "%s\n", err_txt );
			return false;
		}
	}

	if ( ! LogDbgMask )
		LogDbgMask = LDM_DEFAULT;

	return true;
}

//------------------------------------------------------------------------

bool PluginDdci::Start()
{
	LOG_FUNCTION_ENTER;

	L_INF( "plugin version %s initializing (compiled for VDR version %s)", VERSION, VDRVERSION );

	L_DBG_M( LDM_D, "Debug logging mask 0x%04x", LogDbgMask );
	L_DBG_M( LDM_D, "Buffer size %d packets", cfgBufSz );
	L_DBG_M( LDM_D, "Sleep timer %dms", cfgSleepTmo );

	if (CfgIgnAct())
		L_INF( "Ignore-active-flag activated" );

	if (CfgIsClrSct())
		L_DBG_M( LDM_D, "Clear scrambling control bit activated" );

	if (FindDdCi()) {
		int i = 0;
		while (DdCiName *dd_ci_name = GetDdCi()) {
			cString fnameCa( dd_ci_name->FullCaName() );
			cString fnameCi( dd_ci_name->FullCiName() );

			L_DBG_M( LDM_F, "Try to open %s", *fnameCa );
			int ca_fd = CxDevOpen( fnameCa, O_RDWR );
			L_DBG_M( LDM_F, "Try to open %s (w)", *fnameCi );
			int ci_fdw = CxDevOpen( fnameCi, O_WRONLY );
			L_DBG_M( LDM_F, "Try to open %s (r)", *fnameCi );
			int ci_fdr = CxDevOpen( fnameCi, O_RDONLY | O_NONBLOCK );
			if ((ca_fd >= 0) && (ci_fdw >= 0) && (ci_fdr >= 0)) {
				L_INF( "Creating DdCiAdapter %d (%s)", i, *fnameCa );
				adapters[ i++ ] = new DdCiAdapter( ca_fd, ci_fdw, ci_fdr, fnameCa, fnameCi );
			} else {
				L_DBG_M( LDM_D, "Fds -> ca: %d, ciw: %d, cir:%d", ca_fd, ci_fdw, ci_fdr );
				close( ca_fd );
				close( ci_fdw );
				close( ci_fdr );
			}
			delete dd_ci_name;
		}
	}

	L_INF( "plugin started" );

	LOG_FUNCTION_EXIT;

	return true;
}

//------------------------------------------------------------------------

void PluginDdci::Stop()
{
	LOG_FUNCTION_ENTER;

	for (int i = 0; i < MAXDEVICES; i++) {
		if (adapters[ i ])
			adapters[ i ]->Cancel( 3 );
	}

	L_INF( "plugin stopped" );

	LOG_FUNCTION_EXIT;
}

VDRPLUGINCREATOR( PluginDdci ); // Don't touch this!
