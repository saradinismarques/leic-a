# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc     # compiler to use

CFLAGS = -lm -g -Wall
OBJ = FS_TCP_Server.c FS_UDP_Client.c FS_main.c

all: fs

fs:
	@$(CC) $(CFLAGS) $(OBJ) -o ../fs

clean:
	@echo "Cleaning /FS/..." 
	@rm -rvf ../fs %.o 