/*
 * epgdata2xmltv.h: a grabber for the xmltv2vdr plugin
 *
 */

#ifndef __EPGDATA2XMLTV_H
#define __EPGDATA2XMLTV_H

#include <curl/curl.h>
#include <curl/easy.h>

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libexslt/exslt.h>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#if __GNUC__ > 3
#define UNUSED(v) UNUSED_ ## v __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#define esyslog(a...) void( (SysLogLevel > 0) ? syslog_redir(a) : void() )
#define isyslog(a...) void( (SysLogLevel > 1) ? syslog_redir(a) : void() )
#define dsyslog(a...) void( (SysLogLevel > 2) ? syslog_redir(a) : void() )
#define tsyslog(a...) void( (SysLogLevel > 3) ? syslog_redir(a) : void() )

#define EPGDATA2XMLTV_USERAGENT "libcurl-agent/1.0"
#define EPGDATA2XMLTV_URL "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&dataType=xml&dayOffset=%s"

struct data
{
    size_t size;
    int fd;
};

class cepgdata2xmltv
{
private:
    struct data data;
    xsltStylesheetPtr pxsltStylesheet;
    xmlDocPtr sxmlDoc;
    char *strreplace(char *s, const char *s1, const char *s2);
    int  Fetch(const char *path, const char *pin, int day);
    int  DownloadData(const char *url);
    bool Translate(xmlDocPtr pxmlDoc, const char **params);
    void LoadXSLT();
  public:
    cepgdata2xmltv();
    ~cepgdata2xmltv();
    int Process(int argc, char *argv[]);
};

#endif //__EPGDATA2XMLTV_H
