#include "unp.h"


void sig_chld(int);
void str_echo2(int sockfd)
{
     
     long arg1, arg2;
     ssize_t n;
     char line[MAXLINE];
     heartbeat_serv(sockfd, 1, 5);
     for(;;){
          if ( (n = Readline(sockfd, line, MAXLINE) ) == 0 ) {
               return ;         /* 他方のエンドによるクローズ */
          }
          if (sscanf(line, "%ld%ld", &arg1, &arg2) == 2) {
               snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
               
          }else{
               snprintf(line , sizeof(line), "input error\n");
          }
          n = strlen(line);
          Writen(sockfd, line, n);
     }
}


int main(int argc, char *argv[])
{
     int listenfd, connfd;
     pid_t childpid;
     socklen_t clilen;
     struct sockaddr_in cliaddr, servaddr;
     void sig_chld(int);

     listenfd = Socket(AF_INET, SOCK_STREAM, 0);

     bzero(&servaddr, sizeof(servaddr));

     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(SERV_PORT);

     Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
     Listen(listenfd, LISTENQ);
     Signal(SIGCHLD, sig_chld);

     
     for (;;) {
          clilen = sizeof(cliaddr);
          
          if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
               if (errno == EINTR) {
                    continue;
               }else{
                    err_sys("accept error");
               }
               
          }
          if ( (childpid = Fork()) == 0) {	/* child process */
               Close(listenfd);	/* close listening socket */
               str_echo2(connfd);	/* process the request */
               
               exit(0);
          }
          Close(connfd);
          
     }
}

void sig_chld(int signo)
{
     pid_t pid;
     int stat;

     while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
          printf("child %di terminated\n", pid);
     }
     return;
}

 
