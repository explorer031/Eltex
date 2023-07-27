#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <unistd.h>  /* sleep */
#include <signal.h>


int main()
{
   /* erase file content */
   FILE* file = fopen("count.txt", "w");
   fclose(file);

   signal(SIGINT, SIG_IGN);

   for (unsigned long i = 0; ; ++i)
   {
      unsigned char file_flag = 1;
      FILE* file = fopen("count.txt", "a");

      fprintf(file, "%lu\n", i);
      fclose(file);

      sleep(1);
   }

   exit(EXIT_SUCCESS);
}
