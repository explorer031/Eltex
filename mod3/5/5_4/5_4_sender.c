#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SEGMENT_SIZE 1024


int main(int argc, char *argv[])
{
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0)
   {
      perror("Unable to create socket");
      exit(1);
   }

   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = 55778;
   server_addr.sin_addr.s_addr = inet_addr(argv[1]);

   if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
   {
      perror("Unable to connect to receiver");
      exit(2);
   }

   FILE* fp = fopen(argv[2], "r");
   if (fp == NULL)
   {
      perror("Unable to open file.");
      exit(3);
   }

   char buf[SEGMENT_SIZE] = { 0 };

   while (fgets(buf, SEGMENT_SIZE, fp) != NULL)
   {
      if (send(sockfd, buf, sizeof(buf), 0) == -1)
      {
         perror("Unable to send data");
         exit(4);
      }
      bzero(buf, SEGMENT_SIZE);
   }
   
   printf("File sent successfully.\n");

   close(sockfd);

   exit(EXIT_SUCCESS);
}
