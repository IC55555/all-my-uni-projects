#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

void main()
{
   DIR *dir = opendir(".");
   struct stat files[256];
   char *names[256];
   int nums[256];
   struct dirent *curfile;
   int n = 0;

   while ((curfile = readdir(dir)) != NULL && n < 256)
   {
      stat(curfile->d_name, &(files[n]));
      names[n] = (char *)malloc(strlen(curfile->d_name) + 1);
      strcpy(names[n], curfile->d_name);
      nums[n] = n;
      n++;
   }

   closedir(dir);
   int i, j;
   int buf;
   for (i = 0; i < n - 1; i++)
      for (j = 0; j < n - i - 1; j++)
      {
         if ((files[nums[j]]).st_mode & S_IFDIR)
         {
            if ((files[nums[j + 1]]).st_mode & S_IFDIR)
            {
               if ((files[nums[j]]).st_mtime < (files[nums[j + 1]]).st_mtime)
               {
                  buf = nums[j + 1];
                  nums[j + 1] = nums[j];
                  nums[j] = buf;
               }
            }
            else
            {
               buf = nums[j + 1];
               nums[j + 1] = nums[j];
               nums[j] = buf;
            }
         }
         else
         {
            if ((files[nums[j + 1]]).st_mode & S_IFDIR)
            {
            }
            else
            {
               if ((files[nums[j]]).st_mtime < (files[nums[j + 1]]).st_mtime)
               {
                  buf = nums[j + 1];
                  nums[j + 1] = nums[j];
                  nums[j] = buf;
               }
            }
         }
      }

   for (i = 0; i < n; i++)
   {
      if (files[nums[i]].st_mode & S_IFDIR)
      {
         printf("Dir %s %s", names[nums[i]], asctime(gmtime(&((files[nums[i]]).st_mtime))));
      }
      else
      {
         printf("File %s %s", names[nums[i]], asctime(gmtime(&((files[nums[i]]).st_mtime))));
      }
   }
}