#include "ft_ping.h"



void create_socket(t_ping *p){
    p->sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (p->sock < 0){
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
        exit(1);
    }
    p->id = getpid();
}