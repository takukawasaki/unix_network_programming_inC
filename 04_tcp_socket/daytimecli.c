#include	"unp.h"

int main(int argc, char **argv)
{
     int sockfd, n;
     int counter = 0;
     socklen_t len;
     
     char recvline[MAXLINE + 1];
     struct sockaddr_in servaddr, cliaddr;

     if (argc != 2) {
          err_quit("usage: a.out <IPAddress>");
     }
     if ( ( sockfd = socket(AF_INET,SOCK_STREAM, 0) ) < 0) {
          err_sys("socket error");
     }
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(13);

     if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
          err_quit("inet_pton error for %s", argv[1]);
     }
     if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
          err_sys("connect error");
     }else{ //exercise 4.2
          len = sizeof(cliaddr);
          Getsockname(sockfd, (SA *) &cliaddr, &len);
          printf("local addr: %s\n",
                 Sock_ntop((SA *) &cliaddr, len));
     }
     
     while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
          counter++;
          recvline[n] = 0;
          if (fputs(recvline, stdout) == EOF) {
               err_sys("fputs error");
          }
     }
     if (n < 0) {
          err_sys("read error");
     }
     printf("counter = %d\n",counter);
     exit(0);
     
}
