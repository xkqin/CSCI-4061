/*                          Introduction to Operating Systems
                            CSCI 4061
This program demonstrates the use of stat system call to sum up the file sizes in a directory inside the current directory.
Does not go recursively into sub-directories but sums up the sizes of only "files"*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define NAMESIZE 256

int main(int argc, char *argv[])
{
  /* Declarations */
  char *dirname;
  struct stat statbuf;
  DIR *dp;
  struct dirent *direntry;
  dirname = (char*)malloc(NAMESIZE*sizeof(char));
  int totalsum = 0;
  /* End of declarations */

  if(argc < 2)       /*If the user does not enter any directory name*/
    {
      printf("No directory name specified. Executing the function in the current directory.\n");
      dirname = getcwd(dirname,NAMESIZE);
    }
  else              /* If the user enters a directory name */
    {
      dirname = getcwd(dirname,NAMESIZE);
      strcat(dirname,"/");
      strcat(dirname,argv[1]);

    }
  /* Check if the directory entered exists or not*/
  stat(dirname,&statbuf);
  if(!(S_ISDIR(statbuf.st_mode))){
    printf("The directory name is not valid. Directory does not exist\n");
    exit(0);
  }

  if((dp=opendir(dirname))==NULL){
    perror("Error while opening the directory");
    exit(0);
  }
  /* Loop through the directory structure */
  chdir(dirname); //previously missing
  while( (direntry = readdir(dp)) != NULL )
    {
      stat(direntry->d_name,&statbuf);
      if(!(S_ISDIR(statbuf.st_mode)))
	{
	  printf("The size of file %s is :%d bytes\n",direntry->d_name,(int) statbuf.st_size);
	  totalsum += (int) statbuf.st_size;
	}
    }

  printf("The total size of the files in %s is : %d bytes\n",dirname,totalsum);
}
