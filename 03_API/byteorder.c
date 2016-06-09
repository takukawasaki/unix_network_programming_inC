#include "unp.h"

int main(int argc, char *argv[])
{
     typedef union {
          short s;
          char c[sizeof(short)];
     } un;

     un uni;
     uni.s = 0x0102;
     printf("%s: ", CPU_VENDOR_OS);
     if (sizeof(short) == 2) {
          if (uni.c[0] == 1 && uni.c[1] == 2) {
               printf("big-endian\n");
          }else if(uni.c[0] == 2 && (int)uni.c[1] == 1){
               printf("little-endian\n");
          }else{
               printf("unknown\n");
          }
          
     }else{
          printf("sizeof(short) = %lu\n", sizeof(short));
     }
     exit(0);
}
