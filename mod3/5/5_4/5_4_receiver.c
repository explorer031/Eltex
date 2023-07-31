#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SEGMENT_SIZE 1024


int main()
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
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

   if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
   {
      perror("Unable to bind socket");
      exit(2);
   }

   if (listen(sockfd, 1) == 0)
      printf("Listening...\n");
   else
   {
      perror("Unable to listen");
      exit(3);
   }

   struct sockaddr_in new_addr;
   socklen_t addr_size = sizeof(new_addr);
   int new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);

   char buf[SEGMENT_SIZE];
   FILE* fp = fopen("recv.txt", "w");

   while (1)
   {
      int n = recv(new_sock, buf, SEGMENT_SIZE, 0);
      if (n <= 0)
         break;

      fprintf(fp, "%s", buf);
      bzero(buf, SEGMENT_SIZE);
   }

   printf("File received successfully.\n");

   close(sockfd);
   close(new_sock);
   
   exit(EXIT_SUCCESS);
}
