/*
$Id: tsdt.c,v 1.14 2009/11/22 15:36:27 rhabarber1848 Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de  (rasc)


   -- TSDT section
   -- Transport Stream Description Section
   -- ISO 13818

*/

#include "dvbsnoop.h"
#include "tsdt.h"
#include "descriptors/descriptor.h"
#include "strings/dvb_str.h"
#include "misc/output.h"



void section_TSDT (u_char *b, int len)
{

 typedef struct  _TDST {
    u_int      table_id;
    u_int      section_syntax_indicator;		
    u_int      reserved_1;
    u_int      reserved_2;
    u_int      section_length;
    u_int      reserved_3;
    u_int      version_number;
    u_int      current_next_indicator;
    u_int      section_number;
    u_int      last_section_number;

    // N  descriptor

    u_long     crc;
 } TDST;


 TDST   t;
 int	len1;


 
 t.table_id 			 = b[0];
 t.section_syntax_indicator	 = getBits (b, 0, 8, 1);
 t.reserved_1 			 = getBits (b, 0, 9, 1);
 t.reserved_2 			 = getBits (b, 0, 10, 2);
 t.section_length		 = getBits (b, 0, 12, 12);
 t.reserved_3 			 = getBits (b, 0, 24, 18);
 t.version_number 		 = getBits (b, 0, 42, 5);
 t.current_next_indicator	 = getBits (b, 0, 47, 1);
 t.section_number 		 = getBits (b, 0, 48, 8);
 t.last_section_number 		 = getBits (b, 0, 56, 8);

 b   += 8;
 len1 = t.section_length - 5;



 out_nl (3,"TDST-decoding....");
 out_S2B_NL (3,"Table_ID: ",t.table_id, dvbstrTableID (t.table_id));
 if (t.table_id != 0x03) {
   out_nl (3,"wrong Table ID");
   return;
 }
 
 out_SB_NL (3,"section_syntax_indicator: ",t.section_syntax_indicator);
 out_SB_NL (6,"Fixed '0': ",t.reserved_1);
 out_SB_NL (6,"reserved_2: ",t.reserved_2);
 out_SW_NL (5,"Section_length: ",t.section_length);
 out_SL_NL (6,"reserved_3: ",t.reserved_3);
 out_SB_NL (3,"Version_number: ",t.version_number);
 out_S2B_NL(3,"current_next_indicator: ",t.current_next_indicator, dvbstrCurrentNextIndicator(t.current_next_indicator));
 out_SB_NL (3,"Current_next_indicator: ",t.current_next_indicator);
 out_SB_NL (3,"Section_number: ",t.section_number);
 out_SB_NL (3,"Last_Section_number: ",t.last_section_number);


 indent (+1);
 while (len1 > 4) {
   int x;

   x = descriptor (b, MPEG);
   b   += x;
   len1 -= x;
 }
 indent (-1);
 out_NL (3);


 t.crc		 = getBits (b, 0, 0, 32);
 out_SL_NL (5,"CRC: ",t.crc);

}


