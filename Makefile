#########
#
# Makefile pour le projet d'IA
#
#########
LD_FLAGS =

all: serveur truePlayer IA playerChoix

serveur: serveur.c fonctionsTCP.o colonne.o
	gcc -Wall serveur.c -o serveur fonctionsTCP.o colonne.o $(LD_FLAGS)

truePlayer: truePlayer.c fonctionsTCP.o
	gcc -Wall truePlayer.c -o truePlayer fonctionsTCP.o $(LD_FLAGS)

playerChoix: playerChoix.c fonctionsTCP.o
	gcc -Wall playerChoix.c -o playerChoix fonctionsTCP.o $(LD_FLAGS)

fonctionsTCP.o: fonctionsTCP.c
	gcc -Wall fonctionsTCP.c -o fonctionsTCP.o -c  $(LD_FLAGS)
	
IA : IA.java
	javac IA.java

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm serveur