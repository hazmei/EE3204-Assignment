#include <sys/socket.h> // data def. & socket structures
#include <netinet/in.h> // internet const. & structures
#include <stdio.h>      // std. input output
#include <netdb.h>      // data def. for socket subroutines
#include <string.h>
#include <sys/types.h>  // data type definitions
#include <unistd.h>     // std. symbolic const. & types
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>      // socket error codes for network comm. errors
#include <fcntl.h>

#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define MAXSIZE 50    // 50 characters max
