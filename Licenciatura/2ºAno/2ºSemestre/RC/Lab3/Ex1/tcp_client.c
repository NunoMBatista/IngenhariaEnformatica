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

#define BUF_SIZE	1024

void erro(char *msg);

int main(int argc, char *argv[]) {
  char endServer[100]; //Server's ip address
  int fd; //File descriptor
  struct sockaddr_in addr; //Server's address structure containing IP and port
  struct hostent *hostPtr; //Host entry structure containing server's name and IP address

  if (argc != 4) {
    printf("cliente <host> <port> <string>\n");
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

  write(fd, argv[3], 1 + strlen(argv[3])); //Write the string in argv[3] to the server

  char buffer[BUF_SIZE];
  int nread = read(fd, buffer, BUF_SIZE-1); //Read the server's response
  buffer[nread] = '\0'; //Add a null character to the end of the string
  printf("Received from server:\n%s\n", buffer); //Print the server's response

  close(fd); //Close the socket 
  exit(0); //Exit the program
}

void erro(char *msg) {
  printf("Erro: %s\n", msg);
	exit(-1);
}
