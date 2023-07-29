#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>   /* mkfifo */
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


/* Usage: ./4_1_out <sem_id>*/
int main(int argc, char* argv[])
{
   /* open FIFO for reading */
   int fd_fifo = open("4_1_fifo", O_RDONLY | O_NONBLOCK);
   printf("fifo_id = %d\n", fd_fifo);

   if (fd_fifo  == -1)
   {
      perror("Unable to open FIFO\n");
      exit(1);
   }
   
   int semid = atoi(argv[1]);

   struct sembuf lock = {0, -1, 0};
   struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};

   int count = 0;
   semop(semid, &lock, 1);
   ssize_t readed = read(fd_fifo, &count, sizeof(int));

   if (readed == -1)
      exit(2);
   semop(semid, unlock, 2);

   while (count)
   {
      int random = 0;

      semop(semid, &lock, 1);
      readed = read(fd_fifo, &random, sizeof(int));

      if (readed != -1 && readed != 0)
      {
         printf("%d\n", random);
         --count;
      }

      semop(semid, unlock, 2);

      sleep(1);
   }

   close(fd_fifo);
   semctl(semid, 0, IPC_RMID);

   exit(EXIT_SUCCESS);
}