FILE_NAME=proj2
CC=gcc
CFLAGS=-Wall -Wextra -Werror -ansi -pedantic -g

all:: proj2
	$(MAKE) $(MFLAGS) -C public
proj2: *.c *.h
clean::
	rm -f proj2 public/*.diff public/*.myout
