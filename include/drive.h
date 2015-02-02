#ifndef DRIVE_H
#define DRIVE_H


#define ENABLE_HDA       1            /* ENABLE_HD  0 > simulation du disque désactivée */
#define HDA_FILENAME     "vdiskA.bin" /* nom du fichier de stockage du disque simulé */
#define HDA_CMDREG       0x3F6        /* registre de commande du disque maitre */
#define HDA_DATAREGS     0x110        /* base des registres de données (r,r+1,r+2,...r+7) */
#define HDA_IRQ          14           /* Interruption du disque */
#define HDA_MAXCYLINDER  1024           /* Nombre de pistes du disque maître */
#define HDA_MAXSECTOR    32           /* Nombre de secteurs du disque maître */
#define HDA_SECTORSIZE   512           /* Taille (en octet) d'un secteur du disque maître */
#define HDA_STPS         2            /* nombre de SYSTICK pour changer de secteur */
#define HDA_STPC         1            /* nombre de SYSTICK pour changer de piste */
#define HDA_PON_DELAY    30           /* nombre de SYSTICK avant amorce du disque */
#define HDA_POFF_DELAY   30           /* nombre de SYSTICK avant arret du disque */





void check_disk();
unsigned int check_cs(unsigned int c, unsigned int s);
void read_sector(unsigned int c, unsigned int s, unsigned char *buffer);
void read_sector_n(unsigned int c, unsigned int s, unsigned char *buffer,
                   unsigned int n);
void write_sector(unsigned int c, unsigned int s, unsigned char *buffer);
void write_sector_n(unsigned int c, unsigned int s, unsigned char *buffer,
                    unsigned int n);

void format_sector(unsigned int c, unsigned int s, unsigned int nsector,
                   unsigned int value);
void init_master();

#endif
