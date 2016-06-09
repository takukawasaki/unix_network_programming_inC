#include "unp.h"

static int sockfd; 
#define QSIZE 8                 /* input queue size */
#define MAXDG 4096              /* datagram max length */


typedef struct{
     void *dg_data;             /* pointer to datagram */
     size_t dg_len;             /* length of datagram */
     struct sockaddr *dg_sa;    /* pointer to sockaddr of client */
     socklen_t dg_salen;        /* length of sockaddr */
     
} DG;

static DG dg[QSIZE];            /* wait size of queur for process */
static long cntread[QSIZE + 1]; /* counter */

static int iget;                /* next entry */
static int iput;                /* signal handler save nextentry */
static int nqueue;              /* number of entry */
static socklen_t clilen;        /* max length of sockaddr */

static void sig_io(int);
static void sig_hup(int);


void
sigio_dg_echo(int sockfd_arg, SA *pcliaddr, socklen_t clilen_arg)
{
     int i;
     const int on = 1;
     sigset_t zeromask, newmask, oldmask;
     sockfd = sockfd_arg;
     clilen = clilen_arg;

     for (i = 0; i < QSIZE; i++) {
          dg[i].dg_data = Malloc(MAXDG);
          dg[i].dg_sa = Malloc(clilen);
          dg[i].dg_salen = clilen;
          
     }
     iget = iput = nqueue = 0;

     Signal(SIGHUP, sig_hup);
     Signal(SIGIO, sig_io);
     Fcntl(sockfd, F_SETOWN, getpid());
     Ioctl(sockfd, FIOASYNC, &on);
     Ioctl(sockfd, FIONBIO, &on);

     Sigemptyset(&zeromask);
     Sigemptyset(&oldmask);
     Sigemptyset(&newmask);
     Sigaddset(&newmask, SIGIO);

     Sigprocmask(SIG_BLOCK, &newmask, &oldmask);

     for ( ;  ;  ) {
          while (nqueue == 0) {
               sigsuspend(&zeromask);
               
          }
          Sigprocmask(SIG_SETMASK, &oldmask, NULL);
          Sendto(sockfd, dg[iget].dg_data, dg[iget].dg_len, 0,
                 dg[iget].dg_sa, dg[iget].dg_salen);

          if (++iget >= QSIZE) {
               iget = 0;
          }

          Sigprocmask(SIG_BLOCK, &newmask, &oldmask);
          nqueue--;
     }
}

static void
sig_io(int signo)
{
     ssize_t len;
     int nread;
     DG *ptr;

     for (nread = 0;;){
          if (nqueue >= QSIZE) {
               err_quit("receive overflow");
          }
          ptr = &dg[iput];
          ptr->dg_salen = clilen;
          len = recvfrom(sockfd, ptr->dg_data, MAXDG, 0,
                         ptr->dg_sa, &ptr->dg_salen);
          if (len < 0) {
               if (errno == EWOULDBLOCK) {
                    break;
               }else{
                    err_sys("recvfrom error");
               }
          }
          ptr->dg_len = len;
          nread++;
          nqueue++;
          if (++iput >= QSIZE) {
               iput = 0;
          }
     }
     cntread[nread]++;
     
}

static void
sig_hup(int signo)
{
     int i;
     for (i = 0; i < QSIZE; i++) {
          printf("cntread[%d] = %ld\n", i, cntread[i]);
     }
}

int
main(int argc, char *argv[])
{
     int sockfd;
     struct sockaddr_in servaddr,cliaddr;
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(SERV_PORT);
     Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
     sigio_dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
     
     return 0;
}



