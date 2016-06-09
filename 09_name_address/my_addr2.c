#include "myaddress.h"


char **
my_addr(int *addrtype)
{
     struct hostent *hptr;
     char myname[MAXHOSTNAMELEN];
     

     if (gethostname(myname, sizeof(myname)) <0){
          return(NULL);
     }
     if ((hptr = gethostbyname(myname)) == NULL) {
          return(NULL);
     }
     *addrtype = hptr->h_addrtype;
     return(hptr->h_addr_list);
     
}


