#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>


int main()
{
   int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
   if (msqid == -1)
   {
      perror("Unable to create message queue");
      exit(EXIT_FAILURE);
   }
   
   puts("Message queue created successfully!");
   printf("Message queue ID is %d\n", msqid);

   exit(EXIT_SUCCESS);
}