#include "ft_ping.h"
#include <stdint.h>
#include <stdio.h>

#define CYAN        "\e[0;36m"
#define GREEN       "\e[0;32m"
#define BLUE        "\e[0;34m"
#define RED         "\e[0;31m"
#define COLOR_RESET "\e[0m"

#define PACKAGE_SIZE   32

void hexdump(const unsigned char *data, int len){
    for(int i = 0; i < len; i++){
        if(i%16 == 0){
            if(i != 0)
                fprintf(stderr, "\n");
            fprintf(stderr, "0x%04x: ", i);
        }
        fprintf(stderr, "%02x ", data[i]);
        if(i % 16 == 7)
            fprintf(stderr, " ");
    }
    fprintf(stderr, "\n\n");
}

/**
 * @brief picks the value's colour from the size of the field
 */
static const char *color(int bits){
    if (bits <= 8)
        return CYAN;
    if (bits <= 16)
        return BLUE;
    return RED;
}

static void cell(int bits, uint32_t value){
    int width = 2 * bits -1 ;
    int digits = (bits +3 ) /4;
    int len = 2 + digits;
    int left = (width - len) /2;
    int right = width - len -left;

    if (left < 0)
        left = 0;
    if (right < 0)
        right = 0;
    fprintf(stderr, "%*s0x%s%0*x%s%*s|", left, "", color(bits), digits, value, COLOR_RESET, right, "");
}

/**
 * @brief prints the bit graduation
 */
void graduation(void){
    fprintf(stderr, " ");
    for (int i = 0; i < 32; i++){
        if (i % 10 == 0)
            fprintf(stderr, "%d ", i / 10);
        else
            fprintf(stderr, "  ");
    }
    fprintf(stderr, "\n ");
    for (int i = 0; i < 32; i++)
        fprintf(stderr, "%d ", i % 10);
    fprintf(stderr, "\n");
}

/**
 * @brief write a border +-+
 */
static void border(int bits){
    fprintf(stderr, "+");
    for(int i = 0; i < bits; i++)
        fprintf(stderr, "-+");
    fprintf(stderr, "\n");
}

static void draw_fields(const t_field *fields, int n){
    int used = 0;
 
    border(PACKAGE_SIZE);
    fprintf(stderr, "|");
    for (int i = 0; i < n; i++){
        cell(fields[i].bits, fields[i].value);
        used += fields[i].bits;
        if (used == 32){
            fprintf(stderr, "\n");
            border(32);
            used = 0;
            if (i + 1 < n)
                fprintf(stderr, "|");
        }
    }
    if (used != 0)
        fprintf(stderr, "\n[DEBUG] draw_fields: line not complete (%d/32 bits)\n", used);
}

void print_icmp(struct icmphdr *icmp){
    t_field fields[] = {
        {8,  icmp->type},
        {8,  icmp->code},
        {16, ntohs(icmp->checksum)},
        {16, ntohs(icmp->un.echo.id)},
        {16, ntohs(icmp->un.echo.sequence)},
    };

    fprintf(stderr, "ICMP: Echo or Echo Reply Message\n");
    draw_fields(fields, sizeof(fields) / sizeof(fields[0]));
}

void print_ip(struct iphdr *ip){
    uint16_t frag = ntohs(ip->frag_off);
     t_field fields[] = {
        {4,  ip->version},
        {4,  ip->ihl},
        {8,  ip->tos},
        {16, ntohs(ip->tot_len)},
        {16, ntohs(ip->id)},
        {3,  frag >> 13},           // flags: the 3 high bits
        {13, frag & 0x1fff},        // fragment offset: the 13 low bits
        {8,  ip->ttl},
        {8,  ip->protocol},
        {16, ntohs(ip->check)},
        {32, ntohl(ip->saddr)},
        {32, ntohl(ip->daddr)},
    };
 
    fprintf(stderr, "                   Internet Header Format\n");
    draw_fields(fields, sizeof(fields) / sizeof(fields[0]));
}