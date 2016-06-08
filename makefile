#
# C/C++ makefile pour Windows/Linux
# par Vincent Fabioux
# Ne pas editer la partie Compilation à la fin!
#

# Compilateur choisi
CC = gcc

# Extension des fichiers sources
EXTENSION = .c

# Repertoire des fichiers sources et objets (doivent exister)
SRCPATH = src
OBJPATH = obj

# Options du compilateur
CFLAGS = -Wall

# Nom de l'executable
EXEC = projet

# Options de l'editeur de liens
ifeq ($(OS),Windows_NT)
	LDFLAGS =-lGL -lGLU -lglut -lm
else
	LDFLAGS =-lGL -lGLU -lglut -lm
endif


#
# Compilation du programme
# Ne pas modifier!
#
OBJS1 = $(wildcard $(SRCPATH)/*$(EXTENSION))
OBJS2 = $(OBJS1:$(EXTENSION)=.o)
OBJS3 = $(addprefix $(SRCPATH)/$(OBJPATH)/, $(notdir $(OBJS2)))
all: $(OBJS3)
	@ $(CC) -o $(EXEC) $^ $(LDFLAGS)
$(SRCPATH)/$(OBJPATH)/%.o: $(SRCPATH)/%$(EXTENSION)
	@ $(CC) $(CFLAGS) -o $@ -c $<
clean:
	@ rm -rf $(SRCPATH)/$(OBJPATH)/*.o
mrproper: clean
	@ rm -rf $(EXEC)
