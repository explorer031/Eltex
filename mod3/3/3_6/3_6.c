#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <signal.h>
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork, pipe */
#include <sys/wait.h>   /* wait */

#define SIGUSR1 10
#define SIGUSR2 12


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};

static unsigned char file_blocked = 0;
void handler(int sig)
{
   file_blocked = (sig == SIGUSR1 ? 1 : 0);
   return;
}


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

      signal(SIGUSR1, handler);
      signal(SIGUSR2, handler);

      for (int i = 0; i < number; ++i)
      {
         unsigned char random = rand() % 100;
         write(pipefd[1], &random, 1);

         if (file_blocked)  /* waiting for a file release */
            pause();

         if (!file_blocked)
         {
            FILE *file = fopen("3_6.txt", "r");
            fscanf(file, "%hhu", &random);
            fclose(file);
            printf("Child : %d: %hhu\n", i + 1, random);
         }
      }

      close(pipefd[1]);  /* closing pipe_out */
      break;

   default:  /* parent */
      close(pipefd[1]);  /* closing pipe_out */

      for (int i = 0; i < number; ++i)
      {
         unsigned char res = 0;
         read(pipefd[0], &res, 1);

         kill(pid, SIGUSR1);
         FILE* file = fopen("3_6.txt", "w");
         fprintf(file, "%hhu", res);
         fclose(file);
         kill(pid, SIGUSR2);

         printf("Parent: %d: %hhu\n", i + 1, res);
      }

      close(pipefd[0]);  /* closing pipe_in */  
      break;
   }

   exit(EXIT_SUCCESS);
}
