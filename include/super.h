/* ------------------------------
   $Id: super.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   Super bloc management.
   Philippe Marquet, october 2002
   
*/

#ifndef _SUPER_H_
#define _SUPER_H_

/* the index of a bloc full of zeroes */
#define BLOC_NULL	0

struct super_bloc_s {
    unsigned int super_magic, super_first_free_bloc, super_n_free;
    char name[32];
    int id;
    unsigned super_root;
};

struct free_bloc_s {
    unsigned int fb_n_free, fb_next;  /* 0 = pas de suivant */
};

extern struct super_bloc_s current_super;
unsigned int current_volume; 

void init_super(unsigned int vol);
void display_super(unsigned int vol);

void display_filesystem();

/* return RETURN_FAILURE if the super bloc was not valid */
int load_super(unsigned int vol);

void save_super();

/* return RETURN_FAILURE if a super bloc is not loaded */
int super_loaded();

/* return BLOC_NULL in case of failure */
unsigned int new_bloc();

void free_bloc(unsigned int bloc);

/*
 * free_blocs()
 * This method dealloc an array of blocs and put all in the free bloc list
 */
void free_blocs(unsigned int*, unsigned int);

#endif
