#include "unp.h"


void sig_urg(int,int);

int
main(int argc, char *argv[])
{
     int size;
     int listenfd, connfd;
     char buff[100];

     if (argc == 2) {
          listenfd = Tcp_listen(NULL,argv[1], NULL);
          
     }
     if (argc == 3) {
          listenfd = Tcp_listen(argv[1], argv[2], NULL);
          
     }
     if (argc != 2 && argc != 3) {
          err_quit("usage: tcprecv03 <host> <port>");
     }
     size = 4096;
     Setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

     connfd = Accept(listenfd, NULL, NULL);

     Signal(SIGURG,sig_urg);
     Fcntl(connfd, F_SETOWN,getpid());


     for ( ;  ;  ) {
          pause();
     }
     

     return 0;
}

void
sig_urg(int signo, int fd)
{
     int n;
     char buff[2048];

     printf("SIGURG received\n");
     n = Recv(fd, buff, sizeof(buff) - 1 , MSG_OOB);
     buff[n] = 0;
     printf("read %d OOB byte\n",n);
}
