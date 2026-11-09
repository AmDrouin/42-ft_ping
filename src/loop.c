#include "ft_ping.h"

static double get_time(){
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
}

/**
 * @brief listen the socket until the deadline, handle every received package

 * 
 * @param p - general struct
 * @param deadline - max time to listen the socket (ms)
 */
void wait_reply(t_ping *p, double deadline){
    double left_time;

    while((left_time = deadline - get_time()) > 0){

    }

}

void run_loop(t_ping *p){
    double start;

    while(1){
        start = get_time();
        send_echo(p);
        //wait_reply(p);
    }
}