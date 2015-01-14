#include <stdio.h>
#include <stdlib.h>
#include "mbr.h"
#include "drive.h"
#include "super.h"

#define CURRENT_VOLUME 0U

int main(void)
{
    char *cv, ch;
    unsigned int current_vol;

    init_master();
    check_disk();
    load_mbr();

    if((cv = getenv("CURRENT_VOLUME"))!=NULL) {
        current_vol = (unsigned int)atoi(cv);
    } else {
        current_vol = CURRENT_VOLUME;
    }
    if(check_vol(current_vol)) {
        if(!load_super(current_vol)) {
            printf("Un systeme de fichier est deja present sur cette partition,"
                   "voulez-vous continuer ? [o/N] : ");
            (void)scanf(" %c",&ch);
            if(ch=='o' || ch=='O')
                init_super(current_vol);
        } else {
            init_super(current_vol);
        }
    }
    else
        printf("Impossible de creer un systeme de fichier pour"
               "cette partition.\n");

    return 0;

}
