#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>


int main()
{
   mqd_t ds = mq_open("/3_10_queue", O_CREAT | O_RDONLY, 0666, NULL);
   if (ds == (mqd_t)-1)
   {
      perror("Unable to open message queue");
      exit(1);
   }

   struct mq_attr attr;
   mq_getattr(ds, &attr);

   char text[attr.mq_msgsize];  /* just not working without this */

   /* read until end */
   while (mq_receive(ds, text, attr.mq_msgsize, NULL) != -1)
      printf("%d\n", text[0]);

   mq_close(ds);
   exit(EXIT_SUCCESS);
}