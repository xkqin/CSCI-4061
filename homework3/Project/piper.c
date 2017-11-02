/***********************************************************************************************

 CSci 4061 Fall 2017
 Assignment# 3: Piper program for executing pipe commands

 Student name: <full name of first student>
 Student ID:   <first student's ID>

 Student name: <full name of second student>
 Student ID:   <Second student's ID>

 X500 id: <id1>, <id2 (optional)>

 Operating system on which you tested your code: Linux, Unix, Solaris, MacOS
 CSELABS machine: <machine you tested on eg: xyz.cselabs.umn.edu>

 GROUP INSTRUCTION:  Please make only ONLY one  submission when working in a group.
***********************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#define DEBUG

#define MAX_INPUT_LINE_LENGTH 2048 // Maximum length of the input pipeline command
                                   // such as "ls -l | sort -d +4 | cat "
#define MAX_CMDS_NUM   8           // maximum number of commands in a pipe list
                                   // In the above pipeline we have 3 commands
#define MAX_CMD_LENGTH 4096         // A command has no more than 4098  characters

FILE *logfp;

int num_cmds = 0;
char *cmds[MAX_CMDS_NUM];
int cmd_pids[MAX_CMDS_NUM];
int cmd_status[MAX_CMDS_NUM];

static sigjmp_buf jmpbuf;
int ifdone = 0;
static int oldpid;
int fildes[2],pipe2[2];
/*******************************************************************************/
/*   The function parse_command_line will take a string such as
     ls -l | sort -d +4 | cat | wc
     given in the char array commandLine, and it will separate out each pipe
     command and store pointer to the command strings in array "cmds"
     For example:
     cmds[0]  will pooint to string "ls -l"
     cmds[1] will point to string "sort -d +4"
     cmds[2] will point to string "cat"
     cmds[3] will point to string "wc"

     This function will write to the LOGFILE above information.
*/
/*******************************************************************************/

int parse_command_line (char commandLine[MAX_INPUT_LINE_LENGTH], char* cmds[MAX_CMDS_NUM]){

    int i = 0;
    char* com;
   com = strtok(commandLine, "|");
    while (com != NULL){

      if(i == MAX_CMDS_NUM)
      {
          printf("No more than 8 arguments!\n");
          fprintf(logfp, "No more than 8 arguments!\n");
          siglongjmp( jmpbuf, 1 );
      }
      cmds[i] = com;
       i++;
       com = strtok(NULL, "|");
     }
return i;
}

/*******************************************************************************/
/*  parse_command takes command such as
    sort -d +4
    It parses a string such as above and puts command program name "sort" in
    argument array "cmd" and puts pointers to ll argument string to argvector
    It will return  argvector as follows
    command will be "sort"
    argvector[0] will be "sort"
    argvector[1] will be "-d"
    argvector[2] will be "+4"
/
/*******************************************************************************/

void parse_command(char input[MAX_CMD_LENGTH],
                   char command[MAX_CMD_LENGTH],
                   char *argvector[MAX_CMD_LENGTH]){
  int i = 0;
  argvector[i] = strtok(input, " ");
  i++;
  while ((argvector[i] = strtok(NULL, " ")) != NULL){
    i++;
  }
  strcpy(command, argvector[0]);
}


/*******************************************************************************/
/*  The function print_info will print to the LOGFILE information about all    */
/*  processes  currently executing in the pipeline                             */
/*  This printing should be enabled/disabled with a DEBUG flag                 */
/*******************************************************************************/

void print_info(char* cmds[MAX_CMDS_NUM],
		int cmd_pids[MAX_CMDS_NUM],
		int cmd_stat[MAX_CMDS_NUM],
		int num_cmds) {

      #ifdef DEBUG
      int i;
      if(ifdone)
  		fprintf(logfp, "PID        COMMAND            EXIT\n");
  	else
  		fprintf(logfp, "PID        COMMAND\n");

      for(i = 0; i < num_cmds; i++)
      {
  		if(ifdone)
  		{
  			fprintf(logfp, "%d       %s                %d\n", cmd_pids[i], cmds[i], cmd_stat[i]);
  		}
  		else
  		{
  			fprintf(logfp, "%d       %s\n", cmd_pids[i], cmds[i]);
  		}
      }
      #endif
    // for(int i =0; i < num_cmds; i++){
    // fprintf(logfp, "waiting...Process id: %d finished \n ",cmd_pids[i]);
    // fprintf(logfp, "Process id %d finished with exit status %d\n ",cmd_pids[i],cmd_stat[i]);
    // }

}



/*******************************************************************************/
/*     The create_command_process  function will create a child process        */
/*     for the i'th command                                                    */
/*     The list of all pipe commands in the array "cmds"                       */
/*     the argument cmd_pids contains PID of all preceding command             */
/*     processes in the pipleine.  This function will add at the               */
/*     i'th index the PID of the new child process.                            */
/*******************************************************************************/
/*     Following ADDED on  10/27/2017                                          */
/*     This function  will create pipe object, execute fork call, and give   */
/*     appropriate directives to child process for pipe set up and    */
/*     command execution using exec call                                       */


void create_command_process (char cmds[MAX_CMD_LENGTH],   // Command line to be processed
                     int cmd_pids[MAX_CMDS_NUM],          // PIDs of preceding pipeline processes
                                                          // Insert PID of new command processs
		             int i)                               // commmand line number being processed
{
  int statid,  j=0;
  int valexit,  valsignal, valstop, valcont;

  char just_cmd[MAX_CMD_LENGTH];
  char *argvec[MAX_CMD_LENGTH];

  parse_command(cmds, just_cmd, argvec);  // put command in just_cmd  and all arguments in argvec

  if(cmd_pids[i]=fork()){ /*  Parent process */

  /* ..Close the unwanted pipe descriptors.. */
  close(fildes[0]);
  close(fildes[1]);
  }
  else { /* Newly created child process */

  if(fildes[0] != -1){
    /* .. indicates that standard input comes from a file .. */
    dup2(fildes[0],0);
      }
  if(pipe2[1] != -1){
    /* ..indicates that standard output redirect to a pipe.. */
    dup2(pipe2[1],STDOUT_FILENO);
  }

  close(fildes[0]);
  close(fildes[1]);
  close(pipe2[0]);
  close(pipe2[1]);

  if(execvp(just_cmd,argvec)){
          exit(1);
      }
  }

  fildes[0] = pipe2[0];
  fildes[1] = pipe2[1];

  pipe2[0]=-1;
  pipe2[1]=-1;
}


/********************************************************************************/
/*   The function waitPipelineTermination waits for all of the pipeline         */
/*   processes to terminate.                                                    */
/********************************************************************************/

void waitPipelineTermination () {
  for (int i = 0; i < num_cmds; i++){
    fprintf(logfp, "waiting...Process id: %d finished \n ",cmd_pids[i]);
    if(waitpid(cmd_pids[i], NULL, 0) == cmd_pids[i])
    {
        fprintf(logfp, "Process id %d finished with exit status %d\n ",cmd_pids[i],cmd_status[i]);
    }

  }
}

/********************************************************************************/
/*  This is the signal handler function. It should be called on CNTRL-C signal  */
/*  if any pipeline of processes currently exists.  It will kill all processes  */
/*  in the pipeline, and the piper program will go back to the beginning of the */
/*  control loop, asking for the next pipe command input.                       */
/********************************************************************************/

void killPipeline( int signum ) {
  for (int i = 0; i < num_cmds; i++){
    kill(cmd_pids[i],SIGKILL);
  }
  printf("\nprocesses killed go back to the beginning\n");
  siglongjmp( jmpbuf, 1 );

}

/********************************************************************************/

int main(int ac, char *av[]){

  int i,  pipcount;
  //check usage
  if (ac > 1){
    printf("\nIncorrect use of parameters\n");
    printf("USAGE: %s \n", av[0]);
    exit(1);
  }

  /* Set up signal handler for CNTRL-C to kill only the pipeline processes  */

  logfp =  fopen("LOGFILE", "w");


  while (1) {
     //signal(SIGINT, SIG_DFL );
     pipcount = 0;
     fildes[0]=-1;
     fildes[1]=-1;
     pipe2[0] = -1;
     pipe2[1] = -1;
     /*  Get input command file anme form the user */
     char pipeCommand[MAX_INPUT_LINE_LENGTH];
       char* terminator = "quit";
     fflush(stdout);
     struct sigaction act;
       act.sa_handler = killPipeline;
       sigemptyset( &act.sa_mask);
       act.sa_flags = 0;

       if (sigaction(SIGINT, &act, NULL) == -1) {
            perror("Error in settting handler for SIGINT");
       }
       sigsetjmp(jmpbuf, 1);

     printf("Give a list of pipe commands: ");
     gets(pipeCommand);

     printf("You entered : list of pipe commands  %s\n", pipeCommand);
     if ( strcmp(pipeCommand, terminator) == 0  ) {
        fflush(logfp);
        fclose(logfp);
        printf("Goodbye!\n");
        exit(0);
     }

    num_cmds = parse_command_line( pipeCommand, cmds);


    /*  num_cmds indicates the number of commands in the pipeline        */

    /* The following code will create a pipeline of processes, one for   */
    /* each command in the given pipe                                    */
    /* For example: for command "ls -l | grep ^d | wc -l "  it will      */
    /* create 3 processes; one to execute "ls -l", second for "grep ^d"  */
    /* and the third for executing "wc -l"                               */
    ifdone = 0;
    for(i=0;i<num_cmds;i++){
         /*  CREATE A NEW PROCCES EXECUTTE THE i'TH COMMAND    */
         /*  YOU WILL NEED TO CREATE A PIPE, AND CONNECT THIS NEW  */
         /*  PROCESS'S stdin AND stdout  TO APPROPRIATE PIPES    */
          fprintf(logfp, "Command %d info: %s\n", i, cmds[i]);
          if(pipcount < num_cmds-1) { //we do not need pipe for the last command. So, num_cmds-1
       pipe(pipe2);
             pipcount++;
          }
         create_command_process (cmds[i], cmd_pids, i);
    }
    fprintf(logfp, "Number of commands from the input: %d\n\n", num_cmds);
    print_info(cmds, cmd_pids, cmd_status, num_cmds);

    waitPipelineTermination();

    ifdone = 1;
    print_info(cmds, cmd_pids, cmd_status, num_cmds);

  }
} //end main

/*************************************************/
