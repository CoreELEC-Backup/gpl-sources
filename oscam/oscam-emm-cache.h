#ifndef OSCAM_EMM_CACHE_H_
#define OSCAM_EMM_CACHE_H_

void emm_save_cache(void);
void load_emmstat_from_file(void);
void save_emmstat_to_file(void);
void emm_load_cache(void);

// all these functions below use emms md5 hash as indexkey
struct s_emmcache *find_emm_cache(uint8_t *emmd5); // find a certain emm, e.g. to resend it to reader, returns null if nothing found
int32_t emm_edit_cache(uint8_t *emmd5, EMM_PACKET *ep, bool add); // add = false: delete a certain emm from cache   add = true: update lastseen or add emm to cache
struct s_emmstat *get_emm_stat(struct s_reader *rdr, uint8_t *emmd5, uint8_t emmtype); // find a certain emmstat
int32_t remove_emm_stat(struct s_reader *rdr, uint8_t *emmd5); // remove a certain emmstat
int32_t clean_stale_emm_cache_and_stat(uint8_t *emmd5, int64_t gone); // remove stale global emmcache + emmstat where emm lastseen is older than gone ms

#else
static inline void load_emmstat_from_file(void) { }
static inline void save_emmstat_to_file(void) { }
static inline void emm_load_cache(void) { }
static inline void emm_save_cache(void) { }
#endif
