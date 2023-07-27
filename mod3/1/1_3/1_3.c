#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   int proc = fork();
   if (proc == -1)
   {
      perror("fork failure.");
      exit(EXIT_FAILURE);
   }

   if (proc) /* parent */
      for (int i = 1; i <= argc / 2; ++i)
      {
         double a = atof(argv[i]);
         printf("Parent: %f => %f\n", a, a * a);
      }
   else      /* child */
      for (int i = argc / 2 + 1; i < argc; ++i)
      {
         double a = atof(argv[i]);
         printf("Child : %f => %f\n", a, a * a);
      }

   exit(EXIT_SUCCESS);
}
