#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <unistd.h>  /* sleep */
#include <signal.h>


/* exit if recieved third SIGINT */
void handler(int sig)
{
   static unsigned char sigint_count = 0;

   if (++sigint_count == 3)
      exit(EXIT_SUCCESS);

   return;
}

int main()
{
   /* erase file content */
   FILE* file = fopen("count.txt", "w");
   fclose(file);

   signal(SIGINT, handler);

   for (unsigned long i = 0; ; ++i)
   {
      FILE* file = fopen("count.txt", "a");
      fprintf(file, "%lu\n", i);
      fclose(file);

      sleep(1);
   }

   exit(EXIT_SUCCESS);
}
