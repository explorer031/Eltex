#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <semaphore.h>
#include <fcntl.h>

#define str1 "Enter 1st parameter\n"
#define str2 "Enter 2nd parameter\n"
#define act  "Enter action\n"

// количество активных пользователей
sem_t* nclients;

// функция обслуживания
// подключившихся пользователей
void dostuff(int);
// функция обработки ошибок
void error(const char *msg)
{
   perror(msg);
   exit(EXIT_FAILURE);
}

// печать количества активных
// пользователей
void printusers()
{
   int sval = 0;
   sem_getvalue(nclients, &sval);

   if (sval)
      printf("%d users online\n", sval);
   else
      printf("No User online\n");
}

// функция обработки данных
int myfunc(int a, int b, char c)
{
   if (c == '+')
      return a + b;
   if (c == '-')
      return a - b;
   if (c == '*')
      return a * b;
   if (c == '/')
      return a / b;

   return 0;
}


int main(int argc, char *argv[])
{
   printf("===== TCP SERVER =====\n");

   char buff[1024];                        // Буфер для различных нужд
   int sockfd, newsockfd;                  // дескрипторы сокетов
   int portno;                             // номер порта
   int pid;                                // id номер потока
   socklen_t clilen;                       // размер адреса клиента типа socklen_t
   struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента

   /* create read semaphore */
   sem_unlink("nclients_sem");
   nclients = sem_open("nclients_sem", O_CREAT, 0666, 0);
   if (nclients == NULL)
   {
      perror("Unable to create read semaphore");
      exit(EXIT_FAILURE);
   }

   // ошибка в случае если мы не указали порт
   if (argc < 2)
      error("ERROR, no port provided\n");

   // Шаг 1 - создание сокета
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0)
      error("ERROR opening socket");

   // Шаг 2 - связывание сокета с локальным адресом
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
   serv_addr.sin_port = htons(portno);

   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR on binding");

   // Шаг 3 - ожидание подключений, размер очереди - 5
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);

   // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

      if (newsockfd < 0)
         error("ERROR on accept");

      sem_post(nclients);

      // вывод сведений о клиенте
      struct hostent *hst;
      hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);

      printf("+%s [%s] new connect!\n",
             (hst != NULL) ? hst->h_name : "Unknown host",
             (char *)inet_ntoa(cli_addr.sin_addr));

      printusers();

      pid = fork();
      if (pid < 0)
         error("ERROR on fork");
      if (pid == 0)
      {
         close(sockfd);
         dostuff(newsockfd);
         exit(EXIT_SUCCESS);
      }
      else
         close(newsockfd);
   }

   close(sockfd);
   sem_close(nclients);
   sem_unlink("nclients_sem");
   exit(EXIT_SUCCESS);
}


void dostuff(int sock)
{
   char buff[20 * 1024];

   while (1)
   {
      sleep(1);
      int a = 0, b = 0;       // переменные для myfunc
      char c = 0;

      // отправляем клиенту сообщение о первом аргументе
      write(sock, str1, sizeof(str1));
      // обработка первого параметра
      int bytes_recv = read(sock, &buff[0], sizeof(buff));
      if (bytes_recv < 0 || !strcmp(buff, "quit\n")) goto end;
      a = atoi(buff); // преобразование первого параметра в int

      // отправляем клиенту сообщение о втором аргументе
      write(sock, str2, sizeof(str2));
      bytes_recv = read(sock, &buff[0], sizeof(buff));
      if (bytes_recv < 0 || !strcmp(buff, "quit\n")) goto end;
      b = atoi(buff); // преобразование второго параметра в int

      // отправляем клиенту сообщение об арифметическом действии
      write(sock, act, sizeof(act));
      bytes_recv = read(sock, &buff[0], sizeof(buff));
      if (bytes_recv < 0 || !strcmp(buff, "quit\n")) goto end;
      c = buff[0]; // считывание действия

      printf("New task: %d %c %d = ", a, c, b);
      a = myfunc(a, b, c); // вызов пользовательской функции
      printf("%d\n", a);

      bzero(buff, sizeof(buff));

      // преобразование результата в строку
      sprintf(buff, "%d", a);
      buff[strlen(buff)] = '\n';
      buff[strlen(buff) + 1] = '\0';

      // отправляем клиенту результат
      write(sock, &buff[0], strlen(buff) + 1);

      bzero(buff, sizeof(buff));
   }

end:
   close(sock);
   sem_wait(nclients); // уменьшаем счетчик активных клиентов
   printf("-disconnect\n");
   printusers();

   exit(EXIT_FAILURE);
}