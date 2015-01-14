CC	= gcc
LINK 	= gcc
CFLAGS  = -pipe -m32 -Wall -Wextra -ansi -pedantic -std=iso9899:1999 -g -W -fPIE 
INCPATH	= -I. -I$(IDIR)
LDFLAGS = -m32
LIBS	= -lhardware -lm
RM      = rm -f

BIN	= dmps frmt mkvol dvol rmvol vm mknfs dfs test
BINIF	= if_cfile if_nfile if_pfile if_dfile

SRCDIR	= src
IDIR 	= include
ODIR	= obj
LIBDIR 	= lib
BINDIR	= bin
IFILE   = ifile

###------------------------------
### Main targets 
###------------------------------------------------------------

SOURCES	= $(wildcard $(SRCDIR)/*.c)
_OBJ	= $(patsubst %.c, %.o, $(SOURCES))
OBJ 	= $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(_OBJ))
DEPS 	= $(wildcard $(IDIR)/*.h)

BINARIES= $(addprefix $(BINDIR)/,$(BIN))
OBJECTS = $(addprefix $(ODIR)/,\
		inode.o drive.o mbr.o super.o)

all :   $(OBJ) $(BINARIES) ifile


###------------------------------
### Binaries
###-----------------------------------------------------------

$(BINDIR)/mkvol	: $(ODIR)/mkvol.o $(OBJECTS)
$(BINDIR)/rmvol	: $(ODIR)/rmvol.o $(OBJECTS)
$(BINDIR)/mknfs	: $(ODIR)/mknfs.o $(OBJECTS)
$(BINDIR)/dmps	: $(ODIR)/dmps.o $(OBJECTS)
$(BINDIR)/frmt	: $(ODIR)/frmt.o $(OBJECTS)
$(BINDIR)/test	: $(ODIR)/test.o $(OBJECTS)
$(BINDIR)/dvol	: $(ODIR)/mbr.o $(OBJECTS)
$(BINDIR)/dfs	: $(ODIR)/dfs.o $(OBJECTS)
$(BINDIR)/vm	: $(ODIR)/vm.o $(OBJECTS)

$(BINDIR)/% : $(ODIR)/%.o
	$(CC) -o $@ $^ -L$(LIBDIR) $(LIBS)


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

###------------------------------
### ifile
###------------------------------------------------------------

OBJIF      = $(addprefix $(ODIR)/,\
		tools.o mount.o ifile.o)
BINARIESIF = $(addprefix $(BINDIR)/,$(BINIF))
OBJECTSIF  = $(addprefix $(ODIR)/,\
		tools.o mount.o ifile.o if_cfile.o if_pfile.o if_dfile.o if_nfile.o dir.o)

ifile : $(OBJIF) $(BINARIESIF)

$(BINDIR)/if_cfile : $(ODIR)/if_cfile.o $(OBJECTS) $(OBJIF)
$(BINDIR)/if_nfile : $(ODIR)/if_nfile.o $(OBJECTS) $(OBJIF)
$(BINDIR)/if_dfile : $(ODIR)/if_dfile.o $(OBJECTS) $(OBJIF)
$(BINDIR)/if_pfile : $(ODIR)/if_pfile.o $(OBJECTS) $(OBJIF)


INC	= $(addprefix $(IFILE)/,\
	  ifile.h inode.h tools.h mount.h dir.h)

$(ODIR)/if_cfile.o: $(IFILE)/if_cfile.c $(INC)
$(ODIR)/if_dfile.o: $(IFILE)/if_dfile.c $(INC)
$(ODIR)/if_nfile.o: $(IFILE)/if_nfile.c $(INC)
$(ODIR)/if_pfile.o: $(IFILE)/if_pfile.c $(INC)
$(ODIR)/ifile.o: $(IFILE)/ifile.c $(INC)
$(ODIR)/mount.o: $(IFILE)/mount.c $(INC)
$(ODIR)/tools.o: $(IFILE)/tools.c $(INC)
$(ODIR)/dir.o: $(IFILE)/dir.c $(INC)

$(BINDIR)/% : $(ODIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ -L$(LIBDIR) $(LIBS)

$(ODIR)/%.o: $(IFILE)/%.c $(DEPS) $(INC)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<



###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean mrproper realclean
clean:
	rm -rf $(OBJ)	
mrproper:
	rm -rf $(OBJ) $(OBJECTSIF) rm $(BINARIES) $(BINARIESIF) $(BINDIR)/vdiskA.bin $(BINDIR)/vdiskB.bin
realclean: clean 
	$(RM) $(BINDIR)/vdiskA.bin $(BINDIR)/vdiskB.bin

