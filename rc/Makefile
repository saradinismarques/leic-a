.PHONY = all clean

all: server user

#compila a pasta do PD
server:
	@(cd SERVERAPP; make all)

#compila a pasta do USER
user:
	@(cd USERAPP; make all)

#limpa as pastas, deixa os .h e .c
clean:
	@echo "Cleaning up..."
	@(cd SERVERAPP; make clean)
	@(cd USERAPP; make clean)