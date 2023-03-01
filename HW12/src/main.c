#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

static const uint8_t port=23; //telnet port
static const char server_ip[20]="64.13.139.230"; //telehack ip
const uint8_t END_MARK=0x2E; //telnet message end mark 
unsigned char buffer[2048]; //receiving buffer
intmax_t RX_byte = 0;

int main(int argc , char *argv[]) {
    int sock;
    char *font;
    char *msg;
    struct sockaddr_in server;
  
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

      if (buffer[RX_byte - 1] == END_MARK) { //stop data receive data if "." is found
            break;
        }
    };
    memset(buffer, 0, 2048);

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
        if (buffer[RX_byte - 1] == END_MARK) {
            memset(buffer, 0, 2048);
            break;
        }
        memset(buffer, 0, 2048);
    };

    close(sock);
    return 0;
}