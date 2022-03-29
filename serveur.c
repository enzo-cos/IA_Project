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
       sockJoueur1,       /* descripteur socket Joueur */
       sockJoueur2,       /* descripteur socket Joueur  */
       port,            /* numero de port */
       sizeAddr1,         /* taille de l'adresse d'une socket */
       sizeAddr2;        /* taille de l'adresse d'une socket */

  //const int MAX_CL=2;

  struct sockaddr_in addJ1;	/* adresse de la socket client connectee */
  struct sockaddr_in addJ2;	/* adresse de la socket client connectee */

  fd_set readSet;              /* variable pour le select */
  int err=0;    //code erreur
  int nbCl=0;     //nombre de client
  //int nbCoup=0;   //nombre de coup

  
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
  // pid_t pid;
  // int myBool=0;
  // int i=0;
  // while(i < MAX_CL && myBool==0){
  //   pid=fork();
  //   if (pid == -1) {
	// 	  perror("Erreur sur le fork ");
	// 	  return -9;
  //   }
  //   if(pid==0) {
  //     myBool=1;
  //   }
  //   i++;
  // }

    /*
    * attente de connexion
    */
   //Joueur 1
    sizeAddr1 = sizeof(struct sockaddr_in);
    sockJoueur1 = accept(sockConx, 
                (struct sockaddr *)&addJ1, 
                (socklen_t *)&sizeAddr1);
    if (sockJoueur1 < 0) {
        perror("(serveur) erreur sur accept");
        return -5;
    }
    nbCl++;
    //Joueur 2
    sizeAddr2 = sizeof(struct sockaddr_in);
    sockJoueur2 = accept(sockConx, 
                (struct sockaddr *)&addJ2, 
                (socklen_t *)&sizeAddr2);
    if (sockJoueur2 < 0) {
        perror("(serveur) erreur sur accept");
        return -5;
    }
    nbCl++;
  
  while(1){
    /* preparation du fd_set */
    FD_ZERO(&readSet);
    FD_SET(sockConx, &readSet);
    FD_SET(sockJoueur1, &readSet);
    FD_SET(sockJoueur2, &readSet);
    //Traitement du select
    if (err < 0) {
      perror("(servSelect) error in select"); 
      shutdown(sockJoueur1, SHUT_RDWR); close(sockJoueur1);
      shutdown(sockJoueur2, SHUT_RDWR); close(sockJoueur2);
      close(sockConx);
      return -5;
    }

    if(err>=0){
      //Vérification de l'activité
      if (FD_ISSET(sockJoueur1, &readSet)) { 
          //erreur : fermeture et décalage
          //   printf("Déconnexion du client dont l'IP est : %s\n",inet_ntoa(addClient[cpt].sin_addr));
          // shutdown(sockJoueur1, SHUT_RDWR); close(sockJoueur1);

      }
      if(FD_ISSET(sockJoueur2, &readSet)){
        //ERROR : 
        //printf("Déconnexion du client dont l'IP est : %s\n",inet_ntoa(addClient[cpt].sin_addr));
        //shutdown(sockJoueur2, SHUT_RDWR); close(sockJoueur2);
      }
    }
      
  }
    /* 
   * arret de la connexion et fermeture
   */
  shutdown(sockJoueur1, SHUT_RDWR); close(sockJoueur1);
  shutdown(sockJoueur2, SHUT_RDWR); close(sockJoueur2);
  close(sockConx);
  
  return 0;
}