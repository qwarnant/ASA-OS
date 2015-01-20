#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "hardware.h"
#include "drive.h"
#include "hw.h"

void dump(unsigned char *buffer, unsigned int buffer_size,
          int ascii_dump, int octal_dump);

int main(int argc, char **argv)
{

    unsigned int c, s, n;
    unsigned char buffer[SECTOR_SIZE];

    if(argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s numcylindre numsector [size] \n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    else {  
        init_master();
        check_disk();
        c = atoi(argv[1]);
        s = atoi(argv[2]);
        if(check_cs(c,s)) {
            n = SECTOR_SIZE;
            if(argc==4) {
                n = atoi(argv[3]);
                n = n < SECTOR_SIZE ? n : SECTOR_SIZE;
            }
            read_sector_n(c, s, buffer, n);
            dump(buffer, n, 1, 1);
        } else {
            if(c >= MAX_CYLINDER)
                fprintf(stderr, "Cylindre demandee : %u, max : %i\n",
                        c, MAX_CYLINDER-1);
            if(s >= MAX_SECTOR)
                fprintf(stderr, "Secteur demandee : %u, max : %i\n",
                        s, MAX_SECTOR-1 );
        }

    }

    return 0;
}



/* dump buffer to stdout,
   and octal dump if octal_dump; an ascii dump if ascii_dump! */
void dump(unsigned char *buffer,
          unsigned int buffer_size,
          int ascii_dump,
          int octal_dump)
{
    unsigned int i,j;

    for (i=0; i<buffer_size; i+=16) {
        /* offset */
        printf("%.8o",i);

        /* octal dump */
        if (octal_dump) {
            for(j=0; j<8; j++)
                printf(" %.2x", buffer[i+j]);
            printf(" - ");

            for( ; j<16; j++)
                printf(" %.2x", buffer[i+j]);

            printf("\n");
        }
        /* ascii dump */
        if (ascii_dump) {
            printf("%8c", ' ');

            for(j=0; j<8; j++)
                printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');
            printf(" - ");

            for( ; j<16; j++)
                printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');

            printf("\n");
        }

    }
}
