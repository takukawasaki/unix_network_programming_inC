#include "unpthread.h"

static void *doit(void *);

int
main(int argc, char *argv[])
{
     int listenfd, *iptr;
     socklen_t len, addrlen;
     struct sockaddr *cliaddr;

     pthread_t tid;

     if (argc == 2) {
          listenfd = Tcp_listen(NULL, argv[1], &addrlen);
          
     }
     if (argc == 3) {
          listenfd = Tcp_listen(argv[1], argv[2],&addrlen);
     }
     if (argc != 3 && argc != 2) {
          err_quit("usage : tcpserver_thread [<host>] <port or service>");
     }

     cliaddr = Malloc(addrlen);

     for ( ;  ;  ) {
          len = addrlen;
          iptr = Malloc(sizeof(int));
          *iptr = Accept(listenfd, cliaddr, &len);
          Pthread_create(&tid, NULL, &doit, iptr);
     }
     return 0;
}

static void *
doit(void *arg)
{
     int connfd;
     connfd = *((int *) arg);
     free(arg);

     Pthread_detach(pthread_self());
     str_echo(connfd);
     Close(connfd);
     return (NULL);
}
