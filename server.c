#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT    8080
#define MAXLINE 1024
   
// Driver kode
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server - uviklings kenned";
    struct sockaddr_in servaddr, cliaddr; //Vigtigt! håndterer syscalls og functioner til internet address 
       
    // Laver socket file descriptor med params: domain, type, protokol (0 er default)
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }

    // Sætter servaddr og cliaddr i memory til 0
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
       
    // Indsætter server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; //Lytter på alle tilgængelige interfaces
    servaddr.sin_port = htons(PORT); // Sætter porten - htons konverterer til network byte order
       
    // Tilføjer srver addressen til socket
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
        perror("Can't bind address to socket");
        exit(EXIT_FAILURE);
    }
       
    int len, n;
   
    len = sizeof(cliaddr); // len er value/result
   
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';

    // Lav buffer om til MQTT og send til "Display"
    char command[255];
    sprintf(command, "mosquitto_pub -h 192.168.150.73 -t 'topic' -m '%s'", buffer);
    printf(command);
    system(command);

    printf("Client : %s\n", buffer);
    printf("Client message sent to Display");
       
    return 0;
}
