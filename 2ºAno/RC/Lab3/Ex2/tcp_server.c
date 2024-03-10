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

#define SERVER_PORT     9000
#define BUF_SIZE	1024
#define IPV4_SIZE 16

void process_client(int fd);
void erro(char *msg);
char* searchName(char *name);

int main() {
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
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size); //Accept a new connection and store the client's address in client_addr
    if (client > 0) { //If the connection was successful
      if (fork() == 0) { //Create a child process
        close(fd); //Close the server socket in the child process
        process_client(client); //Process the client
        exit(0); //Exit the child process
      }
    close(client); //Close the client socket in the parent process
    }
  }
  return 0;
}

void process_client(int client_fd)
{
  char *welcomeMessage = "Bem-vindo ao servidor de nomes do DEI. Indique o nome de domínio\n";
  write(client_fd, welcomeMessage, strlen(welcomeMessage)); //Write the welcome message to the client

	int nread = 0; //Number of bytes read
	char buffer[BUF_SIZE], *nameIP; //Buffer to store the message

  do {
    char response[BUF_SIZE+100] = ""; // +100 to account for the extra text in the response

    nread = read(client_fd, buffer, BUF_SIZE-1); //Read the message from the client
	  buffer[nread] = '\0'; //Add a null terminator to the end of the message for printing
    nameIP = searchName(buffer);
    char *exitStr = "SAIR";

    if(strcmp(buffer, exitStr) == 0){
      sprintf(response, "Até logo!\n");
      write(client_fd, response, strlen(response));
      break;
    }
    if(nameIP != NULL){
      sprintf(response, "O nome de domínio %s tem associado o endereço IP %s\n", buffer, nameIP);
      write(client_fd, response, strlen(response));
    }
    else{
      sprintf(response, "O nome de domínio %s não tem um endereço IP associado\n", buffer);
      write(client_fd, response, strlen(response)); 
    }
    free(nameIP);
    fflush(stdout); //Flush the output buffer
  } while (nread>0); //Repeat until there are no more bytes to read
	close(client_fd); //Close the client socket
}

char* searchName(char *name){
    FILE *file = fopen("domains.txt", "r");
    
    if(file == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    char line[100];
    while(fgets(line, 100, file) != NULL){
        if(strstr(line, name) != NULL){
            strtok(line, " ");
            char *ip = strtok(NULL, " ");
            ip[strlen(ip) - 1] = '\0'; //Remove the '\n' character
            //Since the string returned by strtok is a pointer to the original string in the functions stack, we need to copy it to a new string in the heap
            char* ip_copy = (char*) malloc(strlen(ip) + 1); 
            if(ip_copy == NULL){
                printf("Error allocating memory\n");
                exit(1);
            }
        
            strcpy(ip_copy, ip);
            return ip_copy;
        }
    }
    return NULL;
}

void erro(char *msg){
	printf("Erro: %s\n", msg);
	exit(-1);
}