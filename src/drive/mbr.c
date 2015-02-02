#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mbr.h"
#include "drive.h"

#define MAGIC 0xDEADBEEF

struct mbr_descr_s mbr;

void check_struc_mbr()
{
    if(sizeof(struct mbr_descr_s)>HDA_SECTORSIZE) {
        printf("le disque n'est pas compatible avec le programme "
               "la taille d'un secteur (%i) ne peut pas contenir "
               "celle du mbr (%lu)\n", HDA_SECTORSIZE, sizeof(struct mbr_descr_s));
        exit(EXIT_FAILURE);
    }
}

int load_mbr()
{
    check_struc_mbr();
    read_sector_n(0, 0, (unsigned char *)&mbr, sizeof(struct mbr_descr_s));

    if(mbr.mbr_magic != MAGIC) {
        mbr.mbr_magic = MAGIC;
        mbr.mbr_n_vol = 0;
        printf("Aucune partition MBR trouvee.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void save_mbr()
{
    write_sector(0, 0,(unsigned char *)&mbr);
}

void display_mbr()
{
    unsigned int i, vol_n_bloc;
    struct vol_descr_s *vol;
    for(i=0; i < mbr.mbr_n_vol; i++) {
        vol = &mbr.mbr_vol[i];
        vol_n_bloc = vol->vol_n_bloc;
        printf("Volume %u, %u secteurs (%u,%u) -> (%u,%u)\n", i, vol->vol_n_bloc,
               vol->vol_first_cylinder, vol->vol_first_sector,
               cylindre_of_bloc(i, vol_n_bloc-1), sector_of_bloc(i, vol_n_bloc-1));
    }
}

int add_vol(unsigned int c, unsigned int s, unsigned int size)
{

    unsigned int nsector, i;
    struct vol_descr_s *vol;
    if(!check_cs(c,s))
        return -1;
    if(mbr.mbr_n_vol >= 8) {
        fprintf(stderr, "Nombre maximum de partition atteint");
        return -1;
    }

    nsector = ceil((double)size/HDA_SECTORSIZE);
    for(i=0; i < mbr.mbr_n_vol; i++) {
        vol = &mbr.mbr_vol[i];
        /* verifie que la partition ne se situe pas
         * dans une partition deja existante */
        if(get_block(c,s) >= get_block(vol->vol_first_cylinder, vol->vol_first_sector) &&
                get_block(c,s) <= get_block(vol->vol_first_cylinder, vol->vol_first_sector+(vol->vol_n_bloc-1))) {
            fprintf(stderr,"Une partition est deja "
                    "presente a cet emplacement.\n");
            return -1;
        }
    }

    vol = &mbr.mbr_vol[mbr.mbr_n_vol];
    mbr.mbr_n_vol++;
    vol->vol_n_bloc = nsector;
    vol->vol_first_cylinder = c;
    vol->vol_first_sector = s;

    return 0;
}


int rmvol(unsigned int i)
{
    unsigned int j;
    if(i<(unsigned int)mbr.mbr_n_vol) {
        for(j = i; j < mbr.mbr_n_vol-1; j++)
            memcpy(&mbr.mbr_vol[j], &mbr.mbr_vol[j+1], sizeof(struct vol_descr_s));
        mbr.mbr_n_vol--;
        return 0;
    }
    return -1;


}

int check_vol(unsigned int vol)
{
    return vol < mbr.mbr_n_vol;
}

unsigned int cylindre_of_bloc(unsigned int vol, unsigned int blk)
{
    struct vol_descr_s *volume;
    volume = &mbr.mbr_vol[vol];
    return volume->vol_first_cylinder + (volume->vol_first_sector + blk)/HDA_MAXSECTOR;
}

unsigned int sector_of_bloc(unsigned int vol, unsigned int blk)
{
    struct vol_descr_s *volume;
    volume = &mbr.mbr_vol[vol];
    return (volume->vol_first_sector + blk) % HDA_MAXSECTOR;
}

void read_bloc(unsigned int vol, unsigned int blk, unsigned char *buffer)
{
    if(vol<8 && blk < HDA_SECTORSIZE) {
        read_sector(cylindre_of_bloc(vol, blk),
                    sector_of_bloc(vol, blk), buffer);
    }
}

void write_bloc(unsigned int vol, unsigned int blk, unsigned char *buffer)
{
    if(vol<8 && blk < HDA_SECTORSIZE) {
        write_sector(cylindre_of_bloc(vol, blk),
                     sector_of_bloc(vol, blk), buffer);
    }
}

void read_bloc_n(unsigned int vol, unsigned int blk, unsigned char *buffer,
                 unsigned int n)
{
    if(vol<8 && blk < HDA_SECTORSIZE) {
        read_sector_n(cylindre_of_bloc(vol, blk),
                      sector_of_bloc(vol, blk), buffer, n);
    }
}

void write_bloc_n(unsigned int vol, unsigned int blk, unsigned char *buffer,
                  unsigned int n)
{
    if(vol<8 && blk < HDA_SECTORSIZE) {
        write_sector_n(cylindre_of_bloc(vol, blk),
                       sector_of_bloc(vol, blk), buffer, n);
    }
}

unsigned int get_block(unsigned int c, unsigned int s)
{
    return c*HDA_MAXSECTOR + s;
}
