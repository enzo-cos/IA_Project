#include "fonctionsTCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>


/*
 **********************************************************
 *  fonction  : socketServeur (numero de port)
 *
 *  resume :    creer la socket du serveur et la retourne
 *  ***********************************************************
 */
int socketServeur(ushort port){
    struct sockaddr_in addServ;	/* adresse socket connex serveur */


    int sockConx;        /* descripteur socket connexion */
    int err=0; 
    int sizeAddr;       /* taille de l'adresse d'une socket */

    /* 
   * creation de la socket, protocole TCP 
   */
  sockConx = socket(AF_INET, SOCK_STREAM, 0);
  if (sockConx < 0) {
    perror("(serveurTCP) erreur de socket");
    return -2;
  }
  
  /* 
   * initialisation de l'adresse de la socket 
   */
  addServ.sin_family = AF_INET;
  addServ.sin_port = htons(port); // conversion en format réseau (big endian)
  addServ.sin_addr.s_addr = INADDR_ANY; 
  // INADDR_ANY : 0.0.0.0 (IPv4) donc htonl inutile ici, car pas d'effet
  bzero(addServ.sin_zero, 8);
  
  sizeAddr = sizeof(struct sockaddr_in);

  /* 
   * attribution de l'adresse a la socket
   */  
  err = bind(sockConx, (struct sockaddr *)&addServ, sizeAddr);
  if (err < 0) {
    perror("(serveurTCP) erreur sur le bind");
    close(sockConx);
    return -3;
  }
  
  /* 
   * utilisation en socket de controle, puis attente de demandes de 
   * connexion.
   */
  err = listen(sockConx, 1);
  if (err < 0) {
    perror("(serveurTCP) erreur dans listen");
    close(sockConx);
    return -4;
  }
  
  return sockConx;
} 

/*
 **********************************************************
 *  fonction : socketClient( nom de machine serveur, 
 *                           numero de port serveur )
 *  resume :   fonction de connexion d'une socket au serveur     
 ***********************************************************
 */

int socketClient(char *nomMachine, ushort port){
  int sock;
  int err;
  struct sockaddr_in addSockServ;  
  socklen_t sizeAdd;       /* taille d'une structure pour l'adresse de socket */
    struct addrinfo *result; /* les adresses obtenues par getaddrinfo */ 
   struct addrinfo hints;   /* parametre pour getaddrinfo */


  /*
   * creation d'une socket, domaine AF_INET, protocole TCP 
   */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("(client) erreur sur la creation de socket");
    return -2;
  }

  /* 
   *  initialisation de l'adresse de la socket - version getaddrinfo
   */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; // AF_INET / AF_INET6 
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  
  
  // récupération de la liste des adresses corespondante au serveur
  char pstr[8];
  sprintf(pstr, "%d", port);
  err = getaddrinfo(nomMachine, pstr, &hints, &result);
  if (err != 0) {
    perror("(client) erreur sur getaddrinfo");
    close(sock);
    return -3;
  }
  
  addSockServ = *(struct sockaddr_in*) result->ai_addr;
  sizeAdd = result->ai_addrlen;
			     
  /* 
   * connexion au serveur 
   */
  err = connect(sock, (struct sockaddr *)&addSockServ, sizeAdd); 

  if (err < 0) {
    perror("(client) erreur a la connection de socket");
    close(sock);
    return -4;
  }

  return sock;
}
