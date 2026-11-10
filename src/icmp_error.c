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

static void print_ip_dump(struct iphdr *ip, struct icmphdr *icmp)
{
    unsigned char   *bytes = (unsigned char *)ip;
    struct in_addr  src;
    struct in_addr  dst;
    int             ip_len = ip->ihl * 4;
    int             i;

    printf("IP Hdr Dump:\n ");
    for (i = 0; i < (int)sizeof(struct iphdr); i++)
        printf("%02x%s", bytes[i], (i % 2) ? " " : "");
    printf("\n");

    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
    printf(" %1x  %1x  %02x", ip->version, ip->ihl, ip->tos);
    printf(" %04x %04x", ntohs(ip->tot_len), ntohs(ip->id));
    printf("   %1x %04x", (ntohs(ip->frag_off) & 0xe000) >> 13,
                           ntohs(ip->frag_off) & 0x1fff);
    printf("  %02x  %02x %04x", ip->ttl, ip->protocol, ntohs(ip->check));
    src.s_addr = ip->saddr;
    dst.s_addr = ip->daddr;
    printf(" %s ", inet_ntoa(src));
    printf(" %s ", inet_ntoa(dst));
    printf("\n");

    printf("ICMP: type %u, code %u, size %u", icmp->type, icmp->code,
           ntohs(ip->tot_len) - ip_len);
    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY)
        printf(", id 0x%04x, seq 0x%04x", icmp->un.echo.id, icmp->un.echo.sequence);
    printf("\n");
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
    
    if(p->verbose)
        print_ip_dump(inner_ip, inner_icmp);
}

