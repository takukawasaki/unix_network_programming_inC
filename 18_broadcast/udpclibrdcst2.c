#include "unp.h"
#include <setjmp.h>



static void recvfrom_alarm(int);
static sigjmp_buf jmpbuf;



void
dg_cli_brdcst(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
     int n;
     const int on = 1;
     char sendline[MAXLINE], recvline[MAXLINE];
     fd_set  rset;
     
     sigset_t sigset_alrm, sigset_empty;
     socklen_t len;
     struct sockaddr *preply_addr;

     preply_addr = Malloc(servlen);

     Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

     Signal(SIGALRM, recvfrom_alarm);

     while (Fgets(sendline, MAXLINE, fp) != NULL) {
          Sendto(sockfd, sendline, strlen(sendline), 0 ,pservaddr, servlen);
          alarm(5);

          for ( ;  ;  ) {
               if (sigsetjmp(jmpbuf, 1) != 0){
                    break;
               }
               
                         
               len =servlen;
               n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
               Sigprocmask(SIG_BLOCK, &sigset_alrm, NULL);
               if ( n < 0) {
                    if (errno == EINTR) {
                         break;
                         
                    }else{
                         err_sys("recvfrom error");
                    }
               }else{
                    recvline[n] = 0;
                    printf("from %s: %s",
                           Sock_ntop_host(preply_addr, len), recvline);
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




