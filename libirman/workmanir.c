/* workmanir.c - test/demo of LIBIR's high level command functions */
/* Copyright (C) 1998 Tom Wheeley, see file COPYING for details    */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "irman.h"

int is_daemon = 0;
char *progname = "workmanir";

enum commands {PLAY = 1, PAUSE, STOP, NEXT, PREV, FWD, REW, EJECT, QUIT};
char *codes[] = {
	/* dummy */	NULL,

	/* PLAY */	"workman-play",
	/* PAUSE */	"workman-pause",
	/* STOP */	"workman-stop",
	/* NEXT */	"workman-next",
	/* PREV */	"workman-prev",
	/* FWD */	"workman-fwd",
	/* REW */	"workman-rew",
	/* EJECT */	"workman-eject",
	/* QUIT */	"workman-power",

	/* end */	NULL
};


void sigterm(int sig)
{
  ir_free_commands();
  ir_finish();
  raise(sig);
}


#ifdef DAEMONIZE
void daemonize(void)
{
  pid_t pid;
	
  if ((pid = fork()) < 0) {		/* error */

    fprintf(stderr, "%s: fork() failed: `%s'\n", progname, ir_strerror(errno));
    exit(1);

  } else if (pid) {			/* parent */

    printf("%s installed (%d)\n", progname, (int)pid);
    exit(0);

  } else {				/* child */

    is_daemon = 1;
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    setsid();
    chdir("/");
    umask(0);
  }
}
#endif


void loop(void)
{
  int i;
  char buf[999];
  char *arg;
  char *str;


  strcpy(buf, "workman -s ");
  arg = buf + strlen(buf);

  for(i=1;i;) {
    str = NULL;
    switch (ir_get_command()) {
      case IR_CMD_ERROR:
        fprintf(stderr, "%s: error reading command: %s\n", progname, ir_strerror(errno));
        exit(1);

      case PLAY: str = "play"; break;
      case PAUSE:str = "pause"; break;
      case STOP: str = "stop"; break;
      case NEXT: str = "forward"; break;
      case PREV: str = "back"; break;
/*      case FWD:  str = "-fwd"; break;*/
/*      case REW:  str = "-rew"; break;*/
      case EJECT:str = "eject"; break;

      case QUIT:
        i = 0;
        break;

      case IR_CMD_UNKNOWN:
        printf("unknown command\n");
      default:
        ;
    }
    if (str) {
      strcpy(arg, str);
      if (system(buf) != 0) {
        fprintf(stderr, "%s: error running workman\n", progname);
        exit(1);
      }
    }
  }


}


int main(int argc, char **argv)
{
  int i;
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
      fprintf(stderr, "usage: workmanir device\n");
      exit(0);
    }
  }

  for (i=1; codes[i] != NULL; i++) {
    if (ir_register_command(codes[i], i) < 0) {
      if (errno == ENOENT) {
        fprintf(stderr, "%s: no code set for `%s'\n", progname, codes[i]);
      } else {
        fprintf(stderr, "error registering `%s': `%s'\n", codes[i], ir_strerror(errno));
      }
    }
  }

  errno = 0;  
  if (ir_init(filename) < 0) {
    fprintf(stderr, "%s: error initialising Irman on %s: `%s'\n", progname, filename, ir_strerror(errno));
    exit(1);
  }

#ifdef DAEMONIZE
  daemonize();
#endif

  loop();

  ir_free_commands();
  ir_finish();

  return 0;
}

/* end of workmanir.c */
