#include "ft_ping.h"
#include <netinet/ip_icmp.h>
#include <sys/types.h>

static const t_icmp_msg g_icmp_msgs[] ={
    {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"},
    {ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
    {ICMP_DEST_UNREACH, ICMP_SR_FAILED,"Source Route Failed"},
    {0, 0, NULL}
};

const char *icmp_error_str(int type, int code){
    for(int i = 0; g_icmp_msgs[i].msg != NULL ;i++){
        if(type == g_icmp_msgs[i].type && code == g_icmp_msgs[i].code){
            return(g_icmp_msgs[i].msg);
        }
    }
    return "Unknown ICMP error";
}


void print_error(t_ping *p, struct icmphdr *icmp, ssize_t len, struct sockaddr_in *from){
    struct iphdr    *inner_ip;
    struct icmphdr  *inner_icmp;
    int inner_ip_len;

    if(len < 8 + (ssize_t)sizeof(struct iphdr))
        return;

    inner_ip = (struct iphdr *)((char *)icmp + 8);

    inner_ip_len = inner_ip->ihl *4;

    if (len < 8 + inner_ip_len + 8)
        return;
    
    inner_icmp = (struct icmphdr *)((char *)inner_ip + inner_ip_len);

    if (ntohs(inner_icmp->un.echo.id) != p->id)
        return;

    printf("%zd bytes from %s: %s\n", 
        len, 
        inet_ntoa(from->sin_addr),
        icmp_error_str(icmp->type, icmp->code));
}