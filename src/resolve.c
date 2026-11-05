#include "ft_ping.h"

void resolve_host(t_ping *p){
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; //IPV4
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if (getaddrinfo(p->host, NULL, &hints, &res) != 0){
        fprintf(stderr, "ft_ping: unknown host\n");
        exit(1);
    }
    memcpy(&p->dest, res->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(res);

    inet_ntop(AF_INET, &p->dest.sin_addr, p->ip_str, sizeof(p->ip_str));
}