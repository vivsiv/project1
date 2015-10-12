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

int main(int argc, char *argv[]){
	if (argc < 2){
		//Usage Error
	}

	// Make a parent server socket
	// socket()

	//Get port from user input
	//int portno = argv[1];

	//Set server address and port

	//Bind socket to address and port
	//bind()

	//Listen for connections on server socket
	//listen()

	//run server forever 
	while(0){
		//Accept incoming connections
		//accept()

		//Fork off a new process
		//fork()

		//Do things with the connection
		//process_connection()
	}
	return 0;
}

//Process each child socket
void process_connection(){
	//Read from client
	//read()

	//Output header
	//printf()

	//Parse Request (Part B)

	//Write data back to client (Part B send a file back)
	//write()
}