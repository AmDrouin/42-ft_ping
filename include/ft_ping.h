#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>


typedef struct s_ping {
    char                    *host; //host argument
    bool                    verbose; // true if on

    struct sockaddr_in      dest; // address resolved
    char                    ip_str[INET_ADDRSTRLEN]; // IP translate in a string

    int                     sock; //socket raw
    uint16_t                id;
}   t_ping;


void    parse_args(int ac, char **av, t_ping *p);
void    resolve_host(t_ping *p);
void    create_socket(t_ping *p);

#endif
