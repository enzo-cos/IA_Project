#########
#
# Makefile pour le projet d'IA
#
#########
LD_FLAGS =

all: serveur

serveur: serveur.c fonctionsTCP.o fctServ.o
	gcc -Wall serveur.c -o serveur fonctionsTCP.o fctServ.o $(LD_FLAGS)

fonctionsTCP.o: fonctionsTCP.c
	gcc -Wall fonctionsTCP.c -o fonctionsTCP.o -c  $(LD_FLAGS)

fctServ.o : fctServ.c
	gcc -Wall fctServ.c -o fctServ.o -c  $(LD_FLAGS)

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm serveur
