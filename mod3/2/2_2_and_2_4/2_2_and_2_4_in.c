#include <stdio.h>
#include <stdlib.h>    /* exit */
#include <string.h>    /* strlen */
#include <sys/stat.h>  /* struct stat */
#include <unistd.h>    /* getuid, getgid */

void usage_check(int argc, char* program_name)
{
   if (argc != 3)
   {
      printf("Usage: %s <string to append> <file path>\n\n", program_name);
      exit(1);
   }
};

void access_check(char* file_path, struct stat* stbuf)
{
   char uf = stbuf->st_uid == getuid();  /* "are we user?" flag */
   char gf = stbuf->st_gid == getgid();  /* "are we in user's group?" flag */
   
   /* comparison with bit masks (rwx) for rights */
   if ((uf && (stbuf->st_mode & 256) == 256) ||      /* read access for user */
      (!uf && gf && (stbuf->st_mode & 32) == 32) ||  /* read access for group */
      (!uf && !gf && (stbuf->st_mode & 4) == 4))     /* read access for others */
      return;

   printf("Permission denied\n\n");
   exit(3);
};


int main(int argc, char *argv[])
{
   usage_check(argc, argv[0]);

   char exists = 1;
   struct stat stbuf;
   if (stat(argv[2], &stbuf) || !S_ISREG(stbuf.st_mode))
   {
      exists = 0;
      printf("No such file. Create one? [Y]es/[n]o: ");

      char opt = 'n';
      scanf("%c", &opt);

      if (opt == 'n')
         exit(2);
   }

   if (exists)
      access_check(argv[2], &stbuf);

   FILE* file = fopen(argv[2], "a");

   if (file == NULL)
   {
      perror("open failure.");
      exit(4);
   }

   fwrite(argv[1], sizeof(argv[1][0]), strlen(argv[1]), file);
   fclose(file);

   exit(EXIT_SUCCESS);
}
