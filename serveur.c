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

struct Joueur
{
  int sockTrans;
  struct sockaddr_in addJ;
  TPion couleur;
  int sizeAddr;
  char *nomJoueur; //Ne sera JAMAIS envoyé
};


int AttenteReq(struct Joueur *J1, struct Joueur *J2){
  TPartieReq *req=NULL;
  TPartieRep *rep=NULL;
  int err;
  err=recv(J1->sockTrans,req,sizeof(struct TPartieReq*),0);
  if(err<=0){
    perror("(serveurTCP) erreur dans la reception");
    shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
    return err;
  }
  if(req->idRequest==PARTIE){
    J1->nomJoueur=req->nomJoueur;
    J1->couleur=BLANC;
  }

  err=recv(J2->sockTrans,req,sizeof(struct TPartieReq*),0);
  if(err<=0){
    perror("(serveurTCP) erreur dans la reception");
    shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
    return err;
  }
  if(req->idRequest==PARTIE){
    J2->nomJoueur=req->nomJoueur;
    J2->couleur=NOIR;
  }

  rep->coul=J1->couleur;
  rep->err=ERR_OK;
  strcpy(rep->nomAdvers,J2->nomJoueur);
  err=send(J1->sockTrans,rep,sizeof(struct TPartieRep*),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
    return err;
  }

  rep->coul=J2->couleur;
  rep->err=ERR_OK;
  strcpy(rep->nomAdvers,J1->nomJoueur);
  err=send(J2->sockTrans,rep,sizeof(struct TPartieRep*),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
    return err;
  }


  return 0;
}


int commencerPartie(struct Joueur *J1, struct Joueur *J2){
  while(1){
    TCoupReq *req=NULL;
    TCoupRep *rep=NULL;
    int err;
    //Verif Attente
    err=recv(J1->sockTrans,req,sizeof(struct TCoupReq*),0);
    if(err<=0){
      perror("(serveurTCP) erreur dans la reception");
      shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
      return err;
    }
    if(req->idRequest==COUP){
      //Vérification Valide

      rep->err=ERR_OK;
      rep->validCoup=VALID;
      rep->propCoup=CONT;//
      err=send(J1->sockTrans,rep,sizeof(struct TCoupRep*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
        return err;
      }
      err=send(J2->sockTrans,rep,sizeof(struct TCoupRep*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
        return err;
      }

      err=send(J2->sockTrans,req,sizeof(struct TCoupReq*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
        return err;
      }
      
    }


    err=recv(J2->sockTrans,req,sizeof(struct TCoupReq*),0);
    if(err<=0){
      perror("(serveurTCP) erreur dans la reception");
      shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
      return err;
    }
    if(req->idRequest==COUP){
     //Verif Valide

      rep->err=ERR_OK;
      rep->validCoup=VALID;
      rep->propCoup=CONT;//
      err=send(J2->sockTrans,rep,sizeof(struct TCoupRep*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
        return err;
      }
      err=send(J1->sockTrans,rep,sizeof(struct TCoupRep*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
        return err;
      }

      err=send(J1->sockTrans,req,sizeof(struct TCoupReq*),0);
      if(err<=0){
        perror("(serveur) erreur sur le send");
        shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
        return err;
      }
    }
  }
}


int LancerPartie(struct Joueur *J1, struct Joueur *J2,int nb){
  int err;
  printf("J1 : %s - %d\n",J1->nomJoueur,J1->couleur);
  printf("J2 : %s - %d\n",J2->nomJoueur,J2->couleur);
  if(nb==J1->couleur){
    printf("Le joueur '%s' aux pions blancs commence\n",J1->nomJoueur);
    err=commencerPartie(J1,J2);
    if(err<0) return -1;
  }else if(nb==J2->couleur){
    printf("Le joueur '%s' aux pions noirs commence\n",J2->nomJoueur);
    err=commencerPartie(J2,J1);
    if(err<0) return -1;
  }else{
    printf("Error, personne ne peut commencer \n");
    return -1;
  }
  return 0;
}



int main(int argc, char** argv) {
  int  sockConx,        /* descripteur socket connexion */
       //sockJoueur1,       /* descripteur socket Joueur */
       //sockJoueur2,       /* descripteur socket Joueur  */
       port;       /* numero de port */
       //sizeAddr1,         /* taille de l'adresse d'une socket */
       //sizeAddr2;        /* taille de l'adresse d'une socket */

  //const int MAX_CL=2;

  //struct sockaddr_in addJ1;	/* adresse de la socket client connectee */
  //struct sockaddr_in addJ2;	/* adresse de la socket client connectee */

 // fd_set readSet;              /* variable pour le select */
  int err=0;    //code erreur
  int nbCl=0;     //nombre de client
  //int nbCoup=0;   //nombre de coup

  //Joueur 1
  struct Joueur J1;
  struct Joueur J2;
  //Joueur 2

  
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
    J1.sizeAddr = sizeof(struct sockaddr_in);
    J1.sockTrans = accept(sockConx, 
                (struct sockaddr *)&J1.addJ, 
                (socklen_t *)&J1.sizeAddr);
    if (J1.sockTrans < 0) {
        perror("(serveur) erreur sur accept");
        return -5;
    }
    nbCl++;
    //Joueur 2
    J2.sizeAddr = sizeof(struct sockaddr_in);
    J2.sockTrans = accept(sockConx, 
                (struct sockaddr *)&J2.addJ, 
                (socklen_t *)&J2.sizeAddr);
    if (J2.sockTrans < 0) {
        perror("(serveur) erreur sur accept");
        return -5;
    }
    nbCl++;

    err=AttenteReq(&J1,&J2);
    if(err<0){
      //ERROR
      printf("Error Attente Req main\n");
    }

    err=LancerPartie(&J1,&J2,0);
    if(err<0){
      //ERROR
      printf("Error LancerPartie main\n");
    }

  // while(1){
   
      
  // }
    /* 
   * arret de la connexion et fermeture
   */
  shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
  shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
  close(sockConx);
  
  return 0;
}


 /* preparation du fd_set et select pour l'ordre */
    /*FD_ZERO(&readSet);
    FD_SET(sockConx, &readSet);
    FD_SET(J1.sockTrans, &readSet);
    FD_SET(J2.sockTrans, &readSet);
    //Traitement du select
    if (err < 0) {
      perror("(servSelect) error in select"); 
      shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
      shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
      close(sockConx);
      return -5;
    }

    if(err>=0){
      //Vérification de l'activité
      if (FD_ISSET(J1.sockTrans, &readSet)) { 
          //erreur : fermeture et décalage
          //   printf("Déconnexion du client dont l'IP est : %s\n",inet_ntoa(addClient[cpt].sin_addr));
          // shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
      }
      if(FD_ISSET(J2.sockTrans, &readSet)){
        //ERROR : 
        //printf("Déconnexion du client dont l'IP est : %s\n",inet_ntoa(addClient[cpt].sin_addr));
        //shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
      }
    }*/