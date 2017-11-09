/*
 * serversock.c
 */

#include "headsock.h"                               // include headsock header file
#define BACKLOG 10

void str_ser(int sockfd);                           // transmitting and receiving function

int main(void) {
    int sin_size;
    int sockfd, con_fd, ret;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;

    pid_t pid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);       //create socket
    if (sockfd <0) {
        printf("Error in socket!\n");
        exit(1);
    }

    my_addr.sin_family = AF_INET;                   // AF_INET - IPv4
    my_addr.sin_port = htons(MYTCP_PORT);           // MYTCP_PORT - port to listen for connection
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // INADDR_ANY - accepts connections to all IPs of the machine
    bzero(&(my_addr.sin_zero), 8);
    ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));    // get's the return from binding command
    // bind() returns 0 on success else, -1 on error

    if (ret < 0) {
        printf("Error in binding\n");
        exit(1);
    }

    // listen(int sockfd, int backlog);
    // listen for connections on a socket
    // returns -1 on error, 0 on success
    ret = listen(sockfd, BACKLOG);
    
    if (ret < 0) {
        printf("Error in listening\n");
        exit(1);
    }

    while (1) {
        printf("Waiting for data\n");
        sin_size = sizeof (struct sockaddr_in);

        // accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        // sockfd is a socket that has been create with socket()
        // addr is a pointer to a sockaddr struct filled with addr of peer socket
        // addrlen is a val-result arg. Initially contain size of struct pointed to by addr
        con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);            //accept the packet

        if (con_fd < 0) {
            printf("Error in accept\n");
            exit(1);
        }

        if ((pid = fork()) == 0) {
            close(sockfd);
            str_ser(con_fd);            //receive packet and response
            close(con_fd);
            exit(0);
        } else
        close(con_fd);                  //parent process
    }

    close(sockfd);
    exit(0);
}

void str_ser(int sockfd) {
    char buf[BUFSIZE];
    FILE *fp;
    char recvs[DATALEN*2];              // uses DATALEN*2 as that is the largest expected packet size (2 DU)
    struct ack_so ack;
    int end = 0, n = 0;
    long lseek = 0;
    int invertDL = 1;                   // for checking which datalen to use
    int acknum = 0;

    printf("Receiving data!\n");

    while(!end) {
        // ssize_t recv(int s, void *buf, size_t len, int flags);
        // return num. bytes received or -1 if error occurred
        // return val. will be 0 when peer has performed an orderly shutdown
        // message will be stored in buf
        // len = size of buf
        if (invertDL) {
            n = recv(sockfd, &recvs, DATALEN, 0);
            invertDL = 0;
        } else {
            n = recv(sockfd, &recvs, DATALEN*2, 0);
            invertDL = 1;
        }

        // printf("received %d\n",n);

        if (n == -1) {
            printf("Error when receiving\n");
            exit(1);
        }

        //printf("recevs[n-1]: %x\n\n",recvs[n-1]);

        if (recvs[n-1] == '\0') {
            end = 1;                    // set end to true
            n--;
        }

        // void *memcpy(void *str1, const void *str2, size_t n)
        // copies n characters from memory area str2 to memory area str1
        memcpy((buf+lseek), recvs, n);
        lseek += n;

        ack.num = ++acknum;
        ack.len = 0;

        // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        if ((n = send(sockfd, &ack, 2, 0))==-1) {
            printf("Send error!\n");    // ack packet send failed
            exit(1);
        }
    }

    if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL)   {
        printf("File doesn't exist\n");
        exit(0);
    }

    fwrite (buf , 1 , lseek , fp);      // write data into file
    fclose(fp);
    printf("A file has been successfully received!\nThe total data received is %d bytes.\n", (int)lseek);
}
