#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void end_func()
{
   puts("Program ended well.");
}

int main(int argc, char *argv[])
{
   int proc = fork();

   for (int i = 0; i < argc; ++i)
   {
      if (proc)
         printf("Parent: %d = %s\n", i, argv[i]);
      else
         printf("Child : %d = %s\n", i, argv[i]);
   }

   /* registering end_func() */
   if (proc && atexit(end_func))
   {
      puts("Unable to register end function.");
   }

   exit(EXIT_SUCCESS);
}
