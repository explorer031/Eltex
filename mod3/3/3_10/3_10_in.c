#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>


int main()
{
   srand(time(NULL));

   mqd_t ds = mq_open("/3_10_queue", O_CREAT | O_WRONLY | O_NONBLOCK, 0666, NULL);
   if (ds == (mqd_t)-1)
   {
      perror("Unable to create message queue");
      exit(1);
   }

   for (unsigned char i = 0; i < 5; ++i)
   {
      char digit = rand() % 9 + 1;

      printf("%d\n", digit);
      
      if (mq_send(ds, &digit, sizeof(digit), 0) == -1)
      {
         perror("Unable to send a message");
         exit(2);
      }
   }

   mq_close(ds);
   mq_unlink("/3_10_queue");
   exit(EXIT_SUCCESS);
}