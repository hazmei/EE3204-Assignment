/*
 * serversock.c
 */

#include "headsock.h"		    // include headsock header file
#define BACKLOG 10

void str_ser(int sockfd);    // transmitting and receiving function

int main(void) {
  int sockfd, con_fd, ret;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr;
  int sin_size;

  //	char *buf;
  pid_t pid;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);     //create socket
  if (sockfd <0) {
    printf("error in socket!");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(MYTCP_PORT);					// port num
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//inet_addr("172.0.0.1");
  bzero(&(my_addr.sin_zero), 8);
  ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));

  if (ret <0) {
    printf("error in binding");
    exit(1);
  }

  // listen(int sockfd, int backlog);
  // listen for connections on a socket
  // returns -1 on error, 0 on success
  ret = listen(sockfd, BACKLOG);
  if (ret <0) {
    printf("error in listening");
    exit(1);
  }

  while (1) {
    printf("waiting for data\n");
    sin_size = sizeof (struct sockaddr_in);
    // accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // sockfd is a socket that has been create with socket()
    // addr is a pointer to a sockaddr struct filled with addr of peer socket
    // addrlen is a val-result arg. Initially contain size of struct pointed to by addr
    con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);            //accept the packet

    if (con_fd <0) {
      printf("error in accept\n");
      exit(1);
    }

    if ((pid = fork())==0) {
      close(sockfd);
      str_ser(con_fd);                                          //receive packet and response
      close(con_fd);
      exit(0);
    }
    else
      close(con_fd);                                         //parent process
  }
  close(sockfd);
  exit(0);
}

void str_ser(int sockfd) {
  char buf[BUFSIZE];
  FILE *fp;
  char recvs[DATALEN*2];
  struct ack_so ack;
  int end = 0, n = 0;
  long lseek=0;
  int invertDL = 1;   // for checking datalen to send
  int acknum = 0;

  printf("receiving data!\n");

  while(!end) {
    // ssize_t recv(int s, void *buf, size_t len, int flags);
    // return num. bytes received or -1 if error occurred
    // return val. will be 0 when peer has performed an orderly shutdown
    // message will be stored in buf
    // len = size of buf
    if (invertDL) {
      n = recv(sockfd, &recvs, DATALEN, 0);
      // printf("receiving (invertDL = 1): %d\n",n);
      invertDL = 0;
    } else {
      n = recv(sockfd, &recvs, DATALEN*2, 0);
      // printf("receiving (invertDL = 0): %d\n",n);
      invertDL = 1;
    }

    if (n == -1) {
      printf("error when receiving\n");
      exit(1);
    }

    // void *memcpy(void *str1, const void *str2, size_t n)
    // copies n characters from memory area str2 to memory area str1
    memcpy((buf+lseek), recvs, n);
    lseek += n;

    // printf("recevs[n-1]: %x\n\n",recvs[n-1]);

    if (recvs[n-1] == '\0')	{
      end = 1; // set end to true
      n--;
    }

    ack.num = ++acknum;
    ack.len = 0;

    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    if ((n = send(sockfd, &ack, 2, 0))==-1) {
      printf("send error!");								//send the ack
      exit(1);
    }


  }

  if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL)	{
    printf("File doesn't exit\n");
    exit(0);
  }

  fwrite (buf , 1 , lseek , fp);					//write data into file
  fclose(fp);
  printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
