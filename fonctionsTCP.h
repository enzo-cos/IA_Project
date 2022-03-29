

#ifndef FONCTIONS_TCP_H
#define FONCTIONS_TCP_H

/* include generaux */
#include <sys/types.h>


int socketServeur(ushort port);


int socketClient(char *nomMachine, ushort port);

#endif
