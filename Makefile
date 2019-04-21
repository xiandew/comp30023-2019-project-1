# Xiande Wen, 905003

CC     = gcc
CFLAGS = -Wall -std=c99
# modify the flags here ^
EXE    = image_tagger
OBJ    = main.o list.o spell.o hashtbl.o
# add any new object files here ^

# top (default) target
all: $(EXE)

# how to link executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

# other dependencies
main.o: list.h spell.h
spell.o: spell.h list.h hashtbl.h
list.o: list.h
hashtbl.o: hashtbl.h

# ^ add any new dependencies here (for example if you add new modules)


# phony targets (these targets do not represent actual files)
.PHONY: clean cleanly all CLEAN

# `make clean` to remove all object files
# `make CLEAN` to remove all object and executable files
# `make cleanly` to `make` then immediately remove object files (inefficient)
clean:
	rm -f $(OBJ)
CLEAN: clean
	rm -f $(EXE)
cleanly: all clean
