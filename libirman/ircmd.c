/* irfcmd.c v0.4.2 (c) 1998-99 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* this code is placed under the LGPL, see www.gnu.org for info    */

/*
 * ircmd.c, Infra red command interface
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef HAVE_MEMORY_H
# include <memory.h>
#endif

#include "chunk.h"
#include "hashtable.h"

#include "irman.h"

#define IR_NAME_BIND	0
#define IR_NAME_ALIAS	1

struct text_ent_s;
struct name_ent_s;

typedef struct text_ent_s {
  char text[IR_CODE_LEN * 2 + 1];
  int cmd;
  struct name_ent_s *bind;
} text_ent_t;

typedef struct name_ent_s {
  char *name;
  int type;			/* IR_NAME_BIND or IR_NAME_ALIAS */
  struct text_ent_s *bind;
  struct name_ent_s *alias;
} name_ent_t;

static ht_t *text_ht = NULL;
static ht_t *name_ht = NULL;
static chunk_t *stuff = NULL;
static int ir_cmd_enabled = 0;

static char irmanrc_portname[IR_PORTNAME_LEN];
static int irmanrc_portname_set = 0;

static text_ent_t *new_text_ent(char *text, int cmd, name_ent_t *bind)
{
  text_ent_t *te;
  
  te = ch_malloc(sizeof (text_ent_t), stuff);
  if (!te) return NULL;
  strncpy(te->text, text, IR_CODE_LEN * 2);
  te->text[IR_CODE_LEN * 2] = '\0';
  te->cmd = cmd;
  te->bind = bind;

  if (ht_add(te->text, te, text_ht) < 0) {
    return NULL;
  }
  
  return te;
}

static name_ent_t *new_name_ent(char *name, int type, void *bind_or_alias)
{
  name_ent_t *ne;
  
  ne = ch_malloc(sizeof (name_ent_t), stuff);
  if (!ne) return NULL;
  ne->name = ch_malloc(strlen(name) + 1, stuff);
  if (!ne->name) return NULL;
  strcpy(ne->name, name);
  ne->type = type;
  ne->bind = NULL;
  ne->alias = NULL;
  
  if (type == IR_NAME_BIND) {
    ne->bind = bind_or_alias;
  }
  if (type == IR_NAME_ALIAS) {
    ne->alias = bind_or_alias;
  }
  
  if (ht_add(ne->name, ne, name_ht) < 0) {
    return NULL;
  }
  
  return ne;
}


int ir_init_commands(char *rcname, int warn)
{
  FILE *rc=NULL;
  static char buf[999];
  char *p, *q, *r, *s;
  int lineno = 0;

  if (ir_cmd_enabled) {
    errno = IR_EENABLED;
    return -1;
  }
  
  if (!(text_ht = ht_new(IR_HT_SIZE))) return -1;
  if (!(name_ht = ht_new(IR_HT_SIZE))) return -1;
  if (!(stuff = ch_new(32000))) return -1;

  ir_cmd_enabled = 1;

  /* load .irmanrc */
  
  /* open file */
  if (rcname) {		/* specified */
    rc = fopen(rcname, "r");
  } else {			/* users, eg ${HOME}/.irmanrc */
    char *home;
    int hl, rl;
    
    home = getenv("HOME");
    if (!home)
      home = ".";
    
    hl = strlen(home);
    rl = strlen(IR_USER_IRMANRC);
    
    rcname = malloc(hl + rl + 2);	/* alloca would be nice */
    if (rcname) {
      /* build rcname */
      strcpy(rcname, home);
      rcname[hl] = '/';
      strcpy(rcname + hl+1, IR_USER_IRMANRC);
      rcname[hl+rl+2] = '\0';
    
      rc = fopen(rcname, "r");
      if (!rc) {			/* system, eg /etc/irmanrc.conf */
        rc = fopen(IR_SYSTEM_IRMANRC, "r");
      }
      free(rcname);
    }
  }
  
  if (rc) {
    while (fgets(buf, 998, rc)) {
      lineno++;
      p=q=r="";
      
      /* skip leading whitespace */
      p = buf + strspn(buf, " \t\n");
      
      /* ignore blank lines and lines beginning with `#' - comments*/ 
      if (!*p || *p == '#')
        continue;
      
      /* separate into three words */
      for (q = p + strcspn(p, " \t\n"); *q && strchr(" \t\n", *q); *q++ = '\0')
        ;
      if (*q) {
        for (r = q + strcspn(q, " \t\n"); *r && strchr(" \t\n", *r); *r++ = '\0')
          ;
        s = r + strcspn(r, " \t\n");
        *s = '\0';
      }
      
#if 0
      printf("irmanrc: p=`%s', q=`%s', r=`%s'\n", p?p:"(null)", q?q:"(null)", r?r:"(null)");
#endif

      if (!strcmp(p, "bind")) {			/* bind */

        if (!*r) {
          if (warn) fprintf(stderr, "irmanrc:%d: syntax error\n", lineno);
          continue;
        }
        if (!ir_valid_code(r)) {
          if (warn) fprintf(stderr, "irmanrc:%d: invalid code: `%s'\n", lineno, r); 
          continue;
        }
        if (ir_bind(q, r) < 0) {
          if (warn) fprintf(stderr, "irmanrc:%d: bind error: `%s'\n", lineno, strerror(errno));
        }

      } else if (!strcmp(p, "alias")) {		/* alias */

        if (!*r) {
          if (warn) fprintf(stderr, "irmanrc:%d: syntax error\n", lineno);
          continue;
        }
        if (ir_alias(q, r) < 0) {
          if (warn) fprintf(stderr, "irmanrc:%d: alias error: `%s'\n", lineno, strerror(errno));
        }
        
      } else if (!strcmp(p, "port")) {		/* port */

        if (!*q) {
          if (warn) fprintf(stderr, "irmanrc:%d: syntax error\n", lineno);
          continue;
        }
        if (strlen(q) > IR_PORTNAME_LEN - 1) {
          if (warn) fprintf(stderr, "irmanrc:%d: port name too long\n", lineno);
          continue;
        }
        irmanrc_portname_set = 1;
        strncpy(irmanrc_portname, q, IR_PORTNAME_LEN);

      } else {
        if (warn) fprintf(stderr, "irmanrc:%d: unknown command `%s'\n", lineno, p);
      }
    
    } /* end while */
  
    fclose(rc);
  }

  return 0;
}


char *ir_default_portname(void)
{
  if (irmanrc_portname_set) {
    return irmanrc_portname;
  } else {
    return NULL;
  }
}


char *ir_text_to_name(char *text)
{
  text_ent_t *text_match = NULL;
  name_ent_t *name_match = NULL;
  
  text_match = ht_match(text, text_ht);
  if (text_match) {
    name_match = text_match->bind;
  }
  if (name_match) {
    return name_match->name;
  } else {
    return text;
  }
}


static text_ent_t *name_to_text_ent(char *name)
{
  name_ent_t *name_match = NULL;
  text_ent_t *text_match = NULL;
  
  name_match = ht_match(name, name_ht);
  while (name_match && name_match->type == IR_NAME_ALIAS) {
    name_match = name_match->alias;
  }

  if (name_match && name_match->type == IR_NAME_BIND) {
    text_match = name_match->bind;
  }
  
  if (!text_match) {
    text_match = ht_match(name, text_ht);
  }
  
  return text_match;
}


char *ir_name_to_text(char *name)
{
  text_ent_t *text_match = NULL;
  
  text_match = name_to_text_ent(name); 
  
  if (!text_match) {
    if (!ir_valid_code(name)) {
      errno = IR_ENOKEY;
      return NULL;
    }

    return name;
  }

  return text_match->text;
}


int ir_bind(char *name, char *text)
{
  name_ent_t *name_match = NULL;
  text_ent_t *text_match = NULL;
  
  name_match = ht_match(name, name_ht);
  if (name_match) {
    errno = IR_EDUPKEY;
    return -1;
  }
  
  text_match = ht_match(text, text_ht);
  if (text_match) {
    if (text_match->bind) {
      errno = IR_EDUPKEY;
      return -1;
    }
  } else {
    text_match = new_text_ent(text, 0, NULL);
  }
  
  if (!text_match)
    return -1;
  
  name_match = new_name_ent(name, IR_NAME_BIND, text_match);
  if (!name_match)
    return -1;
    
  text_match->bind = name_match;

  return 0;
}  


int ir_alias(char *newname, char *name)
{
  name_ent_t *name_match;
  name_ent_t *newname_match;

  newname_match = ht_match(newname, name_ht);
  if (newname_match) {
    errno = IR_EDUPKEY;
    return -1;
  }
  
  name_match = ht_match(name, name_ht);
  if (!name_match) {
    errno = IR_ENOKEY;
    return -1;
  }
  
  newname_match = new_name_ent(newname, IR_NAME_ALIAS, name_match);
  if (!newname_match)
    return -1;
    
  return 0;
}


int ir_register_command(char *name, int command)
{
  text_ent_t *text_match = NULL;

  if (!ir_cmd_enabled) {
    errno = IR_EDISABLED;
    return -1;
  }
  if (command <= 0) {
    errno = IR_EBADCMD;
    return -1;
  }

  text_match = name_to_text_ent(name);

  /* if we have a match then set the command data.  If not, create one */
  if (text_match) {
    if (text_match->cmd != 0) {
      errno = IR_EDUPKEY;
      return -1;
    }
    text_match->cmd = command;
  
  } else {
    
    if (!ir_valid_code(name)) {
      errno = IR_ENOKEY;
      return -1;
    }
    
    text_match = new_text_ent(name, command, NULL);
    if (!text_match) {
      return -1;
    }
  }
  
  return 0;
}

/* removes command associated with code from the hashtable.
 * should we also enable removal indexed by command? - bit fiddly really,
 * but could actually be more useful.
 */

int ir_remove_command(char *name)
{
  text_ent_t *text_match;
  
  text_match = name_to_text_ent(name);
  if (!text_match) {
    errno = IR_ENOKEY;
    return -1;
  }
  
  text_match->cmd = IR_CMD_UNKNOWN;
  
  return 0;
}


/* blocking wait for a code, then return a match if we find one, else
 * return IR_CMD_UNKNOWN for an unknown/spurious code, or IR_CMD_ERROR
 */

int ir_get_command(void)
{
  unsigned char *code;
  text_ent_t *text_match;

  code = ir_get_code();
  if (code) {
    text_match = ht_match(ir_code_to_text(code), text_ht);
    if (text_match) {
      return text_match->cmd;
    } else {
      return IR_CMD_UNKNOWN;
    }
  } else {
    return IR_CMD_ERROR;
  }
}


/* polling wait for a command.  returns IR_CMD_UNKNOWN if nothing to 
 * be read from the port
 */

int ir_poll_command(void)
{
  unsigned char *code;
  text_ent_t *text_match;
  
  code = ir_poll_code();
  if (code) {
    text_match = ht_match(ir_code_to_text(code), text_ht);
    if (text_match) {
      return text_match->cmd;
    } else {
      return IR_CMD_UNKNOWN;
    }
  } else {
    if (errno == ETIMEDOUT) {
      return IR_CMD_UNKNOWN;	/* the more harmless of errors */
    } else {
      return IR_CMD_ERROR;
    }
  }
}

/* free the command table */

void ir_free_commands(void)
{
  if (!ir_cmd_enabled)
    return;
    
  ht_free(&name_ht);
  ht_free(&text_ht);
  ch_free(stuff);
}

/*
 * Ask the user to enter a code, for calibration of their remote
 */

unsigned char *ir_ask_for_code(char *name, int display)
{
  unsigned char *code;
  unsigned char codecpy[IR_CODE_LEN];
  
  for(;;) {
    printf(IR_ASK_GREETING, name);
    if (!(code = ir_get_code())) {
      return NULL;
    }
    if (display) printf("read: `%s'\n", ir_code_to_text(code));

    memcpy(codecpy, code, IR_CODE_LEN); 

    printf(IR_ASK_REPEAT, name);
    if (!(code = ir_get_code())) {
      return NULL;
    }
    if (display) printf("read: `%s'\n", ir_code_to_text(code));

    if (memcmp(code, codecpy, IR_CODE_LEN) != 0) {
      printf(IR_ASK_NOMATCH);
    } else {
      printf(IR_ASK_OK);
      break;
    }
  }
  
  return code;
}

void ir_set_cmd_enabled(int val)
{
  ir_cmd_enabled = val;
}

char *ir_strerror(int eno)
{
  if (eno < 0) {
    switch (eno) {
      case IR_EENABLED:    return "Irman already initialised";
      case IR_EDISABLED:   return "Irman not yet initialised";
      case IR_EHANDSHAKE:  return "Irman handshake failed";
      case IR_EBADCMD:     return "Invalid command code";
      case IR_ENOKEY:      return "Key not found";
      case IR_EDUPKEY:     return "Key already exists";
      default:             return "Unknown error";
    }
  } else {
    return strerror(eno);
  }
}


/* end of ircmd.c */
