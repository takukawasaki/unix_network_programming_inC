#include "unp.h"

void sig_int(int signo)
{
     void pr_cpu_time(void);
     pr_cpu_time();
     exit(0);
}



int
main(int argc, char *argv[])
{
     int listenfd, connfd;
     pid_t childpid;
     void sig_chld(int), sig_int(int), web_child(int);

     socklen_t clilen, addrlen;
     struct sockaddr *cliaddr;

     if (argc == 2) {
          listenfd = Tcp_listen(NULL, argv[1], &addrlen);
          
     }
     if (argc == 3) {
          listenfd = Tcp_listen(argv[1], argv[2] , &addrlen);
          
     }
     if (argc != 2 && argc != 3) {
          err_quit("usage: trad_server [<host>] <port#>");
     }

     cliaddr = Malloc(addrlen);
     Signal(SIGCHLD, sig_chld);
     Signal(SIGINT, sig_int);

     for ( ;  ;  ) {
          if ((connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
               if (errno != EINTR) {
                    continue;
               }else{
                    err_sys("accept error");
               }
          }
          if ((childpid = Fork()) == 0) {
               Close(listenfd);
               web_child(connfd);
               exit(0);
          }
          Close(connfd);
     }

}

pid_t
child_make(int i, int listenfd, int addrlen)
{

     pid_t pid;
     void child_main(int, int,int);
     if ((pid = Fork()) == 0) {
          return (pid);
     }

     child_main(i, listenfd, addrlen);
}


