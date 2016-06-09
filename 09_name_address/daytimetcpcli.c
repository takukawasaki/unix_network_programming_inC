#include "unp.h"

int main(int argc, char *argv[])
{
     int sockfd, n;
     char recvline[MAXLINE];
     struct sockaddr_in servaddr;
     struct in_addr **pptr;
     struct hostent *hp;
     struct servent *sp;

     if (argc != 3) {
          err_quit("usage: daytimetcpcli <hostname> <service>");
     }
     if ((hp = gethostbyname(argv[1])) == NULL) {
          err_quit("host name error for %s: %s", hstrerror(h_errno));
     }
     if ((sp = getservbyname(argv[2],"tcp")) == NULL) {
          err_quit("getservbyname error for %s",argv[2]);
     }

     pptr = (struct in_addr **) hp->h_addr_list;
     for ( ; *pptr != NULL; pptr++) {
          sockfd = Socket(AF_INET, SOCK_STREAM, 0);
          bzero(&servaddr, sizeof(servaddr));

          servaddr.sin_family = AF_INET;
          servaddr.sin_port = sp->s_port;
          memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
          printf("trying %s\n",
                 Sock_ntop((SA *) &servaddr, sizeof(servaddr)));
          if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == 0) {
               break;

          }
          err_ret("connect error");
          close(sockfd);
          
     }
     if (*pptr == NULL) {
          err_quit("unable to quit");
     }
     while ((n = Read(sockfd, recvline, MAXLINE)) > 0) {
          recvline[n] = 0;
          Fputs(recvline,stdout);
     }

     
     exit(0);
     
}
