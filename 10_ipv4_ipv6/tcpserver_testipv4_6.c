#include "unp.h"

void sig_chld(int signo)
{
     pid_t pid;
     int stat;

     pid = wait(&stat);
     printf("child %d terminated\n", pid);
     return;
}

int main(int argc, char *argv[])
{
     int listenfd, connfd;

     socklen_t clilen;
     const int on = 1;
     struct sockaddr_in servaddr, cliaddr;
     void sig_chld(int);
     pid_t childpid;
     
     listenfd = Socket(AF_INET, SOCK_STREAM, 0);

     bzero(&servaddr,sizeof(servaddr));
     
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(SERV_PORT);

     Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
     
     Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
     Listen(listenfd, LISTENQ);


     Signal(SIGCHLD, sig_chld);
     
     for ( ;  ;  ) {
          clilen = sizeof(cliaddr);
          connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
          if ((childpid = Fork())== 0) {
               Close(listenfd);

               str_echo(connfd);
               exit(0);
          }
          Close(connfd);
          
     }

}

