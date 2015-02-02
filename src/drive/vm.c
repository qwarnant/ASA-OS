#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tools.h>
#include "mbr.h"
#include "super.h"
#include "drive.h"
#include "dir.h"
#include "ifile.h"
#include "yield.h"

#define CURRENT_VOLUME 0U

/* ------------------------------
 command list
 ------------------------------------------------------------*/
struct _cmd {
	char *name;
	void (*fun)(struct _cmd *c);
	char *comment;
};
static void dump(unsigned char *buffer, unsigned int buffer_size,
		int ascii_dump, int octal_dump);
static void list(struct _cmd *c);
static void new(struct _cmd *c);
static void del(struct _cmd *c);
static void cd(struct _cmd *c);
static void ls(struct _cmd *c);
static void cat(struct _cmd *c);
static void touch(struct _cmd *c);
static void mkdir(struct _cmd *c);
static void rmdir(struct _cmd *c);
static void compute(struct _cmd *c);
static void top(struct _cmd *c);

static void frmt(struct _cmd *c);
static void mknfs(struct _cmd *c);
static void dfs(struct _cmd *c);
static void dmps(struct _cmd *c);

static void help(struct _cmd *c);
static void save(struct _cmd *c);
static void quit(struct _cmd *c);
static void xit(struct _cmd *c);
static void none(struct _cmd *c);

static struct _cmd commands[] =
		{ { "list", list, "display the partition table" }, { "new", new,
				"create a new partition" },
				{ "del", del, "delete a partition" },
				{ "compute", compute, "" },
                { "top", top, "display all process" },
				/* Directory management */
				{ "ls", ls, "listing the current directory" }, { "cd", cd,
						"change the current directory" }, { "mkdir", mkdir,
						"create a new directory" }, { "rmdir", rmdir,
						"delete a directory" },
				{ "cd", cd,
						"change the current directory to the specified directory" },
				{ "mkdir", mkdir,
						"create a new directory with the specified name" }, {
						"rmdir", rmdir,
						"remove a existing directory with the specified name" },
				/* File management */
				{ "cat", cat,
						"print the content of a specified file on the standard output" },

				{ "touch", touch,
						"create a new empty file with the specified name" },

				{ "frmt", frmt, "formatting disk" }, { "mknfs", mknfs,
						"make new filesystem" }, { "dfs", dfs,
						"display filesystem" }, { "dmps", dmps,
						"display sector content" },

				{ "save", save, "save the MBR" }, { "quit", quit,
						"save the MBR and quit" }, { "exit", xit,
						"exit (without saving)" }, { "help", help,
						"display this help" }, { 0, none,
						"unknown command, try help" } };

/* ------------------------------
 dialog and execute
 ------------------------------------------------------------*/

static void execute(const char *name) {
	struct _cmd *c = commands;

	while (c->name && strcmp(name, c->name))
		c++;
	(*c->fun)(c);
}

static void loop(void) {
	char name[64];
	char temp[64];
    int status;

	while (printf("%s> ", CURRENT_DIRECTORY), scanf("%62s", name) == 1) {
		if (name[0] == '&') {
			strncpy(temp, name + 1, 63);
			status = create_ctx(STACK_WIDTH, execute, temp);

            if(status == RETURN_FAILURE) {
                fprintf(stderr, "Failed to create context : %s\n", name);
                return;
            }

			printf("%s %d\n", temp, status);
		} else {
			execute(name);
		}
	}

	printf("finished\n");
}

static void compute(struct _cmd *c) {
	long i;
	long j;
	for (i = 0; i < 10000000; i++) {
		pow(i, j);
		j++;
		j %= 1000;
	}
	printf("job finished\n");
}

/* ------------------------------
 command execution
 ------------------------------------------------------------*/
static void list(struct _cmd *c) {
	display_mbr();
}

static void top(struct _cmd *c) {
    struct ctx_s tmp = *ctx_ring;
    char state_name[4];
    printf("PID\tEBP\t\tESP\t\tSTATE\t\tSTART\t\tUPTIME\n");
    do {
        get_state_name(tmp.ctx_state, state_name);
        printf("%d\t%p\t%p\t%s\t\t%d\t%d\n", tmp.ctx_id, tmp.ctx_ebp, tmp.ctx_esp, state_name, tmp.ctx_start_time, tmp.ctx_exec_time);
        printf("%p\n", tmp.ctx_next);

        tmp = *tmp.ctx_next;
    } while(tmp.ctx_id != ctx_ring->ctx_id);
}

static void new(struct _cmd *c) {

	unsigned int fc, fs, size, nsector;

	printf("Premier cylindre : ");
	(void) scanf("%u", &fc);

	printf("Premier secteur : ");
	(void) scanf("%u", &fs);

	do {
		printf("taille(>0) : ");
	} while (scanf("%u", &size) && (size <= 0));

	nsector = ceil((double) size / SECTOR_SIZE);
	/* pas de creation de partition dans le MBR et au-dela du disque */
	if (!check_cs(fc, fs) || get_block(fc,fs) <= 0
	|| get_block(fc,fs+nsector) >
	MAX_CYLINDER*MAX_SECTOR) {fprintf(stderr, "Vous ne pouvez pas creer "
	"de partiton a cet emplacement.\n")
		;
	} else {
		if (add_vol(fc, fs, size) != -1) {
			printf("Partion creee\n");
		} else {
			fprintf(stderr, "Une erreur est survenue lors de la creation de "
					"la partition.\n");
		}
	}
}

static void del(struct _cmd *c) {
	int i;
	printf("Numero de partition : ");
	(void) scanf("%i", &i);
	if (rmvol(i) != -1) {
		printf("la partition %i a ete supprimee\n", i);
	} else {
		printf("la partition %i n'est pas presente\n", i);
	}
}

static void save(struct _cmd *c) {
	save_mbr();
	printf("Le MBR a ete sauvegarde\n");
}

static void mknfs(struct _cmd *c) {
	char *cv, ch;
	unsigned int current_vol;
	if ((cv = getenv("CURRENT_VOLUME")) != NULL ) {
		current_vol = (unsigned int) atoi(cv);
	} else {
		current_vol = CURRENT_VOLUME;
	}
	if (check_vol(current_vol)) {
		if (!load_super(current_vol)) {
			printf("Un systeme de fichier est deja present sur cette partition,"
					"voulez-vous continuer ? [o/N] : ");
			(void) scanf(" %c", &ch);
			if (ch == 'o' || ch == 'O')
				init_super(current_vol);
		} else {
			init_super(current_vol);
		}

		init_file_system();
		save_super();

	} else
		printf("Impossible de creer un systeme de fichier pour"
				" la partition %u.\n", current_vol);

}

static void dfs(struct _cmd *c) {
	display_filesystem();
}

static void frmt(struct _cmd *c) {
	unsigned int cy, s, v;
	v = 0;
	printf("formatage du disque en cours...\n");
	fflush(stdout);
	for (cy = 0; cy < MAX_CYLINDER; cy++)
		for (s = 0; s < MAX_SECTOR; s++)
			format_sector(cy, s, SECTOR_SIZE, v);
	printf("fin du formatage.\n");
}

static void dmps(struct _cmd *c) {
	unsigned char buffer[SECTOR_SIZE];
	unsigned int cy, s, size;

	memset(buffer, 0, SECTOR_SIZE);

	do {
		printf("Cylindre (max %i) : ", MAX_CYLINDER - 1);
	} while (scanf("%u", &cy) && cy >= MAX_CYLINDER);

	do {
		printf("Secteur (max %i) : ", MAX_SECTOR - 1);
	} while (scanf("%u", &s) && s >= MAX_SECTOR);

	printf("size (max %i) : ", SECTOR_SIZE);
	(void) scanf("%u", &size);

	if (size <= 0 || size > 512)
		size = SECTOR_SIZE;

	read_sector_n(cy, s, buffer, size);
	dump(buffer, size, 1, 1);

}

static void quit(struct _cmd *c) {
	save_mbr();
	exit(EXIT_SUCCESS);
}

static void do_xit() {
	exit(EXIT_SUCCESS);
}

static void xit(struct _cmd *dummy) {
	do_xit();
}

static void help(struct _cmd *dummy) {
	struct _cmd *c = commands;

	for (; c->name; c++)
		printf("%s\t-- %s\n", c->name, c->comment);
}

static void none(struct _cmd *c) {
	printf("%s\n", c->comment);
}

/* dump buffer to stdout,
 and octal dump if octal_dump; an ascii dump if ascii_dump! */
static void dump(unsigned char *buffer, unsigned int buffer_size,
		int ascii_dump, int octal_dump) {
	unsigned int i, j;

	for (i = 0; i < buffer_size; i += 16) {
		/* offset */
		printf("%.8o", i);

		/* octal dump */
		if (octal_dump) {
			for (j = 0; j < 8; j++)
				printf(" %.2x", buffer[i + j]);
			printf(" - ");

			for (; j < 16; j++)
				printf(" %.2x", buffer[i + j]);

			printf("\n");
		}
		/* ascii dump */
		if (ascii_dump) {
			printf("%8c", ' ');

			for (j = 0; j < 8; j++)
				printf(" %1c ", isprint(buffer[i+j]) ? buffer[i + j] : ' ');
			printf(" - ");

			for (; j < 16; j++)
				printf(" %1c ", isprint(buffer[i+j]) ? buffer[i + j] : ' ');

			printf("\n");
		}

	}
}

static void cd(struct _cmd *c) {
	unsigned int current_dir_inumber = inumber_of_path(CURRENT_DIRECTORY);
	file_desc_t fd;
	int ientry = 0, inumber = 0; /* the entry index */
	struct entry_s entry;
	bool_t findEntry = FALSE;
	int status;
	char pathname[ENTRYMAXLENGTH];

	/* Get the pathname */
	(void) scanf("%s", pathname);

	if (pathname[0] == '.' && pathname[1] == '.') {
		char *token;
		char *lastvalue;
		char newDir[256] = "";

		token = strtok(CURRENT_DIRECTORY, "/");
		if (token != NULL ) {

			while (token != NULL ) {
				strcat(newDir, "/");
				lastvalue = strdup(token);

				token = strtok(NULL, "/");
				if (token != NULL ) {
					strcat(newDir, lastvalue);
				}
			}

			strcpy(CURRENT_DIRECTORY, newDir);
		} else {
			/*
			 * we are in the root folder, no ".." allowed
			 */
			load_file_system_root();
		}
		return;
	}

	/* open ifile */
	status = open_ifile(&fd, current_dir_inumber);
	ffatal(!status, "erreur ouverture fichier %d", inumber);

	/* seek to begin of dir */
	seek2_ifile(&fd, 0);

	/* look after the right entry */
	while (read_ifile(&fd, &entry, sizeof(struct entry_s)) != READ_EOF) {
		if (entry.ent_inumber && strcmp(entry.ent_basename, pathname) == 0) {
			findEntry = TRUE;
			break;
		}
		ientry++;
	}

	if (findEntry == FALSE) {
		fprintf(stderr, "Error no file or directory for : %s\n", pathname);
		return;
	}

	if (strcmp(CURRENT_DIRECTORY, "/") != 0) {
		strcat(CURRENT_DIRECTORY, "/");
	}

	strcat(CURRENT_DIRECTORY, pathname);
}
static void ls(struct _cmd *c) {
	unsigned int current_dir_inumber = inumber_of_path(CURRENT_DIRECTORY);
	file_desc_t fd;
	struct entry_s entry;
	unsigned int ientry = 0, inumber = 0; /* the entry index */
	int status;

	printf("Current directory : %s\n", CURRENT_DIRECTORY);

	/* open ifile */
	status = open_ifile(&fd, current_dir_inumber);
	ffatal(!status, "erreur ouverture fichier %d", inumber);

	/* seek to begin of dir */
	seek2_ifile(&fd, 0);
	/* look after the right entry */
	while (read_ifile(&fd, &entry, sizeof(struct entry_s)) != READ_EOF) {
		if (entry.ent_inumber == 0)
			continue; /* skip empty inode */

		printf("i:%d\t n: %s\n", entry.ent_inumber, entry.ent_basename);
		ientry++;
	}

	printf("Number of entry read : %d\n", ientry);
}
static void cat(struct _cmd *c) {
	file_desc_t fd;
	int status, car;
	unsigned inumber;
	char pathname[ENTRYMAXLENGTH];

	(void) scanf("%s", pathname);

	inumber = inumber_of_path(pathname);

	status = open_ifile(&fd, inumber);
	ffatal(!status, "erreur ouverture fichier %d", inumber);

	while ((car = readc_ifile(&fd)) != READ_EOF)
		putchar(car);

	close_ifile(&fd);
}

static void mkdir(struct _cmd *c) {
	unsigned int inumber, current_dir_inumber;
	char dirname[ENTRYMAXLENGTH];
	int status;

	/* Get the pathname for the new directory */
	(void) scanf("%s", dirname);

	inumber = create_ifile(directory);
	current_dir_inumber = inumber_of_path(CURRENT_DIRECTORY);

	status = add_entry(current_dir_inumber, inumber, dirname);
	ffatal(!status, "Error when creating the new directory : %d", inumber);
}

static void touch(struct _cmd *c) {
	file_desc_t fd;
	unsigned int inumber, current_dir_inumber;
	int status;
	int car;
	char pathname[ENTRYMAXLENGTH];

	(void) scanf("%s", pathname);

	inumber = create_ifile(ordinary);
	ffatal(inumber, "erreur creation fichier");

	current_dir_inumber = inumber_of_path(CURRENT_DIRECTORY);

	/* Add the new entry in the current directory */
	status = add_entry(current_dir_inumber, inumber, pathname);
	ffatal(!status, "Error when creating the new file");

	status = open_ifile(&fd, inumber);
	ffatal(!status, "erreur ouverture fichier %d", inumber);

	/* Fill the new created file */
	car = getchar();
	/* just to get the first value and not reading it,  touch name_file text, it will not take the space
	 between the name_file and the text, so the file will not contains the return line or space*/

	while ((car = getchar()) != EOF) {
		if (writec_ifile(&fd, car) == RETURN_FAILURE) {
			fprintf(stderr,
					"Il n'y a plus de place pour ecrire sur cette partition.\n");
			break;
		}
	}

	close_ifile(&fd);
}

static void rmdir(struct _cmd *c) {
	unsigned int current_dir_inumber, inumber;
	int status;
	char dirname[ENTRYMAXLENGTH];

	(void) scanf("%s", dirname);

	/* Get the inode numbers of directories */
	current_dir_inumber = inumber_of_path(CURRENT_DIRECTORY);
	inumber = inumber_of_path(dirname);

	/* Remove the directory entry */
	status = del_entry(current_dir_inumber, dirname);

	/* Remove the directory file itself */
	status = delete_ifile(inumber);
	ffatal(!status, "Error when removing the ifile %d", inumber);
}

int main(int argc, char **argv) {
	/* Verification du disque et du MBR */
	init_master();
	check_disk();
	load_mbr();
	load_file_system_root();
	/* dialog with user */
	create_ctx(STACK_WIDTH, loop, NULL );

	/* Set up the IRQ system */
	setup_irq(TIMER_IRQ, yield);
	start_hw();

	/* Start the context scheduler */
	yield();

	/* abnormal end of dialog (cause EOF for xample) */
	do_xit();

	/* make gcc -W happy */
	exit(EXIT_SUCCESS);
}
