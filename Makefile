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
		inode.o drive.o mbr.o super.o tools.o ifile.o dir.o)

all :   $(OBJ) $(BINARIES)


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
	$(CC) -o $@ $^ -L$(LIBDIR) $(LDFLAGS) $(LIBS)


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
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

