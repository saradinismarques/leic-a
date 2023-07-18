# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc     # compiler to use

CFLAGS = -lm -g -Wall
OBJ = USER.c

all: pd

pd:
	@$(CC) $(CFLAGS) $(OBJ) -o ../user

clean:
	@echo "Cleaning /USER/..."
	@rm -rvf ../user %.o 