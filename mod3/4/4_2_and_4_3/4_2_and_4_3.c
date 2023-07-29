#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork, pipe */
#include <sys/wait.h>   /* wait */
#include <sys/ipc.h>
#include <sys/sem.h>


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};

union semun
{
   int val;               /* значение для SETVAL */
   struct semid_ds *buf;  /* буферы для IPC_STAT, IPC_SET */
   unsigned short *array; /* массивы для GETALL, SETALL */
   /* часть, особенная для Linux: */
   struct seminfo *__buf; /* буфер для IPC_INFO */
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
   int sem_read = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
   if (sem_read == -1)
   {
      perror("Unable to create read semaphore");
      exit(3);
   }

   union semun arrg;

   arrg.val = 3;  /* 3 processes can read file */
   if (semctl(sem_read, 0, SETVAL, arrg) == -1)
   {
      perror("Unable to set initial semval for sem_read");
      exit(4);
   }

   /* -------------------------------------------------------------- */

   /* create semaphore means occupied_flag */
   int sem_occupied = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
   if (sem_occupied == -1)
   {
      perror("Unable to create write semaphore");
      exit(5);
   }

   arrg.val = 1;  /* write to file available only if file not occupied */
   if (semctl(sem_occupied, 0, SETVAL, arrg) == -1)
   {
      perror("Unable to set initial semval for sem_write");
      exit(6);
   }

   /* -------------------------------------------------------------- */

   struct sembuf lock = {0, -1, 0};
   struct sembuf unlock = {0, 1, 0};

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

         semop(sem_occupied, &lock, 1);
         semop(sem_read, &lock, 1);

         FILE *file = fopen("3_6.txt", "r");
         int readed = fscanf(file, "%hhu", &random);
         fclose(file);

         semop(sem_read, &unlock, 1);
         semop(sem_occupied, &unlock, 1);

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
         
         semop(sem_occupied, &lock, 1);

         FILE* file = fopen("3_6.txt", "w");
         fprintf(file, "%hhu", res);
         fclose(file);

         semop(sem_occupied, &unlock, 1);

         printf("Parent: %d: %hhu\n", i + 1, res);
      }

      close(pipefd[0]);  /* closing pipe_in */  
      break;
   }

   semctl(sem_read, 0, IPC_RMID);
   semctl(sem_occupied, 0, IPC_RMID);
   exit(EXIT_SUCCESS);
}
