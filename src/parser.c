#include "ft_ping.h"

void parse_long_option(char *av, t_ping *p){
    (void) av;
    (void) p;
}
void parse_short_option(char *av, t_ping *p){
    (void) av;
    (void) p;
}

void usage_error(char *msg){
    (void) msg;
}

void parse_args(int ac, char **av, t_ping *p){
    for(int i = 1; i < ac; i++){
        if(strncmp(av[i], "--", 2) == 0)
            parse_long_option(av[i], p);
        else if(strncmp(av[i], "-", 1) == 0)
            parse_short_option(av[i], p);
        else if (!p->host)
            p->host = av[i];
    }
    if (!p->host)
        usage_error("missing host operand");
}