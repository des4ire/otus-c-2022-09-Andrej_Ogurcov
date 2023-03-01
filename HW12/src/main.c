#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

static const uint8_t port=23; //telnet port
static const char service_port[4]="80"; //http service port
char server_ip[20]=""; //variable for telehack ip
const uint8_t END_MARK=0x2E; //telnet message end mark 
unsigned char buffer[2048]; //receiving buffer
intmax_t RX_byte = 0; //
int error; //error variable for getaddrinfo
char hostname [20] = "telehack.com";

 int main(int argc , char *argv[]) {
    int sock;
    char *font;
    char *msg;
	
      if (argc !=3) {
        printf("Usage: %s font message\n", argv[0]);
        exit(EXIT_FAILURE);
	}
      else {
	font = argv[1];
	msg = argv[2];
        printf("Font: %s\n", font);
	printf("Message: %s\n", msg);     
 	}
    
  struct addrinfo hints_1, *res_1;
    memset(&hints_1, '\0', sizeof(hints_1));
 
    error = getaddrinfo( hostname, service_port, &hints_1, &res_1);
    printf ("Errors: %i\n", error);
   
    if (error ==0){
       inet_ntop(AF_INET, &res_1->ai_addr->sa_data[2], server_ip, sizeof(server_ip));
       printf ("Host IP: %s\n", server_ip);
        }
   struct sockaddr_in server;
    
    sock = socket(AF_INET , SOCK_STREAM , 0); //Create socket
    if (sock == -1) {
        perror("Could not create socket. Error");
        return 1;
    }
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("Connection failed. Error");
        return 1;
    }
    puts("Connected!\n");
    
     while (1) {
        if ((RX_byte = recv(sock, buffer, sizeof(buffer), 0)) < 0) {
            perror("Data error");
            exit(EXIT_FAILURE);
        }
	
       if (buffer[RX_byte-1] == END_MARK) { //stop data receive data if "." is found
              break;
    	  }
       };
     
    memset(buffer, 0, sizeof(buffer));

    size_t command_size = strlen(font) + strlen(msg) + 11;
    char *command = calloc(command_size, sizeof(char));

    snprintf(command, command_size, "figlet /%s %s\r\n", font, msg);

    if (send(sock, command, command_size * sizeof(char), 0) < 0) {
        perror("Send command error");
        exit(EXIT_FAILURE);
    }
    free(command);

   while (1) {  //receive message
        if ((RX_byte = recv(sock, buffer, sizeof(buffer), 0)) < 0) {
            perror("Received data error");
            exit(EXIT_FAILURE);
        }

        printf("%s", buffer);
        if (buffer[RX_byte-1] == END_MARK) {
            memset(buffer, 0, sizeof(buffer));
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    };
    close(sock);
    return 0;
}