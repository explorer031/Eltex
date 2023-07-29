#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  /* mkfifo */
#include <sys/stat.h>   /* mkfifo */
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};

/* Usage: ./4_4_in <count of numbers>*/
int main(int argc, char* argv[])
{
   usage_check(argc, argv);

   /* create FIFO */
   if (mkfifo("4_4_fifo", 0666) == -1)
   {
      perror("Unable to create FIFO");
      exit(1);
   }

   /* open FIFO for writing */
   int fd_fifo = open("4_4_fifo", O_RDWR | O_NONBLOCK);
   if (fd_fifo == -1)
   {
      perror("Unable to open FIFO");
      exit(2);
   }

   /* create semaphore */
   sem_t* sem_addr = sem_open("4_4_sem", O_CREAT, 0666, 1);
   if (sem_addr == NULL)
   {
      perror("Unable to create semaphore");
      exit(3);
   }

   /* sending count of numbers*/
   int number = atoi(argv[1]);
   sem_wait(sem_addr);
   write(fd_fifo, &number, sizeof(int));
   sem_post(sem_addr);

   sleep(3);  /* time to start ./4_1_out */

   /* generating numbers */
   srand(time(NULL));
   for (int i = 0; i < number; ++i)
   {
      int random = rand() % 1000;

      sem_wait(sem_addr);

      write(fd_fifo, &random, sizeof(int));
      printf("%d\n", random);

      sem_post(sem_addr);

      sleep(1);
   }

   close(fd_fifo);
   sem_close(sem_addr);
   
   exit(EXIT_SUCCESS);
}