#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include "fonctionsTCP.h"
#include "fctServ.h"
#include "protocolColonne.h"
#include "validation.h"


int main(int argc, char** argv) {
  int  sockConx,        /* descripteur socket connexion */
       sockTrans,       /* descripteur socket transmission */
       port,            /* numero de port */
       sizeAddr;        /* taille de l'adresse d'une socket */

  const int MAX_CL=2;

  struct sockaddr_in addClient;	/* adresse de la socket client connectee */

  
  /*
   * verification des arguments
   */
  if (argc != 2) {
    printf ("usage : %s port\n", argv[0]);
    return -1;
  }
  
  port  = atoi(argv[1]);

  sockConx=socketServeur(port);
  if(sockConx<1){ 
    return -1;
  }

  /*
  Pool de processus
  */
  pid_t pid;
  int myBool=0;
  int i=0;
  while(i < MAX_CL && myBool==0){
    pid=fork();
    if (pid == -1) {
		  perror("Erreur sur le fork ");
		  return -9;
    }
    if(pid==0) {
      myBool=1;
    }
    i++;
  }
  
while(1){
  
    /*
    * attente de connexion
    */
    sizeAddr = sizeof(struct sockaddr_in);
    sockTrans = accept(sockConx, 
                (struct sockaddr *)&addClient, 
                (socklen_t *)&sizeAddr);
    if (sockTrans < 0) {
        perror("(serveurTCP) erreur sur accept");
        return -5;
    }
    //Traitement du client 
}

    /* 
   * arret de la connexion et fermeture
   */
  shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
  close(sockConx);
  
  return 0;
}