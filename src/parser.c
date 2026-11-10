#include "ft_ping.h"

static void print_help(){
    printf("Usage: ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
    printf(" -v, --verbose          verbose output\n");
    printf(" -?, --help             give this help list\n");
    exit(0);
}

static void usage_error(char *msg){
    fprintf(stderr, "ft_ping: %s\n", msg);
    fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
    exit(64);
}

static void parse_long_option(char *av, t_ping *p){
    if(strcmp(av, "--help") == 0)
        print_help();
    else if (strcmp(av, "--verbose") == 0)
        p->verbose = true;
    else if (strcmp(av, "--usage") == 0){
        printf("Usage: ping [-v?] [--verbose] [--help] [--usage] HOST\n");
        exit (0);
    }
    else {
        fprintf(stderr, "ft_ping: unrecognized option '%s'\n", av);
        fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
        exit(64);
    }
}

static void parse_short_option(char *av, t_ping *p){
    for (int i = 1; av[i]; i++){
        if (av[i] == 'v')
            p->verbose = true;
        else if (av[i] == '?')
            print_help();
        else {
             fprintf(stderr, "ft_ping: invalid option -- '%c'\n", av[i]);
            fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
            exit(64);
        }
    }
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