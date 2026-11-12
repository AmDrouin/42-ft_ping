#include "ft_ping.h"

#define CYAN        "\e[0;36m"
#define GREEN       "\e[0;32m"
#define BLUE        "\e[0;34m"
#define RED         "\e[0;31m"
#define COLOR_RESET "\e[0m"

#define PACKAGE_SIZE   32

void dbg_graduation(void){
    fprintf(stderr, " ");
    for (int i = 0; i < 32; i++){
        if (i % 10 == 0)
            fprintf(stderr, "%d ", i / 10);
        else
            fprintf(stderr, "  ");
    }
    fprintf(stderr, "\n ");
    for (int i = 0; i < 32; i++)
        fprintf(stderr, "%d ", i % 10);
    fprintf(stderr, "\n");
}

/**
 * @brief write a border +-+
 * 
 * @param i how many bits do you need ?? 
 * @warning i > 0
 */
void border(int bits){
    fprintf(stderr, "+");
    for(int i = 0; i < bits; i++)
        fprintf(stderr, "-+");
    fprintf(stderr, "\n\n");
    dbg_graduation();
}

