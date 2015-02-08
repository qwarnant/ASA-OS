#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "drive.h"
#include "mbr.h"
#include "super.h"
#include "tools.h"

#define NNBPB (HDA_SECTORSIZE/sizeof(int))

static unsigned char clear[HDA_SECTORSIZE];

static unsigned int get_vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc,
                            bool_t do_allocate)
{
    struct inode_s inode;
    unsigned int position, blocs[NNBPB], buf[NNBPB];

    read_inode(inumber, &inode);

    if(fbloc < N_DIRECT_BLOCS) { /* de 0 à 1 */
        if(do_allocate && !inode.ind_direct[fbloc]) {
            inode.ind_direct[fbloc] = new_bloc();
            write_inode(inumber,&inode);
        }
        return inode.ind_direct[fbloc];

    } else if(fbloc < (NNBPB + N_DIRECT_BLOCS)) { /* de 2 à NNBPB+2 */
        fbloc -= N_DIRECT_BLOCS;

        if(!do_allocate && !inode.ind_indirect) {
            return 0;
        }

        if(!inode.ind_indirect) {
            inode.ind_indirect = new_bloc();
            write_inode(inumber,&inode);
            if(inode.ind_indirect)
                write_bloc(current_volume, inode.ind_indirect, clear);
            else
                return 0;
        }

        read_bloc(current_volume, inode.ind_indirect, (unsigned char *)blocs);

        if(do_allocate && !blocs[fbloc]) {
            if((blocs[fbloc] = new_bloc()))
                write_bloc(current_volume, inode.ind_indirect, (unsigned char *)blocs);
        }

        return blocs[fbloc];

    } else if (fbloc < NNBPB*NNBPB + NNBPB + N_DIRECT_BLOCS) { /* au max*/
        fbloc -= NNBPB+N_DIRECT_BLOCS;

        if(!do_allocate && !inode.ind_d_indirect)
            return 0;

        if(!inode.ind_d_indirect) {
            inode.ind_d_indirect = new_bloc();
            write_inode(inumber,&inode);
            if(inode.ind_d_indirect)
                write_bloc(current_volume, inode.ind_d_indirect, clear);
            else
                return 0;
        }

        read_bloc(current_volume, inode.ind_d_indirect, (unsigned char *)blocs);

        position = (double)fbloc/NNBPB;
        fbloc %= NNBPB;

        if(!do_allocate && !blocs[position])
            return 0;


        if(!blocs[position]) {
            blocs[position] = new_bloc();
            write_bloc(current_volume, inode.ind_d_indirect, (unsigned char *)blocs);
            if(blocs[position])
                write_bloc(current_volume, blocs[position], clear);
            else
                return 0;
        }

        read_bloc(current_volume, blocs[position], (unsigned char *)buf);

        if(do_allocate && !buf[fbloc]) {
            if((buf[fbloc] = new_bloc()))
                write_bloc(current_volume, blocs[position], (unsigned char *)buf);
            else
                return 0;
        }

        return buf[fbloc];

    } else {
        return 0; /* Erreur ecriture/lecture au-dela de la capacité maximale */
    }
    return 0;

}

void read_inode(unsigned int inumber, struct inode_s *inode)
{
    read_bloc_n(current_volume, inumber, (unsigned char *)inode,
                sizeof(struct inode_s));
}

void write_inode(unsigned int inumber, struct inode_s *inode)
{
    write_bloc_n(current_volume, inumber, (unsigned char *)inode,
                 sizeof(struct inode_s));
}

unsigned int create_inode(enum file_type_e type)
{
    unsigned int inumber;
    struct inode_s inode;    printf("test3\n");

    memset(&inode, 0, sizeof(struct inode_s));
    inode.ind_type = type;

    inumber = new_bloc();

    if(!inumber) /*disque plein*/
        printf("Disque plein\n");
    else
        write_inode(inumber, &inode);

    return inumber;
}

unsigned int delete_inode(unsigned int inumber)
{
    unsigned int i;
    unsigned int blocs[NNBPB],buf[NNBPB];
    struct inode_s inode;

    read_inode(inumber, &inode);
    free_blocs(inode.ind_direct, N_DIRECT_BLOCS);

    if(inode.ind_indirect) {
        read_bloc(current_volume, inode.ind_indirect, (unsigned char*)blocs);
        free_blocs(blocs, NNBPB);
        free_bloc(inode.ind_indirect);
    }

    if(inode.ind_d_indirect) {
        read_bloc(current_volume, inode.ind_d_indirect, (unsigned char*)blocs);
        for(i = 0; i < NNBPB; i++) {
            if(blocs[i]) {
                read_bloc(current_volume, blocs[i], (unsigned char*)buf);
                free_blocs((unsigned int*)buf, NNBPB);
            }
        }

        free_blocs(blocs, NNBPB);
        free_bloc(inode.ind_d_indirect);
    }
    free_bloc(inumber);

    return 0;
}

unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc) {
	return get_vbloc_of_fbloc(inumber, fbloc, FALSE);
}

unsigned int allocate_vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc) {
	return get_vbloc_of_fbloc(inumber, fbloc, TRUE);
}

