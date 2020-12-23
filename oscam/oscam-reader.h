#ifndef _OSCAM_READER_H_
#define _OSCAM_READER_H_

const struct s_cardsystem *get_cardsystem_by_caid(uint16_t caid);
struct s_reader *get_reader_by_label(char *lbl);
const char *reader_get_type_desc(struct s_reader *rdr, int32_t extended);

bool hexserialset(struct s_reader *rdr);
void hexserial_to_newcamd(uint8_t *source, uint8_t *dest, uint16_t caid);
void newcamd_to_hexserial(uint8_t *source, uint8_t *dest, uint16_t caid);

S_ENTITLEMENT *cs_add_entitlement(struct s_reader *rdr, uint16_t caid, uint32_t provid, uint64_t id, uint32_t class, time_t start, time_t end, uint8_t type, uint8_t add);
void cs_clear_entitlement(struct s_reader *rdr);

int32_t hostResolve(struct s_reader *reader);
int32_t network_tcp_connection_open(struct s_reader *);
void network_tcp_connection_close(struct s_reader *, char *);
void block_connect(struct s_reader *rdr);
int32_t is_connect_blocked(struct s_reader *rdr);

void reader_do_idle(struct s_reader *reader);
void casc_check_dcw(struct s_reader *reader, int32_t idx, int32_t rc, uint8_t *cw);
void reader_do_card_info(struct s_reader *reader);
int32_t reader_slots_available(struct s_reader *reader, ECM_REQUEST *er);

void cs_card_info(void);
int32_t reader_init(struct s_reader *reader);
void remove_reader_from_active(struct s_reader *rdr);
int32_t restart_cardreader(struct s_reader *rdr, int32_t restart);
void init_cardreader(void);
void kill_all_readers(void);

#endif
