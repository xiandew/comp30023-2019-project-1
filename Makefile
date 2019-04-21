# Xiande Wen, 905003
CC     = gcc
CFLAGS = -Wall -std=c99
# modify the flags here ^
EXE    = image_tagger
OBJDIR   = bin
OBJ   = $(addprefix $(OBJDIR)/, image_tagger.o preg_match_all.o)
DEPS   = preg_match_all.h

# top (default) targets
all: $(EXE)

# how to link executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

$(OBJ): | $(OBJDIR)

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR):
	mkdir -p $@

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
