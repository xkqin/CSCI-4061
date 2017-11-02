/*  This progrma illustrate use of time() and strftime() functions */
/*  to obtain current date and time in the desired format           */

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define BUFSIZE  256

void  main (int argc, char * argv[])  {
char  buffer[BUFSIZE];
char* datetime;

int retval;
time_t  clocktime;
struct tm  *timeinfo;
   time (&clocktime);
   timeinfo = localtime( &clocktime );
   strftime(buffer, 64, "%b-%d-%Y-%H-%M-%S", timeinfo);
   printf("Current date and time is: %s\n", buffer);
}
