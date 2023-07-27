#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main()
{
   while (1)
   {
      printf("\n>>> ");

      static char path[255] = { 0 };
      scanf("%s", path);
      
      unsigned char end_flag = 0;
      if (getchar() == '\n')
         end_flag = 1;

      char* argv[10] = { path };  /* 0th argument is program name */
      static char argv_help[10][255] = { { 0 } };
      static char arg_buf[255] = { 0 };
      unsigned char argc = 1;

      /* reading program arguments */
      while (!end_flag && argc < 10)
      {
         char c = getchar();

         if (c != ' ' && c != '\n')
            strncat(arg_buf, &c, 1);
         else
         {
            /* copying the argument from buffer */
            strncpy(argv_help[argc], arg_buf, 255);
            argv[argc] = argv_help[argc];

            /* clearing buffer */
            for (int i = 0; i < 255; ++i)
               arg_buf[i] = 0;

            ++argc;
         }

         if (c == '\n')
            end_flag = 1;
      }

      pid_t pid = -1;

      switch (pid = fork())
      {
      case -1:
         perror("Fork failure.");
         break;
      case 0:
         execvp(path, argv);
         exit(EXIT_SUCCESS);
         break;
      default:
         wait(NULL);
      }

      /* clearing arrays */
      for (int i = 0; i < 10; ++i)
         for (int j = 0; j < 255; ++j)
            argv_help[i][j] = 0;

      for (int i = 0; i < 255; ++i)
         path[i] = 0;

   }

   exit(EXIT_SUCCESS);
}
