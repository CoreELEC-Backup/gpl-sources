/*
$Id: ts2secpes.c,v 1.16 2009/11/22 15:36:34 rhabarber1848 Exp $


 DVBSNOOP

 a dvb sniffer  and mpeg2 stream analyzer tool
 http://dvbsnoop.sourceforge.net/

 (c) 2001-2006   Rainer.Scherg@gmx.de



 -- Transport Stream Sub-Decode  PES / SECTION

*/




#include "dvbsnoop.h"
#include "ts2secpes.h"
#include "ts_misc.h"
#include "sections/sectables.h"
#include "pes/pespacket.h"
#include "misc/packet_mem.h"
#include "misc/output.h"





#define TS_SUBDEC_BUFFER   (512*1024)
enum  { TSD_no_error = 0, TSD_output_done,
	TSD_no_pui, TSD_error, TSD_continuity_error,
	TSD_scrambled_error, TSD_mem_error};


typedef struct _TS_SUBDEC {
	int	mem_handle;
    int pid;
	int     status;			// content is invalid?
	int     continuity_counter;	// 4 bit max !!
	int     packet_counter;
	int	payload_length;		// total length of PES or SECTION to be read, 0 = unspecified
	long pkt_nr;  // start packet number
} TS_SUBDEC;


static TS_SUBDEC tsds[MAX_PID+1];

static TS_SUBDEC* get_TSD(int pid) {
  TS_SUBDEC* tsd = &tsds[pid];
  if (tsd->mem_handle < 0) { // memory is not allocated yet
    if ((tsd->mem_handle = packetMem_acquire (TS_SUBDEC_BUFFER)) < 0) {
      tsd->status = TSD_mem_error;
    }
  }
  return tsd;
}

//------------------------------------------------------------ 

//
// -- init TS sub decoding buffer
// -- return: < 0: fail
//
int ts2SecPesInit (void)
{
  int i;
  for (i = 0; i < sizeof(tsds)/sizeof(TS_SUBDEC); ++i) {
    TS_SUBDEC* tsd = &tsds[i]; 
    tsd->mem_handle = -1;
    tsd->pid = i;
    tsd->status = TSD_no_pui;
    tsd->continuity_counter = -1;
    tsd->packet_counter = 0;
    tsd->payload_length = 0;
  }
  return 0;
}




//
// -- free TS sub decoding buffer
//
void ts2SecPesFree (void)
{
  int i;
  for (i = 0; i < sizeof(tsds)/sizeof(TS_SUBDEC); ++i) {
    TS_SUBDEC* tsd = &tsds[i];
    if (tsd->mem_handle >= 0) {
      packetMem_free (tsd->mem_handle);
    }
  }
}






//
// -- add TS data 
// -- return: 0 = fail
//
int ts2SecPes_AddPacketStart (long pkt_nr, int pid, int cc, u_char *b, u_int len)
{
    int l;
    TS_SUBDEC* tsd = get_TSD(pid);

    // -- duplicate packet ?
    if ((pid == tsd->pid) && (cc == tsd->continuity_counter)) {
	    return 1;
    }

    tsd->status = TSD_no_error;
    tsd->pid = pid;
    tsd->continuity_counter = cc;
    tsd->packet_counter = 1;
    tsd->pkt_nr = pkt_nr;

    // -- Save PES/PS or SECTION length information of incoming packet
    // -- set 0 for unspecified length
    l = 0;

// -- TS can contain multiple packets streamed in payload, so calc will be wrong!!!
// -- so I skip this at this time...
// -- $$$ code modification mark (1) start
//    if (len > 6) {
//	// Non-System PES (<= 0xBC) will have an unknown length (= 0)
//	if (b[0]==0x00 && b[1]==0x00 && b[2]==0x01 && b[3]>=0xBC) {
//		l = (b[4]<<8) + b[5];		// PES packet size...
//		if (l) l += 6;			// length with PES-sync, etc.
//   	} else {
//		int pointer = b[0]+1;
//		if (pointer+3 <= len) {	// not out of this packet?
//			l = ((b[pointer+1] & 0x0F) << 8) + b[pointer+2]; // sect size  (get_bits)
//		}
//		if (l) l += pointer + 3;	// length with pointer & tableId
//   	}
//   }
// -- $$$ code modification mark (1) end
//

    tsd->payload_length = l;


    packetMem_clear (tsd->mem_handle);
    if (! packetMem_add_data (tsd->mem_handle,b,len)) {
	tsd->status = TSD_mem_error;
	return 0;
    }
    
    return 1;
}


int ts2SecPes_AddPacketContinue (int pid, int cc, u_char *b, u_int len)
{
    TS_SUBDEC* tsd = get_TSD(pid);

    // -- duplicate packet?  (this would be ok, due to ISO13818-1)
    if ((pid == tsd->pid) && (cc == tsd->continuity_counter)) {
	    return 1;
    }

    // -- discontinuity error in packet ?
    if ((tsd->status == TSD_no_error) && (cc != (++tsd->continuity_counter%16))) {
	tsd->status = TSD_continuity_error;
    }

    tsd->continuity_counter = cc;

    if (tsd->status == TSD_no_error) {
	if (!packetMem_add_data (tsd->mem_handle,b,len) ) {
		tsd->status = TSD_mem_error;
	} else {
    		tsd->packet_counter++;
	  	return 1;
	}
    }

    return 0;
}



//------------------------------------------------------------ 


//
// -- TS  SECTION/PES  subdecoding
// -- check TS buffer and push data to sub decoding buffer
// -- on new packet start, output old packet data
//
void ts2SecPes_subdecode (u_char *b, int len, long pkt_nr, u_int opt_pid)
{
    u_int  transport_error_indicator;		
    u_int  payload_unit_start_indicator;		
    u_int  pid;		
    u_int  transport_scrambling_control;		
    u_int  continuity_counter;		
    u_int  adaptation_field_control;

    TS_SUBDEC* tsd = NULL;

 //fprintf(stdout,  "-># ts2SecPes_subdecode: len=%d, opt_pid=%u\n", len, opt_pid);

 pid				 = getBits (b, 0,11,13);

 tsd = get_TSD(pid);

 // -- filter pid?
 if (opt_pid >= 0 && opt_pid <= MAX_PID) {
	 if (opt_pid != pid)  return;
 }

 // -- no ts subdecode for special pids...
 if (check_TS_PID_special (pid)) return;


 transport_error_indicator	 = getBits (b, 0, 8, 1);
 payload_unit_start_indicator	 = getBits (b, 0, 9, 1);
 transport_scrambling_control	 = getBits (b, 0,24, 2);
 adaptation_field_control	 = getBits (b, 0,26, 2);
 continuity_counter		 = getBits (b, 0,28, 4);



 len -= 4;
 b   += 4;


 // -- skip adaptation field
 if (adaptation_field_control & 0x2) {
	int n;

	n = b[0] + 1;
	b += n;
	len -= n;
 }


 // -- push data to subdecoding collector buffer
 // -- on packet start, output collected data of buffer
 if (adaptation_field_control & 0x1) {

	// -- payload buffering/decoding

	// -- oerks, this we cannot use
	if (/*transport_scrambling_control || */transport_error_indicator) {
		tsd->status = TSD_scrambled_error;
		return;
	}

    // -- fillup scrambled data
    if (transport_scrambling_control) {
        int i; for (i = 0; i < len; ++i) b[i] = 0xCA;
    }

	// -- if payload_start, check PES/SECTION
	if (payload_unit_start_indicator) {

		// -- sections: pui-start && pointer != 0 push data to last section!
		// -- (PES would be also 0x00)

		int SI_offset = b[0];	// pointer
		if (SI_offset) {
		  ts2SecPes_AddPacketContinue (pid, continuity_counter, b+1, (u_long)SI_offset);
		  // -- because re-add data below, we have to fake cc
		  tsd->continuity_counter--;
		}

		// $$$ TODO: here we have a flaw, when pointer != 0, we do not display the new 
		//           TS packet, but we are subdecoding (display) using the TS overflow data...
		//           Workaround: pass SI_offset to output to display, that we are using data
		//                       from next TS packet... (this should do for now)

		// -- output data of prev. collected packets
		// -- if not already decoded or length was unspecified
     		if ((tsd->status != TSD_output_done) && packetMem_length(tsd->mem_handle))  {
			ts2SecPes_Output_subdecode (SI_offset, tsd->pid);
		}

		// -- first buffer data (also "old" prior to "pointer" offset...)
		ts2SecPes_AddPacketStart (pkt_nr, pid, continuity_counter, b, (u_long)len);

	} else {

		// -- add more data
		ts2SecPes_AddPacketContinue (pid, continuity_counter, b, (u_long)len);

	}

 }

}



//
// -- check if TS packet should already be sent to sub-decoding and output... 
// -- if so, do sub-decoding and do output
// -- return: 0 = no output, 1 = output done
//
// $$$ Remark: this routine is obsolete and in fact does nothing,
//             due to code modification mark (1)
//
int  ts2SecPes_checkAndDo_PacketSubdecode_Output (u_int pid)
{
    TS_SUBDEC* tsd = &tsds[pid];

    // -- subdecode section if we already have enough data
    if (tsd->status != TSD_output_done) {
        u_char* b = packetMem_buffer_start (tsd->mem_handle);
        u_int len = (u_int) packetMem_length (tsd->mem_handle);
        if (b && len && !(b[0]==0x00 && b[1]==0x00 && b[2]==0x01)) {
            u_int sect_len;
            u_int pointer = b[0]+1;
            b += pointer;
            sect_len = ((b[1] & 0x0F) << 8) + b[2] + 3; // sect size  (getBits)
            if (sect_len <= len) {
                ts2SecPes_Output_subdecode(0, tsd->pid);
                return 1;
            }
        }
    }

	return 0;
}



//
// -- last packet read subdecode output
// --- This is needed when eof arrives, when reading files
// --- and no new PUSI will follow.
// -- return: 0 = no output, 1 = output done
//
int  ts2SecPes_LastPacketReadSubdecode_Output (void)
{
    int i;
    for (i = 0; i < sizeof(tsds)/sizeof(TS_SUBDEC); ++i) {
      TS_SUBDEC* tsd = &tsds[i];
      if (tsd->mem_handle >= 0 && tsd->status != TSD_output_done) {
		ts2SecPes_Output_subdecode (0, tsd->pid);
	  }
    }
	return 0;
}




//
// -- TS  SECTION/PES  subdecoding  output
// --  overleap_bytes: !=0 indicator how many bytes are from the "next" packet
// --                  (pointer!=0)
//
void ts2SecPes_Output_subdecode (u_int overleap_bytes, u_int pid)
{
     TS_SUBDEC* tsd = get_TSD(pid);
     //fprintf(stdout, "-># ts2SecPes_Output_subdecode: tsd.status=%u, overleap_bytes=%u\n", tsd.status, overleap_bytes);

     indent (+1);
     out_NL (3);
     if (tsd->pid > MAX_PID) {
     	out_nl (3,"TS sub-decoding (%d packet(s) from %08lu):", tsd->packet_counter, tsd->pkt_nr);
     } else {
     	out_nl (3,"TS sub-decoding (%d packet(s) from %08lu stored for PID 0x%04x):",
			tsd->packet_counter,tsd->pkt_nr,tsd->pid & 0xFFFF);
     }
     
     if (overleap_bytes) {
     	out_nl (3,"Subdecoding takes %u bytes from next TS packet", overleap_bytes);
     }

     out_nl (3,"=====================================================");

     if (tsd->status != TSD_no_error) {
   	char *s = "";

	switch (tsd->status) {
	   case TSD_error:  		s = "unknown packet error"; break;
	   case TSD_no_pui:  		s = "no data collected, no payload start"; break;
	   case TSD_continuity_error:  	s = "packet continuity error"; break;
	   case TSD_scrambled_error:  	s = "packet scrambled or packet error"; break;
	   case TSD_mem_error:  	s = "subdecoding buffer (allocation) error"; break;
	   case TSD_output_done:  	s = "[data already displayed (this should never happen)]"; break;
	}
     	out_nl (3,"Packet cannot be sub-decoded: %s",s);

     } else {
        u_char *b = packetMem_buffer_start (tsd->mem_handle);
        u_int len = (u_int) packetMem_length (tsd->mem_handle);

	if (b && len) {

	    // -- PES/PS or SECTION

	    if (b[0]==0x00 && b[1]==0x00 && b[2]==0x01) {

		out_nl (3,"TS contains PES/PS stream (length=%u)...", len);
		ts2ps_pes_multipacket (b, len, tsd->pid);

	    } else {
		int pointer = b[0]+1;
		b += pointer;

		out_nl (3,"TS contains Section (length=%u)...", len);
		ts2sec_multipacket (b, len-pointer, tsd->pid);

	    }

	} else {
		out_nl (3,"No prev. packet start found...");
	}


     }

     out_NL (3);
     out_NL (3);
     indent (-1);
     tsd->status = TSD_output_done;
}




//
// -- decode SI packets in saved TS data
// -- check for consecutive SI packets
//

void  ts2sec_multipacket (u_char *b, int len, u_int pid)
{
  int sect_len;

	while (len > 0) {

		if (b[0] == 0xFF) break;			// stuffing, no more data

		// sect_len  = getBits (b, 0, 12, 12) + 3;
		sect_len = ((b[1] & 0x0F) << 8) + b[2] + 3; 	// sect size  (getBits)
		if (sect_len > len) {				// this should not happen!
			out_nl (3,"$$$ something is wrong here!!...");
			break;
		}

		out_nl (3,"SI packet (length=%d): ",sect_len);
		out_NL (9);
			print_databytes (9,"SI packet hexdump:", b, sect_len);
		out_NL (9);

	    	indent (+1);
			decodeSI_packet (b, sect_len, pid);
	    	indent (-1);
		out_NL (3);

		b += sect_len;
		len -= sect_len;

	}
}


//
// -- decode PS/PES packets in saved TS data
// -- check for consecutive PS/PES packets
//

void  ts2ps_pes_multipacket (u_char *b, int len, u_int pid)
{
  int pkt_len;

	while (len > 0) {
		// we are on packet start:  b[0..2] =  0x000001

		pkt_len = 0;
		//if (b[3] >0xBC) {			// has length field?
		//	pkt_len = (b[4]<<8) + b[5];	// PES packet size... (getBits)
		//	if (pkt_len) pkt_len += 6;	// not 0? get total length
		//}


		if (pkt_len == 0) {			// unbound stream?, seek next pkt

			int i = 5;
			while (i < (len-3)) {
				i++;			// seek next 0x000001
				if (b[i]   != 0x00) continue;
				if (b[i+1] != 0x00) continue;
				if (b[i+2] != 0x01) continue;

				pkt_len = i;
				break;
			}

		}

		if (pkt_len == 0) {			// still not found, or last pkt in buffer
			pkt_len = len;
		}


		out_nl (3,"PS/PES packet (length=%d): ",pkt_len);
		out_NL (9);
			print_databytes (9,"PS/PES packet hexdump:", b, pkt_len);
		out_NL (9);

	    	indent (+1);
		decodePS_PES_packet (b, pkt_len, pid);
	    	indent (-1);
		out_NL (3);

		b += pkt_len;
		len -= pkt_len;

	}

}



// 
// $$$ TODO: discontinuity signalling flag check?
//
//
// $$$ TODO: hexdump prior to decoding (-pd 9)
