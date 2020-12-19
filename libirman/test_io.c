/* test_io.c - test/demo of LIBIRMAN's low level functions      */
/* Copyright (C) 1998 Tom Wheeley, see file COPYING for details */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "irman.h"

int main(int argc, char **argv)
{
  int datum;
  int i;
  int fd;

  if (argc < 2) {
    fprintf(stderr, "usage: test_io device\n");
    fprintf(stderr, "       where device is the serial port at which the infra red receiver resides\n");
    fprintf(stderr, "       eg /dev/ttyS1 for COM2 on a PC running Linux\n");
    exit(0);
  }

  errno = 0;  
  if ((fd = ir_open_port(argv[1])) < 0) {
    fprintf(stderr, "unable to open port `%s' (%s)\n", argv[1], ir_strerror(errno));
    return 1;
  }

  /* read garbage */
  while ((datum = ir_read_char(IR_GARBAGE_TIMEOUT)) >= 0)
    ;
   
  errno = 0;
  ir_write_char('I'); putchar('I');
  tcdrain(fd);
  ir_usleep(IR_HANDSHAKE_GAP);
  ir_write_char('R'); putchar('R');
  putchar('\n');
  if (errno) {
    fprintf(stderr, "error writing handshake (%s)\n", ir_strerror(errno));
  }
  
  errno = 0;
  while ((datum = ir_read_char(IR_HANDSHAKE_TIMEOUT)) != 'O') {
    if (datum < 0) {
      fprintf(stderr, "error reading handshake (%s)\n", ir_strerror(errno));
      exit(1);
    } else {
      putchar(datum);
      putchar('!');
      fflush(stdout);
    }
  }
  putchar('O'); fflush(stdout);
  
  errno = 0;
  datum = ir_read_char(IR_HANDSHAKE_TIMEOUT);
  if (datum < 0) {
    fprintf(stderr, "error reading handshake (%s)\n", ir_strerror(errno));
    exit(1);
  } else {
    putchar(datum);
    if (datum != 'K') putchar('!');
  }
  putchar('\n');

  for (;;) {
    for (i=0; i<IR_CODE_LEN; i++) {
      datum = ir_read_char(IR_BLOCKING);
      if (datum < 0) {
        fprintf(stderr, "error reading data: `%s'\n", ir_strerror(errno));
        errno = 0;
      } else {
        printf("[%02x]", datum);
      }
    }
    putchar('\n');
  }

  ir_close_port();
  return 0;
}

/* end of test_io.c */
