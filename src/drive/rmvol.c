#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "mbr.h"
#include "drive.h"


void print_usage(char **argv)
{
    printf("Usage: %s num_volume\n",
           argv[0]);
}

int main(int argc, char **argv)
{

    unsigned int i;
    char ch;

    init_master();
    check_disk();
    load_mbr();

    if(argc < 2) {
        print_usage(argv);
        exit(EXIT_FAILURE);
    }

    i = atoi(argv[1]);

    if(rmvol(i)!=-1) {
        display_mbr();
        printf("Voulez vous sauvegarder les changements ? [O/n]\n");
        (void)scanf(" %c",&ch);
        if(ch!='n' && ch!='N')
            save_mbr();
        else
            printf("Aucun changement effectue.\n");
    } else {
        printf("la partition %i n'est pas presente\n",i);
    }

    return 0;
}
