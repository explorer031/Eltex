#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>


mqd_t msqid_to = 0, msqid_from = 0;
char msq_name_to[16] = { 0 }, msq_name_from[16] = { 0 };

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
   char* system_msg = "System: Your interlocutor has left the chat.\nMessage queues have been deleted.";

   if (mq_send(msqid_to, system_msg, strlen(system_msg), 1) == -1)
      perror("Unable to warn your interlocutor about your chat leaving");

   mq_close(msqid_to);
   mq_close(msqid_from);
   exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
   puts("===== Identification stage =====");
   char username[32] = { 0 };
   printf("Enter your username: ");
   fgets(username, sizeof(username), stdin);
   username[strlen(username) - 1] = ':';
   username[strlen(username)] = ' ';

   if (atoi(argv[1]) == 1)
   {
      strcpy(msq_name_to, "/3_11_queue1");
      strcpy(msq_name_from, "/3_11_queue2");
   }
   else
   {
      strcpy(msq_name_to, "/3_11_queue2");
      strcpy(msq_name_from, "/3_11_queue1");
   }

   msqid_to = mq_open(msq_name_to, O_CREAT | O_WRONLY | O_NONBLOCK, 0666, NULL);
   msqid_from = mq_open(msq_name_from, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, NULL);
   
   if (msqid_to == (mqd_t)-1 || msqid_from == (mqd_t)-1)
   {
      perror("Unable to create/open message queue");
      exit(EXIT_FAILURE);
   }

   puts("----------------------------------------------");
   puts("Type Ctrl+Z to enter your message.");
   puts("Type Ctrl+C to stop communicating and delete the message queue.\n");
   signal(SIGTSTP, sigtstp_handler);
   signal(SIGINT, sigint_handler);

   char msg_text[128] = { 0 };
   char send[256] = { 0 };

   struct mq_attr mq_attr;
   mq_getattr(msqid_from, &mq_attr);
   char recv[mq_attr.mq_msgsize];

   while (1)
   {
      /* reading available messages */
      end_flag = 0;
      while (!end_flag)
      {
         unsigned int prio = 0;
         if (mq_receive(msqid_from, recv, mq_attr.mq_msgsize, &prio) != -1)
         {
            printf("%s\n\n", recv);
            
            if (prio == 1)
            {
               mq_close(msqid_to);
               mq_unlink(msq_name_to);

               mq_close(msqid_from);
               mq_unlink(msq_name_from);

               exit(EXIT_SUCCESS);
            }
         }
      }
      
      strcpy(send, username);

      printf("\rYou: ");
      fgets(msg_text, sizeof(msg_text), stdin);
      msg_text[strlen(msg_text) - 1] = '\0';

      puts("");

      strcat(send, msg_text);
      /* sending our message */
      if (mq_send(msqid_to, send, sizeof(send), 0) == -1)
         perror("Couldn't send your message, please try again");
   }

   return 0;
}