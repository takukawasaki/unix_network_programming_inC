#include "unp.h"
#include <limits.h>             /* OPEN_MAX  */

int main(int argc, char *argv[])
{
     #define OPEN_MAX 256

     int i, maxi, maxfd, listenfd, connfd, sockfd;
     int nready;
     ssize_t n;
     char line[MAXLINE];
     socklen_t clilen;
     
     struct pollfd client[OPEN_MAX];
     struct sockaddr_in cliaddr, servaddr;
     
     listenfd = Socket(AF_INET, SOCK_STREAM, 0);

     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(SERV_PORT);

     Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

     Listen(listenfd, LISTENQ);

     client[0].fd = listenfd;
     client[0].events = POLLRDNORM;
     for (i = 1;i < OPEN_MAX; i++) {
          client[i].fd = -1;    /* -1 は利用可能なエントリー */
     }

     maxi = 0;                  /* client[]配列の最大添字 */
     
     for (;;) {
          nready = Poll(client, maxi + 1, INFTIM);
          
          if (client[0].revents & POLLRDNORM) { /* 新規クライアント */
               clilen = sizeof(cliaddr);
               connfd = Accept(listenfd, (SA *)&cliaddr, sizeof(cliaddr));
               for (i = 0; i < OPEN_MAX; i++) {
                    if (client[i].fd < 0) {
                         client[i].fd = connfd; /* ディスクリプタの保存 */
                         break;
                    }
               }

               if (i == OPEN_MAX) {
                    err_quit("too many clients");
               }

               client[i].events = POLLRDNORM;
               
               if (i > maxi) {
                    maxi = i;
               }
               if (i > maxi) {
                    maxi = i;   /* client 配列の最大添字 */
               }
               if (-nready <= 0) {
                    continue;   /* 読み出し可能ディスクリプタはない */
                    
               }

          }
          
          for (i = 0; i <= maxi; i++) { /* client からのデータを検査 */
               if ((sockfd = client[i].fd) < 0) {
                    continue;
               }
               if (client[i].revents & (POLLRDNORM | POLLERR)) {
                    if ((n = Readline(sockfd, line, MAXLINE)) < 0) {
                         if (errno == ECONNRESET) {
                              
                         Close(sockfd);
                         client[i].fd = -1;
                         
                         }else{
                              err_sys("readline error");
                         }
                         
                         
                    }else if(n == 0){
                         Close(sockfd);
                         client[i].fd = -1;
                    }else {
                         Writen(sockfd, line, n);
                    }
                    if (-nready <= 0) {
                         break;
                    }
               }
          }
     }
}

 
