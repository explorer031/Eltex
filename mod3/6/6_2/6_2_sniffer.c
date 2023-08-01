#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
   if (argc != 2)
   {
      printf("Usage: a.out <port>\n");
      exit(1);
   }

   int sockfd;                           /* Дескриптор сокета */
   int clilen, n;                        /* Переменные для различных длин и количества символов */
   char line[1000];                      /* Массив для принятой и отсылаемой строки */
   struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */

   /* Заполняем структуру для адреса сервера */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(argv[1]));
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   /* Создаем UDP сокет */
   if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
   {
      perror(NULL); /* Печатаем сообщение об ошибке */
      exit(1);
   }

   /* Настраиваем адрес сокета */
   if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
   {
      perror(NULL);
      close(sockfd);
      exit(1);
   }

   char string[1000] = { 0 };

   while (1)
   {
      /* Основной цикл обслуживания*/
      clilen = sizeof(cliaddr);
      
      puts("Listening...");

      /* Ожидаем прихода запроса от клиента и читаем его */
      if ((n = recvfrom(sockfd, line, 999, 0,
                        (struct sockaddr *)&cliaddr, &clilen)) < 0)
      {
         perror(NULL);
         close(sockfd);
         exit(1);
      }
      
      /* ловим и игнорируем ответ сервера (нам нужны только сообщения от клиента) */
      recvfrom(sockfd, NULL, 999, 0, (struct sockaddr *)&cliaddr, &clilen);
      
      /* =============================================
                    IPv4 packet structure
            +12          source_ip
            +16        destination_ip
            +20   sour_port  |   dest_port
            +24     length   |  control_sum
            +28            DATA
      ============================================== */

      puts("========================================");
      struct in_addr sip, dip;
      sip.s_addr = *(uint32_t*)(line + 12);
      dip.s_addr = *(uint32_t*)(line + 16);
      printf("Client IP:   %s\n", inet_ntoa(sip));
      printf("Server IP:   %s\n\n", inet_ntoa(dip));

      printf("Client port: %d\n", htons(*(uint16_t*)(line + 20)));
      printf("Server port: %d\n\n", htons(*(uint32_t*)(line + 22)));

      /* Печатаем принятый текст на экране */
      printf("Content: %s", line + 28);
      puts("========================================\n");

      /* Печатаем текст в бинарный файл */
      FILE* fp = fopen("intercepted", "wb");
      fprintf(fp, "%s", line + 28);
      
      fclose(fp);
   }

   return 0;
}