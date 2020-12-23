/* 
 * MuMuDVB - UDP-ize a DVB transport stream.
 * File for Conditionnal Access Modules support
 * 
 * (C) 2009-2011 Brice DUBOST
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


#ifndef _CAM_H
#define _CAM_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/dvb/ca.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "ts.h"
#include "mumudvb.h"
#include "autoconf.h"

/**@file
 * @brief cam support
 * 
 * Header file for cam support, contains mainly the structure for carrying cam parameters
 */

#include <libdvben50221/en50221_stdcam.h>

struct ca_info {
  int initialized; //are the cai complete ?
  int ready; //We wait a pool between each channel sending
  int sys_num;
  uint16_t sys_id[256];
  char app_name[256];
};

#define MAX_ENQUIRY_ANSWER_LENGTH 20
#define DISPLAY_TYPE_LIST 1
#define DISPLAY_TYPE_MENU 2

/** @brief the parameters for the cam
 * This structure contain the parameters needed for the CAM
 */
typedef struct cam_p_t{
  /**Do we activate the support for CAMs*/
  int cam_support;
  /**The came number (in case of multiple cams)*/
  int cam_number;
  /** Do we reask channels asked and keept scrambled and what is the interval between reasks*/
  int cam_reask_interval;
  int cam_type;
  int need_reset;
  int reset_counts;
  int max_reset_number;
  int timeout_no_cam_init;
  int reset_interval;
  struct en50221_transport_layer *tl;
  struct en50221_session_layer *sl;
  struct en50221_stdcam *stdcam;
  int ca_resource_connected;
  int camthread_shutdown;
  pthread_t camthread;
  int moveca;
  int mmi_state;
  int mmi_enq_blind;
  int mmi_enq_length;
  int mmi_enq_entered;
  char mmi_enq_answer[10];
  /** Used to say if we received the CA info callback */
  long ca_info_ok_time;
  /** The delay for sending the PMT to the CAM*/
  int cam_delay_pmt_send;
  /** The delay between two PMT asking */
  int cam_interval_pmt_send;
  long cam_pmt_send_time;
  char filename_cam_info[DEFAULT_PATH_LEN];
  mumu_string_t cam_menu_string;
  mumu_string_t cam_menulist_str;
  int cam_mmi_autoresponse;
  /** Do we follow the version of the PMT for the CAM ?*/
  int cam_pmt_follow;
}cam_p_t;

/*****************************************************************************
 * Code for dealing with libdvben50221
 *****************************************************************************/

#define SL_MAX_SESSIONS 16

#define MMI_STATE_CLOSED 0
#define MMI_STATE_OPEN 1
#define MMI_STATE_ENQ 2
#define MMI_STATE_MENU 3

#define MAX_WAIT_AFTER_RESET 30
#define CAM_DEFAULT_MAX_RESET_NUM 5
#define CAM_DEFAULT_RESET_INTERVAL 30

/**
 * States a CAM in a slot can be in.
 */

#define DVBCA_CAMSTATE_MISSING 0
#define DVBCA_CAMSTATE_INITIALISING 1
#define DVBCA_CAMSTATE_READY 2

/**
 * The types of CA interface we support.
 */

#define DVBCA_INTERFACE_LINK 0
#define DVBCA_INTERFACE_HLCI 1

void init_cam_v(cam_p_t *cam_p);
int cam_send_ca_pmt( mumudvb_ts_packet_t *pmt, struct ca_info *cai);
int convert_desc(struct ca_info *cai, uint8_t *out, uint8_t *buf, int dslen, uint8_t cmd, int quiet);
int convert_pmt(struct ca_info *cai, mumudvb_ts_packet_t *pmt, uint8_t list, uint8_t cmd,int quiet);
int cam_start(cam_p_t *, int, mumu_chan_p_t *);
void cam_stop(cam_p_t *);
int read_cam_configuration(cam_p_t *cam_p, mumudvb_channel_t *c_chan, char *substring);
int cam_new_packet(int pid, int curr_channel, cam_p_t *cam_p, mumudvb_channel_t *actual_channel);

void cam_pmt_follow(unsigned char *ts_packet,  mumudvb_channel_t *actual_channel);
#endif
