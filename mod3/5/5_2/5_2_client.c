#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


unsigned char end_flag = 0;
void sigtstp_handler(int sig)
{
   printf("\r");
   end_flag = 1;
   return;
}

int sockfd;  /* Дескриптор сокета */
void sigint_handler(int sig)
{
   printf("\r");
   close(sockfd);
   exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
   int n, len;                           /* Переменные для различных длин и количества символов */
   char sendline[1000], recvline[1000];  /* Массивы для отсылаемой и принятой строки */
   struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов сервера и клиента */

   /* Проверка правильности использования */
   if (argc != 4)
   {
      printf("Usage: ./5_2_client <IP address> <send_port> <recv_port>\n");
      exit(1);
   }

   /* Создаем UDP сокет */
   if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
   {
      perror("Unable to create socket"); /* Печатаем сообщение об ошибке */
      exit(2);
   }

   /* Заполняем структуру для адреса клиента */
   bzero(&cliaddr, sizeof(cliaddr));
   cliaddr.sin_family = AF_INET;
   cliaddr.sin_port = htons(atoi(argv[3]));
   cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   /* Настраиваем адрес сокета */
   if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
   {
      perror("Unable to bind socket");
      close(sockfd); /* По окончании работы закрываем дескриптор сокета */
      exit(3);
   }

   /* Заполняем структуру для адреса собеседника */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(argv[2]));
   if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
   {
      printf("Invalid IP address\n");
      close(sockfd); /* По окончании работы закрываем дескриптор сокета */
      exit(4);
   }

   puts("----------------------------------");
   puts("Type Ctrl+Z to enter your message.");
   puts("Type Ctrl+C to stop communication.");
   puts("----------------------------------\n");
   signal(SIGTSTP, sigtstp_handler);
   signal(SIGINT, sigint_handler);

   /* Делаем сокет неблокирующимся */
   fcntl(sockfd, F_SETFL, O_NONBLOCK);

   while (1)
   {
      end_flag = 0;
      while(!end_flag)
      {
         /* Ожидаем ответа и читаем его. Максимальная
         допустимая длина датаграммы – 1000 символов,
         адрес отправителя нам не нужен */
         if ((n = recvfrom(sockfd,
                           recvline,
                           1000,
                           0,
                           (struct sockaddr *)NULL,
                           NULL)) > 0)
         {
         /* Печатаем пришедший ответ */
         printf("Him => %s\n", recvline);
         }
      }

      /* Вводим строку, которую отошлем серверу */
      printf("You => ");
      fgets(sendline, 1000, stdin);

      /* Отсылаем датаграмму */
      if (sendto(sockfd,
                 sendline,
                 strlen(sendline) + 1,
                 0,
                 (struct sockaddr *)&servaddr,
                 sizeof(servaddr)) < 0)
      {
         perror("Unable to send your message. Try again");
      }

      puts("");
   }

   return 0;
}