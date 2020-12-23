/*
$Id: bskyb_uk.c,v 1.0 2016/12/24 22:56:38 LraiZer Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de  (rasc)


  -- Private Data Structures for:
  -- bskyb.uk

*/

#include "dvbsnoop.h"
#include "bskyb_uk.h"
#include "dvb_descriptor_bskyb.h"

static PRIV_DESCR_ID_FUNC pdescriptors[] = {
	{ 0xB1, DVB_SI,   descriptor_PRIVATE_BskybUK_LogicChannelDescriptor1 },
	{ 0xB2, DVB_SI,   descriptor_PRIVATE_BskybUK_LogicChannelDescriptor2 },
	{ 0x00,	0,        NULL } // end of table  (id = 0x00, funct = NULL)
};

//
// -- Return private section/descriptor id tables
// -- for this scope
//

void getPrivate_BskybUK ( PRIV_SECTION_ID_FUNC **psect,
		PRIV_DESCR_ID_FUNC **pdesc)
{
   *psect = NULL;
   *pdesc = pdescriptors;
}

