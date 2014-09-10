

#ifndef __ADMIN_CLI__H_
#define __ADMIN_CLI__H_


#define CLI_MAX_FD  32

#define CLI_NFOUND  0
#define CLI_PROMPT  1
#define CLI_DISC  2
#define CLI_KEY   3
#define CLI_EMPTY 4
#define CLI_MORE  5

#define     ABS(x)          ((x) > 0 ? (x) : -(x))

// a buffer waiting for "poll write event" on a tcp/socket for CLI
typedef struct  s_lout
{
  struct list_head list;  // the chain list of pending data
  char  *o_buf;   // a buffer
  int o_len;    // lenght of o_buf
} t_lout;


typedef struct
{
  pthread_mutex_t mutex;
  time_t startTimeMono;
  unsigned long sentReqOK;
  unsigned long sentReqErr;
  unsigned long rcvRspOK;
  unsigned long rcvRspErr;

  unsigned long statusCodes[600]; 

} t_stats;

// a CLI connection on admin port
typedef struct
{
  int cl_fd;    // associated socket
  int cl_pos;   // position in cl_cmd
  char  cl_cmd[1000]; // the user input accumulated until '\n' | '\r'
  time_t  cl_lasttcpread; // time of last remote read
  time_t  cl_lasttcpwrite;// time of last remote write
  int cl_nbkey; // number of retry for password
  int cl_okkey; // password ok
  t_lout  cl_lout;  // list of pending data
  struct httpClient_t * cl_client;
  pthread_t tidRInThread;
  int tidRInThreadRunning;
  int verbose;
  t_stats stats;
} t_cli;

int AdmTcpMsgCount(t_cli *cl);
void  AdmPrintSync(int fd, char *fmt, ...);
void  AdmPrint(t_cli *cl, char *fmt, ...);
void  AdmPrintTable(t_cli *cl, int nb, int tab[], char *fmt, char pstart, char pend, char psep);
void  AdmPrintDefTable(t_cli *cl, int nb, int tab[], char *fmt);
void  AdmPrintTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt, char pstart, char pend, char psep);
void  AdmPrintDefTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt);

char  *AdmAdvance(char *pt);
char  *AdmAccept(char *pt);
char  *AdmAcceptKeep(char *pt, char *dst);

int CB_AdmTcpRequest(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpEvent(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpListen(void *tb, int fd, void *r1, void *r2, int revents);
void  AdmTcpInit();
void  AdmClockSc(time_t now);


#endif

