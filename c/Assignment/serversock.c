/*
 * serversock.c
 */

#include "headsock.h"
#define BACKLOG 10

void str_ser(int sockfd);	// function declaration for tx and rx

int main(void) {
    int sockfd, con_fd, ret;
    int sin_size;
    struct sockaddr_in server_addr;	// sockaddr_in is defined in netinet/in.h
    struct sockaddr_in client_addr;	// with the following members:
					// sa_family_t		sin_family
					// in_port_t		sin_port
					// struct in_addr	sin_addr
					// unsigned char	sin_zero[8]

    sockfd = socket(AF_INET, SOCK_STREAM, 0);	// create socket

    if (sockfd == -1){	// -1 is returned on error
        printf("Unable to create socket!");
        exit(1);
    }

    server_addr.sin_family = AF_INET; 		// AF_INET = IPv4 Internet protocol
    server_addr.sin_port = htons(MYTCP_PORT); 	// MYTCP_PORT defined in headsock.h
						// htons = convert IPv4 in host byte order to IPv4 address in network byte order
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(server_addr.sin_zero), 8);

    ret = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)); // binds socket to interface
    if (ret < 0) {
        printf("Unable to bind!");
        exit(1);
    }

    ret = listen(sockfd, BACKLOG);
    if (ret < 0) {
        printf("Unable to listen for connection!");
        exit(1);
    }

    while(1) {
        // Wait for data and accept data
    }
}

// receive packet and response
void str_ser(int sockfd){

}
