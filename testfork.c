#include <stdio.h>
#include <unistd.h>
int main () {
   pid_t pid = fork();
   printf("hello\n");
   return(0);
}
