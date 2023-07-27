#include <stdio.h>
#include <stdlib.h>    /* exit */
#include <sys/stat.h>  /* struct stat */
#include <unistd.h>    /* getuid, getgid */


void usage_check(int argc, char* program_name)
{
   if (argc != 2)
   {
      printf("Usage: %s <file path>\n\n", program_name);
      exit(1);
   }
};

void access_check(char* file_path)
{
   struct stat stbuf;
   if (stat(file_path, &stbuf) || !S_ISREG(stbuf.st_mode))
   {
      printf("No such file\n\n");
      exit(2);
   }

   char uf = stbuf.st_uid == getuid();  /* "are we user?" flag */
   char gf = stbuf.st_gid == getgid();  /* "are we in user's group?" flag */
   
   /* comparison with bit masks (rwx) for rights */
   if ((uf && (stbuf.st_mode & 256) == 256) ||      /* read access for user */
      (!uf && gf && (stbuf.st_mode & 32) == 32) ||  /* read access for group */
      (!uf && !gf && (stbuf.st_mode & 4) == 4))     /* read access for others */
      return;

   printf("Permission denied\n\n");
   exit(3);
};


int main(int argc, char *argv[])
{
   usage_check(argc, argv[0]);
   access_check(argv[1]);

   FILE* file = fopen(argv[1], "r");

   if (file == NULL)
   {
      perror("open failure.");
      exit(4);
   }

   char c = 0;
   while (fread(&c, 1, 1, file) != 0)
      printf("%c", c);

   fclose(file);

   exit(EXIT_SUCCESS);
}
