#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#include <netinet/ip_icmp.h>
#include <sys/time.h>

# define PACKET_SIZE 64
# define DATA_SIZE 56


typedef struct s_ping {
    char                    *host; //host argument
    bool                    verbose; // true if on

    struct sockaddr_in      dest; // address resolved
    char                    ip_str[INET_ADDRSTRLEN]; // IP translate in a string

    int                     sock; //socket raw
    uint16_t                id;

    uint16_t                seq;//next sequence's nb
    long                    sent;//nb packages send

    long                    received; //how many answer
}   t_ping;

// struct icmphdr{
//     u_int8_t                type; //octet 0
//     u_int8_t                code; //octet 1
//     uint16_t                checksum;//octet2-3
//     union {
//         struct {
//             uint16_t        id;//octet4-5
//             uint16_t        sequence;//octet6-7
//         } echo;
//         uint32_t            gateway;
//     } un;
// };


void        parse_args(int ac, char **av, t_ping *p);
void        resolve_host(t_ping *p);
void        create_socket(t_ping *p);
uint16_t    checksum(void *data, int len);
void send_echo(t_ping *p);
void run_loop(t_ping *p);
#endif
