#include "ft_ping.h"
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>


static double get_time(){
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
}

static void print_reply(t_ping *p, struct iphdr *ip, struct icmphdr *icmp, ssize_t icmp_len, struct sockaddr_in *from){
    
    struct timeval  sent;
    double          rtt;

    memcpy(&sent, (char *)icmp + 8 ,sizeof(sent));
    rtt = get_time() - (sent.tv_sec *1000.0 + sent.tv_usec / 1000.0);

    if (p->received == 0 || rtt < p->rtt_min)
        p->rtt_min = rtt;
    if (p->received == 0 || rtt > p->rtt_max)
        p->rtt_max = rtt;
    p->rtt_sum += rtt;
    p->rtt_sum2 += rtt * rtt;
    p->received++;

    printf("%zd bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
        icmp_len,
        inet_ntoa(from->sin_addr),
        ntohs(icmp->un.echo.sequence),
        ip->ttl,
        rtt);
}

static void handle_packet(t_ping *p, char *buf, ssize_t n, struct sockaddr_in *from){
    struct  iphdr *ip = (struct iphdr *)buf;
    int ip_len = ip->ihl * 4;
    if(n < ip_len + (ssize_t)sizeof(struct icmphdr))
        return;
    struct icmphdr *icmp = (struct icmphdr *)(buf+ip_len);
     if(DEBUG){    
        DEBUG_PRINT("Received packet ------- START");
        graduation();
        print_ip(ip);
        print_icmp(icmp);
        DEBUG_PRINT("Received packet ------- END");
    }
    if (icmp->type == ICMP_ECHOREPLY){
        if (ntohs(icmp->un.echo.id) != p->id)
            return;
        print_reply(p, ip, icmp, n - ip_len, from);
    }
    else if (icmp->type == ICMP_TIME_EXCEEDED || icmp->type == ICMP_DEST_UNREACH)
        print_error(p, icmp, n - ip_len, from);
}

static void read_package(t_ping *p){
    char buf[1024];
    struct sockaddr_in  from;
    socklen_t       from_len;
    ssize_t n;

    from_len = sizeof(from);
    n = recvfrom(p->sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &from_len);
    if(n < 0)
        return;
    handle_packet(p, buf, n, &from);
        //-DEBUG
    // struct icmphdr *icmp = (struct icmphdr *)(buf + 20);
    // printf("type = %d\n", icmp->type);
}

/**
 * @brief listen the socket until the deadline, handle every received package

 * 
 * @param p - general struct
 * @param deadline - max time to listen the socket (ms)
 */
void wait_reply(t_ping *p, double deadline){
    double left_time;
    fd_set fds;
    struct timeval tv;

    while((left_time = deadline - get_time()) > 0 && !g_stop){
        tv.tv_sec = (long)(left_time /1000);
        tv.tv_usec= (long)(left_time * 1000) % 1000000;
        FD_ZERO(&fds);
        FD_SET(p->sock, &fds);
        int ret = select(p->sock+1, &fds, NULL, NULL, &tv);
        if(ret >0)
            read_package(p);
    }

}

void run_loop(t_ping *p){
    double start;

    while(!g_stop){
        start = get_time();
        send_echo(p);
        wait_reply(p, start + 1000);
    }
}