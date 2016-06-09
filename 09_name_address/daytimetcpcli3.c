#include "unp.h"

int main(int argc, char *argv[])
{
     int sockfd, n;
     char recvline[MAXLINE];
     struct sockaddr_in servaddr;
     struct sockaddr_in6 servaddr6;
     struct sockaddr *sa;
     socklen_t salen;
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
          if (hp->h_addrtype == AF_INET) {
               sa = (SA *)&servaddr;
               salen = sizeof(servaddr);
          }else if(hp->h_addrtype == AF_INET6){
               sa = (SA *)&servaddr6;
               salen = sizeof(servaddr6);
          }else{
               err_quit("unknown addrtype %d", hp->h_addrtype);
          }
          
          bzero(&servaddr, sizeof(servaddr));
          sa->sa_family = hp->h_addrtype;
          sock_set_port(sa, salen, sp->s_port);
          sock_set_addr(sa, salen, *pptr);

          printf("trying %s\n",
                 Sock_ntop(sa, salen));
          if (connect(sockfd,sa,salen) == 0) {
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
