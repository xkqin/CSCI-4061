/***********************************************************************************************

 CSci 4061 Fall 2017
 Assignment# 2:   I/O Programming on UNIX/LINUX

 Student name: <Weihang Xiao>   <Xukun Qin>
 Student ID:   <5423302>   <5188260>
 X500 id: <xiao0155>, <qinxx215>

 Operating system on which you tested your code: Linux, Unix, MacOS
 CSELABS machine: <csel-kh4250-10.cselabs.umn.edu>

 GROUP INSTRUCTION:  Please make only ONLY one  submission when working in a group.

***********************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#define NAMESIZE 256
#define TOKENSIZE 100
#define MAX_DIR_PATH 2048
#define BUFSIZE 256
off_t FLF = 0;
char* FL = NULL;
off_t SLF = 0;
char* SL = NULL;
off_t TLF = 0;
char* TL = NULL;
off_t count = 0;
struct stat statbuf;
char accessmodestring[10];
char transfermodestring[10];
int number;
char linkcheck = 0;
/*
  find the 0 file in the given directory tree
  parameters: input_dir_name: input pathname

*/
void Result2(char *input_dir_name)
{
struct dirent *direntry;
  DIR *dp;
  if(chdir(input_dir_name) == -1)
  {

      perror ("can not change directory ");
      exit (1);

  }
  if ( (dp = opendir(input_dir_name)) == NULL)
  {
      perror("ERROR: Unable to open directory\n");
      exit(4);
  }
  //go through every element in the direntry
	while( (direntry = readdir(dp)) != NULL )
		{
			/*************************/

			if(lstat(direntry->d_name,&statbuf) == -1)
      {
        perror("lstat");
        exit(EXIT_FAILURE);
      }
			if (strcmp (direntry->d_name, ".") == 0)
	continue;

			if (strcmp (direntry->d_name, "..") == 0)
	continue;

			if (S_ISDIR(statbuf.st_mode)) {
					/* check this directory */
        //  printf("%s\n",direntry->d_name );
      if(  realpath(direntry->d_name,input_dir_name) ==NULL)
        {
          perror("ERROR: 11Unable to have the realpath\n");
          exit(4);
        }
				Result2(input_dir_name);

				  /* Finally, restore the original working directory. */

			}

      if (S_ISREG(statbuf.st_mode)) {
        char *file;
        file = (char *) malloc(NAMESIZE * sizeof(char));
        if(realpath(direntry->d_name, file) == NULL)
        {
          perror("ERROR: Unable to have the realpath\n");
          exit(4);
        }
        if(statbuf.st_size == 0)
        {
            printf("The 0 size file is %s \n", file);
        }
			}
			/*************************/
		}
    if (chdir ("..") == -1)
      {
        perror ("can not go back ");
        exit (1);
      }
      if (closedir(dp) == -1)
      {
        perror ( "Error in closing the directory:");
        exit(4);
      }
}

// get accessmode string and transfer it to a char array with 9-digits number.
//
char *getAccessModeString ( const mode_t mode, char mstr[] )
{
    sprintf(mstr, "000000000");

    // Get user access bits
    if ( S_IRUSR & mode )  mstr[0] = '1';
    if ( S_IWUSR & mode )  mstr[1] = '1';
    if ( S_IXUSR & mode )  mstr[2] = '1';

    // Get group access bits
    if ( S_IRGRP & mode )  mstr[3] = '1';
    if ( S_IWGRP & mode )  mstr[4] = '1';
    if ( S_IXGRP & mode )  mstr[5] = '1';

    // Get other access bits
    if ( S_IROTH & mode )  mstr[6] = '1';
    if ( S_IWOTH & mode )  mstr[7] = '1';
    if ( S_IXOTH & mode )  mstr[8] = '1';
}

// transfer three input digits to a char[] which contains 9-digits number.
//for example, transfer 777 to 111111111.
void transferOctalToAccessmodeStr (int number, char mstr[])
{
  // get three seperate digits from the input number
	int first = number % 10;
	int second = number / 10 % 10;
	int third = number / 100 % 10;
  //transfer each digit number to a char type
	char a = first + '0';
	char b = second + '0';
	char c = third + '0';
  sprintf(mstr, "000000000");

	char value[3] = {c, b, a};
	int j = 0;
	for (int i = 0; i < 3; i++)
	{

			if (value[i] == '7')
			{
				mstr[j] = '1';
				mstr[j + 1] = '1';
				mstr[j + 2] = '1';
				j+=3;
			} else if (value[i] == '6')
			{
				mstr[j] = '1';
				mstr[j + 1] = '1';
				mstr[j + 2] = '0';
				j+=3;
			} else if (value[i] == '5')
			{
				mstr[j] = '1';
				mstr[j + 1] = '0';
				mstr[j + 2] = '1';
				j+=3;
			} else if (value[i] == '4')
			{
				mstr[j] = '1';
				mstr[j + 1] = '0';
				mstr[j + 2] = '0';
				j+=3;
			} else if (value[i] == '3')
			{
				mstr[j] = '0';
				mstr[j + 1] = '1';
				mstr[j + 2] = '1';
				j+=3;
			}
			else if (value[i] == '2')
			{
				mstr[j] = '0';
				mstr[j + 1] = '1';
				mstr[j + 2] = '0';
				j+=3;
			}
			else if (value[i] == '1')
			{
				mstr[j] = '0';
				mstr[j + 1] = '0';
				mstr[j + 2] = '1';
				j+=3;
			}
			else if (value[0] == '0')
			{
				mstr[j] = '0';
				mstr[j + 1] = '0';
				mstr[j + 2] = '0';
				j+=3;
			}

	}

}
/*
  find the three largest file and
  the permission of a specificly file in the given directory tree
  parameters: input_dir_name: input pathname  choice: which fucntionality
  do you want

*/
void Result(char *input_dir_name, int choice)
{

  struct dirent *direntry;
  DIR *dp;
  if(chdir(input_dir_name) == -1)
  {

      perror ("can not change directory ");
      exit (1);

  }
  if ( (dp = opendir(input_dir_name)) == NULL)
  {
      perror("ERROR: Unable to open directory\n");
      exit(4);
  }
    //go through every element in the direntry
	while( (direntry = readdir(dp)) != NULL )
		{
      count++;
			/*************************/
			if (lstat(direntry->d_name, &statbuf) ==-1)
      {
        perror("lstat");
        exit(EXIT_FAILURE);
      }
			if (strcmp (direntry->d_name, ".") == 0)
			continue;

			if (strcmp (direntry->d_name, "..") == 0)
			continue;


          if (S_ISDIR(statbuf.st_mode)) {

              /* check this directory */
            if(realpath(direntry->d_name,input_dir_name) ==NULL)
            {
              perror("ERROR: Unable to have the realpath\n");
              exit(4);
            }
            Result(input_dir_name, choice);
          //  continue;
              /* Finally, restore the original working directory. */
          }
			if (!(S_ISDIR(statbuf.st_mode))) {
            char *file;
            file = (char *) malloc(NAMESIZE * sizeof(char));
                //          printf("%s123\n",direntry->d_name );
            if (realpath(direntry->d_name, file) == NULL )
            {
              if(S_ISLNK(statbuf.st_mode))
              {
                linkcheck = 1;
              }
              else{
              perror("ERROR: Unable to have the realpath\n");

              exit(4);
            }
            }

              // if the user enter 3, then execute this case.
            	if (choice == 3){
              // find each file's accessmode using 9 digits to represent
            	 getAccessModeString(statbuf.st_mode, accessmodestring);
               // get the 9 digits number from the input number
            	 transferOctalToAccessmodeStr(number, transfermodestring);
               // conpare input number permission and each file's permission, if it is the same, then print the path.
            	 if (strcmp(accessmodestring, transfermodestring) == 0)
            	 {
                 if(linkcheck == 1)
                 {
                   char *link;
                   link = (char *) malloc(NAMESIZE * sizeof(char));
                   link = getcwd(link,NAMESIZE);
                   strcat(link, "/");
                   strcat(link, direntry->d_name);
                   printf("The link \"%s\" is a bad soft link which poiting to a file that dones not exist and the permission is %d \n", link,number);
                   linkcheck = 0;
                   continue;
                 }
                  if ( !(S_ISLNK(statbuf.st_mode)) ) {// if the item is not a symbolic link
            	 		printf("The permission of file \"%s\" is %d : \n", file, number);
                  continue;
                } else { //other, get the path of point to another file.
                  char *link;
                  link = (char *) malloc(NAMESIZE * sizeof(char));
                  link = getcwd(link,NAMESIZE);
                  strcat(link, "/");
                  strcat(link, direntry->d_name);
                  printf("The permission of link \"%s\", pointing to \"%s\" is %d \n", link, file, number);
            	 		continue;
                }
              }
            }
              //find the three largest size file pathname and size
				if (statbuf.st_size > FLF)
				{
					 TLF = SLF;
					 SLF = FLF;
           TL  = SL;
					 SL = FL;
					 FLF = statbuf.st_size;
					FL = file;

				}
				else if (statbuf.st_size > SLF )
					{
								 TLF = SLF;
								 TL = SL;
								SLF = statbuf.st_size;
								SL = file;

					}
				else if (statbuf.st_size > TLF)
					{
								TLF = statbuf.st_size;
								TL = file;

					}

			}

			/*************************/
		}
    if (chdir ("..") == -1)
      {
        perror ("can not go back ");
        exit (1);
      }
		if (closedir(dp) == -1)
    {
      perror ( "Error in closing the directory:");
      exit(4);
    }
}
void  copyFile (char* srcfile, char * copyfile)  {
  char  buffer[BUFSIZE];
  ssize_t  count;
  int fdin,  fdout;
  mode_t perms;
  struct stat st;
  stat(srcfile, &st);
  perms = st.st_mode;


  if  ( (fdin = open (srcfile,  O_RDONLY))  == -1) {
    perror ( "Error in opening the input file:");
    printf("%s\n", srcfile);
   exit (2);
  }
  /*    printf( "Input file opened with descriptor %d \n", fdin);  */

  if  ( (fdout = open (copyfile, (O_WRONLY | O_CREAT),  perms)) == -1 ) {
    perror ( "Error in creating the output file:");
    exit (2);
  }
  /*    printf( "Output file opened with descriptor %d \n", fdout);  */

  while ( (count=read(fdin, buffer, BUFSIZE)) > 0 ) {
    if ( write (fdout, buffer, count) != count )
      perror ("Error in writing" );
  }

  if ( count == -1 ) {
    perror ( "Error while reading the input file: ");
  }

  close(fdin);
  close(fdout);
}
/*
  copy the file direntry and symbolic link in source pathname to
  backup pathname
  parameters: rspath: the source pathname  bkpath: the backup pathname

*/

void ResultBack(char* rspath, char* bkpath) {
  struct dirent *direntry;

    DIR *dp;

    if(chdir(rspath) == -1)
    {
        perror ("can not change directory ");
        exit (1);
    }

    if ( (dp = opendir(rspath)) == NULL)
    {
        perror("ERROR: Unable to open directory\n");
        exit(4);
    }

  	while( (direntry = readdir(dp)) != NULL )
  		{
  			/*************************/

        if (lstat(direntry->d_name, &statbuf) == -1) {
          {
            perror("lstat");
            exit(EXIT_FAILURE);
          }
        }
        if (strcmp (direntry->d_name, ".") == 0)
    continue;

        if (strcmp (direntry->d_name, "..") == 0)
    continue;
    // check if it is a file and copy it to the backp pathname
          if (S_ISREG(statbuf.st_mode)) {
            char * srcp,*backp;
            srcp = (char *) malloc(NAMESIZE * sizeof(char));
            backp = (char *) malloc(NAMESIZE * sizeof(char));
            strcpy(srcp, rspath);
            strcat(srcp, "/");
            strcat(srcp, direntry->d_name);

            strcpy(backp, bkpath);
            strcat(backp, "/");
            strcat(backp, direntry->d_name);

            copyFile(srcp,backp);
          }
          // check if it is a directory recursivly to the directory tree
          if (S_ISDIR(statbuf.st_mode)) {

            char * dirP,*scrP,*realD;
            dirP = (char *) malloc(NAMESIZE * sizeof(char));
            scrP = (char *) malloc(NAMESIZE * sizeof(char));
            realD = (char *) malloc(NAMESIZE * sizeof(char));
          //  realpath(direntry->d_name, rspath);
            strcpy(scrP, rspath);
            strcat(scrP, "/");
            strcat(scrP, direntry->d_name);
            strcpy(dirP, bkpath);
            strcat(dirP, "/");
            strcat(dirP, direntry->d_name);
            mode_t perm;
            struct stat st;
            if(stat(direntry->d_name, &st) == -1)
            {
              perror("stat");
              exit(EXIT_FAILURE);
            }
            perm = st.st_mode;
            //   if((realpath(dirP,realD)) ==NULL)
            // {
            //   perror("ERROR: ddddUnable to have the realpath\n");
            //   exit(4);
            // }
            if( (mkdir(dirP, perm)) != 0){
              printf("Can not creat that directory\n");
              exit(4);
              }
            ResultBack(scrP,dirP);
          }
          // check if it is a symbolic link
          if(S_ISLNK(statbuf.st_mode)){
            char * targetlk,*namelk;
            targetlk = (char *) malloc(NAMESIZE * sizeof(char));
            namelk = (char *) malloc(NAMESIZE * sizeof(char));
          if(  realpath(direntry->d_name, targetlk) ==NULL)
            {
              perror("ERROR: lllUnable to have the realpath\n");
              exit(4);
            }
            strcpy(namelk, bkpath);
            strcat(namelk, "/");
            strcat(namelk, direntry->d_name);
            if ( (symlink(targetlk,namelk)) == -1) {
              perror("Failed to create a symbolic link");
              exit(2);
            }

          }
      }
      if (chdir ("..") == -1)
        {
          perror ("can not go back ");
          exit (1);
        }
}

int main(int srcfile, char *argv[])
{
	int choice = -1;
	char *input_dir_name, *dirpath, *chptr, *backpath,*backpath2,*backpath3;
	struct stat statbuf;
	struct dirent *direntry;
	DIR *dp;
	input_dir_name = (char *) malloc(NAMESIZE * sizeof(char));
	dirpath = (char *) malloc(NAMESIZE * sizeof(char));
  backpath = (char *) malloc(NAMESIZE * sizeof(char));
  backpath2 = (char *) malloc(NAMESIZE * sizeof(char));
	printf("SELECT THE FUNCTION YOU WANT TO EXECUTE:\n");
	printf("1. Find the 3 largest files in a directory\n");
	printf("2. List all zero length files in a directory\n");
	printf("3. Find all files with permission 777 in a directory\n");
	printf("4. Create a backup of a directory\n");
	printf("5. Exit\n");
	printf("\n");
	printf("ENTER YOUR CHOICE: ");
	scanf("%d", &choice);
	printf("Enter a directory name in the current directory: ");
	scanf("%s", input_dir_name);
	/**********************************************************/
	/*Form a full path to the directory and check if it exists*/
	/**********************************************************/

  if (realpath(input_dir_name, dirpath) == NULL)
  {
    perror("ERROR: Unable to have the realpath\n");
    exit(4);
  }

	if(choice == 1){
		printf("\nEXECUTING \"1. Find the 3 largest files in a directory\"\n");
		/********************************************************/
		/**************Function to perform choice 1**************/
		/********************************************************/
		Result(dirpath,1);
				if (count >= 5)
				{
          printf ("The pathname of first largest file is %s/\n The size is %ld\n", FL,FLF);
        	printf ("The pathname of second largest file is %s/\n The size is %ld\n",SL,SLF);
        	printf ("The pathname of third largest file is %s/\n The size is %ld\n",TL,TLF);
			 }
			 if (count == 4)
			 {
         printf ("The pathname of first largest file is %s/\n The size is %ld\n", FL,FLF);
       	printf ("The pathname of second largest file is %s/\n The size is %ld\n",SL,SLF);
			}
			if (count == 3)
			{
    printf ("The pathname of first largest file is %s/\n The size is %ld\n", FL,FLF);
		 }
		 if (count == 2)
		 {
			 printf("This directory is empty\n");
		}

	}

	else if(choice == 2){
		printf("\nEXECUTING \"2. List all zero length files in a directory\"\n");
    Result2(dirpath);
		/********************************************************/
		/**************Function to perform choice 2**************/
		/********************************************************/

	}

	else if(choice == 3){
		printf("\nEXECUTING \"3. Find all files with permission 777 in a directory\"\n");
		/********************************************************/
		/**************Function to perform choice 3**************/
		/********************************************************/
    printf("\nType in three digit octal permission number for rwx: ");
		scanf("%d", &number);
		//printf("\nthe number is %d: ", number);
		Result(dirpath, 3);
	}

	else if(choice == 4){
		printf("\nEXECUTING \"4. Create a backup of a directory\"\n");
    char  buffer[BUFSIZE];
    time_t  clocktime;
    struct tm  *timeinfo;
       time (&clocktime);
       timeinfo = localtime( &clocktime );
       strftime(buffer, 64, "%b-%d-%Y-%H-%M-%S", timeinfo);
       mode_t perm;
       struct stat st;
      if( lstat(input_dir_name, &st) == -1)
      {
        perror("stat");
        exit(EXIT_FAILURE);
      }
       perm = st.st_mode;
  if(  (mkdir(dirpath,perm)) != 0) {
    //double check if the directory exit
    if (errno == EEXIST){

      strcpy(backpath,dirpath);
      strcat(backpath, ".bak");
      if( (mkdir(backpath,perm)) != 0){
        if(errno == EEXIST){
          //if the directory with name .bak exists creat move it to name with time
          strcpy(backpath2,backpath);
          strcat(backpath2, ".");
          strcat(backpath2, buffer);
        if( (mkdir(backpath2,perm)) != 0){
          printf("Can not creat that directory2\n");
          }
          //If a directory by that name exists, then
          //move it to “directoryName.bak” appended with current date-time string
        if(rename(backpath,backpath2) !=0)
        {
          perror("ERROR: Unable to rename\n");
          exit(4);
        }

          printf("A bak up directory name with time is created to :");
         printf("%s\n", backpath2);
        }

      }
      else{
              //if the direntry exists create backup dir with .bak
        //copy dirpath to backpath
        ResultBack(dirpath,backpath);
        printf("A bak up directory name with .bak is created to :");
        printf("%s\n", backpath);
    }
  }
}



		/********************************************************/
		/**************Function to perform choice 4**************/
		/********************************************************/
	}

	else if(choice == 5){
		printf("\nEXIT SEE YOU \n");

		/********************************************************/
		/**************Function to perform choice 5**************/
		/********************************************************/
	}

	else{
		printf("Invalid choice\n");
		exit(1);
	}
	free(input_dir_name);
	free(dirpath);
	return 0;
}
