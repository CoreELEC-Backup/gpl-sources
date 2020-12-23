/*
* module-cccshare.h
*
*  Created on: 26.02.2011
*      Author: schlocke
*/
#ifndef MODULE_CCCSHARE_H_
#define MODULE_CCCSHARE_H_

// In this file put functions that are shared between module-cccam.c and module-cccshare.c

int32_t chk_ident(FTAB *ftab, struct cc_card *card);
int32_t cc_srv_report_cards(struct s_client *cl);
LLIST *get_cardlist(uint16_t caid, LLIST **list);

void cc_free_card(struct cc_card *card);
void cc_free_cardlist(LLIST *card_list, int32_t destroy_list);
int32_t cc_cmd_send(struct s_client *cl, uint8_t *buf, int32_t len, cc_msg_type_t cmd);
int32_t sid_eq(struct cc_srvid *srvid1, struct cc_srvid *srvid2);
int32_t sid_eq_nb(struct cc_srvid *srvid1, struct cc_srvid_block *srvid2);
int32_t sid_eq_bb(struct cc_srvid_block *srvid1, struct cc_srvid_block *srvid2);
int32_t same_card(struct cc_card *card1, struct cc_card *card2);
int32_t same_card2(struct cc_card *card1, struct cc_card *card2, int8_t compare_grp);
void cc_UA_oscam2cccam(uint8_t *in, uint8_t *out, uint16_t caid);
void cc_SA_oscam2cccam(uint8_t *in, uint8_t *out);
void set_card_timeout(struct cc_card *card);

struct cc_srvid *is_good_sid(struct cc_card *card, struct cc_srvid *srvid_good);
struct cc_srvid_block *is_sid_blocked(struct cc_card *card, struct cc_srvid *srvid_blocked);

void add_good_sid(struct cc_card *card, struct cc_srvid *srvid_good);
void remove_good_sid(struct cc_card *card, struct cc_srvid *srvid_good);
void add_sid_block(struct cc_card *card, struct cc_srvid *srvid_blocked, bool temporary);
void remove_sid_block(struct cc_card *card, struct cc_srvid *srvid_blocked);

void merge_sids(struct cc_card *carddst, struct cc_card *cardsrc);

void cccam_refresh_share(void);

int32_t hide_card_to_client(struct cc_card *card, struct s_client *cl);
int32_t unhide_card_to_client(struct cc_card *card, struct s_client *cl);
int32_t hidecards_card_valid_for_client(struct s_client *cl, struct cc_card *card);

#endif
