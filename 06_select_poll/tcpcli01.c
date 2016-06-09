#include "unp.h"


void str_cli_select(FILE *fp, int sockfd)
{
     int maxfdpl;
     fd_set rset;               /* 読み出し */
     char sendline[MAXLINE], recvline[MAXLINE];

     FD_ZERO(&rset);

     for(;;){
          FD_SET(fileno(fp), &rset);
          FD_SET(sockfd, &rset);
          maxfdpl = max(fileno(fp), sockfd) + 1;
          Select(maxfdpl, &rset , NULL, NULL, NULL);

          if (FD_ISSET(sockfd, &rset)) {
               if (Readline(sockfd, recvline, MAXLINE) == 0) {
                    err_quit("str_cli: server terminated prematurely");
               }
               Fputs(recvline, stdout);
          }
          if (FD_ISSET(fileno(fp), &rset)) {
               if (Fgets(sendline, MAXLINE, fp) == NULL) {
                    return;
               }
               Writen(sockfd, sendline, strlen(sendline));
          }
     }
     
}


int main(int argc, char *argv[])
{
     int sockfd;
     struct sockaddr_in servaddr;

     if (argc != 2) {
          err_quit("usage: tcpcli <IPaddress>");
     }
     sockfd = Socket(AF_INET, SOCK_STREAM, 0);
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(SERV_PORT); /* SERV_PORT 9877 */
     Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
     
     Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
     
     str_cli_select(stdin, sockfd);
     exit(0);
          

}
