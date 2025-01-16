#include "fcntl.h"
#include <stdio.h>
int main()
{
    int fd = open("c", O_APPEND);
    int fd2 = open("c", O_APPEND);
   dup2(fd, fd2);
   perror("dup2");
write(fd, "Hello, ", 7);
   perror("f");
write(fd2, "world!\n", 7);
   perror("f2");
close(fd);
close(fd2);
}