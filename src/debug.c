#include "ft_ping.h"
#include <cstdio>

#define CYAN        "\e[0;36m"
#define GREEN       "\e[0;32m"
#define BLUE        "\e[0;34m"
#define RED         "\e[0;31m"
#define COLOR_RESET "\e[0m"

/**
 * @brief write a border +-+
 * 
 * @param i how many bits do you need ?? 
 * @warning i > 0
 */
void dbg_border(int bits){
    fprintf(stderr, "+");
    for(int i = 0; i < bits; i++)
        fprintf(stderr, "-+");
    fprintf(stderr, "\n");
}