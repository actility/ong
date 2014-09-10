/*
 * Strongly inspired from this post:
 * http://fixunix.com/ssh/547909-ssh-client-insists-dev-tty.html
 */

#define _XOPEN_SOURCE
#include <features.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ioctl.h>

#define min(x,y) ( (x)<(y)?(x):(y) )
#define max(x,y) ( (x)>(y)?(x):(y) )
#define FALSE 0
#define TRUE 1


int pty_get(char *name);

/* ----------------------------------------------------------------------
 * Chunk of code lifted from PuTTY's misc.c to manage buffers of
 * data to be written to an fd.
 */

#define BUFFER_GRANULE  512

typedef struct bufchain_tag {
  struct bufchain_granule *head, *tail;
  int buffersize;           /* current amount of buffered data */
} bufchain;
struct bufchain_granule {
  struct bufchain_granule *next;
  int buflen, bufpos;
  char buf[BUFFER_GRANULE];
};

static void bufchain_init(bufchain *ch)
{
  ch->head = ch->tail = NULL;
  ch->buffersize = 0;
}

static int bufchain_size(bufchain *ch)
{
  return ch->buffersize;
}

static void bufchain_add(bufchain *ch, const void *data, int len)
{
  const char *buf = (const char *)data;

  if (len == 0) return;

  ch->buffersize += len;

  if (ch->tail && ch->tail->buflen < BUFFER_GRANULE) {
    int copylen = min(len, BUFFER_GRANULE - ch->tail->buflen);
    memcpy(ch->tail->buf + ch->tail->buflen, buf, copylen);
    buf += copylen;
    len -= copylen;
    ch->tail->buflen += copylen;
  }
  while (len > 0) {
    int grainlen = min(len, BUFFER_GRANULE);
    struct bufchain_granule *newbuf;
    newbuf = (struct bufchain_granule *)
    malloc(sizeof(struct bufchain_granule));
    if (!newbuf) {
      fprintf(stderr, "filter: out of memory\n");
      exit(1);
    }
    newbuf->bufpos = 0;
    newbuf->buflen = grainlen;
    memcpy(newbuf->buf, buf, grainlen);
    buf += grainlen;
    len -= grainlen;
    if (ch->tail)
      ch->tail->next = newbuf;
    else
      ch->head = ch->tail = newbuf;
    newbuf->next = NULL;
    ch->tail = newbuf;
  }
}

static void bufchain_consume(bufchain *ch, int len)
{
  struct bufchain_granule *tmp;

  assert(ch->buffersize >= len);
  while (len > 0) {
    int remlen = len;
    assert(ch->head != NULL);
    if (remlen >= ch->head->buflen - ch->head->bufpos) {
      remlen = ch->head->buflen - ch->head->bufpos;
      tmp = ch->head;
      ch->head = tmp->next;
      free(tmp);
      if (!ch->head)
        ch->tail = NULL;
    } else
      ch->head->bufpos += remlen;
    ch->buffersize -= remlen;
    len -= remlen;
  }
}

static void bufchain_prefix(bufchain *ch, void **data, int *len)
{
  *len = ch->head->buflen - ch->head->bufpos;
  *data = ch->head->buf + ch->head->bufpos;
}

/* ----------------------------------------------------------------------
 * End of bufchain stuff. Main program begins.
 */


/*
 * Allocate a pty. Returns a file handle to the master end of the
 * pty, and stores the full pathname to the slave end into `name'.
 * `name' is a buffer of size at least FILENAME_MAX.
 * 
 * Does not return on error.
 */
int pty_get(char *name)
{
    int fd;

    fd = open("/dev/ptmx", O_RDWR);

    if (fd < 0) {
	perror("/dev/ptmx: open");
	exit(1);
    }

    if (grantpt(fd) < 0) {
	perror("grantpt");
	exit(1);
    }
    
    if (unlockpt(fd) < 0) {
	perror("unlockpt");
	exit(1);
    }

    name[FILENAME_MAX-1] = '\0';
    strncpy(name, ptsname(fd), FILENAME_MAX-1);

    {
	struct winsize ws;
	struct termios ts;

	if (!ioctl(0, TIOCGWINSZ, &ws))
	    ioctl(fd, TIOCSWINSZ, &ws);
	if (!tcgetattr(0, &ts))
	    tcsetattr(fd, TCSANOW, &ts);
    }

    return fd;
}

#define LOCALBUF_LIMIT 65536

int signalpipe[2];

void sigchld(int sig)
{
  write(signalpipe[1], "C", 1);
}

struct termios oldattrs, newattrs;

void attrsonexit(void)
{
  tcsetattr(0, TCSANOW, &oldattrs);
}

int main(int argc, char **argv)
{
  int masterr, masterw, slaver, slavew, pid;
  char ptyname[FILENAME_MAX];
  bufchain tochild, tostdout;
  int tochild_active, tostdout_active, fromstdin_active, fromchild_active;
  int exitcode = -1;
  pid_t childpid = -1;

        --argc, ++argv;         /* point at argument after "--" */

  /*
   * Allocate the pipe for transmitting signals back to the
   * top-level select loop.
   */
  if (pipe(signalpipe) < 0) {
    perror("pipe");
    return 1;
  }

  /*
   * Now that pipe exists, we can set up the SIGCHLD handler to
   * write to one end of it. We needn't already know details like
   * which child pid we're waiting for, because we don't need that
   * until we respond to reading the far end of the pipe in the
   * main select loop.
   */
  signal(SIGCHLD, sigchld);

  /*
   * Allocate the pty or pipes.
   */
  masterr = pty_get(ptyname);
  masterw = dup(masterr);
  slaver = open(ptyname, O_RDWR);
  slavew = dup(slaver);
  if (slaver < 0) {
    perror("slave pty: open");
    return 1;
  }

  bufchain_init(&tochild);
  bufchain_init(&tostdout);
  tochild_active = tostdout_active = TRUE;
  fromchild_active = fromstdin_active = TRUE;

  /*
   * Fork and execute the command.
   */
  pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    int i;
    /*
     * We are the child.
     */
    close(masterr);
    close(masterw);

    fcntl(slaver, F_SETFD, 0);    /* don't close on exec */
    fcntl(slavew, F_SETFD, 0);    /* don't close on exec */
    close(0);
    dup2(slaver, 0);
    close(1);
    dup2(slavew, 1);
    int fd;
    close(2);
    dup2(slavew, 2);
    setsid();
    setpgid(0, 0);
    tcsetpgrp(0, getpgrp());
    if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
      ioctl(fd, TIOCNOTTY);
      close(fd);
    }
    ioctl(slavew, TIOCSCTTY);
    
    /* Close everything _else_, for tidiness. */
    for (i = 3; i < 1024; i++)
      close(i);
    if (argc > 0) {
      execvp(argv[0], argv);     /* assumes argv has trailing NULL */
    } else {
      execl(getenv("SHELL"), getenv("SHELL"), NULL);
    }
    /*
     * If we're here, exec has gone badly foom.
     */
    perror("exec");
    exit(127);
  }

  /*
   * Now we're the parent. Close the slave fds and start copying
   * stuff back and forth.
   */
  close(slaver);
  close(slavew);
  childpid = pid;

  tcgetattr(0, &oldattrs);
  newattrs = oldattrs;
  newattrs.c_iflag &= ~(IXON | IXOFF | ICRNL | INLCR);
  newattrs.c_oflag &= ~(ONLCR | OCRNL);
  newattrs.c_lflag &= ~(ISIG | ICANON | ECHO);
  atexit(attrsonexit);
  tcsetattr(0, TCSADRAIN, &newattrs);
  

  while (1) {
    fd_set rset, wset;
    char buf[65536];
    int maxfd, ret;

    FD_ZERO(&rset);
    FD_ZERO(&wset);
    maxfd = 0;

    FD_SET(signalpipe[0], &rset);
    maxfd = max(signalpipe[0]+1, maxfd);

    if (tochild_active && bufchain_size(&tochild)) {
      FD_SET(masterw, &wset);
      maxfd = max(masterw+1, maxfd);
    }
    if (tostdout_active && bufchain_size(&tostdout)) {
      FD_SET(1, &wset);
      maxfd = max(1+1, maxfd);
    }
    if (fromstdin_active && bufchain_size(&tochild) < LOCALBUF_LIMIT) {
      FD_SET(0, &rset);
      maxfd = max(0+1, maxfd);
    }
    if (fromchild_active && bufchain_size(&tostdout) < LOCALBUF_LIMIT) {
      FD_SET(masterr, &rset);
      maxfd = max(masterr+1, maxfd);
    }

    do {
      ret = select(maxfd, &rset, &wset, NULL, NULL);
    } while (ret < 0 && (errno == EINTR || errno == EAGAIN));

    if (ret < 0) {
      perror("select");
      return 1;
    }

    if (FD_ISSET(masterr, &rset)) {

      if (FD_ISSET(masterr, &rset)) {
        ret = read(masterr, buf, sizeof(buf));
        if (ret <= 0) {
          /*
           * EIO from a pty master just means end of
           * file, annoyingly. Why can't it report
           * ordinary EOF?
           */
          if (errno == EIO)
            ret = 0;
          if (ret < 0) {
            perror("child process: read");
          }
          close(masterr);
          fromchild_active = FALSE;
          ret = 0;
        }
      } else
        ret = 0;

      if (ret) {
        bufchain_add(&tostdout, buf, ret);
      }
    }
    if (FD_ISSET(0, &rset)) {

      if (FD_ISSET(0, &rset)) {
        ret = read(0, buf, sizeof(buf));
        if (ret <= 0) {
          if (ret < 0) {
            perror("stdin: read");
          }
          close(0);
          fromstdin_active = FALSE;
          ret = 0;
        }
      } else
        ret = 0;

      if (ret) {
        bufchain_add(&tochild, buf, ret);
      }
    }
    if (FD_ISSET(1, &wset)) {
      void *data;
      int len, ret;
      bufchain_prefix(&tostdout, &data, &len);
      if ((ret = write(1, data, len)) < 0) {
        perror("stdout: write");
        close(1);
        close(masterr);
        tostdout_active = fromchild_active = FALSE;
      } else
        bufchain_consume(&tostdout, ret);
    }
    if (FD_ISSET(masterw, &wset)) {
      void *data;
      int len;
      bufchain_prefix(&tochild, &data, &len);
      if ((ret = write(masterw, data, len)) < 0) {
        perror("child process: write");
        close(0);
        close(masterw);
        tochild_active = fromstdin_active = FALSE;
      } else
        bufchain_consume(&tochild, ret);
    }
    if (FD_ISSET(signalpipe[0], &rset)) {
      ret = read(signalpipe[0], buf, 1);
      if (ret == 1 && buf[0] == 'C') {
        int pid, code;
        pid = wait(&code);     /* reap the exit code */
        if (pid == childpid)
          exitcode = code;
      }
    }

    /*
     * If there can be no further data from a direction (the
     * input fd has been closed and the buffered data is used
     * up) but its output fd is still open, close it.
     */
    if (!fromstdin_active && !bufchain_size(&tochild) && tochild_active) {
      tochild_active = FALSE;
      close(masterw);
    }
    if (!fromchild_active && !bufchain_size(&tostdout) && tostdout_active){
      tostdout_active = FALSE;
      close(1);
    }

    /*
     * Termination condition with pipes is that there's still
     * data flowing in at least one direction.
     * 
     * Termination condition for a pty-based run is that the
     * child process hasn't yet terminated and/or there is
     * still buffered data to send.
     */
    if (exitcode < 0)
      /* process is still active */;
    else if (tochild_active && bufchain_size(&tochild))
      /* data still to be sent to child's children */;
    else if (tostdout_active && bufchain_size(&tostdout))
      /* data still to be sent to stdout */;
    else
      break;           /* terminate */
  }

  close(masterw);
  close(masterr);

  if (exitcode < 0) {
    int pid, code;
    pid = wait(&code);
    exitcode = code;
  }

  return (WIFEXITED(exitcode) ? WEXITSTATUS(exitcode) :
      128 | WTERMSIG(exitcode));
}
