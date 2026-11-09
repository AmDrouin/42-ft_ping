#include "ft_ping.h"

static double get_time(){
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
}


void run_loop(t_ping *p){
    double start;

    while(1){
        start = get_time();
        send_echo(p);
        wait_reply(p);
    }
}