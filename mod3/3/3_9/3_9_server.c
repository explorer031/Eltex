#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_CLIENTS 5


struct msgbuf
{
   long mtype;
   struct data
   {
      unsigned int id;
      char from_whom[16];
      char text[64];
   } msg_data;
};
typedef struct msgbuf msgbuf;
typedef struct data data;

int msqid = 0;
void sigint_handler(int sig)
{
   msgctl(msqid, IPC_RMID, NULL);
   exit(EXIT_SUCCESS);
}

int main()
{
   msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
   if (msqid == -1)
   {
      perror("Unable to create message queue");
      exit(EXIT_FAILURE);
   }

   puts("Message queue created successfully!");
   printf("Message queue ID is %d\n\n", msqid);
   puts("--------------------------------------------\n");
   signal(SIGINT, sigint_handler);

   unsigned int clients[MAX_CLIENTS] = { 0 };
   unsigned int count = 0;

   msgbuf buf = { 1, { 1, { 0 }, { 0 } } };
   while (1)
   {
      /* reading available message */
      if (msgrcv(msqid, &buf, sizeof(buf.msg_data), 1, IPC_NOWAIT) == -1)
         continue;

      /* check if we recieved message from new client */
      unsigned char new = 1;
      for (int i = 0; i < MAX_CLIENTS; ++i)
         if (buf.msg_data.id == clients[i]) new = 0;
      
      if (new)
      {
         if (count == MAX_CLIENTS)
         {
            buf.mtype = buf.msg_data.id;
            strcpy(buf.msg_data.text, "You can not join chat: clients cap reached.");
            msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT);
         }
         else
         {
            ++count;
            for (int i = 0; i < MAX_CLIENTS; ++i)
               if (!clients[i])
               {
                  clients[i] = buf.msg_data.id;
                  break;
               }
         }
         continue;
      }
      
      /* if one of clients leaves chat (recieved system message) */
      if (!strcmp(buf.msg_data.from_whom, "System"))
      {
         printf("\n%s: %s\n",
            buf.msg_data.from_whom, buf.msg_data.text);
         for (int i = 0; i < MAX_CLIENTS; ++i)
         {
            /* send message about client's leaving */
            if (clients[i] && clients[i] != buf.msg_data.id)
            {
               buf.mtype = clients[i];
               if (msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT) == -1)
                  fprintf(stderr, "Couldn't send message to %u.\n", clients[i]);
            }
            else if (buf.msg_data.id == clients[i])
               clients[i] = 0;  /* free space */
         }
         continue;
      }

      /* print recieved message*/
      printf("\n(%u) %s: %s\n",
         buf.msg_data.id, buf.msg_data.from_whom, buf.msg_data.text);

      /* forwarding a message from the server to clients */
      for (int i = 0; i < MAX_CLIENTS; ++i)
         if (clients[i] && clients[i] != buf.msg_data.id)
         {
            buf.mtype = clients[i];
            if (msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT) == -1)
               fprintf(stderr, "Couldn't forward message to %u.\n", clients[i]);
         }
   }

   exit(EXIT_SUCCESS);
}