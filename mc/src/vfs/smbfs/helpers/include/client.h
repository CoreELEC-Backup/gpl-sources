/*
   Unix SMB/Netbios implementation.
   Version 1.9.
   SMB parameters and setup
 */

#ifndef _CLIENT_H
#define _CLIENT_H

/* the client asks for a smaller buffer to save ram and also to get more
   overlap on the wire. This size gives us a nice read/write size, which
   will be a multiple of the page size on almost any system */
#define CLI_BUFFER_SIZE (0xFFFF)

/*
 * These definitions depend on smb.h
 */

typedef struct file_info
{
    SMB_OFF_T size;
    uint16 mode;
    uid_t uid;
    gid_t gid;
    /* these times are normally kept in GMT */
    time_t mtime;
    time_t atime;
    time_t ctime;
    pstring name;
} file_info;

struct print_job_info
{
    uint16 id;
    uint16 priority;
    size_t size;
    fstring user;
    fstring name;
    time_t t;
};

struct pwd_info
{
    BOOL null_pwd;
    BOOL cleartext;
    BOOL crypted;

    fstring password;

    uchar smb_lm_pwd[16];
    uchar smb_nt_pwd[16];

    uchar smb_lm_owf[24];
    uchar smb_nt_owf[24];
};

struct cli_state
{
    int port;
    int fd;
    uint16 cnum;
    uint16 pid;
    uint16 mid;
    uint16 vuid;
    int protocol;
    int sec_mode;
    int rap_error;
    int privileges;

    fstring eff_name;
    fstring desthost;
    fstring user_name;
    fstring domain;

    /*
     * The following strings are the
     * ones returned by the server if
     * the protocol > NT1.
     */
    fstring server_type;
    fstring server_os;
    fstring server_domain;

    fstring share;
    fstring dev;
    struct nmb_name called;
    struct nmb_name calling;
    fstring full_dest_host_name;
    struct in_addr dest_ip;

    struct pwd_info pwd;
    unsigned char cryptkey[8];
    uint32 sesskey;
    int serverzone;
    uint32 servertime;
    int readbraw_supported;
    int writebraw_supported;
    int timeout;                /* in milliseconds. */
    int max_xmit;
    int max_mux;
    char *outbuf;
    char *inbuf;
    int bufsize;
    int initialised;
    int win95;
    uint32 capabilities;

    /*
     * Only used in NT domain calls.
     */

    uint32 nt_error;            /* NT RPC error code. */
    uint16 nt_pipe_fnum;        /* Pipe handle. */
    unsigned char sess_key[16]; /* Current session key. */
    unsigned char ntlmssp_hash[258];    /* ntlmssp data. */
    uint32 ntlmssp_cli_flgs;    /* ntlmssp client flags */
    uint32 ntlmssp_srv_flgs;    /* ntlmssp server flags */
    uint32 ntlmssp_seq_num;     /* ntlmssp sequence number */
    DOM_CRED clnt_cred;         /* Client credential. */
    fstring mach_acct;          /* MYNAME$. */
    fstring srv_name_slash;     /* \\remote server. */
    fstring clnt_name_slash;    /* \\local client. */
    uint16 max_xmit_frag;
    uint16 max_recv_frag;
};

#endif /* _CLIENT_H */
