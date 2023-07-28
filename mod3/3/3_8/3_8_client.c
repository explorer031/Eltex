#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define SIGUSR1 10

struct msgbuf
{
   long mtype;
   struct data
   {
      char from_whom[32];
      char text[256];
   } msg_data;
};
typedef struct msgbuf msgbuf;
typedef struct data data;

int msqid = 0;
unsigned int uid = 0, cid = 0;

unsigned char end_flag = 0;
void sigtstp_handler(int sig)
{
   end_flag = 1;
   return;
}

void sigint_handler(int sig)
{
   msgbuf buf = { cid,
   { "System", "Your interlocutor has left the chat.\nMessage queue has been deleted." } };

   msgsnd(msqid, &buf, sizeof(buf.msg_data), IPC_NOWAIT);
   msgctl(msqid, IPC_RMID, NULL);
   exit(EXIT_SUCCESS);
}

void identification(int* msqid, unsigned int* uid, unsigned int* cid)
{
   puts("===== Identification stage =====");

   printf("Enter message queue ID: ");
   scanf("%d", msqid);

   printf("Enter your ID: ");
   scanf("%u", uid);

   printf("Enter your companion ID: ");
   scanf("%u", cid);
}


int main()
{
   identification(&msqid, &uid, &cid);

   msgbuf buf = { cid, { { 0 }, { 0 } } };

   char username[32] = { 0 };
   printf("Enter your username: ");
   getc(stdin);
   fgets(username, sizeof(username), stdin);
   username[strlen(username) - 1] = '\0';

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
         {
            printf("\n%s: %s\n",buf.msg_data.from_whom, buf.msg_data.text);
            if (!strcmp(buf.msg_data.from_whom, "System"))
               exit(EXIT_SUCCESS);
         }
      }
      
      buf.mtype = cid;
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