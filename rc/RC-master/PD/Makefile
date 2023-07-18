# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc     # compiler to use

CFLAGS = -lm -g -Wall
OBJ = PD_client.c PD_server.c PD_main.c

all: pd

pd:
	@$(CC) $(CFLAGS) $(OBJ) -o ../pd

clean:
	@echo "Cleaning /PD/..." 
	@rm -rvf ../pd %.o 