#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void main( int argc, char*  argv[] )
{
    pid_t childpid ;
    int pipeid[2];
    int  stat;

    stat = pipe( pipeid );

    if  (stat ) {
       exit(1) ;
     }

    if ( childpid= fork())
    {
       /* this is the parent process */
       printf("Created a child process\n");
       dup2( pipeid[1], 1);
       close(  pipeid[1] );
        execlp("ls",  "ls", "-l", NULL);
    }
    else
    {
        /* this is the child process */
        printf("I am the newly created child process\n");
        printf("Now I will execute a different program\n");

        close ( pipeid[1] );
        dup2( pipeid[0], 0);
        close( pipeid[0] );

        { char* newargs[3];
          newargs[0] = "wc";
          newargs[1] = "-cl";
          newargs[2] = 0;

          if ( execlp ("sort", "sort",  NULL) )
             printf ("exec returned non-zero value\n");

          printf("Hello! Are you there?\n");
        }
      }
}
