#ifndef DRIVE_H
#define DRIVE_H

#define SECTOR_SIZE 512
#define MAX_SECTOR 16
#define MAX_CYLINDER 16

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
