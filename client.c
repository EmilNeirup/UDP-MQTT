#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     8080
#define MAXLINE 1024
  
int setText(unsigned int file, char *text) {
	i2c_smbus_write_byte_data(file, 0x00, 0x28);
	i2c_smbus_write_byte_data(file, 0x00, 0x0D);
	i2c_smbus_write_byte_data(file, 0x00, 0x01);
	i2c_smbus_write_byte_data(file, 0x00, 0x06);
	i2c_smbus_write_byte_data(file, 0x00, 0x02);

	i2c_smbus_write_byte_data(file, 0x00, 0x80);
	for(int i = 0; ; i++) {
		if(text[i] == 0x00)
			break;
		if (i == 15)
			i2c_smbus_write_byte_data(file, 0x00, 0xc0);
		i2c_smbus_write_byte_data(file, 0x40, text[i]);
	}
}

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client - revisor ulle";
    struct sockaddr_in     servaddr;
   
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;
       
    int n, len;
       
    sendto(sockfd, (const char *)hello, strlen(hello),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    printf("Hello message sent.\n");
           
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
   
    close(sockfd);
    return 0;
}
