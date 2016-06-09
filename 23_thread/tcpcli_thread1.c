#include "unpthread.h"


void *copyto(void *);

static int sockfd;
static FILE *fp;


void
str_cli_thread(FILE *fp_arg, int sockfd_arg)
{
     char recvline[MAXLINE];
     pthread_t tid;

     sockfd = sockfd_arg;
     fp = fp_arg;

     Pthread_create(&tid, NULL, copyto, NULL);
     while (Readline(sockfd, recvline, MAXLINE)  != NULL) {
          Fputs(recvline, stdout);
     }

     
}

void *
copyto(void *arg)
{
     char sendline[MAXLINE];
     while (Fgets(sendline, MAXLINE, fp) != NULL) {
          Writen(sockfd, sendline, strlen(sendline));
     }
     Shutdown(sockfd, SHUT_WR);
     return(NULL);
     
}


int
main(int argc, char *argv[])
{
     int sockfd;
     struct sockaddr_in servaddr;
     if (argc != 3) {
          err_quit("usage: tcpcli_thread1 <host> <port>");
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     bzero(&servaddr ,sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(atoi(argv[2]));
     Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
     Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
     str_cli_thread(stdin, sockfd);
     exit(0);

}
