#include "unp.h"
#include "pre.h"



static int nchildren;
static pid_t *pids;

void
sig_int(int signo)
{
     int i;
     void pr_cpu_time(void);
     for (i = 0; i < nchildren; i++) {
          kill(pids[i], SIGTERM);
          
     }
     while(wait(NULL) > 0){
          ;
     }

     if (errno != ECHILD) {
          err_sys("wait error");
     }

     pr_cpu_time();
     exit(0);
}


int
main(int argc, char *argv[])
{
     int listenfd, i;
     socklen_t addrlen;
     void sig_int(int);
     pid_t child_make(int, int,int);

     if (argc == 3) {
          listenfd = Tcp_listen(NULL, argv[1], &addrlen);
     }
     if (argc == 4) {
          listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
          
     }
     if (argc != 3 && argc != 4) {
          err_quit("usage: prefork_server01 [<host>] <port#> <#children>");
     }

     nchildren = atoi(argv[argc-1]);

     pids = Calloc(nchildren, sizeof(pid_t));

     for (i=0 ;i < nchildren ;i++  ) {
          pids[i] = child_make(i, listenfd, addrlen);
          
     }

     Signal(SIGINT, sig_int);

     for ( ;   ;  ) {
          pause();              /* 実際の処理は子プロセスが行う */
          
     }

     return 0;
}
