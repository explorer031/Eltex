#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
   /* erase file content */
   FILE* file = fopen("count.txt", "w");
   fclose(file);

   for (unsigned long i = 0; ; ++i)
   {
      FILE* file = fopen("count.txt", "a");
      fprintf(file, "%lu\n", i);
      fclose(file);

      sleep(1);
   }

   exit(EXIT_SUCCESS);
}