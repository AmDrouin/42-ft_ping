#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct s_ping {
    char            *host; //host argument
    bool            verbose; // 1 if on

}   t_ping;


void parse_args(int ac, char **av, t_ping *p);
#endif
