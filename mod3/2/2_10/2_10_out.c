#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


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

   int fd_fifo = 0;
   /* open FIFO for reading and writing */
   if ((fd_fifo = open("2_10_fifo", O_NONBLOCK, O_RDONLY)) == -1)
   {
      perror("Unable to open FIFO\n");
      exit(2);
   }

   int number = atoi(argv[1]);
   for (int i = 0; i < number; ++i)
   {
      int random = 0;
      if (read(fd_fifo, &random, sizeof(int)) != -1)
         printf("%d\n", random);
   }

   close(fd_fifo);

   exit(EXIT_SUCCESS);
}
