
/***********************************************************************************************

 CSci 4061 Fall 2017
 Assignment# 5:

 Student name: <Weihang Xiao>
 Student ID:   <5423302>

 Student name: <Xukun Qin>
 Student ID:   <5188260>

 X500 id: <xiao0155>, <qinxx215>

 Operating system on which you tested your code: Linux, Unix, Solaris, MacOS
CSELABS machine: <csel-kh4250-10.cselabs.umn.edu>

 GROUP INSTRUCTION:  Please make only ONLY one  submission when working in a group.
***********************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#define SERVER_PORT "6789" /* CHANGE THIS TO THE PORT OF YOUR SERVER */
#define BUFFER_SIZE 1024


char config[BUFFER_SIZE], content[10][256];

pthread_mutex_t  clientMutex;


typedef struct item{
	int sid;
	char IP[BUFFER_SIZE];
} item;

//parsing the file config
void parserFile(FILE ** quoteFiles, int *fileLen)
{

	FILE *file;
	int count = 0;
	size_t size = BUFFER_SIZE;
	char line[256], *temp, *save;

	if((file = fopen(config, "r")) == NULL)	// open config file
	{
		perror("open: ");
		exit(0);
	}

	while(fgets(line, 256, file) != NULL)					// parser the names of each file in config
	{
		strcpy(content[count], strtok(line, " :\n"));
		temp = strtok(NULL, " :\n");
		if((quoteFiles[count] = fopen(temp, "r")) == NULL) continue;	// open quote file
		count++;
	}
	*fileLen = count;
	fclose(file);
}




// clientThread
// client handler thread function
void * clientThread(void * input)
{
	int clientSocket = ((item *)input)->sid;
	char request[BUFFER_SIZE],* response, IP[BUFFER_SIZE];
	strcpy(IP, ((item *)input)->IP);			// copy  IP
	pthread_mutex_unlock(&clientMutex);

	int fileLen, i;
	FILE ** quoteFiles = malloc(10 * (sizeof(FILE*)));
	response = malloc(sizeof(char)*BUFFER_SIZE);

	parserFile(quoteFiles, &fileLen);
	while(1)
	{
		// get a client's request
		if (recv(clientSocket, request, BUFFER_SIZE,0) < 0){
			printf("Error reading from stream socket");
			perror("Aborting");
			close(clientSocket);
			exit(1);
		}
		if(strcmp(request, "BYE\n") == 0)										// exit client thread
		{
			break;
		}
		else if(strcmp(request, "GET: LIST\n") == 0){
		 // print list of quotes
		 	int i;
		 	strcpy(response, "");
		 	for(i = 0; i < fileLen; i++)
		 	{
		 		strcat(response, content[i]);
		 		strcat(response, "\n");
		 	}
	 }
		else
		{
			// get a Quote
			// Trys to find matching file, gets random one for "ANY"
			while(1){
			int count;
			char quote[BUFFER_SIZE], temp[BUFFER_SIZE];
			printf("%s\n", (request+16));
			for(count = 0; count < fileLen; count++)				//determine the file id
			{
				//printf("%s\n", content[count]);
				if(strncmp(content[count], (request+16), strlen(content[count])) == 0)
				{
					break;
				}
			}
			if(strcmp((request+16), "ANY\n") == 0)					// Check if client requested for ANY
			{
				count = (((double)rand())/RAND_MAX * fileLen);		// Generate psuedorandom number
			}
			if(count == fileLen)										// No file matches name, return appropriate response
			{
				sprintf(response, "We don't have quotes for %s\n", request);
				break;
			}
			if(fgets(quote, BUFFER_SIZE, quoteFiles[count]) == 0){		// Get quote
				rewind(quoteFiles[count]);								// go back to the beginning if at end
				fgets(quote, BUFFER_SIZE, quoteFiles[count]);
			}
			strcpy(response, quote);									// Save the quote
			fgets(quote, BUFFER_SIZE, quoteFiles[count]);				// Get name of quote's speaker
			strcat(response, quote);									// put at end of retval
			break;
		}
		}
		// Send back a response
		if (send(clientSocket, response, BUFFER_SIZE,0) < 0){
			printf("Error writing on stream socket");
			perror("Aborting");
			close(clientSocket);
			exit(1);
		}
	}
	free(response);						// Free up malloced files
	free(quoteFiles);
	pthread_exit(0);					// Client thread is done
}

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}



// closeServer
void closeServer(int sig)
{
	exit(0);
}


int main(int argc, char** argv)
{
    int sockfd, new_fd, i;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    pthread_t tid;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    item cd;

	if(argc != 2)		// make sure arguments are in order
	{
		printf("Usage: quote_server config\n");
		exit(0);
	}
	strcpy(config, argv[1]);	// copy name of config file


	pthread_mutex_init(&clientMutex, NULL);
	signal(SIGINT, closeServer);		// Set up server closing protcol

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 		// use my IP

    if ((rv = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }


    while(1) {
        sin_size = sizeof their_addr;
        pthread_mutex_lock(&clientMutex);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);		// accept a client
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        cd.sid = new_fd;
        inet_ntop(AF_INET, (struct sockaddr *)&their_addr, cd.IP, sizeof cd.IP);
        pthread_create(&tid, NULL, clientThread, ((void *) &cd));	// create a new thread
    }
    return 0;
}
