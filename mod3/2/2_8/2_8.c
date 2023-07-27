#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <fcntl.h>      /* open() mods */
#include <time.h>       /* time */
#include <unistd.h>     /* fork, pipe */
#include <sys/wait.h>   /* wait */


void usage_check(int argc, char* argv[])
{
   if (argc != 2 || atoi(argv[1]) < 0)
   {
      printf("Usage: %s <positive integer>\n\n", argv[0]);
      exit(1);
   }
};


int main(int argc, char *argv[])
{
   usage_check(argc, argv);

   int number = atoi(argv[1]);
   
   int to_child[2] = { 0 },
      to_parent[2] = { 0 };
   if (pipe(to_parent) == -1 || pipe(to_child))
   {
      close(to_parent[0]);
      close(to_parent[1]);
      close(to_child[0]);
      close(to_child[1]);
      perror("Unable to create pipe");
      exit(1);
   }

   pid_t pid = -1;
   switch (pid = fork())
   {
   case -1:  /* error */
      perror("Fork failure\n");
      break;

   case 0:  /* child */
      srand(time(NULL));  /* updating the seed of PRNG */
      for (int i = 0; i < number; ++i)
      {
         unsigned char random = rand() % 100;
         write(to_parent[1], &random, sizeof(random));

         unsigned short doubled = 0;
         read(to_child[0], &doubled, sizeof(doubled));
         printf("Child: %d: %hu\n", i + 1, doubled);
      }
      break;

   default:  /* parent */
      FILE* file = fopen("2_8.txt", "w");

      for (int i = 0; i < number; ++i)
      {
         unsigned char res = 0;
         read(to_parent[0], &res, sizeof(res));
         printf("Parent: %d: %hhu\n", i + 1, res);
         fprintf(file, "%d: %hhu\n", i + 1, res);

         unsigned short doubled = res * 2;
         write(to_child[1], &doubled, sizeof(doubled));
      }
 
      fclose(file);
   }

   close(to_parent[0]);
   close(to_parent[1]);
   close(to_child[0]);
   close(to_child[1]);

   exit(EXIT_SUCCESS);
}
