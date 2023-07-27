#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork, pipe */
#include <sys/wait.h>   /* wait */


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};


int main(int argc, char *argv[])
{
   usage_check(argc, argv);

   int number = atoi(argv[1]);
   
   int pipefd[2] = { 0 };
   if (pipe(pipefd) == -1)
   {
      perror("Unable to create pipe");
      exit(1);
   }

   pid_t pid = -1;
   switch (pid = fork())
   {
   case -1:  /* error */
      perror("Fork failure\n");
      break;

   case 0:  /* child */
      close(pipefd[0]);  /* closing pipe_in */
      srand(time(NULL));  /* updating the seed of PRNG */
      for (int i = 0; i < number; ++i)
      {
         unsigned char random = rand() % 100;
         write(pipefd[1], &random, 1);
      }
      close(pipefd[1]);  /* closing pipe_out */
      break;

   default:  /* parent */
      close(pipefd[1]);  /* closing pipe_out */

      FILE* file = fopen("2_7.txt", "w");

      for (int i = 0; i < number; ++i)
      {
         unsigned char res = 0;
         read(pipefd[0], &res, 1);
         printf("%d: %hhu\n", i + 1, res);
         fprintf(file, "%d: %hhu\n", i + 1, res);
      }

      close(pipefd[0]);  /* closing pipe_in */  
      fclose(file);
   }

   exit(EXIT_SUCCESS);
}
