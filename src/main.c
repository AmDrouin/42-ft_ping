#include "ft_ping.h"

volatile sig_atomic_t g_stop = 0;
bool DEBUG = false;

static void handle_sigint(int sig)
{
    (void)sig;
    g_stop = 1;
}

int main(int ac, char **av){
    t_ping p;
    border(64);
    memset(&p, 0, sizeof(p));
    parse_args(ac, av, &p);
    DEBUG_PRINT("HOST: %s, VERBOSE: %d", p.host, p.verbose);
    resolve_host(&p);
    create_socket(&p);
    printf("PING %s (%s): %d data bytes", p.host, p.ip_str, DATA_SIZE);
    if (p.verbose)
        printf(", id 0x%04x = %u", p.id, p.id);
    printf("\n");
    signal(SIGINT, handle_sigint);
    run_loop(&p);
    print_stats(&p);
    close(p.sock);
    return(p.received == 0);
}