/*******************************
tcp_client.c: the source file of the client in tcp transmission
********************************/

#include "headsock.h"		// include headsock header file

// declare str_cli() function
void str_cli(FILE *fp, int sockfd);        //used for socket transmission

int main(int argc, char **argv)
{
	int sockfd, ret;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;				// *sh is a pointer
	struct in_addr **addrs;		// **addrs is dbl pointer

	if (argc != 2) {		// check if argument is not 2
		printf("parameters not match");
	}

// gethostbyname() is absolete :: https://linux.die.net/man/3/gethostbyname
// returns struct of type hostent for given hostname. name is either by hostname or IPv4 or IPv6
	sh = gethostbyname(argv[1]);	                            //get host's information from 2nd input argument
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
	printf("canonical name: %s\n", sh->h_name);
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);
	switch(sh->h_addrtype)
	{
		case AF_INET:		// AF_INET is for IPv4. IPv6 is AF_INET6
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}

	addrs = (struct in_addr **)sh->h_addr_list;                       //get the server(receiver)'s ip address
	// socket(int domain,, int type, int protocol)
	// returns file descriptor that refers to that endpoint
	// SOCK_STREAM :: provides sequenced, reliable, 2 way connection based stream
	sockfd = socket(AF_INET, SOCK_STREAM, 0);                         //create the socket - each connection will create a new process
	if (sockfd <0)
	{
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
	ret = connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));         //connect the socket with the server(receiver)
	if (ret != 0) {		// on connection/binding succeeds, 0 is return else -1 is returned on error
		printf ("connection failed\n");
		close(sockfd);
		exit(1);
	}

	str_cli(stdin, sockfd);                       //perform the transmission

	close(sockfd);
	exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
	char sends[MAXSIZE];

	printf("Please input a string (less than 50 character):\n");
	// fgets(char *str, int n, FILE *stream)
	// str :: pointer to an array of chars where string read is stored
	// n :: max number of characters to be read
	// stream :: pointer to a FILE obj that identifies the stream where char are read from
	if (fgets(sends, MAXSIZE, fp) == NULL) {
		printf("error input\n");
	}
	// send(int sockfd, const void *buf, size_t len, int flags);
	// send a message on a socket
	send(sockfd, sends, strlen(sends), 0);		//send the string to the server(receiver)
	printf("send out!!\n");
}
