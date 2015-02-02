#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "mbr.h"
#include "drive.h"


void print_usage(char **argv)
{
    printf("Usage: %s --fc first_cylindre --fs first_sector -s size (>0)\n",
           argv[0]);
}

int main(int argc, char **argv)
{

    unsigned int size, fc, fs, nsector;
    int option_index;
    int c;
    char ch;

    size = 0;
    fc = 0;
    fs = 0;
    option_index = 0;

    static struct option long_options[4] = {
        {"fc", required_argument, 0,  'a' },
        {"fs", required_argument, 0,  'b' },
        {"s",  required_argument, 0,   0  },
        {0, 0, 0, 0}
    };

    while((c = getopt_long(argc, argv, "a:b:s:",
                           long_options, &option_index)) != -1) {

        switch (c) {
        case 'a':
            fc = atoi(optarg);
            break;

        case 'b':
            fs = atoi(optarg);
            break;

        case 's':
            size = atoi(optarg);
            break;

        case '?':
            break;

        default:
            abort();
        }
    }

    if (optind < argc) {
        print_usage(argv);
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
        exit(EXIT_FAILURE);
    }

    if(size == 0) {
        print_usage(argv);
        exit(EXIT_FAILURE);
    }

    nsector = ceil((double)size/HDA_SECTORSIZE);
    /* pas de creation de partition dans le MBR et au-dela du disque */
    if(!check_cs(fc, fs)
            || get_block(fc,fs) <= 0
            || get_block(fc,fs+nsector) >
            HDA_MAXCYLINDER*HDA_MAXSECTOR) {
        fprintf(stderr,"Vous ne pouvez pas creer "
                "de partiton a cet emplacement.\n");
        exit(EXIT_FAILURE);
    }

    init_master();
    check_disk();
    load_mbr();

    if(add_vol(fc, fs, size)!=-1) {
        printf("Partion creee : \n");
        display_mbr();
        printf("Voulez vous sauvegarder les changements ? [O/n] : ");
        (void)scanf(" %c",&ch);
        if(ch!='n' && ch!='N')
            save_mbr();
        else
            printf("Aucun changement effectue.\n");
    }

    return 0;
}
