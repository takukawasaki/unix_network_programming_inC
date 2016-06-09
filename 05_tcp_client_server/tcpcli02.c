#include "unp.h"
#include "sum.h"

void
str_cli_with_send_binary(FILE *fp, int sockfd)
{
     char	sendline[MAXLINE], recvline[MAXLINE];
     struct args args;
     struct result result;
     
     while (Fgets(sendline, MAXLINE, fp) != NULL) {
          if (sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2) != 2) {
               printf("invalid input: %s", sendline);
               continue;
          }
       
          Writen(sockfd, &args, sizeof(args));

          if (Readn(sockfd, &result, sizeof(result)) == 0)
               err_quit("str_cli: server terminated prematurely");
          printf("%ld\n",result.sum);
     }
}


int main(int argc, char *argv[])
{
     int i,sockfd[5];
     struct sockaddr_in servaddr;

     if (argc != 2) {
          err_quit("usage: tcpcli <IPaddress>");
     }
     for (i = 0; i < 5; i++){
          sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);
          bzero(&servaddr, sizeof(servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_port = htons(SERV_PORT); /* SERV_PORT 9877 */
          Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
          
          Connect(sockfd[i], (SA *) &servaddr, sizeof(servaddr));
     }
     str_cli(stdin, sockfd[0]);
     exit(0);

}
