#include <stdio.h>
#include <stdlib.h>
#include "hw.h"
#include "frmt.h"
#include "drive.h"
#include "hardware.h"


int main(int argc, char **argv)
{

    unsigned int c, s, v;
    v = 0;

    if(argc > 2) {
        fprintf(stderr, "Usage: %s [valeur=0] \n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    if(argc == 2) {
        v = atoi(argv[2]);
    }
    else {
        init_master();
        check_disk();
        printf("formatage du disque en cours...\n");
        fflush(stdout);
        for(c = 0; c < MAX_CYLINDER; c++)
            for(s = 0; s < MAX_SECTOR; s++)
                format_sector(c, s, SECTOR_SIZE, v);
        printf("fin du formatage.\n");
        fflush(stdout);
    }
    return 0;
}
