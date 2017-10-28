/*
 * clientsock.c: sender sends 1 DU, wait for ack then send 2 DU's , waits for ACK and repeat
 *
 * Measure msg transfer time and throughput for various sizes of data units
 * and compare it with stop-and-wait protocol where batch size is fixed to 1.
 *
 * Choose appropriate values for data unit size and measure performance
 * Repeat experiment several times and plot the average values in report with
 * brief description of results, assumptions made, etc.
*/

#include "headsock.h"		// include headsock header file

// function declaration
float str_cli(FILE *fp, int sockfd, long *len);             //transmission function
void tv_sub(struct  timeval *out, struct timeval *in);	    //calcu the time interval between out and in

int main(int argc, char **argv) {
  int sockfd, ret;
	float ti, rt;
	long len;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc != 2) {			// check if argument is not 2
		printf("parameters not match");
	}

	// gethostbyname() is absolete :: https://linux.die.net/man/3/gethostbyname
	// returns struct of type hostent for given hostname. name is either by hostname or IPv4 or IPv6
	sh = gethostbyname(argv[1]);	   //get host's information
	if (sh == NULL) {
		printf("error when gethostby name");
		exit(0);
	}

	/*
			struct hostent {
			    char  *h_name;            // official name of host
			    char **h_aliases;         // alias list
			    int    h_addrtype;        // host address type
			    int    h_length;          // length of address
			    char **h_addr_list;       // list of addresses
			}
			#define h_addr h_addr_list[0] // for backward compatibility
	*/
	printf("canonical name: %s\n", sh->h_name);					//print the remote host's information
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);

	switch(sh->h_addrtype) {
		case AF_INET:			// AF_INET is for IPv4. IPv6 is AF_INET6
			printf("AF_INET\n");
      break;
		default:
			printf("unknown addrtype\n");
      break;
	}

	addrs = (struct in_addr **)sh->h_addr_list;		//get the server(receiver)'s ip address
	// socket(int domain,, int type, int protocol)
	// returns file descriptor that refers to that endpoint
	// SOCK_STREAM :: provides sequenced, reliable, 2 way connection based stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);     //create the socket
	if (sockfd <0) {
		printf("error in socket");
		exit(1);
	}
	/*
			struct sockaddr_in {
		    short            sin_family;   // e.g. AF_INET, AF_INET6
		    unsigned short   sin_port;     // e.g. htons(3490)
		    struct in_addr   sin_addr;     // see struct in_addr, below
		    char             sin_zero[8];  // zero this if you want to
			};
	*/
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYTCP_PORT);
	// memcpy(str1,str2,n)
	// str1 :: pointer to destination array where content is to be copied
			// type-casted to pointer of type void*
	// str2 :: pointer to source of data to be copied
			// type-casted to pointer of type void*
	// n :: number of bytes to be copied
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);
	// connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
	ret = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));         //connect the socket with the host
	if (ret != 0) {
		printf ("connection failed\n");
		close(sockfd);
		exit(1);
	}

	if((fp = fopen ("myfile.txt","r+t")) == NULL) {
		printf("File doesn't exit\n");
		exit(0);
	}

	ti = str_cli(fp, sockfd, &len);                       //perform the transmission and receiving
	rt = (len/(float)ti);                                 //caculate the average transmission rate
	printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", ti, (int)len, rt);

	close(sockfd);
	fclose(fp);
	exit(0);
}

// transmission and receiving function
float str_cli(FILE *fp, int sockfd, long *len) {
	char *buf;
	long lsize, ci;
	char sends[DATALEN];	// packet to be sent
	struct ack_so ack;
	int n, slen;
	float time_inv = 0.0;
	struct timeval sendt, recvt;
	ci = 0;

	// int fseek(FILE *stream, long int offset, int whence)
	// where whence:	SEEK_SET / SEEK_CUR / SEEK_END
	fseek(fp , 0 , SEEK_END);
	// len = ftell(fp);
	// returns current file pos. of given stream
	lsize = ftell(fp);		// lsize get's the last pos of the file
	rewind(fp);						// sets back the file pos. to the beginning of the file
	printf("The file length is %d bytes\n", (int)lsize);
	printf("the packet length is %d bytes\n",DATALEN);

	// allocate memory to contain the whole file.
	buf = (char *) malloc (lsize);
	if (buf == NULL)
    exit (2);

	// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
	// ptr :: pointer to block of mem w min size of nmemb
	// size :: size in bytes of each element to be read
	// nmemb :: num. of elements, each with size bytes
	// stream :: pointer to FILE object that specifies input stream
  // reads data from the given stream into the array pointed to, by ptr
	fread(buf,1,lsize,fp);		// returns the num. of elements read == lsize

  /*** the whole file is loaded in the buffer. ***/
	buf[lsize] ='\0';									       //append the end byte
	gettimeofday(&sendt, NULL);							 //get the current time

	while(ci <= lsize) {
		if ((lsize+1-ci) <= DATALEN)	         // check if at last data unit
			slen = lsize+1-ci;
		else	// else send at 1 data unit worth
			slen = DATALEN;
		// void *memcpy(void *str1, const void *str2, size_t n)
		// copies n characters from memory area str2 to memory area str1
		memcpy(sends, (buf+ci), slen);
		// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
		n = send(sockfd, &sends, slen, 0);	    // returns num of bytes sent on success, else -1 for error
		if(n == -1) {
			printf("send error!");								//send the data
			exit(1);
		}
		ci += slen;
	}

	// ssize_t recv(int s, void *buf, size_t len, int flags);
	// return num. bytes received or -1 if error occurred
	// return val. will be 0 when peer has performed an orderly shutdown
	// message will be stored in buf
	// len = size of buf
	if ((n = recv(sockfd, &ack, 2, 0))==-1) {
		printf("error when receiving\n");
		exit(1);
	}

	if (ack.num != 1|| ack.len != 0)
		printf("error in transmission\n");

	gettimeofday(&recvt, NULL);
	*len= ci;                                                         // get current time
	tv_sub(&recvt, &sendt);                                           // get the whole trans time
	time_inv += (recvt.tv_sec)*1000.0 + (recvt.tv_usec)/1000.0;
	return(time_inv);
}

void tv_sub(struct  timeval *out, struct timeval *in) {
	if ((out->tv_usec -= in->tv_usec) <0) {
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
