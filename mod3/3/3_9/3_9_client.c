#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


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
unsigned int uid = 2;
char username[32] = { 0 };
msgbuf buf = { 1, { 2, { 0 }, { 0 } } };

unsigned char end_flag = 0;
void sigtstp_handler(int sig)
{
   end_flag = 1;
   return;
}

void sigint_handler(int sig)
{
   buf.mtype = 1;
   buf.msg_data.id = uid;
   strcpy(buf.msg_data.from_whom, "System");
   strcpy(buf.msg_data.text, username);
   strcat(buf.msg_data.text, " leaves chat.");

   msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT);
   exit(EXIT_SUCCESS);
}

void identification()
{
   puts("===== Identification stage =====");

   printf("Enter message queue ID: ");
   scanf("%d", &msqid);

   printf("Enter your ID: ");
   scanf("%u", &uid);

   printf("Enter your username: ");
   getc(stdin);
   fgets(username, sizeof(username), stdin);
   username[strlen(username) - 1] = '\0';

   if (!strcmp(username, "System"))
      exit(EXIT_FAILURE);
}

void init_client()
{
   buf.mtype = 1;
   buf.msg_data.id = uid;
   strcpy(buf.msg_data.from_whom, username);

   if (msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT) == -1)
   {
      perror("Unable to initialize communication with server.\n");
      exit(EXIT_FAILURE);
   }
}

int main()
{
   identification();

   init_client();

   puts("----------------------------------------------");
   puts("Type Ctrl+Z to enter your message.");
   puts("Type Ctrl+C to stop communicating and delete the message queue.");
   signal(SIGTSTP, sigtstp_handler);
   signal(SIGINT, sigint_handler);

   while (1)
   {
      /* reading available messages */
      end_flag = 0;
      while (!end_flag)
      {
         if (msgrcv(msqid, &buf, sizeof(buf.msg_data), uid, IPC_NOWAIT) != -1)
            printf("\n%s: %s\n",buf.msg_data.from_whom, buf.msg_data.text);
      }
      
      /* entering client's message */
      buf.mtype = 1;
      buf.msg_data.id = uid;
      strcpy(buf.msg_data.from_whom, username);

      printf("\nYou: ");
      fgets(buf.msg_data.text, sizeof(buf.msg_data.text), stdin);
      buf.msg_data.text[strlen(buf.msg_data.text) - 1] = '\0';

      /* sending our message */
      if (msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT) == -1)
         perror("Couldn't send your message, please try again.\n");
   }

   return 0;
}