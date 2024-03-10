/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surgem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define SERVER_PORT     9000
#define BUF_SIZE	1024

void process_client(int fd, struct sockaddr_in client_addr, int client_id);
void erro(char *msg);

int main() {
  int client_id = 0;
  int fd, client; //File descriptors for server and client sockets
  struct sockaddr_in addr, client_addr; //Server and client address structures
  int client_addr_size; //Size of client address structure

  bzero((void *) &addr, sizeof(addr)); //Clear addr structure
  addr.sin_family = AF_INET; //Set addr family to AF_INET, which is the internet family
  addr.sin_addr.s_addr = htonl(INADDR_ANY); //Set server's ip address to any local address
  addr.sin_port = htons(SERVER_PORT); //Set server's port to SERVER_PORT

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //Create socket and store its file descriptor in fd
	  erro("na funcao socket");

  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0) //Associate the socket with the address and port in addr
	  erro("na funcao bind");
  
  if( listen(fd, 5) < 0) //Set the socket to listen for incoming connections
	  erro("na funcao listen");
  
  client_addr_size = sizeof(client_addr); //Set client_addr_size to the size of client_addr structure
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0); //Clean zombies
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size); //Accept a new connection and store the client's address in client_addr
    if (client > 0) { //If the connection was successful
      client_id++;
      if (fork() == 0) { //Create a child process
        close(fd); //Close the server socket in the child process
        process_client(client, client_addr, client_id); //Process the client
        exit(0); //Exit the child process
      }
    close(client); //Close the client socket in the parent process
    }
  }
  return 0;
}

void process_client(int client_fd, struct sockaddr_in client_addr, int client_id) 
{
	int nread = 0; //Number of bytes read
	char buffer[BUF_SIZE]; //Buffer to store the message
  char message[BUF_SIZE];

  sprintf(message, "Server received connection from (IP:port) %s:%hu; already received %d connections!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_id);

  do {
    nread = read(client_fd, buffer, BUF_SIZE-1); //Read the message from the client
	   buffer[nread] = '\0'; //Add a null character to the end of the message
	    if(nread > 0){
          printf("** New message received **\nClient %d connecting from (IP:port) %s:%hu says %s\n", client_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer); //Print the message
          write(client_fd, message, strlen(message) + 1); //Write the message to the client        
        }
        fflush(stdout); //Flush the output buffer
  } while (nread>0); //Repeat until the message is empty
	close(client_fd); //Close the client socket
}

void erro(char *msg){
	printf("Erro: %s\n", msg); 
	exit(-1);
}
