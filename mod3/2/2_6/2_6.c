#include <stdio.h>
#include <stdlib.h>     /* exit */
#include <string.h>     /* strcmp */
#include <sys/types.h>  /* DIR type */
#include <sys/stat.h>   /* struct stat */
#include <dirent.h>     /* struct dirent */
#include <unistd.h>     /* getuid, getgid */


void usage_check(int argc, char* program_name)
{
   if (argc != 1)
   {
      printf("Usage: %s\n\n", program_name);
      exit(1);
   }
};

int access_check(char* file_path)
{
   struct stat stbuf;
   if (stat(file_path, &stbuf) || !S_ISDIR(stbuf.st_mode))
   {
      printf("No such directory\n\n");
      return 1;
   }

   char uf = stbuf.st_uid == getuid();  /* "are we user?" flag */
   char gf = stbuf.st_gid == getgid();  /* "are we in user's group?" flag */
   
   /* comparison with bit masks (rwx) for rights */
   if ((uf && (stbuf.st_mode & 256) == 256) ||      /* read access for user */
      (!uf && gf && (stbuf.st_mode & 32) == 32) ||  /* read access for group */
      (!uf && !gf && (stbuf.st_mode & 4) == 4))     /* read access for others */
      return 0;

   printf("Permission denied\n");
   return 2;
};

void ls(char* d_path)
{
   DIR* start_ptr = opendir(d_path);  /* points to first entry in dir */

   struct dirent* dir_buf = readdir(start_ptr);  /* read first entry */
   while (dir_buf != NULL)
   {
      char path[257];  /* path to entry */
      snprintf(path, sizeof path, "%s/%s", d_path, dir_buf->d_name);

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
}

int main(int argc, char *argv[])
{
   usage_check(argc, argv[0]);

   puts("commands:");
   puts("cd <directory> - change directory");
   puts("ls <directory> - output the contents of the directory");
   while (1)
   {
      static char curr_dir[255] = { 0 };
      getcwd(curr_dir, 255);
      printf("\n./2_6: %s >>> ", curr_dir);

      static char cmd[3] = { 0 };
      static char dir_path[128] = { 0 };
      scanf("%s", cmd);
      
      unsigned char no_args = 0;
      if (getchar() == '\n')
         no_args = 1;
      else
         scanf("%s", dir_path);

      if (!strcmp(cmd, "cd") && !no_args)
      {
         if (chdir(dir_path))
            puts("Failed to change directory");
      }
      else if (!strcmp(cmd, "ls"))
      {
         if (no_args)
         {
            if(!access_check(curr_dir))
               ls(curr_dir);
         }
         else if(!access_check(dir_path))
            ls(dir_path);
      }
      else
         puts("Wrong command!");

      /* clearing arrays */
      for (int i = 0; i < 3; ++i)
         cmd[i] = 0;
      for (int i = 0; i < 255; ++i)
         dir_path[i] = 0;
   }   

   exit(EXIT_SUCCESS);
}
