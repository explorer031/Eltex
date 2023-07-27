#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};


int main(int argc, char* argv[])
{
   usage_check(argc, argv);

   /* create FIFO */
   if (access("2_10_fifo", F_OK) && mkfifo("2_10_fifo", S_IRWXU | S_IRWXG | S_IRWXO) == -1)
   {
      perror("Unable to create FIFO\n");
      exit(1);
   }

   int fd_fifo = 0;
   /* open FIFO for writing */
   if ((fd_fifo = open("2_10_fifo", O_WRONLY)) == -1)
   {
      perror("Unable to open FIFO\n");
      exit(2);
   }

   srand(time(NULL));
   int number = atoi(argv[1]);
   for (int i = 0; i < number; ++i)
   {
      int random = rand() % 1000;
      printf("%d\n", random);
      write(fd_fifo, &random, sizeof(int));
      
      sleep(1);
   }

   close(fd_fifo);

   exit(EXIT_SUCCESS);
}
