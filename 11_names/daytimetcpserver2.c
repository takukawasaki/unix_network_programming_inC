#include "unp.h"
#include <time.h>


int main(int argc, char *argv[])
{
     int listenfd, connfd;
     socklen_t addrlen, len;

     char buff[MAXLINE];
     time_t ticks;
     struct sockaddr *cliaddr;

     if (argc == 2) {
          listenfd = Tcp_listen(NULL, argv[1], &addrlen);
     }
     if(argc == 3){
          listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
     }
     if (argc != 2 && argc != 3) {
          err_quit("usage: daytimetcpserv2 [<host>] <service or port#>");
     }
     
     cliaddr = Malloc(addrlen);
 
     for ( ;  ;  ) {
          len = addrlen;
          connfd = Accept(listenfd, cliaddr, &len);
          printf("connection from %s\n", Sock_ntop(cliaddr, len));
          ticks = time(NULL);
          snprintf(buff, sizeof(buff), "%.24s\r\n",ctime(&ticks));
          Write(connfd, buff, strlen(buff));

          Close(connfd);
     }

     
     return 0;
}
