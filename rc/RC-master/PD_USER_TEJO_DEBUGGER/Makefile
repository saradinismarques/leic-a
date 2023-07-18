
MAKEFLAGS += --no-print-directory
.PHONY = all clean


all: clean pd user

#compila a pasta do PD
pd:
	@(cd PD; make all)

#compila a pasta do USER
user:
	@(cd USER; make all)

#limpa as pastas, deixa os .h e .c
clean:
	@echo "Cleaning up..."
	@(cd PD; make clean)
	@(cd USER; make clean)