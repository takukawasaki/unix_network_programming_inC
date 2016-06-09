#include "unp.h"

#define SAP_NAME "sap.mcast.net"
#define SAP_PORT "9875"

void loop(int, socklen_t);


int
main(int argc, char *argv[])
{
     int sockfd;
     const int on = 1;
     socklen_t salen;
     struct sockaddr *sa;

     if (argc == 1) {
          sockfd = Udp_client(SAP_NAME, SAP_PORT,(void  *)&sa, &salen);
          
     if (argc == 4){
          
          sockfd = Udp_client(argv[1],argv[2], (void *)&sa, &salen);
     }
     if(argc != 4 && argc != 1){
          err_quit("usage: mysdr <mcast-addr> <port#> <interface-name>");
     }
     
     Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
     Bind(sockfd, sa, salen);

     Mcast_join(sockfd, sa, salen, (argc == 4) ? argv[3] : NULL, 0);
     loop(sockfd, salen);
     exit(0);

     }
}

void
loop(int sockfd, socklen_t salen)
{
     char buf[MAXLINE + 1];
     socklen_t len;
     ssize_t n;
     struct sockaddr *sa;
     struct sap_packet{
          uint32_t sap_header;
          uint32_t sap_src;
          char sap_data[1];
     } *sapptr;

     sa = Malloc(salen);

     for ( ;  ;  ) {
          len = salen;
          n = Recvfrom(sockfd, buf, MAXLINE, 0, sa, &len);
          buf[n] = 0;
          sapptr = (struct sap_packet *) buf;
          if ((n -= 2 * sizeof(uint32_t)) <= 0) {
               err_quit("n = %d",n);
          }
          printf("From %s\n%s\n", Sock_ntop(sa, len), sapptr->sap_data);
          
     }

     
}
     
