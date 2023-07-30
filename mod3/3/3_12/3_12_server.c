#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

#define MAX_CLIENTS 5


mqd_t msqid_to_client[MAX_CLIENTS] = { 0 };
char client_queue[MAX_CLIENTS][64] = { 0 };
mqd_t msqid_to_server = 0;

void sigint_handler(int sig)
{
   printf("\rServer off\n");
   mq_close(msqid_to_server);
   mq_unlink("/Server");

   for (int i = 0; i < MAX_CLIENTS; ++i)
      if (msqid_to_client[i])
      {
         mq_close(msqid_to_client[i]);
         mq_unlink(client_queue[i]);
      }

   exit(EXIT_SUCCESS);
}

int main()
{
   msqid_to_server = mq_open("/Server", O_CREAT | O_RDONLY, 0666, NULL);
   
   if (msqid_to_server == (mqd_t)-1)
   {
      perror("Unable to create/open message queue to server");
      exit(EXIT_FAILURE);
   }

   puts("Message queue to server created successfully!");
   puts("---------------------------------------------\n");
   signal(SIGINT, sigint_handler);

   char send[256] = { 0 };
   char msq_name[64] = { 0 };

   struct mq_attr mq_attr;
   mq_getattr(msqid_to_server, &mq_attr);
   char recv[mq_attr.mq_msgsize];
   
   unsigned int count = 0;

   while (1)
   {
      /* reading available message */
      unsigned int prio = 0;
      if (mq_receive(msqid_to_server, recv, sizeof(recv), &prio) == -1)
         continue;
      
      if (prio == 1)  /* new client */
      {
         strcpy(msq_name, "/");
         strcat(msq_name, recv);
         mqd_t msqid = 0;

         if ((msqid = mq_open(msq_name, O_RDWR | O_NONBLOCK)) == -1)
         {
            fprintf(stderr, "Unable to answer %s", msq_name);
            continue;
         }

         if (count == MAX_CLIENTS)
         {
            strcpy(send, "You can not join chat: clients cap reached.");
            mq_send(msqid, send, sizeof(send), 1);
            mq_close(msqid);
            continue;
         }
         else
         {
            strcpy(send, "System: ");
            strcat(send, recv);
            strcat(send, " joined chat.");

            ++count;
            for (int i = 0; i < MAX_CLIENTS; ++i)
               if (!msqid_to_client[i])
               {
                  msqid_to_client[i] = msqid;
                  strcpy(client_queue[i], msq_name);
                  break;
               }
         }
      }
      /* if one of clients leaves chat */
      else if (prio == 2)
      {
         strcpy(msq_name, "/to_");
         strcat(msq_name, recv);
         mqd_t msqid = 0;
         
         strcpy(send, "System: ");
         strcat(send, recv);
         strcat(send, " has left the chat.");

         printf("%s\n\n", send);
         for (int i = 0; i < MAX_CLIENTS; ++i)
         {
            if (!strcmp(client_queue[i], msq_name))  /* release resources */
            {
               mq_close(msqid_to_client[i]);
               strcpy(client_queue[i], "");
               msqid_to_client[i] = 0;
            }
            else if (msqid_to_client[i])  /* send message about client's leaving */
               mq_send(msqid_to_client[i], send, sizeof(send), 0);
         }
         continue;
      }
      else strcpy(send, recv);

      /* print recieved message*/
      printf("%s\n\n", send);

      /* forwarding a message from the server to clients */
      for (int i = 0; i < MAX_CLIENTS; ++i)
         if (msqid_to_client[i] && mq_send(msqid_to_client[i], send, sizeof(send), 0) == -1)
            fprintf(stderr, "Couldn't forward message %s", client_queue[i]);

      /* clearing */
      for (int i = 0; i < 256; ++i)
      {
         if (i < 64) msq_name[i] = 0;
         send[i] = 0;
      }
      for (long i = 0; i < sizeof(recv); ++i) recv[i] = 0;
   }

   exit(EXIT_SUCCESS);
}