#include "unp.h"
#define MAXSOCKADDR 100

int main(int argc, char *argv[])
{
     int sockfd, n;
     char recvline[MAXLINE + 1];
     socklen_t len;
     struct sockaddr *sa;
     if (argc != 3) {
          err_quit("usage: daytimetcpcli <hostname/idaddr> <service/port#>");
     }
     sockfd = Tcp_connect(argv[1], argv[2]);

     sa = Malloc(MAXSOCKADDR);
     len = MAXSOCKADDR;

     Getpeername(sockfd, sa, &len);
     printf("connected to %s\n",Sock_ntop_host(sa, len));

     while ((n = Read(sockfd, recvline, MAXLINE))) {
          recvline[n] = 0;
          Fputs(recvline, stdout);
     }
     exit(0);
}
