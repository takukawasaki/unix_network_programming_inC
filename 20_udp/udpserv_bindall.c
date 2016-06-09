#include	"unpifi.h"
#include "unp.h"



void	dg_echo2(int, SA *, socklen_t, SA *);

int
main(int argc, char **argv)
{
     int					sockfd;
     const int			on = 1;
     pid_t				pid;

     struct sockaddr_in		*sa,cliaddr, wild;
     struct ifi_info		*ifi, *ifihead;




     for (ifihead = ifi = Get_ifi_info(AF_INET, 1);
          ifi != NULL; ifi = ifi->ifi_next) {

          /*4bind unicast address */
          sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
          Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

          sa = (struct sockaddr_in *)ifi->ifi_addr;
          sa->sin_family = AF_INET;
          sa->sin_port = htons(SERV_PORT);
          
          Bind(sockfd, (SA *)sa, sizeof(*sa));
          printf("bound %s\n", Sock_ntop((SA *)sa, sizeof(*sa)));

          if ( (pid = Fork()) == 0) {		/* child */
               dg_echo2(sockfd, (SA *)&cliaddr, sizeof(cliaddr), (SA *)sa);
               exit(0);		/* never executed */
          }
          

          if (ifi->ifi_flags & IFF_BROADCAST) {
               /* 4try to bind broadcast address */
               sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
               Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

               sa = (struct sockaddr_in *)ifi->ifi_addr;
               sa->sin_family = AF_INET;
               sa->sin_port = htons(SERV_PORT);
                         
               if (bind(sockfd, (SA *)sa, sizeof(*sa)) < 0) {
                    if (errno == EADDRINUSE) {
                         printf("EADDRINUSE: %s\n",
                                Sock_ntop((SA *)sa, sizeof(*sa)));
                         Close(sockfd);
                         continue;
                    } else
                         err_sys("bind error for %s",
                                 Sock_ntop((SA *)sa, sizeof(*sa)));
               }
               printf("bound %s\n", Sock_ntop((SA *)sa, sizeof(*sa)));

               if ( (pid = Fork()) == 0) {		/* child */
                    dg_echo2(sockfd, (SA *)&cliaddr, sizeof(cliaddr), (SA *)sa);
                    exit(0);		/* never executed */
               }
          }
     }

     /* 4bind wildcard address */
     sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
     Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
     bzero(&wild, sizeof(wild));
     wild.sin_family = AF_INET;
     wild.sin_addr.s_addr = htonl(INADDR_ANY);
     wild.sin_port = htons(SERV_PORT);
     
     Bind(sockfd, (SA *)&wild, sizeof(wild));
     
     printf("bound %s\n", Sock_ntop((SA *)&wild,sizeof(wild)));

     if ( (pid = Fork()) == 0) {		/* child */
          dg_echo2(sockfd, (SA *)&cliaddr, sizeof(cliaddr), (SA *) &wild);;
          exit(0);		/* never executed */
     }
     exit(0);
}


void
dg_echo2(int sockfd, SA *pcliaddr, socklen_t salen,SA *myaddr)
{
     int			n;
     char		mesg[MAXLINE];
     socklen_t	len;


 
     for ( ; ; ) {
          len = salen;
          n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
          printf("child %d, datagram from %s",
                 getpid(), Sock_ntop(pcliaddr, len));
          printf(", to %s\n", Sock_ntop(myaddr, salen));

          Sendto(sockfd, mesg, n, 0, pcliaddr, len);
     }
}
