#include "ft_ping.h"

void create_socket(t_ping *p){
    p->sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (p->sock < 0){
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
        exit(1);
    }
    p->id = getpid();
}

uint16_t checksum(void *data, int len){
    uint16_t    *p = data;
    uint32_t    sum = 0;
    
    while(len > 1){
        sum += *p++;
        len -=2;
    }
    if (len == 1)
        sum += *(uint8_t *)p;
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return (uint16_t)~sum;
}

void send_echo(t_ping *p){
    char            package[PACKET_SIZE];
    struct icmphdr  *icmp = (struct icmphdr *)package;
    struct timeval  now;

    memset(package, 0, sizeof(package));
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(p->id);
    icmp->un.echo.sequence = htons(p->seq);

    gettimeofday(&now, NULL);
    memcpy(package + 8, &now, sizeof(now));

    icmp->checksum = 0;
    icmp->checksum = checksum(package, sizeof(package));

    if(DEBUG){
        DEBUG_PRINT("Sent packet ------- START");
        graduation();
        print_icmp(icmp);
        DEBUG_PRINT("Sent packet ------- END");
    }
    if (sendto(p->sock, package, sizeof(package), 0, (struct sockaddr *)&p->dest, sizeof(p->dest)) < 0)
        fprintf(stderr, "ft_ping: sending packet: %s\n", strerror(errno));
    else
        p->sent++;
    p->seq++;
}   