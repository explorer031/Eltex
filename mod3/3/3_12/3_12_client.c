#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include <malloc.h>


mqd_t msqid_to_server = 0, msqid_from_server = 0;
char username[32] = { 0 };
char recv[8192];

unsigned char end_flag = 0;
void sigtstp_handler(int sig)
{
   printf("\r");
   end_flag = 1;
   return;
}

void sigint_handler(int sig)
{
   printf("\r");
   
   if (mq_send(msqid_to_server, username, strlen(username), 2) == -1)
      perror("Unable to warn your interlocutors about your chat leaving");

   mq_close(msqid_to_server);
   mq_close(msqid_from_server);
   mq_close(msqid_from_server);

   exit(EXIT_SUCCESS);
}

void identification()
{
   printf("Enter your username: ");

   fgets(username, sizeof(username), stdin);
   username[strlen(username) - 1] = '\0';

   if (!strcmp(username, "System"))
   {
      perror("You can not use this name\n");
      exit(EXIT_FAILURE);
   }
}

void init_client()
{
   msqid_to_server = mq_open("/Server", O_RDWR | O_NONBLOCK);
   if (msqid_to_server == (mqd_t)-1)
   {
      perror("Unable to open message queue to server");
      exit(EXIT_FAILURE);
   }

   char msq_to_user[64] = "/";
   strcat(msq_to_user, username);
   msqid_from_server = mq_open(msq_to_user, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, NULL);
   if (msqid_from_server == (mqd_t)-1)
   {
      perror("Unable to create message queue from server");
      exit(EXIT_FAILURE);
   }

   if (mq_send(msqid_to_server, username, strlen(username), 1) == -1)
   {
      perror("Unable to initialize communication with server\n");
      exit(EXIT_FAILURE);
   }

   struct mq_attr mq_attr;
   mq_getattr(msqid_from_server, &mq_attr);

   unsigned int prio = 0;
   unsigned char flag = 0;
   while (!flag)
   {
      int res = mq_receive(msqid_from_server, recv, mq_attr.mq_msgsize, &prio);
      if (res != -1 && res != 0)
      {
         flag = 1;

         if (prio == 1)
         {
            mq_close(msqid_to_server);

            mq_close(msqid_from_server);
            mq_unlink(msq_to_user);

            exit(EXIT_SUCCESS);
         }
      }
   }

   return;
}


int main()
{
   identification();
   init_client();

   puts("---------------------------------------------------------------");
   puts("Type Ctrl+Z to enter your message.");
   puts("Type Ctrl+C to stop communicating and delete the message queue.");
   puts("---------------------------------------------------------------\n");
   signal(SIGTSTP, sigtstp_handler);
   signal(SIGINT, sigint_handler);

   char msg_text[128] = { 0 };
   char send[256] = { 0 };

   while (1)
   {
      /* reading available messages */
      end_flag = 0;
      while (!end_flag)
         if (mq_receive(msqid_from_server, recv, sizeof(recv), NULL) != -1)
            printf("%s\n\n", recv);

      printf("\rYou: ");
      fgets(msg_text, sizeof(msg_text), stdin);
      msg_text[strlen(msg_text) - 1] = '\0';

      strcpy(send, username);
      strcat(send, ": ");
      strcat(send, msg_text);
      
      /* sending our message */
      if (mq_send(msqid_to_server, send, sizeof(send), 0) == -1)
         perror("Couldn't send your message, please try again");
      else  /* catch the answer */
      {
         unsigned char answer = 0;
         while (!answer)
            if (mq_receive(msqid_from_server, recv, sizeof(recv), NULL) != -1)
               answer = 1;
      }

      puts("");
      
   }

   return 0;
}