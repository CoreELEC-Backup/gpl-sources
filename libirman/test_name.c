/* test_name.c - test/demo of LIBIRMAN's high level functions   */
/* Copyright (C) 1999 Tom Wheeley, see file COPYING for details */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "irman.h"

int main(int argc, char **argv)
{
  unsigned char *code;
  unsigned char *oldcode;
  char *name = NULL;
  char *filename;

  if (ir_init_commands(NULL, 1) < 0) {
    fprintf(stderr, "error initialising commands: %s\n", ir_strerror(errno));
    exit(1);
  }

  filename = ir_default_portname();
  
  if (argc == 2) {
    filename = argv[1];
  } else {
    if (!filename) {
      fprintf(stderr, "usage: test_name device\n");
      fprintf(stderr, "       where device is the serial port at which the infra red receiver resides\n");
      fprintf(stderr, "       eg /dev/ttyS1 for COM2 on a PC running Linux\n");
      exit(0);
    }
  }

  errno = 0;  
  if (ir_init(filename) < 0) {
    fprintf(stderr, "error initialising Irman: `%s'\n", ir_strerror(errno));
    exit(1);
  }

  printf("ok\n");

  errno = 0;
  for(;;) {
    code = ir_get_code();
    if (code) {
      name = ir_code_to_name(code);
      printf("rx `%s'\n", name);
    
    /* error handling dullness */
    } else {
      if (errno == IR_EDUPCODE) {
        printf("rx `%s' (dup)\n", name);
      } else {
        fprintf(stderr, "error reading code: `%s'\n", ir_strerror(errno));
        ir_finish();
        ir_free_commands();
        exit(1);
      }
      errno = 0;
    }
    oldcode = code;
  }

  ir_free_commands();
  ir_finish();
  return 0;
}

/* end of test_func.c */
