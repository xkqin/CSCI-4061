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
       printf("PARENT:: Created a child process and now I will exeiute the 'date' command\n");
       dup2( pipeid[1], 1);
       close(  pipeid[1] );
       system ("date");
       close ( 1 );
       wait( &stat );
    }
    else
    {
        /* this is the child process */
        printf("CHILD:: I am the newly created child process\n");
        printf("CHILD:: Now I will execute 'wc' command\n");

        close ( pipeid[1] );
        dup2( pipeid[0], 0);
        close( pipeid[0] );

        { char* newargs[3];
          newargs[0] = "wc";
          newargs[1] = "-wlc";
          newargs[1] = 0;

          if ( execvp ("wc", newargs) )
             printf ("exec returned non-zero value\n");

          printf("Hello! Are you there?\n");
        }
      }
}
