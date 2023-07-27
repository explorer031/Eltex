#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <unistd.h>  /* sleep */
#include <signal.h>


/* print message about SIGINT or SIGQUIT */
void handler(int sig)
{
   if (sig == SIGINT)
      puts("SIGINT received");
   else  /* SIGQUIT */
      puts("SIGQUIT received");

   return;
}

int main()
{
   /* erase file content */
   FILE* file = fopen("count.txt", "w");
   fclose(file);

   signal(SIGINT, handler);
   signal(SIGQUIT, handler);

   for (unsigned long i = 0; ; ++i)
   {
      FILE* file = fopen("count.txt", "a");
      fprintf(file, "%lu\n", i);
      fclose(file);

      sleep(1);
   }

   exit(EXIT_SUCCESS);
}