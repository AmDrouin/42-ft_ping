#include "ft_ping.h"


int main(int ac, char **av){
    t_ping p;

    memset(&p, 0, sizeof(p));
    parse_args(ac, av, &p);
    printf("DEBUG-- HOST: %s, VERBOSE: %d\n", p.host, p.verbose);
    return(0);
}