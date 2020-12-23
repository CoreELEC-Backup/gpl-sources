#ifndef OSCAM_CONF_CHK_H
#define OSCAM_CONF_CHK_H

void chk_iprange(char *value, struct s_ip **base);
void chk_caidtab(char *value, CAIDTAB *caidtab);
void chk_caidvaluetab(char *value, CAIDVALUETAB *tab);
void chk_cacheex_valuetab(char *lbrlt, CECSPVALUETAB *tab);
void chk_cacheex_cwcheck_valuetab(char *lbrlt, CWCHECKTAB *tab);
void chk_cacheex_hitvaluetab(char *lbrlt, CECSPVALUETAB *tab);
void chk_tuntab(char *tunasc, TUNTAB *ttab);
void chk_services(char *labels, SIDTABS *sidtabs);
void chk_ftab(char *value, FTAB *ftab);
void chk_cltab(char *classasc, CLASSTAB *clstab);
void chk_port_tab(char *portasc, PTAB *ptab);
void chk_port_camd35_tab(char *portasc, PTAB *ptab);
void chk_ecm_whitelist(char *value, ECM_WHITELIST *ecm_whitelist);
void chk_ecm_hdr_whitelist(char *value, ECM_HDR_WHITELIST *ecm_hdr_whitelist);

void clear_sip(struct s_ip **sip);
void clear_ptab(struct s_ptab *ptab);
void clear_cacheextab(CECSPVALUETAB *ctab);

#endif
