#include "unp.h"
#include "sum.h"

void str_echo3(int sockfd)
{
     
     ssize_t n;
     struct args args;
     struct result result;
     

     for(;;){
          if ((n = Readn(sockfd, &args, sizeof(args)) == 0)) {
               return;
          }
          result.sum = args.arg1 + args.arg2;
          Writen(sockfd, &result, sizeof(result));
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
               str_echo3(connfd);	/* process the request */
               
               exit(0);
          }
          Close(connfd);
          
     }
}

 
