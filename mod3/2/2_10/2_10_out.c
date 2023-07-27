#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
   int fd_fifo = 0;
   /* open FIFO for reading and writing */
   if ((fd_fifo = open("2_10_fifo", O_RDONLY)) == -1)
   {
      perror("Unable to open FIFO\n");
      exit(2);
   }

   unsigned char stop = 0;
   while (!stop)
   {
      int random = 0;
      ssize_t readed = read(fd_fifo, &random, sizeof(int));
      
      if (readed != -1 && readed != 0)
         printf("%d\n", random);
      else
         stop = 1;
   }

   close(fd_fifo);

   exit(EXIT_SUCCESS);
}
