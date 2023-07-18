# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc     # compiler to use

CFLAGS = -lm -g -Wall
OBJ = AS_UDP_Server.c AS_TCP_Server.c AS_UDP_Client.c AS_main.c

all: as

as:
	@$(CC) $(CFLAGS) $(OBJ) -o ../as

clean:
	@echo "Cleaning /AS/..." 
	@rm -rvf ../as %.o 