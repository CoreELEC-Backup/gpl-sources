/*
$Id: net_str.c,v 1.2 2009/11/22 15:36:31 rhabarber1848 Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de


 -- Network strings  (IP, UDP, ICMP)

*/

#include "dvbsnoop.h"
#include "net_str.h"
#include "strtable_misc.h"

/*
  -- Assigned Internet Protocol Numbers
  -- RFC 790
  -- (incomplete, only stuff used with DVB)
 */

char *netStr_RFC790_protocol_nr (u_int i)
{
  STR_TABLE  Table[] = {
     {  0x00, 0x00,  "Reserved" },
     {  0x01, 0x01,  "ICMP" },
     {  0x03, 0x03,  "Gateway-to-Gateway" },
     {  0x06, 0x06,  "TCP" },
     {  0x11, 0x11,  "UDP" },

     {  0x00, 0xFF,  "see: RFC790" },	// filler to match all entries
     {  0,0, NULL }
  };

  return findTableID (Table, i);
}





