// headfile for TCP program
#include <sys/socket.h> // Contains data definitions and socket structures
#include <netinet/in.h> // Defines Internet constants and structures
#include <stdio.h>
#include <netdb.h>	// Contains data defintions for socket subroutines
#include <string.h>
#include <sys/types.h>	// Contain data type definitions
#include <unistd.h>	// Contains standard symbolic constants and types
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>	// Defines socket error codes for network communication errors
#include <fcntl.h>
#include <sys/time.h>

#define NEWFILE (O_WRONLY|O_CREAT|O_TRUNC)
#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define DATALEN 500		// length of data
#define BUFSIZE 60000
#define PACKLEN 508		// data unit size
#define HEADLEN 8		// header size

struct pack_so			//data packet structure
{
	uint32_t num;		// the sequence number
	uint32_t len;		// the packet length
	char data[DATALEN];	//the packet data
};

struct ack_so
{
	uint8_t num;
	uint8_t len;
};

