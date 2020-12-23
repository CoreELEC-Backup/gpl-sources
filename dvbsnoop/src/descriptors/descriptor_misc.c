/*
$Id: descriptor_misc.c,v 1.3 2009/11/22 15:36:06 rhabarber1848 Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de (rasc)



 -- Descriptors  misc. routines, helpers, etc.

*/


#include "dvbsnoop.h"
#include "misc/helper.h"
#include "misc/output.h"




void  out_nl_calc_NPT (int v, unsigned long long npt) 
{
  long sec,usec;

  /*
        system clock frequency  == (90kHz)
	NPT_seconds      = NPT / (System_Clock_Frequency / 300)
	NPT_microseconds = ( (NPT * 1000000) / (System_Clock_Frequency / 300) )
	 		   - (NPT_seconds * 1000000)
   */

  sec = npt / (300);
  usec= ((npt * 10000) / 3) - (sec * 1000000);

  out_nl (v,"  [= %lu.%06lu sec]",sec,usec);
}









