/* irio.c v0.4.2 (c) 1998-99 Tom Wheeley <tomw@tsys.demon.co.uk> */
/* this code is placed under the LGPL, see www.gnu.org for info  */

/*
 * irio.c, Irman infrared controller interface
 */

#ifdef HAVE_CONFIG_H 
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif
  
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef HAVE_SYS_FLOCK_H
# include <sys/flock.h>
#endif

#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif

#include <sys/ioctl.h>

#include "irman.h"

/* wrapper for ir_close_port() for use by atexit() */
static RETSIGTYPE ir_close_port_on_exit(void) { (void) ir_close_port(); }

static int portfd=0;
static int portflags=0;
static int oldflags=0;
static struct termios oldterm;
static struct termios portterm;

/*
 * Note regarding terminal settings.
 *
 * These work on my system.  I am quite confident they will work on other
 * systems.  The termios setup code is originally from another program
 * designed to talk to a serial device (casio diary) written by someone who I
 * can't remember but I presume they knew what they were doing.
 *
 * More information on Unix serial port programming can be obtained from
 *   http://www.easysw.com/~mike/serial/index.html
 *
 */

/*
 * Ignore the things in SUNATTEMPT.  They're not even needed for a Sun.
 */
 

int ir_open_port(char *filename)
{
  int status;
  int parnum = 0;
  int hand = TIOCM_DTR | TIOCM_RTS;
  int baudrate=B9600;

  /* get a file descriptor */
  if ((portfd=open(filename, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
    return -1;
  }

  /* check to see that the file is a terminal */ 
  if (!isatty(portfd)) {
    close(portfd);
    portfd = 0;
    return -1;
  }

  /* lock the file for ourselves */
  if (flock(portfd, LOCK_EX | LOCK_NB) < 0) {
    /* unable to get lock */
    close(portfd);
    portfd = 0;
    return -1;
  }

  /* get port attributes, store in oldterm */
  if (tcgetattr(portfd, &oldterm) < 0) {
    close(portfd);
    portfd = 0;
    return -1;
  }

  /* get port flags, save in oldflags */
  if ((oldflags = fcntl(portfd, F_GETFL)) < 0) {
    close(portfd);
    portfd = 0;
    return -1;
  }

  /* now we have read the old attributes for the port, we can restore them
   * upon exit. if we had done this bfore, and exited beore reading in the
   * old attributes, we would have overwritten the old settings with zeros.  
   *
   * this way, if we do exit before we get here, we simply rely on the OS closing
   * the port for us, which is fine as we haven't changed anything yet.
   */

  atexit(ir_close_port_on_exit);

  /* copy old attrs into new structure */
  portterm = oldterm;
  portflags = oldflags;

  /* remove old parity setting, size and stop setting */
  portterm.c_cflag &= ~PARENB; 
  portterm.c_cflag &= ~PARODD;
  portterm.c_cflag &= ~CSTOPB;
  portterm.c_cflag &= ~CSIZE;

  /* set character size, stop bits and parity */
  portterm.c_cflag |= CS8;
  portterm.c_cflag |= parnum;

  /* enable receiver, and don't change ownership */
  portterm.c_cflag |= CREAD | CLOCAL;

  /* disable flow control */
#ifdef CNEW_RTSCTS
  portterm.c_cflag &= ~CNEW_RTSCTS;
#else
#ifdef CRTSCTS
  portterm.c_cflag &= ~CRTSCTS;
#endif
#ifdef CRTSXOFF
  portterm.c_cflag &= ~CRTSXOFF;
#endif
#endif

  /* read characters immediately in non-canonical mode */
  /* Thanks to Bill Ryder, <bryder@sgi.com> */
  portterm.c_cc[VMIN] = 1; 
  portterm.c_cc[VTIME] = 1;

  /* set the input and output baud rate */
  cfsetispeed(&portterm, baudrate);
  cfsetospeed(&portterm, baudrate);

  /* set non-canonical mode (we don't want any fancy terminal processing!) */
  portterm.c_lflag = 0;

  /* Ignore breaks and make terminal raw and dumb. */
  portterm.c_iflag = 0;
  portterm.c_iflag |= IGNBRK;
  portterm.c_oflag &= ~OPOST;

  /* now clean the serial line and activate the new settings */
  tcflush(portfd, TCIOFLUSH);
  if (tcsetattr(portfd, TCSANOW, &portterm) < 0) {
    close(portfd);
    portfd = 0;
    return -1;
  }  

  /* set non-blocking */
  if (fcntl(portfd, F_SETFL, (portflags |= O_NONBLOCK)) < 0) {
    /* we've got this far -- we now have to restore old settings */
    ir_close_port();
    return -1;
  }

  /* we power down the device */
  if(ioctl(portfd, TIOCMGET, &status) < 0)
    {
      perror("could not get status\n");
      return -1;
    }
  status &= ~hand;
  if(ioctl(portfd, TIOCMSET, &status) < 0)
    {
      perror("could not set power down");
      return -1;
    }
  tcdrain(portfd);
  ir_usleep(IR_POWER_OFF_LATENCY);

  /* now power up again */
  status |= hand;
  if(ioctl(portfd, TIOCMSET, &status) < 0)
    {
      perror("could not set power up");
      return -1;
    }
  tcdrain(portfd);

  /* wait a little while for everything to settle through */
  ir_usleep(IR_POWER_ON_LATENCY);

  /* flush serial fifo */
  tcflush(portfd, TCIOFLUSH);

  return portfd;
}


/* close the port, restoring old settings */

int ir_close_port(void)
{
  int retval = 0;
  
  if (!portfd) {	/* already closed */
    errno = EBADF;
    return -1;
  }
  
  /* restore old settings */
  if (tcsetattr(portfd, TCSADRAIN, &oldterm) < 0) {
    retval = -1;
  }

  if (fcntl(portfd, F_SETFL, oldflags) < 0) {
    retval = -1;
  }
  
  close(portfd);
  portfd=0;
  
  return retval;
}


/* write a character.  nothing interesting happens here */

int ir_write_char(unsigned char data)
{
  if (write (portfd, &data, 1) != 1)
    return -1;
  else {
#ifdef DEBUG_COMM
    printf("{%02x}", data);fflush(stdout);
#endif
    return 0; 
  }
}


/* read a character, with a timeout.
 * timeout < 0	-  block indefinitely
 * timeout = 0  -  return immediately
 * timeout > 0  -  timeout after `timeout' microseconds
 *                 use the nice macros in irman.h to define sec, msec, usec
 */ 

int ir_read_char(long timeout)
{
  unsigned char rdchar;
  int ok;
  fd_set rdfds;
  struct timeval tv;
  
  FD_ZERO(&rdfds);
  FD_SET(portfd, &rdfds);
  
  /* block until something to read or timeout occurs.  select() is damn cool */
  
  if (timeout < 0) {
    ok = select(portfd + 1, &rdfds, NULL, NULL, NULL);
  } else {
    tv.tv_sec=timeout / 1000000;
    tv.tv_usec=(timeout % 1000000);
    ok = select(portfd + 1, &rdfds, NULL, NULL, &tv);
  }
  
  if (ok > 0) {
    ok = read(portfd, &rdchar, 1);
    if (ok == 0) {
      return EOF;
    }
#ifdef COMM_DEBUG
    printf("[%02x]", rdchar);fflush(stdout);
#endif
    return rdchar;
  } else if (ok < 0) {
    return EOF-1;
  } else {
    errno = ETIMEDOUT;
    return EOF-1;
  }
   
  return 0;
} 


/* just about the only function where we don't care about errors! */

void ir_clear_buffer(void)
{
  while (ir_read_char(IR_GARBAGE_TIMEOUT) >= 0)
    ;
}


/* some systems have it, some systems don't.  This just makes life easier,
 * hence I have left this function visible (also for irfunc.c)
 */

void ir_usleep(unsigned long usec)
{
  struct timeval tv;

  tv.tv_sec=usec / 1000000;
  tv.tv_usec=(usec % 1000000);
  (void) select(0, NULL, NULL, NULL, &tv);
}


/* end of irio.c */
