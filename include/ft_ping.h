#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

typedef struct s_ping {
    char                    *host; //host argument
    bool                    verbose; // true if on

    struct sockaddr_in      dest; // address resolved
    char                    ip_str[INET_ADDRSTRLEN]; // IP translate in a string
}   t_ping;


void parse_args(int ac, char **av, t_ping *p);
void resolve_host(t_ping *p);
#endif
