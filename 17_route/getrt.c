#include    <stdio.h>
#include    <string.h>
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <linux/rtnetlink.h>
#include    <linux/if_arp.h>

int main()
{
     int soc;
     struct sockaddr_nl    sa;
     int    n;
     char    buf[4096];
     struct nlmsghdr    *nlhdr;

     soc=socket(AF_NETLINK,SOCK_DGRAM,NETLINK_ROUTE);

     sa.nl_family=AF_NETLINK;
     sa.nl_groups=RTMGRP_LINK;
     bind(soc,(struct sockaddr *)&sa,sizeof(sa));

     while(1){
          n=recv(soc,buf,sizeof(buf),0);
          if(n<0){
               perror("recv");
               return(1);
                       
          }

          for(nlhdr=(struct nlmsghdr *)buf;NLMSG_OK(nlhdr,n);nlhdr=NLMSG_NEXT(nlhdr,n)){
               struct ifinfomsg    *ifimsg;
               struct rtattr    *rta;
               int    rtalist_len;

               printf("len:%d,",nlhdr->nlmsg_len);
               printf("type:%x,",nlhdr->nlmsg_type);

               if(nlhdr->nlmsg_type!=RTM_NEWLINK){
                    printf("Error:%d\n",nlhdr->nlmsg_type);
                    continue;
                                
               }

               ifimsg=NLMSG_DATA(nlhdr);
               if(ifimsg->ifi_family!=AF_UNSPEC&&ifimsg->ifi_family!=AF_INET6){
                    printf("Error family:%x\n",ifimsg->ifi_family);
                    continue;
                                
               }

               printf("family:%x,",ifimsg->ifi_family);
               printf("type:%x,",ifimsg->ifi_type);
               printf("index:%d\n",ifimsg->ifi_index);
               printf("flags:%x\n",ifimsg->ifi_flags);
               printf(" ");
               if(ifimsg->ifi_flags&IFF_UP)printf("UP,");
               if(ifimsg->ifi_flags&IFF_BROADCAST)printf("BROADCAST,");
               if(ifimsg->ifi_flags&IFF_DEBUG)printf("DEBUG,");
               if(ifimsg->ifi_flags&IFF_LOOPBACK)printf("LOOPBACK,");
               if(ifimsg->ifi_flags&IFF_POINTOPOINT)printf("POINTOPOINT,");
               if(ifimsg->ifi_flags&IFF_NOTRAILERS)printf("NOTRAILERS,");
               if(ifimsg->ifi_flags&IFF_RUNNING)printf("RUNNING,");
               if(ifimsg->ifi_flags&IFF_NOARP)printf("NOARP,");
               if(ifimsg->ifi_flags&IFF_PROMISC)printf("PROMISC,");
               if(ifimsg->ifi_flags&IFF_MASTER)printf("MASTER,");
               if(ifimsg->ifi_flags&IFF_SLAVE)printf("SLAVE,");
               if(ifimsg->ifi_flags&IFF_MULTICAST)printf("MULTICAST,");
               if(ifimsg->ifi_flags&IFF_PORTSEL)printf("PORTSEL,");
               if(ifimsg->ifi_flags&IFF_AUTOMEDIA)printf("AUTOMEDIA,");
               if(ifimsg->ifi_flags&IFF_DYNAMIC)printf("DYNAMIC,");
               if(ifimsg->ifi_flags&IFF_LOWER_UP)printf("LOWER_UP,");
               if(ifimsg->ifi_flags&IFF_DORMANT)printf("DORMANT,");
               if(ifimsg->ifi_flags&IFF_ECHO)printf("ECHO,");
               printf("\n");
               printf("change:%x\n",ifimsg->ifi_change);

               rtalist_len=nlhdr->nlmsg_len-NLMSG_LENGTH(sizeof(struct ifinfomsg));
               for(rta=IFLA_RTA(ifimsg);RTA_OK(rta,rtalist_len);rta=RTA_NEXT(rta,rtalist_len)){
                    switch(rta->rta_type){
                    case IFLA_IFNAME:
                         printf(" type:%x\n",rta->rta_type);
                         printf("  IFName:%s\n",(char *)RTA_DATA(rta));
                         break;
                    case IFLA_LINK:
                         printf(" type:%x\n",rta->rta_type);
                         printf("  Link Type:%d\n",*(int *)RTA_DATA(rta));
                         break;
                    default:
                         break;
                                         
                    }
                                
               }
                       
          }
          printf("\n");
              
     }

     close(soc);
     return(0);
     
}
