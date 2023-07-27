#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
   pid_t pid = getpid();
   printf("pid:  %d\n", pid);
   printf("ppid: %d\n", getppid());

   printf("uid:  %d\n", getuid());
   printf("euid: %d\n", geteuid());

   printf("gid:  %d\n", getgid());

   printf("sid:  %d\n", getsid(pid));

   pause();
   exit(EXIT_SUCCESS);
}
