/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 1024

void erro(char *msg);

int main(int argc, char *argv[]) {
  char endServer[100]; //Server's ip address
  int fd; //File descriptor
  struct sockaddr_in addr; //Server's address structure containing IP and port
  struct hostent *hostPtr; //Host entry structure containing server's name and IP address

  if (argc != 3) {
    printf("cliente <host> <port>\n");
    exit(-1);
  }

  strcpy(endServer, argv[1]); //Copy server's ip address to endServer
  if ((hostPtr = gethostbyname(endServer)) == 0) //Get server's ip address and check if it was successful
    erro("Não consegui obter endereço");

  bzero((void *) &addr, sizeof(addr)); //Clear addr structure
  addr.sin_family = AF_INET; //Set addr family to AF_INET, which is the internet family 
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr; //Set server's ip address
  addr.sin_port = htons((short) atoi(argv[2])); //Set server's port 

  if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1) //Create socket and store its file descriptor in fd
	  erro("socket");

  if (connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0) //Use fd to connect to server whose address is in addr
	  erro("Connect");

  char messageSent[BUF_SIZE]; //Buffer to store the message
  char messageReceived[BUF_SIZE]; //Buffer to store the message received

  int nread = read(fd, messageReceived, BUF_SIZE); //Read the message from the server
  messageReceived[nread] = '\0'; //Add the null character to the end of the message
  printf("%s", messageReceived); 

  char *exitStr = "SAIR";

  while(strcmp(messageSent, exitStr) != 0){
    fgets(messageSent, BUF_SIZE, stdin); //Read the message from the user
    messageSent[strlen(messageSent) - 1] = '\0'; //Remove the '\n' character
    
    write(fd, messageSent, 1 + strlen(messageSent)); //Write the string in argv[3] to the server

    messageReceived[0] = '\0'; //Clear the messageReceived buffer  
    nread = read(fd, messageReceived, BUF_SIZE); //Read the message from the server
    messageReceived[nread] = '\0'; //Add the null character to the end of the message

    printf("%s", messageReceived); //Print the message
  }
  
  close(fd); //Close the socket
  exit(0); //Exit the program
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
