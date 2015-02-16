CC	= gcc
LINK 	= gcc
CFLAGS  = -m32 -Wall -g
INCPATH	= -I. -I$(IDIR)
LDFLAGS = -m32
LIBS	= -lhardware -lm -lpthread
RM      = rm -f

BIN	= core prodcons

SRCDIR	= src
COREDIR = core
IDIR 	= include
ODIR	= obj
LIBDIR 	= lib
BINDIR	= bin

###------------------------------
### Main targets 
###------------------------------------------------------------

SOURCES	= $(wildcard $(SRCDIR)/*.c)
DEPS 	= $(wildcard $(IDIR)/*.h)
_OBJ	= $(patsubst %.c, %.o, $(SOURCES))
OBJ 	= $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(_OBJ))
BINARIES= $(addprefix $(BINDIR)/,$(BIN))
OBJECTS = $(addprefix $(ODIR)/,\
		init.o irq.o sem.o yield.o)

all :    $(BINARIES)


###------------------------------
### Binaries
###-----------------------------------------------------------
$(BINDIR)/core	: $(ODIR)/core.o $(OBJECTS)
$(BINDIR)/prodcons : $(ODIR)/prodcons.o $(OBJECTS)

$(BINDIR)/% : $(ODIR)/%.o
	$(CC) -o $@ $^ -L$(LIBDIR) $(LDFLAGS) $(LIBS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean mrproper realclean
clean:
	rm -rf obj/* bin/*
mrproper:
	rm -rf $(OBJ) $(OBJECTSIF) rm $(BINARIES) $(BINARIESIF) vdiskA.bin vdiskB.bin
realclean: clean 
	$(RM) vdiskA.bin vdiskB.bin

