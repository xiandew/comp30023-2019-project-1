# Xiande Wen, 905003

CC     = gcc
CFLAGS = -Wall -std=c99
EXE    = image_tagger
OBJDIR = bin
OBJ    = $(addprefix $(OBJDIR)/, image_tagger.o response.o reg_match.o player.o)
DEPS   = response.h reg_match.h player.h

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
	rm -f -r $(OBJDIR)
CLEAN: clean
	rm -f $(EXE)
cleanly: all clean
