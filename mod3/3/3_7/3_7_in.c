#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <time.h>


struct msgbuf
{
    long mtype;
    char mtext[1];
};
typedef struct msgbuf msgbuf;


/* Usage: ./3_7_in */
int main()
{
   srand(time(NULL));

   key_t key = 3310;
   int msqid = msgget(key, IPC_CREAT | 0666);
   if (msqid == -1)
   {
      perror("Unable to create message queue");
      exit(1);
   }

   unsigned char i = 0;
   for (i = 0; i < 5; ++i)
   {
      char digit = rand() % 10;
      printf("%d\n", digit);
      
      msgbuf buf = { (i == 4 ? 255 : 1), { digit } };

      if (msgsnd(msqid, &buf, sizeof(buf.mtext), 0) == -1)
      {
         perror("Unable to send a message");
         exit(2);
      }
   }

   exit(EXIT_SUCCESS);
}