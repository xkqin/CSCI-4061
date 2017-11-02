#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


int main (int argc, char* argv[]) {
long maxpath;
mode_t  perms =  0740;

  if ( argc != 2) {
     printf( "Usage: %s Directory-Path-Name\n ", argv[0] );
     exit(1);
  }

  if ( (mkdir(argv[1], perms)) == 0 ) {
     printf( "New directory created successfully.\n");
  }
  else  {
     perror("Error in directory creation");
  }
}
