#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


int main(int argc, char **argv)
{
   int sockfd;                           /* Дескриптор сокета */
   int n, len;                           /* Переменные для различных длин и количества символов */
   char sendline[1000] = { 0 },          /* Массивы для отсылаемой строки и буфера */
      buf[1000] = { 0 };
   struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */

   /* Сначала проверяем наличие второго аргумента в
   командной строке. При его отсутствии ругаемся и прекращаем работу */
   if (argc != 3)
   {
      printf("Usage: a.out <IP address> <port>\n");
      exit(1);
   }

   /* Создаем RAW сокет */
   if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
   {
      perror("Unable to create RAW-socket"); /* Печатаем сообщение об ошибке */
      exit(1);
   }

   /* Заполняем структуру для адреса клиента */
   bzero(&cliaddr, sizeof(cliaddr));
   cliaddr.sin_family = AF_INET;
   cliaddr.sin_port = htons(0);
   cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   /* Настраиваем адрес сокета */
   if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
   {
      perror("Unable to bind socket");
      close(sockfd); /* По окончании работы закрываем дескриптор сокета */
      exit(1);
   }

   /* Заполняем структуру для адреса сервера */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(argv[2]));
   if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
   {
      perror("Invalid IP address");
      close(sockfd); /* По окончании работы закрываем дескриптор сокета */
      exit(1);
   }

   /* Вводим строку, которую отошлем серверу */
   printf("String => ");
   fgets(buf, 1000, stdin);

   /* Добавление UDP заголовка перед полезной нагрузкой */
   /* порт отправителя оставим нулевым */
   memcpy(sendline + 2, &servaddr.sin_port, sizeof(in_port_t));  /* порт получателя */
   uint16_t dg_length = htons(strlen(buf) + 9);  /* длина датаграммы (вместе с заголовком) */ 
   memcpy(sendline + 4, &dg_length, sizeof(uint16_t));
   /* контрольная сумма также нулевая */
   strcat(sendline + 8, buf);  /* полезная нагрузка */

   /* Отсылаем датаграмму */
   if (sendto(sockfd,
              sendline,
              dg_length,
              0,
              (struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0)
   {
      perror("Unable to send your message");
      close(sockfd);
      exit(1);
   }

   close(sockfd);
   return 0;
}