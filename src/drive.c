#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw.h"
#include "drive.h"
#include "hardware.h"

static void goto_sector(unsigned int c, unsigned int s);
static void empty_it() {return;}

void read_sector(unsigned int c, unsigned int s, unsigned char *buffer)
{
    if(check_cs(c,s)) {
        goto_sector(c,s);
        _out(HDA_DATAREGS,1);
        _out(HDA_CMDREG,CMD_READ);
        _sleep(HDA_IRQ); /* attente de HDA_IRQ */
        memcpy(buffer, MASTERBUFFER, SECTOR_SIZE);
    }
}

void read_sector_n(unsigned int c, unsigned int s, unsigned char *buffer,
                   unsigned int n)
{
    if(check_cs(c,s)) {
        goto_sector(c,s);
        _out(HDA_DATAREGS,1);
        _out(HDA_CMDREG,CMD_READ);
        _sleep(HDA_IRQ); /* attente de HDA_IRQ */
        memcpy(buffer, MASTERBUFFER, n < SECTOR_SIZE ? n : SECTOR_SIZE);
    }
}

void write_sector(unsigned int c, unsigned int s, unsigned char *buffer)
{
    if(check_cs(c,s)) {
        memcpy(MASTERBUFFER, buffer, SECTOR_SIZE);
        goto_sector(c,s);
        _out(HDA_CMDREG,CMD_WRITE);
        _sleep(HDA_IRQ); /* attente de HDA_IRQ */
    }
}

void write_sector_n(unsigned int c, unsigned int s, unsigned char *buffer,
                    unsigned int n)
{
    if(check_cs(c,s)) {
        memcpy(MASTERBUFFER, buffer, n < SECTOR_SIZE ? n : SECTOR_SIZE);
        goto_sector(c,s);
        _out(HDA_CMDREG, CMD_WRITE);
        _sleep(HDA_IRQ); /* attente de HDA_IRQ */
    }
}

void format_sector(unsigned int c, unsigned int s, unsigned int nsector,
                   unsigned int value)
{
    if(check_cs(c,s)) {
        nsector = nsector > SECTOR_SIZE ? SECTOR_SIZE : nsector;
        goto_sector(c,s);
        _out(HDA_DATAREGS, (nsector >> 8) & 0xFF);
        _out(HDA_DATAREGS+1, nsector&0xFF);

        _out(HDA_DATAREGS+2, (value >> 24) & 0xFF);
        _out(HDA_DATAREGS+3, (value >> 16) & 0xFF);
        _out(HDA_DATAREGS+4, (value >> 8) & 0xFF);
        _out(HDA_DATAREGS+5, value & 0xFF);

        _out(HDA_CMDREG, CMD_FORMAT);
        _sleep(HDA_IRQ); /* attente de HDA_IRQ */
    }
}


void goto_sector(unsigned int c, unsigned int s)
{
    /* Deplacer la tête de lecture */
    _out(HDA_DATAREGS, (c >> 8) & 0xFF);    /* ecriture du poid fort */
    _out(HDA_DATAREGS+1, c&0xFF);           /* ecriture du poid faible */
    _out(HDA_DATAREGS+2, (s>>8) & 0xFF);
    _out(HDA_DATAREGS+3, s&0xFF);
    _out(HDA_CMDREG, CMD_SEEK);
    _sleep(HDA_IRQ); /* attente de HDA_IRQ */
}


void init_master() {
    unsigned int i;
    char *c, HW_CONFIG[100];

    if((c = getenv("HW_CONFIG"))==NULL)
        strcpy(HW_CONFIG, "hardware.ini");
    else
        strcpy(HW_CONFIG,c);
    if(init_hardware(HW_CONFIG) == 0) {
        fprintf(stderr, "Error in hardware initialization\n");
        exit(EXIT_FAILURE);
    }
    /* Interreupt handlers */
    for(i=0; i<16; i++)
        IRQVECTOR[i] = empty_it;

    /* Allows all IT */
    _mask(1);
}

void check_disk()
{

    int sector_size, sector_max, cylindre_max, error;
    error = 0;
    _out(HDA_CMDREG,CMD_DSKINFO);

    /* Lecture des informations du disque virtuel */
    sector_size  = (_in(HDA_DATAREGS+4)<<8 | (_in(HDA_DATAREGS+5)&0xFF));
    sector_max = (_in(HDA_DATAREGS+2)<<8 | (_in(HDA_DATAREGS+3)&0xFF));
    cylindre_max = (_in(HDA_DATAREGS)<<8 | (_in(HDA_DATAREGS+1)&0xFF));

    /*printf("taille d'un secteur : \t\t%i\n", sector_size);
    printf("nombre maximum de cylindres :\t%i\n", cylindre_max);
    printf("nombre maximum de secteurs :\t%i\n\n", sector_max);*/

    if(sector_size!=SECTOR_SIZE) {
        printf("Mauvaise valeur pour la taille du secteur (%i)\n",
               SECTOR_SIZE);
        error = 1;
    }
    if(sector_max!=MAX_SECTOR) {
        printf("Mauvaise valeur pour le nombre max de secteur (%i)\n",
               MAX_SECTOR);
        error = 1;
    }
    if(cylindre_max!=MAX_CYLINDER)  {
        printf("Mauvaise valeur pour le nombre max de cylindre (%i)\n",
               MAX_CYLINDER);
        error = 1;
    }

    if(error) {
        printf("Veuillez recompiler avec les bonnes valeurs\n");
        exit(EXIT_FAILURE);
    }

}

/*!
 * \brief check_cs verifie que le cylindre et le secteur sont correctes
 * \param c le numero de cylindre
 * \param s le numero de secteur
 * \return
 */
unsigned int check_cs(unsigned int c, unsigned int s)
{
    return  (c < MAX_CYLINDER && s < MAX_SECTOR);
}
