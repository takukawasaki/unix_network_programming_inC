#include "unp.h"

int my_open(const char *, int);

int main(int argc, char *argv[])
{
     int fd, n;
     char buf[BUFFSIZE];
     if (argc != 2) {
          err_quit("usage: mycat <pathname>\n");
     }
     if ((fd = my_open(argv[1], O_RDONLY)) < 0) {
          err_sys("cannot open %s", argv[1]);
     }
     while ((n = Read(fd,buf, BUFFSIZE)) > 0) {
          Write(STDOUT_FILENO, buf,n);
          
     }
     exit(0);
}
