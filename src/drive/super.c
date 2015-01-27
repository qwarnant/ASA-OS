#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mbr.h"
#include "drive.h"
#include "super.h"

#define MAGIC 0x0000DEAD
#define SUPER 0

struct free_bloc_s {
    unsigned int fb_n_free, fb_next;  /* 0 = pas de suivant */
};

struct super_bloc_s {
    unsigned int super_magic, super_first_free_bloc, super_n_free;
    char name[32];
    int id;
};

static struct super_bloc_s current_super;

void init_super(unsigned int volume)
{
    if(mbr.mbr_vol[volume].vol_n_bloc > 1 && volume < mbr.mbr_n_vol) {
        struct vol_descr_s *vol;
        struct super_bloc_s sb;
        struct free_bloc_s fb;
        unsigned int free_size;

        vol = &mbr.mbr_vol[volume];
        sb.super_magic = MAGIC;
        sb.super_first_free_bloc = 1;
        sb.super_n_free = free_size = vol->vol_n_bloc - 1;

        fb.fb_next = 0;
        fb.fb_n_free = free_size;

        current_super = sb;
        current_volume = volume;
        save_super();
        write_bloc_n(volume, SUPER+1, (unsigned char *)&fb,
                     sizeof(struct free_bloc_s));
    } else {
        printf("Initialisation de super impossible taille insuffisante"
               " ou numero de volume incorrect\n");
    }
}


int load_super(unsigned int vol)
{
    current_volume = vol;
    read_bloc_n(vol, SUPER, (unsigned char *)&current_super,
                sizeof(struct super_bloc_s));
    return MAGIC==current_super.super_magic ? 0 : 1;
}

void save_super()
{
    write_bloc_n(current_volume, SUPER, (unsigned char *)&current_super,
                 sizeof(struct super_bloc_s));
}


unsigned int new_bloc()
{

    unsigned int bloc, result;
    struct free_bloc_s fb;
    result = bloc = current_super.super_first_free_bloc;
printf("current : %s\n",current_super.super_n_free);
    if(bloc!=0 && current_super.super_n_free>0) {
        read_bloc_n(current_volume, bloc, (unsigned char *)&fb,
                    sizeof(struct free_bloc_s));

        if(fb.fb_n_free==1) { /* un seul bloc de libre */
            current_super.super_first_free_bloc = fb.fb_next;
        } else { /* plusieurs bloc libre */
            ++current_super.super_first_free_bloc;
            --fb.fb_n_free;
            write_bloc_n(current_volume, bloc+1,
                         (unsigned char *)&fb, sizeof(struct free_bloc_s));
        }
        --current_super.super_n_free;
        save_super();
    }
    return result;
}


void free_bloc(unsigned int bloc)
{
    unsigned int bloc_prec, i;
    struct free_bloc_s fb, fb_prec;

    if(bloc!=0) { /* Pas de liberation du super bloc*/

        read_bloc_n(current_volume, bloc, (unsigned char *)&fb,
                    sizeof(struct free_bloc_s));

        bloc_prec = current_super.super_first_free_bloc;

        if(current_super.super_n_free == 0) { /* il n'y a pas de bloc libre */
            current_super.super_first_free_bloc = bloc;
            fb.fb_next = 0;
        } else {
            if(bloc_prec < bloc) { /* recherche du premier bloc libre precedant */

                for(i = bloc_prec; i < bloc && i!=0; i=fb_prec.fb_next ) {
                    read_bloc_n(current_volume, i, (unsigned char *)&fb_prec,
                                sizeof(struct free_bloc_s));
                    bloc_prec = i;
                }

                fb.fb_next = fb_prec.fb_next;
                fb_prec.fb_next = bloc;
                write_bloc_n(current_volume, bloc_prec,
                             (unsigned char *)&fb_prec, sizeof(struct free_bloc_s));

            } else { /* pas de bloc libre precedant */
                fb.fb_next = current_super.super_first_free_bloc;
                current_super.super_first_free_bloc = bloc;
            }
        }
        fb.fb_n_free = 1;
        write_bloc_n(current_volume, bloc,
                     (unsigned char *)&fb, sizeof(struct free_bloc_s));

        ++current_super.super_n_free;
        save_super();
    }

}


void free_blocs(unsigned int *tab, unsigned int size)
{
    unsigned int i;
    for(i = 0; i < size; i++) {
        free_bloc(tab[i]);
    }
}


void display_filesystem()
{
    unsigned int i, vol_n_bloc, nb_bloc_size, util, dispo, percent, present, current_volume_temp;
    struct vol_descr_s *vol;

    current_volume_temp = current_volume; //copy

    printf("Sys de Fichiers -   Blocks -  Utilise - Disponible - Uti\n");
    for(i=0; i < mbr.mbr_n_vol; i++) {

        vol = &mbr.mbr_vol[i];
        vol_n_bloc = vol->vol_n_bloc;
        nb_bloc_size = vol_n_bloc*SECTOR_SIZE;

        if(!load_super(i)) { //warning : modify the current volume var
            present = 1;
            dispo = current_super.super_n_free*SECTOR_SIZE;
            util = nb_bloc_size - dispo;
            percent = floor((double)(util*100)/(nb_bloc_size));
        } else {
            present = 0;
            util = 0;
            dispo = nb_bloc_size;
            percent = 0;
        }

        printf("%15s   %8u   %8u   %10u   %3u\n",
               present==1 ? "present" : "non", nb_bloc_size,
               util, dispo, percent );
    }

    current_volume = current_volume_temp; //restore

}
