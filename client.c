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
  
//  Driver kode
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client - revisor ulle";
    struct sockaddr_in     servaddr; //Vigtigt! håndterer syscalls og functioner til internet address 
   
    // Laver socket file descriptor med params: domain, type, protokol (0 er default)
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Indsætter server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // Lytter på alle tilgængelige interfaces
    servaddr.sin_port = htons(PORT); // Sætter porten - htons konverterer til network byte order
       
    int n, len;
       
    sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("Hello message sent.\n");

    // Modtager fra server       
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
   
    close(sockfd);
    return 0;
}
