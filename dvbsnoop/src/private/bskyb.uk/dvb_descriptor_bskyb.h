/*
$Id: dvb_descriptor_bskyb.h,v 1.0 2016/12/24 22:56:38 LraiZer Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de (rasc)


 -- private DVB Descriptors  bskyb.uk

*/

#ifndef _BSKYB_DVB_DESCRIPTOR_H
#define _BSKYB_DVB_DESCRIPTOR_H

void out_CATEGORY_ID(unsigned short *c2, unsigned short *c3);
void descriptor_PRIVATE_BskybUK_LogicChannelDescriptor1 (u_char *b);
void descriptor_PRIVATE_BskybUK_LogicChannelDescriptor2 (u_char *b);

#endif

