#include "unp.h"


int main(int argc, char *argv[])
{
     int sockfd;
     struct sockaddr_in servaddr;
     struct linger ling;
     
     if (argc != 2) {
          err_quit("usage: tcpcli <IPaddress>");
     }
     sockfd = Socket(AF_INET, SOCK_STREAM, 0);
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(SERV_PORT); /* SERV_PORT 9877 */
     Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
     
     Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
     
     str_cli(stdin, sockfd);

     ling.l_onoff = 1;
     ling.l_linger = 0;
     Setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
     
     exit(0);
          

}
