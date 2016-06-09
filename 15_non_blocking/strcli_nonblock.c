#include "unp.h"

void
str_cli_nonblock(FILE *fp, int sockfd)
{
     int maxfdp1,val, stdineof;
     ssize_t n, nwritten;
     fd_set readset, writeset;
     char to[MAXLINE], fr[MAXLINE];
     char *toinputptr, *tooutputptr, *frinputptr, *froutputptr;

     val = Fcntl(sockfd, F_GETFL, 0);
     Fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

     val = Fcntl(STDIN_FILENO, F_GETFL, 0);
     Fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

     val = Fcntl(STDOUT_FILENO, F_GETFL, 0);
     Fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);


     toinputptr = tooutputptr = to;
     frinputptr = froutputptr = fr;
     stdineof = 0;
     
     maxfdp1 = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;

     for ( ;  ;  ) {
          FD_ZERO(&readset);
          FD_ZERO(&writeset);
          if (stdineof == 0 && toinputptr < &to[MAXLINE]) {
               FD_SET(STDIN_FILENO, &readset);
          }
          if (frinputptr < &fr[MAXLINE]) {
               FD_SET(sockfd, &readset);
          }
          if (tooutputptr != toinputptr) {
               FD_SET(sockfd, &writeset);
          }
          if (froutputptr != frinputptr) {
               FD_SET(STDOUT_FILENO, &writeset);
          }

          Select(maxfdp1, &readset, &writeset, NULL, NULL);

          if (FD_ISSET(STDIN_FILENO, &readset)) {
               if ((n = read(STDIN_FILENO, toinputptr, &to[MAXLINE] - toinputptr)) < 0) {
                    if (errno != EWOULDBLOCK) {
                         err_sys("read error on stdin");
                    }
               } else if(n == 0){
                    fprintf(stderr, "%s: EOF on stdin\n", gf_time());
                    stdineof = 1;
                    if (tooutputptr == toinputptr) {
                         Shutdown(sockfd, SHUT_WR);
                    }
                    
               } else {
                    fprintf(stderr, "%s: read %ld bytes from stdin\n", gf_time(), n);
                    toinputptr += n;
                    FD_SET(sockfd, &writeset);
               }
               
          }
          if (FD_ISSET(sockfd, &readset)) {
               if ((n = read(sockfd, frinputptr, &fr[MAXLINE] - frinputptr)) < 0) {
                    if (errno != EWOULDBLOCK) {
                         err_sys("read error on socket");
                    }
                    
               }else if(n == 0){
                    fprintf(stderr, "%s: EOF on socket\n", gf_time());
                    if (stdineof) {
                         return;
                    }else{
                         err_quit("strcli_nonblock: server terminated prematurely");
                         
                    }
               }else {
                    fprintf(stderr,"%s: read %ld bytes from socket\n", gf_time(), n);
                    frinputptr += n;
                    FD_SET(STDOUT_FILENO, &writeset);
                    
                    
               }
          }
          if (FD_ISSET(STDOUT_FILENO, &writeset) && ((n = frinputptr - froutputptr) > 0)) {
               if ((nwritten  = write(STDOUT_FILENO, froutputptr, n)) < 0) {
                    if (errno != EWOULDBLOCK) {
                         err_sys("write error to stdout");
                    }
                    
               }else{
                    fprintf(stderr, "%s: wrote %ld bytes to stdout\n", gf_time(), nwritten);
                    froutputptr += nwritten;
                    if (froutputptr == frinputptr) {
                         froutputptr = frinputptr =fr;
                         
                    }
               }
          }
          if (FD_ISSET(sockfd, &writeset) && ((n = toinputptr - tooutputptr ) > 0)) {
               if ((nwritten = write(sockfd, tooutputptr, n)) < 0) {
                    if (errno != EWOULDBLOCK) {
                         err_sys("write error to socket");
                    }
                    
               }else{
                    fprintf(stderr, "%s: wrote %ld bytes to socket\n", gf_time(), nwritten);
                    tooutputptr += nwritten;
                    if (tooutputptr == toinputptr) {
                         toinputptr = tooutputptr = to;
                         if ((stdineof)) {
                              Shutdown(sockfd, SHUT_WR);
                         }
                    }
               }
          }
     }
}



     


