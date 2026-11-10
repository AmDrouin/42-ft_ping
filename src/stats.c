#include "ft_ping.h"
#include "math.h"

void print_stats(t_ping *p){
    double var;
    double avg;

    printf("--- %s ping statistics ---\n", p->host);
    printf("%ld packets transmitted, ", p->sent);
    printf("%ld packets received, ", p->received);
    
    if(p->sent)
        printf("%d%% packet loss", (int)((p->sent - p->received)*100/p->sent));
    printf("\n");

    if(p->received == 0)
        return;
    avg = p->rtt_sum / p->received;
    var = p->rtt_sum2 / p->received - avg * avg; //moy square - square moye
    if(var < 0)
        var = 0;
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
        p->rtt_min,
        avg,
        p->rtt_max,
        sqrt(var));
}