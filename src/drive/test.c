#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mbr.h"
#include "super.h"
#include "drive.h"

#define CURRENT_VOLUME 0U

int main()
{
    char *cv;
    unsigned int i, nb_bloc;
    unsigned int current_vol, nb_free;
    /* initialize random seed: */
    srand(time(NULL));

    /* Verification du disque et du MBR */
    init_master();
    check_disk();
    load_mbr();

    if((cv = getenv("CURRENT_VOLUME"))!=NULL) {
        current_vol = (unsigned int)atoi(cv);
    } else {
        current_vol = CURRENT_VOLUME;
    }

    if(!check_vol(current_vol)) {
        printf("le mbr ne contient pas la parition %i\n", current_vol);
        exit(EXIT_FAILURE);
    }
    if(load_super(current_vol)) {
        printf("Impossible de charger le super de la partition %i\n",
               current_vol);
        exit(EXIT_FAILURE);
    }
    nb_bloc = 0;
    printf("Appel de new_bloc() sur la partition %u :\n", current_vol);
    fflush(stdout);
    while((i=new_bloc())!=0) {
        printf("%3i - ",i);
        ++nb_bloc;
        fflush(stdout);
    }
    printf("\nVerification du disque plein :\n");
    display_filesystem();

    nb_free = (nb_bloc/2);
    if(nb_free>0)
        --nb_free;

    printf("Iteration de %u free_bloc() d'un bloc choisi aleatoirement "
           "pas de liberation successive du meme bloc\n", nb_free);

    for(i = 1; i <= nb_free; i++)
        free_bloc(rand()%2+(i*2));

    display_filesystem();
    fflush(stdout);

    printf("remplissage de la partition a nouveau\n");
    i = 0;
    while(new_bloc())
        i++;

    printf("%i blocs alloues sur %u liberes\n",i, nb_free);
    return 0;
}
