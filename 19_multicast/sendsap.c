#include "unp.h"
#include <sys/utsname.h>
#define SENDRATE 5


void recv_all(int , socklen_t);
void send_all(int, SA *, socklen_t);

int
main(int argc, char *argv[])
{
     int sendfd, recvfd;
     const int on = 1;
     socklen_t salen;
     struct sockaddr *sasend, *sarecv;

     if (argc != 3) {
          err_quit("usage: sendrecv <IP-multicast-address> <port#>");
     }

     sendfd = Udp_client(argv[1],argv[2], (void *)&sasend, &salen);
     recvfd = Socket(sasend->sa_family, SOCK_DGRAM, 0);

     Setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, &on ,sizeof(on));

     sarecv = Malloc(salen);
     memcpy(sarecv, sasend, salen);
     Bind(recvfd, sarecv, salen);

     Mcast_join(recvfd, sasend, salen, NULL, 0);
     Mcast_set_loop(sendfd, 0);

     if (Fork() == 0) {
          recv_all(recvfd, salen);
     }
     send_all(sendfd, sasend, salen);
     
     return 0;
}


void
send_all(int sendfd, SA *sadest, socklen_t salen)
{
     static char line[MAXLINE];
     struct utsname myname;

     if (uname(&myname) < 0) {
          err_sys("uname error");
          
     }
     snprintf(line, sizeof(line), "%s, %d\n", myname.nodename, getpid());
     for ( ;  ;  ) {
          Sendto(sendfd, line, strlen(line), 0, sadest, salen);
          sleep(SENDRATE);
     }
}

void
recv_all(int recvfd, socklen_t salen)
{
     int n;
     char line[MAXLINE];
     socklen_t len;
     struct sockaddr *safrom;
     safrom = Malloc(salen);

     for ( ;  ;  ) {
          len = salen;
          n = Recvfrom(recvfd, line, MAXLINE, 0, safrom, &len);
          line[n] = 0;
          printf("from %s: %s", Sock_ntop(safrom,len), line);
     }

          
}
