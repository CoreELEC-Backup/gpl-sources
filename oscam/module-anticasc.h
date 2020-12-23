#ifndef MODULE_ANTICASC_H_
#define MODULE_ANTICASC_H_

#ifdef CS_ANTICASC
extern void ac_init(void);
extern void ac_init_stat(void);
extern void ac_do_stat(void);
extern void ac_clear(void);
extern void ac_copy_vars(struct s_auth *src, struct s_auth *dst);
extern void ac_init_client(struct s_client *cl, struct s_auth *account);
extern void ac_chk(struct s_client *cl, ECM_REQUEST *er, int32_t level);
extern void insert_zaplist(ECM_REQUEST *er, struct s_client *client);
static inline bool acosc_enabled(void) { return cfg.acosc_enabled; }
extern bool anticasc_logging(char *txt);
#else
static inline void ac_init(void) { }
static inline void ac_init_stat(void) { }
static inline void ac_do_stat(void) { }
static inline void ac_clear(void) { }
static inline void ac_copy_vars(struct s_auth *UNUSED(src), struct s_auth *UNUSED(dst)) { }
static inline void ac_init_client(struct s_client *UNUSED(cl), struct s_auth *UNUSED(account)) { }
static inline void ac_chk(struct s_client *UNUSED(cl), ECM_REQUEST *UNUSED(er), int32_t UNUSED(level)) { }
static inline void insert_zaplist(ECM_REQUEST *UNUSED(er), struct s_client *UNUSED(client)) { }
static inline bool acosc_enabled(void) { return 0; }
static inline bool anticasc_logging(char *UNUSED(txt)) { return 0; }
#endif

#endif
