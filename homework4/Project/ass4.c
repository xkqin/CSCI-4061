/***********************************************************************************************

 CSci 4061 Fall 2017
 Assignment# 4:

 Student name: <Weihang Xiao>
 Student ID:   <5423302>

 Student name: <Xukun Qin>
 Student ID:   <5188260>

 X500 id: <xiao0155>, <qinxx215>

 Operating system on which you tested your code: Linux, Unix, Solaris, MacOS
CSELABS machine: <csel-kh4250-10.cselabs.umn.edu>

 GROUP INSTRUCTION:  Please make only ONLY one  submission when working in a group.
***********************************************************************************************/
#include <pthread.h>
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
#define NUM_THREADS 100

typedef struct item
{
	int size;
	char * path;
} item;

pthread_mutex_t dirL, sizemutex;
struct stat statbuf;
int first;




void * Result(void * arg){
 //(item *) arg;

 DIR * dir;
 struct dirent *direntry;
 char *temPath, *dirname;
 int i = 0, threadID = 0;
 item *parentItem, currentItem;
 //the max number of thread is 100
 pthread_t tid[100];

 parentItem = (item	 *) arg;
 dirname = (char *) malloc(NAMESIZE * sizeof(char));
 strcpy(dirname, parentItem->path);
 //extract the path of Item and copy it to dirname
 if(first) {
	 first = 0;
 } else {
	 pthread_mutex_unlock(&dirL);
 }


 currentItem.size = 0;
 currentItem.path = (char *) malloc(NAMESIZE * sizeof(char));
 temPath = (char *) malloc(NAMESIZE * sizeof(char));


 if ( (dir = opendir(dirname)) == NULL)
 {
		 printf("ERROR: Unable to open directory %s\n",dirname);
		 exit(4);
 }

 while( (direntry = readdir(dir)) != NULL )
 {

			if (strcmp (direntry->d_name, ".") == 0)
 			continue;

 			if (strcmp (direntry->d_name, "..") == 0)
 			continue;
			// skip the "." and ".." direntry
			strcpy(temPath, dirname);
			strcat(temPath, "/");
			strcat(temPath, direntry->d_name);

 			if (lstat(temPath, &statbuf) ==-1)
       {
         perror("lstat");
         exit(EXIT_FAILURE);
       }


			 if (S_ISDIR(statbuf.st_mode)) {
				 // if it is a directory goes into a critecal section
				pthread_mutex_lock(&dirL);
				strcpy(currentItem.path, temPath);
				pthread_create(&tid[threadID], NULL, Result, (void *)&currentItem);
				threadID++;
			}
			else if (S_ISREG(statbuf.st_mode)) {
				currentItem.size += statbuf.st_size;
			}
		}

			for (int i = 0; i < threadID; i++) {
				pthread_join(tid[i], NULL);
			}
			//wait for all the thread to finish
			printf("DEBUG %s   %d\n",dirname,currentItem.size);

	 //  critecal section
			pthread_mutex_lock(&sizemutex);
			parentItem->size += currentItem.size;
			pthread_mutex_unlock(&sizemutex);
			//free the variable
			free(dirname);
			free(currentItem.path);
			free(temPath);
			//closedir(dir);
			pthread_exit(NULL);


}

int main(int argc, char *argv[])
{
	char *input_dir_name, *dirpath, *fdirpath;
	pthread_t tid;
	struct item total;
	//initial the mutex
	pthread_mutex_init(&dirL, NULL);
	pthread_mutex_init(&sizemutex, NULL);
	input_dir_name = (char *) malloc(NAMESIZE * sizeof(char));
  fdirpath = (char *) malloc(NAMESIZE * sizeof(char));
	printf("Enter a directory tree in the current directory: ");
	scanf("%s", fdirpath);
	if (realpath(fdirpath, input_dir_name) == NULL)
	{
 		printf("The directory %s does not exit \n",input_dir_name);
		//We did error checking to make sure that the user specified directory exists.
		exit(4);
	}

	first = 1; //if the fistt thread, there is no waiting needed. So disable signaling
	total.size = 0;
	total.path = (char *) malloc(NAMESIZE * sizeof(char));
	strcpy(total.path, input_dir_name);
	pthread_create(&tid, NULL, Result, ((void *) &total));
	pthread_join(tid, NULL);
	//wait for the thread to finish
	printf("\nTotal Size is : %d\n\n", total.size);
	free(input_dir_name);
	return 0;
}
