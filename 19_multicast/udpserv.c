#include "unp.h"



int main(int argc, char *argv[])
{
     int   udpfd;
     char mesg[MAXLINE];
     socklen_t len;
     const int on = 1;
     struct sockaddr_in cliaddr, servaddr;
     void sig_chld(int);
     time_t ticks;
     
     udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
     Setsockopt(udpfd, SOL_SOCKET,SO_REUSEADDR,&on, sizeof(on));

     bzero(&servaddr, sizeof(servaddr));

     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port = htons(SERV_PORT);
     
     Bind(udpfd, (SA *) &servaddr, sizeof(servaddr) );

     Signal(SIGCHLD, sig_chld);

     for(;;){
          len = sizeof(cliaddr);
          
          Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);

          printf("Connection from %s, port %d\n",
                 Inet_ntop(AF_INET, &cliaddr.sin_addr,mesg,sizeof(mesg)),
                 ntohs(cliaddr.sin_port));
       
          ticks = time(NULL);
          snprintf(mesg, MAXLINE, "%.24s\r\n", ctime(&ticks));
          Sendto(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, len);

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


