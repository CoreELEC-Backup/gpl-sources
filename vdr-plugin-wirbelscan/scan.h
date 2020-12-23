/*
 * wirbelscan: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef __SCAN_H__
#define __SCAN_H__

#include "common.h"

#define warning(msg...) _log(__FUNCTION__,__LINE__,1,false,msg)
#define info(msg...)    _log(__FUNCTION__,__LINE__,2,false,msg)

#endif
