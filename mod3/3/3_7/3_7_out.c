#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>


struct msgbuf
{
    long mtype;
    char mtext[1];
};
typedef struct msgbuf msgbuf;


/* Usage: ./3_7_out <msgque key> */
int main()
{
   key_t key = 3310;

   int msqid = msqid = msgget(key, 0);
   if (msqid == -1)
   {
      perror("Unable to open message queue");
      exit(1);
   }

   msgbuf buf = { 0, { 0 } };
   do
   {
      if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1)
      {
         perror("Unable to recieve a message");
         exit(2);
      }
      printf("%d\n", buf.mtext[0]);
   } while (buf.mtype != 255);

   msgctl(msqid, IPC_RMID, NULL);
   exit(EXIT_SUCCESS);
}