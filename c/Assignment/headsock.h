// headfile for TCP program
#include <sys/socket.h>     // Contains data definitions and socket structures
#include <netinet/in.h>     // Defines Internet constants and structures
#include <stdio.h>
#include <netdb.h>          // Contains data defintions for socket subroutines
#include <string.h>
#include <sys/types.h>      // Contain data type definitions
#include <unistd.h>         // Contains standard symbolic constants and types
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>          // Defines socket error codes for network communication errors
#include <fcntl.h>
#include <sys/time.h>

#define NEWFILE (O_WRONLY|O_CREAT|O_TRUNC)
#define MYTCP_PORT 4950
#define DATALEN 9000         // length of data 500 for 1 DU
#define BUFSIZE 60000
#define PACKLEN 508         // data unit size
#define HEADLEN 8           // header size

struct pack_so {            // data packet structure
    uint32_t num;           // pkt seq number
    uint32_t len;           // pkt length
    char data[DATALEN];     // pkt data
};

struct ack_so {             // ack pkt structure
    uint8_t num;            // ack seq number
    uint8_t len;
};
