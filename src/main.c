#include "ft_ping.h"

volatile sig_atomic_t g_stop = 0;

static void handle_sigint(int sig)
{
    (void)sig;
    g_stop = 1;
}

int main(int ac, char **av){
    t_ping p;

    memset(&p, 0, sizeof(p));
    parse_args(ac, av, &p);
    // printf("DEBUG-- HOST: %s, VERBOSE: %d\n", p.host, p.verbose);
    resolve_host(&p);
    create_socket(&p);
    printf("PING %s (%s): %d data bytes", p.host, p.ip_str, 56);
    if (p.verbose)
        printf(", id 0x%04x = %u", p.id, p.id);
    printf("\n");
    //need to be in the loop
    //send_echo(&p);
    //
    signal(SIGINT, handle_sigint);
    run_loop(&p);
    printf("DEBUG--- stop");
    close(p.sock);
    return(0); //TODO : if no reply -> return 1
}