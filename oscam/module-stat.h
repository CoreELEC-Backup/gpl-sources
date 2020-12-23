#ifndef MODULE_STAT_H_
#define MODULE_STAT_H_

void save_stat_to_file(int32_t thread);
int32_t clean_stat_by_rc(struct s_reader *rdr, int8_t rc, int8_t inverse);
int32_t clean_all_stats_by_rc(int8_t rc, int8_t inverse);
int32_t clean_stat_by_id(struct s_reader *rdr, uint16_t caid, uint32_t prid, uint16_t srvid, uint16_t chid, uint16_t ecmlen);
void clear_reader_stat(struct s_reader *rdr);
void clear_all_stat(void);
READER_STAT **get_sorted_stat_copy(struct s_reader *rdr, int32_t reverse, int32_t *size);
void update_ecmlen_from_stat(struct s_reader *rdr);

#ifdef WITH_LB
void init_stat(void);
void stat_finish(void);
void load_stat_from_file(void);
void lb_destroy_stats(struct s_reader *rdr);
void send_reader_stat(struct s_reader *rdr, ECM_REQUEST *er, struct s_ecm_answer *ea, int8_t rc);
void stat_get_best_reader(ECM_REQUEST *er);
void lb_mark_last_reader(ECM_REQUEST *er);
void check_lb_auto_betatunnel_mode(ECM_REQUEST *er);
uint32_t lb_auto_timeout(ECM_REQUEST *er, uint32_t ctimeout);
bool lb_check_auto_betatunnel(ECM_REQUEST *er, struct s_reader *rdr);
void lb_set_best_reader(ECM_REQUEST *er);
void lb_update_last(struct s_ecm_answer *ea_er, struct s_reader *reader);
uint16_t lb_get_betatunnel_caid_to(ECM_REQUEST *er);
void readerinfofix_get_stat_query(ECM_REQUEST *er, STAT_QUERY *q);
void readerinfofix_inc_fail(READER_STAT *s);
READER_STAT *readerinfofix_get_add_stat(struct s_reader *rdr, STAT_QUERY *q);
#else
static inline void init_stat(void) { }
static inline void stat_finish(void) { }
static inline void load_stat_from_file(void) { }
static inline void lb_destroy_stats(struct s_reader *UNUSED(rdr)) { }
static inline void send_reader_stat(struct s_reader *UNUSED(rdr), ECM_REQUEST *UNUSED(er), struct s_ecm_answer *UNUSED(ea), int8_t UNUSED(rc)) { }
static inline void stat_get_best_reader(ECM_REQUEST *UNUSED(er)) { }
static inline void lb_mark_last_reader(ECM_REQUEST *UNUSED(er)) { }
static inline void check_lb_auto_betatunnel_mode(ECM_REQUEST *UNUSED(er)) { }
static inline uint32_t lb_auto_timeout(ECM_REQUEST *UNUSED(er), uint32_t ctimeout) { return ctimeout; }
static inline bool lb_check_auto_betatunnel(ECM_REQUEST *UNUSED(er), struct s_reader *UNUSED(rdr)) { return 0; }
static inline void lb_set_best_reader(ECM_REQUEST *UNUSED(er)) { }
static inline void lb_update_last(struct s_ecm_answer *UNUSED(ea_er), struct s_reader *UNUSED(reader)) { }
static inline uint16_t lb_get_betatunnel_caid_to(ECM_REQUEST *UNUSED(er)) { return 0; }
#endif

#endif
