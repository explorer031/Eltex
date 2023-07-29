#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>   /* mkfifo */
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>


/* Usage: ./4_4_out */
int main(int argc, char* argv[])
{
   /* open FIFO for reading */
   int fd_fifo = open("4_4_fifo", O_RDONLY | O_NONBLOCK);

   if (fd_fifo  == -1)
   {
      perror("Unable to open FIFO\n");
      exit(1);
   }

   sem_t* sem_addr = sem_open("4_4_sem", 0);
   if (sem_addr == NULL)
   {
      perror("Unable to open semaphore");
      exit(3);
   }

   int count = 0;
   sem_wait(sem_addr);
   ssize_t readed = read(fd_fifo, &count, sizeof(int));

   if (readed == -1)
      exit(2);
   sem_post(sem_addr);

   while (count)
   {
      int random = 0;

      sem_wait(sem_addr);
      readed = read(fd_fifo, &random, sizeof(int));

      if (readed != -1 && readed != 0)
      {
         printf("%d\n", random);
         --count;
      }

      sem_post(sem_addr);

      sleep(1);
   }

   close(fd_fifo);
   sem_close(sem_addr);
   sem_unlink("4_4_sem");

   exit(EXIT_SUCCESS);
}