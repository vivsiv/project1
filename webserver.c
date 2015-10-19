/* 
	A simple web server that parses HTTP requests and
	dumps the header and returns text and image files in its directory
	by Colin Terndup and Vivek Sivakumar
*/

#include <stdio.h>
#include <sys/types.h> // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h> // for the waitpid() system call
#include <signal.h>	 //signal name macros, and the kill() prototype */
#include <unistd.h>	
#include <time.h> // get current time for server response
#include <sys/stat.h>
#include <fcntl.h>	

void error(char *msg){
	perror(msg);
	exit(1);
}


void parseHttpRequest(char *request, char* filename){
	char* token;
	token = strtok(request, " ");
	token = strtok(NULL, " ");
	strncpy(filename, token + 1, strlen(token) - 1);
	printf("File Requested: %s\n", filename);
}

void getContentType(char *filename, char* content_type){
	char* filetype = strtok(filename, ".");
	filetype = strtok(NULL, ".");
	printf("filetype %s\n", filetype);
	strncpy(content_type, "Content-Type: ", strlen("Content-Type: "));
	
	if (!strcmp(filetype,"jpeg") || !strcmp(filetype,"gif")){
		printf("Type = Image!\n");
		strcat(content_type, "image/");
		if (!strcmp(filetype,"jpeg")) strcat(content_type,"jpeg");
		else strcat(content_type,"gif");
	}
	else {
		printf("Type = Html!\n");
		strcat(content_type, "text/html");
	}
	strcat(content_type,"\n\n");
}

void writeResponse(int sock, char *filename){
	write(sock, "HTTP/1.1 ", 9);
	printf("HTTP/1.1 ");
	int filed = open(filename, O_RDONLY);
	char* statusCode;
	if (filed < 0){
		filename = "404.html";
		filed = open(filename, O_RDONLY);
		write(sock, "404 NOT FOUND\n", 14);
		printf("404 NOT FOUND\n");
	}
	else {
		write(sock, "200 OK\n", 7);
		printf("200 OK\n");
	}

	write(sock, "Connection: close\n", 18);
	printf("Connection: close\n");

	write(sock, "Date: ", 6);
	time_t current_time = time(NULL);
	char *datetime = ctime(&current_time);
	write(sock, datetime, strlen(datetime));
	//write(sock, "\n", 1);
	printf("Date: %s", datetime);

	write(sock, "Last-Modified: ", 15);
	// get file info
	struct stat file_info;
	stat(filename, &file_info);
	char *last_modified = ctime(&file_info.st_mtime);
	write(sock, last_modified, strlen(last_modified));
	//write(sock, "\n", 1);
	printf("Last-Modified: %s", last_modified);

	
	write(sock, "Content-Length: ", 16);
	char filesize[8];
	sprintf(filesize, "%lld", file_info.st_size);
	write(sock, filesize, strlen(filesize));
	write(sock, "\n", 1);
	printf("Content-Length: %s\n", filesize);

	char content_type[50];
	getContentType(filename, content_type);
	write(sock, content_type, strlen(content_type));
	printf("%s", content_type);

	char file_contents[256];
	int bread;
	bread = read(filed, file_contents, 255);
	printf("bread %d\n\n", bread);
	while (bread > 0){
		write(sock, file_contents, strlen(file_contents));
		printf("Data: %s\n", file_contents);
		bzero(file_contents, sizeof(file_contents));
		bread = read(filed, file_contents, 255);
		printf("bread %d\n\n", bread);
	}	
	write(sock, "\n", 1);
}

//Process each child socket
void process_connection(int sock){
	int n;
	char buffer[256];
	char* fileRequested;

	bzero(buffer,256);

	//Read from client
	n = read(sock, buffer, 255);

	if (n < 0){
		error("ERROR reading from socket");
	}
	//Output header
	printf("%s\n\n", buffer);

	char* filename = malloc(20);
	parseHttpRequest(buffer, filename);
	//Write data back to client (Part B send a file back)
	//Parse Request (Part B)
	writeResponse(sock, filename);
	//n = write(sock, "I got your message", 18);
	free(filename);
}

int main(int argc, char *argv[]){
	int sockfd, newsockfd, portno, process_id;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	if (argc < 2){
		error("ERROR, no port provided");
	}

	// Make a parent server socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}

	//Get port from user input
	portno = atoi(argv[1]);

	//Set server address and port
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//Bind socket to address and port
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR binding socket");
	}

	listen(sockfd,1);

	//run server forever 
	while(1){
		//Accept incoming connections
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
		//Fork off a new process
		process_id = fork();
		if (process_id < 0){
			error("ERROR on fork");
		}

		if (process_id == 0){
			close(sockfd);
			process_connection(newsockfd);
			close(newsockfd);
			exit(0);
		}

		else {
			close(newsockfd);
		}
	}
	return 0;
}