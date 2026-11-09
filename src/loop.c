#include "ft_ping.h"
#include <stdio.h>
#include <sys/select.h>

static double get_time(){
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
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
    printf("reçu %zd octets de %s\n", n, inet_ntoa(from.sin_addr));
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

    while((left_time = deadline - get_time()) > 0){
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

    while(1){
        start = get_time();
        send_echo(p);
        wait_reply(p, start + 1000);
    }
}