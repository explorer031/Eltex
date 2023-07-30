#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <sys/stat.h>
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork */
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


unsigned char end_flag = 0;
void sigint_handler(int sig)
{
   end_flag = 1;
   return;
}


int main()
{
   /* create semaphore */
   sem_unlink("4_910_sem");
   sem_t* sem_addr = sem_open("4_910_sem", O_CREAT | O_RDWR, 0666, 1);
   if (sem_addr == NULL)
   {
      perror("Unable to create semaphore");
      exit(1);
   }

   /* create shared memory */
   shm_unlink("4_910_shmem");
   int shmid = shm_open("4_910_shmem", O_CREAT | O_RDWR, 0666);
   if (shmid == -1)
   {
      perror("Unable to create shared memory");
      exit(2);
   }

   /* expand shared memory */
   if (ftruncate(shmid, 12 * sizeof(char) + sizeof(short) + sizeof(double)) == -1)
   {
      perror("Unable to truncate shared memory");
      exit(3);
   }
   
   srand(time(NULL));

   unsigned int total = 0;
   while (!end_flag)
   {
      unsigned char count = rand() % 10 + 1;

      pid_t child_1 = fork();
      if (!child_1)  /* child (for max/min) */
      {
         sleep(3);
         char* shm_addr = mmap(0, 12 * sizeof(char) + sizeof(short) + sizeof(double),
      PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

         unsigned char min = 255, max = 0;

         for (unsigned char i = 0; i < count; ++i)
         {
            unsigned char number = 0;

            sem_wait(sem_addr);
            memcpy(&number, shm_addr + i * sizeof(char), sizeof(number));
            sem_post(sem_addr);

            if (number < min) min = number;
            if (number > max) max = number;

            sleep(1);
         }

         /* writing max/min in memory */
         memcpy(shm_addr + count * sizeof(char), &min, sizeof(min));
         memcpy(shm_addr + (count + 1) * sizeof(char), &max, sizeof(max));

         exit(EXIT_SUCCESS);
      }
      else if (child_1 == -1)
      {
         perror("Fork failure");
         exit(4);
      }

      pid_t child_2 = fork();
      if (!child_2)  /* child (for sum and arithmetic mean) */
      {
         sleep(3);
         char* shm_addr = mmap(0, 12 * sizeof(char) + sizeof(short) + sizeof(double),
      PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
         
         unsigned short sum = 0;
         double arithmetic_mean = 0;

         for (unsigned char i = 0; i < count; ++i)
         {
            unsigned char number = 0;
            sem_wait(sem_addr);
            memcpy(&number, shm_addr + i * sizeof(char), sizeof(number));
            sem_post(sem_addr);

            sum += number;

            sleep(1);
         }

         arithmetic_mean = sum / (double)count;

         /* writing sum and arithmetic mean in memory */
         memcpy(shm_addr + (count + 2) * sizeof(char), &sum, sizeof(sum));
         memcpy(shm_addr + (count + 2) * sizeof(char) + sizeof(sum),
            &arithmetic_mean, sizeof(double));

         exit(EXIT_SUCCESS);
      }
      else if (child_2 == -1)
      {
         perror("Fork failure");
         exit(5);
      }

      char* shm_addr = mmap(0, 12 * sizeof(char) + sizeof(short) + sizeof(double),
      PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
      
      signal(SIGINT, sigint_handler);

      printf("numbers: ");
      for (unsigned char i = 0; i < count; ++i)
      {
         unsigned char number = rand() % 256;
         printf("%hhu ", number);
         
         sem_wait(sem_addr);
         memcpy(shm_addr + i * 1, &number, 1);
         sem_post(sem_addr);

         sleep(1);
      }

      wait(NULL);
      wait(NULL);

      unsigned char min = 0, max = 0;
      unsigned short sum = 0;
      double arithmetic_mean = 0;

      /* reading results */
      memcpy(&min, shm_addr + count * sizeof(char), sizeof(min));
      memcpy(&max, shm_addr + (count + 1) * sizeof(char), sizeof(max));
      memcpy(&sum, shm_addr + (count + 2) * sizeof(char), sizeof(sum));
      memcpy(&arithmetic_mean, shm_addr + (count + 2) * sizeof(char) + sizeof(sum), sizeof(double));

      /* printing results */
      printf("\nmin = %hhu\n", min);
      printf("max = %hhu\n", max);
      printf("sum = %hu\n", sum);
      printf("arithmetic mean = %f\n\n", arithmetic_mean);

      ++total;
   }

   printf("Total count is %u\n", total);

   sem_unlink("4_910_sem");
   shm_unlink("4_910_shmem");
   exit(EXIT_SUCCESS);
   return 0;
}