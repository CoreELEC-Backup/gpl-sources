/* 
 * mumudvb - UDP-ize a DVB transport stream.
 * File for Session Announcement Protocol Announces
 * 
 * (C) 2008-2009 Brice DUBOST
 * 
 * The latest version can be found at http://mumudvb.net/
 * 
 * Copyright notice:
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** @file
 * @brief Header File for Session Announcement Protocol Announces
 * @author Brice DUBOST
 * @date 2008-2009
 */

#ifndef _SAP_H
#define _SAP_H

#include "mumudvb.h"

/**refer to  RFC 2974 : sap IP address*/
#define SAP_IP4  "224.2.127.254"
/**refer to  RFC 2974 : sap IP address*/
#define SAP_IP6  "FF05::2:7FFE"
/**refer to  RFC 2974 : sap port*/
#define SAP_PORT 9875
/**refer to  RFC 2974 : sap time to live*/
#define SAP_DEFAULT_TTL 255 

/**intervall between sap announces*/
#define SAP_DEFAULT_INTERVAL 5

#define SAP_HEADER4_BYTE0 0x20 /**00100000 : version 1 and nothing else*/
#define SAP_HEADER4_BYTE1 0x00 /**No auth header*/

#define SAP_HEADER6_BYTE0 0x30 /**00110000 : version 1 and IPv6*/
#define SAP_HEADER6_BYTE1 0x00 /**No auth header*/

#define SAP_HEAD_LEN4 8
#define SAP_HEAD_LEN6 20


/**@brief sap_message*/
typedef struct{
  /**the buffer*/
  unsigned char buf[MAX_UDP_SIZE]; 
  /**Lenght of the sap message*/
  int len;
  /**the version of the sap message, MUST be changed when sap changes*/
  int version;
  /** Do we have to send this message ?*/
  int to_be_sent;
}mumudvb_sap_message_t;


/**@brief General parameter for sap announces*/
typedef struct sap_p_t{
  /**the sap messages array*/
  mumudvb_sap_message_t *sap_messages4; 
  /**the sap messages array*/
  mumudvb_sap_message_t *sap_messages6; 
  /**do we send sap announces ?*/
  option_status_t sap; 
  /**Interval between two sap announces in second*/
  int sap_interval;
  /** The ip address of the server that sends the sap announces*/
  char sap_sending_ip4[20];
  /** The ip address of the server that sends the sap announces*/
  char sap_sending_ip6[IPV6_CHAR_LEN];
  /**the default cat : ie the playlist group (mainly for vlc)*/
  char sap_default_group[SAP_GROUP_LENGTH];
  /**The URI The URI should be a pointer to additional information about the
  conference*/
  char sap_uri[256];
  /**The organisation wich made the announces*/
  char sap_organisation[256];
  /** The socket for sending the announces*/
  int sap_socketOut4;
  /** The socket for sending the announces*/
  struct sockaddr_in sap_sOut4;
  /** The socket for sending the announces*/
  int sap_socketOut6;
  /** The socket for sending the announces*/
  struct sockaddr_in6 sap_sOut6;
  /** The serial number for the sap announces*/
  int sap_serial;
  /** The time when the last sap announces have been sent*/
  long sap_last_time_sent;
  /** The sap ttl (the norm ask it to be 255)*/
  int sap_ttl;
}sap_p_t;

void init_sap_v(sap_p_t *sap_vars);
int init_sap(sap_p_t *sap_vars, multi_p_t multi_p);
void sap_send(sap_p_t *sap_vars, int num_messages);
int sap_update(mumudvb_channel_t *channel, sap_p_t *sap_vars, int curr_channel, multi_p_t multi_p);
int read_sap_configuration(sap_p_t *sap_vars, mumudvb_channel_t *c_chan, char *substring);
void sap_poll(sap_p_t *sap_vars,int number_of_channels,mumudvb_channel_t  *channels, multi_p_t multi_p, long now);

#endif
