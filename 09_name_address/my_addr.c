#include "unp.h"
#include <sys/utsname.h>
char **
my_addr(int *addrtype)
{
     struct hostent *hptr;
     struct utsname myname;

     if (uname(&myname) <0){
          return(NULL);
     }
     if ((hptr = gethostbyname(myname.nodename)) == NULL) {
          return(NULL);
     }
     *addrtype = hptr->h_addrtype;
     return(hptr->h_addr_list);
     
}


