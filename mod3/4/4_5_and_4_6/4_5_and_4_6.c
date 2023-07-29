#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork, pipe */
#include <sys/wait.h>   /* wait */
#include <semaphore.h>


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
   
   /* create pipe */
   int pipefd[2] = { 0 };
   if (pipe(pipefd) == -1)
   {
      perror("Unable to create pipe");
      exit(1);
   }
   
   /* -------------------------------------------------------------- */

   /* create read semaphore */
   sem_t* sem_read = sem_open("4_56_read_sem", O_CREAT, 0666, 3);
   if (sem_read == NULL)
   {
      perror("Unable to create read semaphore");
      exit(1);
   }

   /* -------------------------------------------------------------- */

   /* create semaphore means occupied_flag */
   sem_t* sem_occupied = sem_open("4_56_occupied_sem", O_CREAT, 0666, 1);
   if (sem_occupied == NULL)
   {
      perror("Unable to create occupied semaphore");
      exit(3);
   }

   /* -------------------------------------------------------------- */

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

         sem_wait(sem_occupied);
         sem_wait(sem_read);

         FILE *file = fopen("3_6.txt", "r");
         int readed = fscanf(file, "%hhu", &random);
         fclose(file);

         sem_post(sem_read);
         sem_post(sem_occupied);

         if (readed > 0)
            printf("Child : %d: %hhu\n", i + 1, random);
      }

      close(pipefd[1]);  /* closing pipe_out */
      break;

   default:  /* parent */
      close(pipefd[1]);  /* closing pipe_out */

      for (int i = 0; i < number; ++i)
      {
         unsigned char res = 0;
         read(pipefd[0], &res, 1);
         
         sem_wait(sem_occupied);

         FILE* file = fopen("3_6.txt", "w");
         fprintf(file, "%hhu", res);
         fclose(file);

         sem_post(sem_occupied);

         printf("Parent: %d: %hhu\n", i + 1, res);
      }

      close(pipefd[0]);  /* closing pipe_in */  
      break;
   }

   sem_close(sem_occupied);
   sem_close(sem_read);

   sem_unlink("4_56_occupied_sem");
   sem_unlink("4_56_read_sem");

   exit(EXIT_SUCCESS);
}
