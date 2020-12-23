/*
$Id: ait.c,v 1.7 2009/11/22 15:36:05 rhabarber1848 Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de (rasc)


 --  AIT Section
 --  MHP Application Information Section
 --  TS 102 812

*/




#include "dvbsnoop.h"
#include "ait.h"
#include "mhp_misc.h"

#include "descriptors/descriptor.h"
#include "strings/dvb_str.h"
#include "strings/dsmcc_str.h"
#include "misc/helper.h"
#include "misc/output.h"





void section_MHP_AIT (u_char *b, int len)
{
 /* TS 102 812  */

 int        len2;
 u_int      table_id;



 out_nl (3,"AIT-decoding....");
 table_id = outBit_S2x_NL (3,"Table_ID: ",	b, 0, 8,
				(char *(*)(u_long))dvbstrTableID );
 if (table_id != 0x74) {
   out_nl (3,"wrong Table ID");
   return;
 }


 outBit_Sx_NL (4,"Section_syntax_indicator: ",	b, 8, 1);
 outBit_Sx_NL (6,"reserved_1: ",		b, 9, 1);
 outBit_Sx_NL (6,"reserved_2: ",		b,10, 2);
 outBit_Sx_NL (5,"section_length: ",		b,12,12);
 outBit_Sx_NL (3,"test_application_flag: ",	b,24, 1);
 outBit_S2x_NL(3,"application_type: ",		b,25, 15,
			(char *(*)(u_long))dsmccStrMHP_application_type );
 outBit_Sx_NL (6,"reserved_3: ",		b,40, 2);
 outBit_Sx_NL (3,"Version_number: ",		b,42, 5);
 outBit_S2x_NL(3,"Current_next_indicator: ",	b,47, 1,
			(char *(*)(u_long))dvbstrCurrentNextIndicator );
 outBit_Sx_NL (3,"Section_number: ",		b,48, 8);
 outBit_Sx_NL (3,"Last_section_number: ",	b,56, 8);


 outBit_Sx_NL (6,"reserved_4: ",		b,64, 4);
 len2 = outBit_Sx_NL (3,"common_descriptors_length: ",	b,68,12);
 b += 10;

 indent (+1);
 while (len2 > 0) {
	int x;

 	x = descriptor (b, MHP_AIT);
	b += x;
	len2 -= x;
 }
 indent (-1);
 out_NL (4);


 outBit_Sx_NL (6,"reserved_5: ",			b, 0, 4);
 len2 = outBit_Sx_NL (3,"application_loop_length: ",	b, 4,12);
 b += 2;

 indent (+1);
 while (len2 > 0) {
	int x;
	int len3;


	x = mhp_application_identifier (3, b);
	b += x;
	len2 -= x;

 	outBit_S2x_NL (3,"application_control_code: ",	b, 0, 8,
			(char *(*)(u_long))dsmccStrMHP_application_control_code);
 	outBit_Sx_NL (6,"reserved: ",			b, 8, 4);
	len3 = outBit_Sx_NL (3,"application_descriptor_loop_length: ",	b, 12,12);
	b += 3;
	len2 -= 3;

 	while (len3 > 0) {
		int x;

 		x = descriptor (b, MHP_AIT);
		b += x;
		len2 -= x;
		len3 -= x;
	}
 	indent (-1);
 	out_NL (4);


 }
 indent (-1);


 outBit_Sx_NL (5,"CRC: ",	b,0,32);
}








