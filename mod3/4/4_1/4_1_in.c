#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  /* mkfifo */
#include <sys/stat.h>   /* mkfifo */
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
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

/* Usage: ./4_1_in <count of numbers>*/
int main(int argc, char* argv[])
{
   usage_check(argc, argv);

   /* create FIFO */
   if (mkfifo("4_1_fifo", 0666) == -1)
   {
      perror("Unable to create FIFO");
      exit(1);
   }

   /* open FIFO for writing */
   int fd_fifo = open("4_1_fifo", O_RDWR | O_NONBLOCK);
   if (fd_fifo == -1)
   {
      perror("Unable to open FIFO");
      exit(2);
   }

   /* create semaphore */
   int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
   if (semid == -1)
   {
      perror("Unable to create semaphore");
      exit(3);
   }

   union semun arrg;
   arrg.val = 1;
   if (semctl(semid, 0, SETVAL, arrg) == -1)
   {
      perror("Unable to set initial semval");
      exit(4);
   }

   printf("Semaphore ID is %d\n", semid);

   struct sembuf lock = {0, -1, 0};
   struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};

   /* sending count of numbers*/
   int number = atoi(argv[1]);
   semop(semid, &lock, 1);
   write(fd_fifo, &number, sizeof(int));
   semop(semid, unlock, 2);

   sleep(3);  /* time to start ./4_1_out */

   /* generating numbers */
   srand(time(NULL));
   for (int i = 0; i < number; ++i)
   {
      int random = rand() % 1000;

      semop(semid, &lock, 1);

      write(fd_fifo, &random, sizeof(int));
      printf("%d\n", random);

      semop(semid, unlock, 2);

      sleep(1);
   }

   close(fd_fifo);

   exit(EXIT_SUCCESS);
}