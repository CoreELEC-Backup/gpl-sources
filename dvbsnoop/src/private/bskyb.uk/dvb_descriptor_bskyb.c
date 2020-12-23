/*
$Id: dvb_descriptor_bskyb.c,v 1.0 2016/12/24 22:56:38 LraiZer Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de (rasc)

 -- Private DVB Descriptors  bskyb.uk

*/

#include "dvbsnoop.h"
#include "dvb_descriptor_bskyb.h"
#include "strings/dvb_str.h"
#include "misc/hexprint.h"
#include "misc/output.h"

/*
 *
 * Private DVB descriptors
 * User Space: bskyb.uk
 *
 */
void out_CATEGORY_ID(unsigned short *c2, unsigned short *c3)
{
		switch (*c2)
		{
			case 0x10: out(4,"%s", "Sky Info"); return;
			case 0x30: out(4,"%s", "Shopping"); return;
			case 0x50: out(4,"%s", "Kids"); return;
			case 0x70: out(4,"%s", "Entertainment"); return;
			case 0x90: out(4,"%s", "Radio"); return;
			case 0xB0: out(4,"%s", "News"); return;
			case 0xD0: out(4,"%s", "Movies"); return;
			case 0xF0: out(4,"%s", "Sports"); return;

			case 0x00: out(4,"%s", "Sky Help"); return;
			case 0x20: out(4,"%s", "Unknown (0x20)"); return;
			case 0x40: out(4,"%s", "Unknown (0x40)"); return;
			case 0x60: out(4,"%s", "Unknown (0x60)"); return;
			case 0x80: out(4,"%s", "Unknown (0x80)"); return;
			case 0xA0: out(4,"%s", "Unknown (0xA0)"); return;
			case 0xC0: out(4,"%s", "Unknown (0xC0)"); return;
			case 0xE0: out(4,"%s", "Sports Pub"); return;
		}
		switch (*c3)
		{
			case 0x1F: out(4,"%s", "Lifestyle and Culture"); break;
			case 0x3F: out(4,"%s", "Adult"); break;
			case 0x5F: out(4,"%s", "Gaming and Dating"); break;
			case 0x7F: out(4,"%s", "Documentaries"); break;
			case 0x9F: out(4,"%s", "Music"); break;
			case 0xBF: out(4,"%s", "Religion"); break;
			case 0xDF: out(4,"%s", "International"); break;
			case 0xFF: out(4,"%s", "Specialist"); break;

			case 0x0F: out(4,"%s", "Unknown (0x0F)"); break;
			case 0x2F: out(4,"%s", "Unknown (0x2F)"); break;
			case 0x4F: out(4,"%s", "Unknown (0x4F)"); break;
			case 0x6F: out(4,"%s", "Unknown (0x6F)"); break;
			case 0x8F: out(4,"%s", "Unknown (0x8F)"); break;
			case 0xAF: out(4,"%s", "Unknown (0xAF)"); break;
			case 0xCF: out(4,"%s", "Unknown (0xCF)"); break;
			case 0xEF: out(4,"%s", "Unknown (0xEF)"); break;
		}
}

/*
   0xB1  Logic Channel Descriptor
   BskyB-Basic ver 1.0.0, 24.12.2016
*/
void descriptor_PRIVATE_BskybUK_LogicChannelDescriptor1 (u_char *b)
{
	unsigned int tag = b[0];
	unsigned int len = b[1];
	unsigned int i;

	out_NL(4);
	out_S2B_NL(4,"DVB-DescriptorTag: ", tag, "--> BSkyB UK Logic Channel Descriptor1 ");
	out_SB_NL(4, "descriptor_length: ", len);

	b += 2;

	indent(+1);

	unsigned char regional = b[0];
	unsigned char region_id = b[1];
	out_SB_NL(4, "regional: ", regional);
	out_SB_NL(4, "region_id: ", region_id);

	b += 2;

	for (i = 0; i < (len-2); i += 9) {
		unsigned short int service_id = (b[i] << 8) | b[i + 1];
		unsigned char service_type = b[i + 2];
		unsigned short int channel_id = (b[i + 3] << 8) | b[i + 4];
		unsigned short int lcn_id = (b[i + 5] << 8 ) | b[i + 6];
		unsigned short int sky_id = (b[i + 7] << 8 ) | b[i + 8];

		out_NL(4);
		out_SW_NL(4, "service_id: ", service_id);
		out_SW_NL(4, "service_type: ", service_type);
		out_SW_NL(4, "channel_id: ", channel_id);
		out_SW_NL(4, "lcn_id: ", lcn_id);
		out_SW_NL(4, "sky_id: ", sky_id);
	}

	indent(-1);
}

/*
   0xB2  Logic Channel Descriptor
   BskyB-Unified ver 1.0, 24.12.2016
*/
void descriptor_PRIVATE_BskybUK_LogicChannelDescriptor2 (u_char *b)
{
	unsigned int tag = b[0];
	unsigned int len = b[1];

	out_NL(4);
	out_S2B_NL(4,"DVB-DescriptorTag: ", tag, "--> BSkyB UK Logic Channel Descriptor2 ");
	out_SB_NL(4, "descriptor_length: ", len);

	b += 2;

	indent(+1);

	if (len >= 4)
	{
		unsigned short category_id = (b[2] << 8) | b[3];
		unsigned short c2 = b[2];
		unsigned short c3 = b[3];

		out_SB_NL(4, "category_id: ", category_id);
		out(4,"%s\"","category_name: ");
		out_CATEGORY_ID(&c2, &c3);
		out_nl(4, "\"");
	}
	else
		out_nl(4, "category_name: \"Unassigned\"");

	indent(-1);
}

