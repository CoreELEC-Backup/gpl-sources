/*
 * extended_frontend.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */
#ifndef _EXTENDED_DVBFRONTEND_H_
#define _EXTENDED_DVBFRONTEND_H_


/* compat version between w_scan and wirbelscan.
 * For details on both of them see http://wirbel.htpc-forum.de
 */

#define   SCAN_TERRESTRIAL      0 /* DVB-T/T2                */
#define   SCAN_CABLE            1 /* DVB-C                   */
#define   SCAN_SATELLITE        2 /* DVB-S/S2                */
#define   SCAN_RESERVE1         3 /* was: pvrinput           */
#define   SCAN_RESERVE2         4 /* was: pvrinput(fm radio) */
#define   SCAN_TERRCABLE_ATSC   5 /* ATSC VSB and/or QAM     */
#define   SCAN_NO_DEVICE        6
#define   SCAN_TRANSPONDER      999


typedef int fe_polarization_t;
typedef enum fe_west_east_flag { EAST_FLAG, WEST_FLAG } fe_west_east_flag_t;

struct satellite_transponder {
  __u32                 symbol_rate;                    // symbols per second //
  fe_code_rate_t        fec_inner;
  fe_modulation_t       modulation_type;
  fe_pilot_t            pilot;
  fe_rolloff_t          rolloff;
  fe_delivery_system_t  modulation_system;
  fe_polarization_t     polarization;
  __u32                 orbital_position;
  fe_west_east_flag_t   west_east_flag;
  __u8                  scrambling_sequence_selector;   // 6.2.13.3 S2 satellite delivery system descriptor //
  __u8                  multiple_input_stream_flag;     // 6.2.13.3 S2 satellite delivery system descriptor //
  __u32                 scrambling_sequence_index;      // 6.2.13.3 S2 satellite delivery system descriptor //
  __u8                  input_stream_identifier;        // 6.2.13.3 S2 satellite delivery system descriptor //
};


struct  tuning_parameters {
  __u32 frequency;                        /* satellite: intermediate frequency in kHz */
  fe_spectral_inversion_t inversion;
  union {
     struct satellite_transponder sat;
     } u;
};

#endif
