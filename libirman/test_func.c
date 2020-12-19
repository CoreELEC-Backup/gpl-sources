/* test_func.c - test/demo of LIBIRMAN's mid level functions    */
/* Copyright (C) 1998 Tom Wheeley, see file COPYING for details */

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
  unsigned char *code = NULL;
  unsigned char *oldcode = NULL;
  char *text;

  if (argc < 2) {
    fprintf(stderr, "usage: test_func device\n");
    fprintf(stderr, "       where device is the serial port at which the infra red receiver resides\n");
    fprintf(stderr, "       eg /dev/ttyS1 for COM2 on a PC running Linux\n");
    exit(0);
  }

  errno = 0;  
  if (ir_init(argv[1]) < 0) {
    fprintf(stderr, "error initialising Irman: `%s'\n", ir_strerror(errno));
    exit(1);
  }

  printf("ok\n");

  errno = 0;
  for(;;) {
    code = ir_get_code();
    if (code) {
      text = ir_code_to_text(code);
      printf("rx `%s'\n", text);
    
    /* error handling dullness */
    } else {
      if (errno == IR_EDUPCODE) {
        printf("rx `%s' (dup)\n", oldcode == NULL ?
                                  "(null - bug)" :
                                  ir_code_to_text(oldcode));
      } else {
        fprintf(stderr, "error reading code: `%s'\n", ir_strerror(errno));
        ir_finish();
        exit(1);
      }
      errno = 0;
    }
    oldcode = code;
  }

  ir_finish();
  return 0;
}

/* end of test_func.c */
