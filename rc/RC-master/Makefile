
MAKEFLAGS += --no-print-directory
.PHONY = all clean


all: clean pd user as fs
cleanall: clean cleanexes

#compila a pasta do PD
pd:
	@(cd PD; make all)

#compila a pasta do USER
user:
	@(cd USER; make all)

as:
	@(cd AS; make all)

#compila a pasta do FS
fs:
	@(cd FS; make all)

#faz push do projecto para o github
push: 
	(git add .)
	(git commit -m "make git")
	(git push) 

cleanexes:
	@(cd FS; rm -rf -d USERS)
	@(cd AS; rm -rf -d USERS)

#limpa as pastas, deixa os .h e .c
clean:
	@echo "Cleaning up..."
	@(cd PD; make clean)
	@(cd USER; make clean)
	@(cd AS; make clean)
	@(cd FS; make clean)