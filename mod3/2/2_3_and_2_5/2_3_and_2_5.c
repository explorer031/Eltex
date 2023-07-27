#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* DIR type */
#include <sys/stat.h>   /* struct stat */
#include <dirent.h>     /* struct dirent */
#include <unistd.h>     /* getuid, getgid */


void usage_check(int argc, char* program_name)
{
   if (argc != 2)
   {
      printf("Usage: %s <directory path>\n\n", program_name);
      exit(1);
   }
};

void access_check(char* file_path)
{
   struct stat stbuf;
   if (stat(file_path, &stbuf) || !S_ISDIR(stbuf.st_mode))
   {
      printf("No such directory\n\n");
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
   
   DIR* start_ptr = opendir(argv[1]);  /* points to first entry in dir */

   struct dirent* dir_buf = readdir(start_ptr);  /* read first entry */
   while (dir_buf != NULL)
   {
      char path[257];  /* path to entry */
      snprintf(path, sizeof path, "%s/%s", argv[1], dir_buf->d_name);

      static struct stat st_buf;
      if (stat(path, &st_buf))
         printf("un: ");  /* unknown type (can't get type) */
      else if (S_ISREG(st_buf.st_mode))
         printf("f : ");  /* regular file */
      else if (S_ISDIR(st_buf.st_mode))
         printf("d : ");  /* directory */
      else if (S_ISCHR(st_buf.st_mode))
         printf("cd: ");  /* character device */
      else if (S_ISBLK(st_buf.st_mode))
         printf("bd: ");  /* block device */
      else if (S_ISFIFO(st_buf.st_mode))
         printf("fi: ");  /* FIFO */
      else if (S_ISSOCK(st_buf.st_mode))
         printf("sk: ");  /* socket */

      printf("%s\n", dir_buf->d_name);
      dir_buf = readdir(start_ptr);
   }

   closedir(start_ptr);

   exit(EXIT_SUCCESS);
}
