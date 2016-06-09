#include "unp.h"


int main(int argc, char *argv[])
{

     int i, maxi, maxfd, listenfd, connfd, sockfd;
     int nready, client[FD_SETSIZE];
     ssize_t n;
     const int on = 1;
     fd_set rset, allset;
     char line[MAXLINE];
     socklen_t clilen;
     struct sockaddr_in cliaddr, servaddr;
     listenfd = Socket(AF_INET, SOCK_STREAM, 0);

     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(8888);

     Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
     
     Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

     Listen(listenfd, LISTENQ);

     maxfd = listenfd;          /* 初期化 */

     maxi = -1;                 /* client[] の添字 */
     

     for (i = 0; i < FD_SETSIZE; i++) {
          client[i] = -1;       /* -1 は利用可能なエントリを示す */
          
     }
     FD_ZERO(&allset);
     FD_SET(listenfd, &allset);

     for (;;) {
          rset  = allset;       /* 構造体の代入 */
          nready = Select(maxfd + 1, &rset, NULL,NULL,NULL);
          if (FD_ISSET(listenfd, &rset)) { /* 新規クライアント */
               clilen = sizeof(cliaddr);
               connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
               for (i = 0; i < FD_SETSIZE; i++) {
                    if (client[i] < 0) {
                         client[i] = connfd; /* ディスクリプタの保存 */
                         break;
                    }
               }

               if (i == FD_SETSIZE) {
                    err_quit("too many clients");
               }

               FD_SET(connfd, &allset); /* 新しいディスクリプタを集合に加える */
               if (connfd > maxfd) {
                    maxfd = connfd; /* select 用 */
               }
               if (i > maxi) {
                    maxi = i;   /* client 配列の最大添字 */
               }
               if (-nready <= 0) {
                    continue;   /* 読み出し可能ディスクリプタはない */
                    
               }

          }
          for (i = 0; i <= maxi; i++) { /* client からのデータを検査 */
               if ((sockfd = client[i]) < 0) {
                    continue;
               }
               if (FD_ISSET(sockfd, &rset)) {
                    if ((n = Readline(sockfd, line, MAXLINE)) == 0) {
                         /* client がクローズした */
                         Close(sockfd);
                         FD_CLR(sockfd, &allset);
                         client[i] = -1;
                    }else{
                         Writen(sockfd,line, n);
                    }
                    if (-nready <= 0) {
                         break; /* 読み出し可能ディスクリプタなし */
                    }
               }
          }
     }
}

 
