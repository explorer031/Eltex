#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <unistd.h>  /* sleep */
#include <signal.h>


int main()
{
   /* erase file content */
   FILE* file = fopen("count.txt", "w");
   fclose(file);

   for (unsigned long i = 0; ; ++i)
   {
      signal(SIGINT, SIG_IGN);
      signal(SIGQUIT, SIG_IGN);

      FILE* file = fopen("count.txt", "a");
      fprintf(file, "%lu\n", i);
      fclose(file);
      sleep(1);  /* just for testing */

      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);

      sleep(1);
   }

   exit(EXIT_SUCCESS);
}