#########
#
# Makefile pour le projet d'IA
#
#########
LD_FLAGS =

all: serveur joueur joueur2 truePlayer

serveur: serveur.c fonctionsTCP.o fctServ.o colonne.o
	gcc -Wall serveur.c -o serveur fonctionsTCP.o fctServ.o colonne.o $(LD_FLAGS)

truePlayer: truePlayer.c fonctionsTCP.o fctServ.o
	gcc -Wall truePlayer.c -o truePlayer fonctionsTCP.o fctServ.o $(LD_FLAGS)

joueur: joueur.c fonctionsTCP.o fctServ.o
	gcc -Wall joueur.c -o joueur fonctionsTCP.o fctServ.o $(LD_FLAGS)

joueur2: joueur2.c fonctionsTCP.o fctServ.o
	gcc -Wall joueur2.c -o joueur2 fonctionsTCP.o fctServ.o $(LD_FLAGS)

fonctionsTCP.o: fonctionsTCP.c
	gcc -Wall fonctionsTCP.c -o fonctionsTCP.o -c  $(LD_FLAGS)

fctServ.o : fctServ.c
	gcc -Wall fctServ.c -o fctServ.o -c  $(LD_FLAGS)

validation.o : validation.c
	gcc -Wall validation.c -o validation.o -c  $(LD_FLAGS)

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm serveur