#include "unp.h"
#include <setjmp.h>



static void recvfrom_alarm(int);
static sigjmp_buf jmpbuf;
static int pipefd[2];

void
dg_cli_brdcst(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
     int n, maxfdp1;
     const int on = 1;
     char sendline[MAXLINE], recvline[MAXLINE];
     fd_set  rset;
     socklen_t len;
     struct sockaddr *preply_addr;

     preply_addr = Malloc(servlen);

     Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

     Pipe(pipefd);
     maxfdp1 = max(sockfd, pipefd[0]) + 1;
     FD_ZERO(&rset);
     
     Signal(SIGALRM, recvfrom_alarm);

     while (Fgets(sendline, MAXLINE, fp) != NULL) {
          Sendto(sockfd, sendline, strlen(sendline), 0 ,pservaddr, servlen);
          alarm(5);

          for ( ;  ;  ) {
               FD_SET(sockfd, &rset);
               FD_SET(pipefd[0], &rset);
               if ((n = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
                    if (errno == EINTR){
                         continue;
                    }else{
                         err_sys("select error");
                    }
               }
               if (FD_ISSET(sockfd, &rset)) {
                    len = servlen;
                    n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
                    recvline[n] = 0;
                    printf("from %s: %s",
                           Sock_ntop_host(preply_addr, len), recvline);
                    
               }
               if (FD_ISSET(pipefd[0], &rset)) {
                    Read(pipefd[0], &n, 1);
                    break;
               }
          }

     }
}

static void
recvfrom_alarm(int signo)
{

     siglongjmp(jmpbuf, 1);
}

int main(int argc, char *argv[])
{
     int sockfd;
     struct sockaddr_in servaddr;

     if (argc != 2) {
          err_quit("usage: udpclibrdcst <IP Address> ");
          
     }

     sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(SERV_PORT);

     Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
     dg_cli_brdcst(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));
     exit(0);
}




