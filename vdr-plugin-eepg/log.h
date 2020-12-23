/*
 * log.h
 *
 *  Created on: 08.5.2012
 *      Author: d.petrovski
 */

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <vdr/tools.h>
#include <vdr/thread.h>
#include "setupeepg.h"

#define VERBOSE 1
/* 0 = only print errors, 1 = print channels and themes, 2 = print channels, themes, titles, summaries 3 = debug mode */
/* all is logged into /var/log/syslog */


inline bool CheckLevel(int level)
{
#ifdef DEBUG
  if (cSetupEEPG::getInstance()->LogLevel >= level)
#else
  if (VERBOSE >= level)
#endif
  {
    return true;
  }
  return false;
}

inline const char* PrepareLog(std::string message)
{
  message = "EEPG: " + message;
  return message.c_str();
}

#define MAXSYSLOGBUF 256

//void LogVsyslog(int errLevel, const char * message, ...)
inline void LogVsyslog(int errLevel, int const& lineNum, const char * function, const char * message, ...)
{
  va_list ap;
  char fmt[MAXSYSLOGBUF];
  if (errLevel == LOG_DEBUG) {
    snprintf(fmt, sizeof(fmt), "[%d] %s:%d %s", cThread::ThreadId(), function, lineNum, message);
  } else {
    snprintf(fmt, sizeof(fmt), "[%d] %s", cThread::ThreadId(), message);
  }
  va_start(ap,message);
  vsyslog ( errLevel, fmt, ap );
  va_end(ap);
}

#define LogI(a, b...) void( CheckLevel(a) ? LogVsyslog ( LOG_INFO, __LINE__, __FUNCTION__, b ) : void() )
#define LogE(a, b...) void( CheckLevel(a) ? LogVsyslog ( LOG_ERR, __LINE__, __FUNCTION__, b ) : void() )
#define LogD(a, b...) void( CheckLevel(a) ? LogVsyslog ( LOG_DEBUG, __LINE__, __FUNCTION__, b ) : void() )
//#define LogE(a, b...) void( CheckLevel(a) ? esyslog ( b ) : void() )
//#define LogD(a, b...) void( CheckLevel(a) ? dsyslog ( b ) : void() )
#define prep(s) PrepareLog(s)
#define prep2(s) s


//void LogF(int level, const char * message, ...)  __attribute__ ((format (printf,2,3)));

//void LogF(int level, const char * message, ...)
//{
//  if (CheckLevel(level)) {
//    va_list ap;
//    va_start(ap,message);
//    vsyslog (LOG_ERR, PrepareLog(message), ap );
//    va_end(ap);
//  }
//}

#endif /* LOG_H_ */
