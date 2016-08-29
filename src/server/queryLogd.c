
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include "queryLogd.h"

void request_handler (int sock);

int main( int argc, char *argv[] ) {
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, pid;
   
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0) {
	perror("ERROR opening socket");
	exit(1);
    }
   
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
   
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	perror("ERROR on binding");
	exit(1);
    }
   
    /* Now start listening for the clients, here
     * process will go in sleep mode and will wait
     * for the incoming connection
     */
   
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
   
    while (1) {
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	
	if (newsockfd < 0) {
	    perror("ERROR on accept");
	    exit(1);
	}
      
	/* Create child process */
	pid = fork();
	
	if (pid < 0) {
	    perror("ERROR on fork");
	    exit(1);
	}
      
	if (pid == 0) {
	    /* This is the client process */
	    close(sockfd);
	    request_handler(newsockfd);
	    exit(0);
	}
	else {
	    close(newsockfd);
	}
	
    } /* end of while */
}

void request_handler (int sock) {
    int n;
    char buffer[256];
    char grepCmd[512];
    char *logFilePath="/var/log/system.log"; //TODO: Hardcoded for MacOSX
    bzero(buffer,256);
    n = read(sock,buffer,255);
   
    if (n < 0) {
	perror("ERROR reading from socket");
	exit(1);
    }
   
    printf("Here is the pattern: %s\n",buffer);

    if(strcmp(buffer, "")!=0){
	char *pos;
	if ((pos=strchr(buffer, '\n')) != NULL){
	    *pos = '\0';
	}

	sprintf(grepCmd,"grep \"%s\" %s", buffer, logFilePath);
	FILE *cmd;
	char result[1024];

	cmd = popen(grepCmd, "r");
	if (cmd == NULL) {
	    perror("popen");
	    exit(EXIT_FAILURE);
	}
	while (fgets(result, sizeof(result), cmd)) {
	    printf("%s", result);
	}
	pclose(cmd);
    }
    
    n = write(sock,"I got your message",18);
   
    if (n < 0) {
	perror("ERROR writing to socket");
	exit(1);
    }
    
}
