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
  char nomJoueur[32]; //Ne sera JAMAIS envoyé
};

int AttenteReq(struct Joueur *J1, struct Joueur *J2,int sockConx){
  TPartieReq req;
  TPartieRep rep;
  int err;
  fd_set readSet; 
   /* preparation du fd_set et select */
   int cpt=0;
  while(cpt<2){
    FD_ZERO(&readSet);
    FD_SET(sockConx, &readSet);
    FD_SET(J1->sockTrans, &readSet);
    FD_SET(J2->sockTrans, &readSet);
    err = select(FD_SETSIZE, &readSet, NULL, NULL, NULL);
    //Traitement du select
    if (err < 0) {
      perror("(servSelect) error in select"); 
      shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
      shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
      close(sockConx);
      return -5;
    }

    if(err>=0){
      //Vérification de l'activité
      if (FD_ISSET(J1->sockTrans, &readSet)) { 
        err=recv(J1->sockTrans,&req,sizeof(TPartieReq),0);
        if(err<=0){
          perror("(serveurTCP) erreur dans la reception");
          shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
          return err;
        }
        printf("J1 connected\n");
        cpt++;
        if(req.idRequest==PARTIE){

          //J1->nomJoueur=req.nomJoueur;
          strcpy(J1->nomJoueur,req.nomJoueur);
          J1->couleur=BLANC;
        }

      }
      if(FD_ISSET(J2->sockTrans, &readSet)){
        err=recv(J2->sockTrans,&req,sizeof(TPartieReq),0);
        if(err<=0){
          perror("(serveurTCP) erreur dans la reception");
          shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
          return err;
        }
        printf("J2 connected\n");
        cpt++;
        if(req.idRequest==PARTIE){
         strcpy(J2->nomJoueur,req.nomJoueur);
        J2->couleur=NOIR;
        }
      }
    }
  }


  rep.coul=J1->couleur;
  rep.err=ERR_OK;
  strcpy(rep.nomAdvers,J2->nomJoueur);
  err=send(J1->sockTrans,&rep,sizeof(TPartieRep),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
    return err;
  }
  rep.coul=J2->couleur;
  rep.err=ERR_OK;

  strcpy(rep.nomAdvers,J1->nomJoueur);
  err=send(J2->sockTrans,&rep,sizeof(TPartieRep),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(J2->sockTrans, SHUT_RDWR); close(J2->sockTrans);
    return err;
  }
  return 0;
}

void finPartie(){
  printf("Fin de la partie .\n");
}

int traiteCoup(struct Joueur *Jcoup,struct Joueur *Jadverse){
  TCoupReq req;
  TCoupRep rep;
  int err;

  //Réception du coup
  err=recv(Jcoup->sockTrans,&req,sizeof(TCoupReq),0);
  if(err<=0){
    perror("(serveurTCP) erreur dans la reception");
    shutdown(Jcoup->sockTrans, SHUT_RDWR); close(Jcoup->sockTrans);
    return err;
  }
  printf("Réception du coup de %s : %d\n",Jcoup->nomJoueur,req.typeCoup);
  printf("Coup des %d : %d\n",Jcoup->couleur,req.coul);
  if(req.idRequest==COUP){
    //Vérification du coup
    if(!validationCoup(Jcoup->couleur+1,req,&rep.propCoup)){
      //A faire après ??
      rep.err=ERR_COUP;
      rep.validCoup=TRICHE;
    }else{
      rep.err=ERR_OK;
      rep.validCoup=VALID;
    }
    printf("FIN VALIDATION\n");
    //Envoi de la réponse au joueur 
    err=send(Jcoup->sockTrans,&rep,sizeof(TCoupRep),0);
    if(err<=0){
      perror("(serveur) erreur sur le send");
      shutdown(Jcoup->sockTrans, SHUT_RDWR); close(Jcoup->sockTrans);
      return err;
    }
    //Envoie de la réponse à l'adversaire
    err=send(Jadverse->sockTrans,&rep,sizeof(TCoupRep),0);
    if(err<=0){
      perror("(serveur) erreur sur le send");
      shutdown(Jadverse->sockTrans, SHUT_RDWR); close(Jadverse->sockTrans);
      return err;
    }

    //Vérifier propCoup pour résultat
    if(rep.propCoup!=CONT){
      finPartie();
      return -1;
    }
    //Si tout est valide, Envoie du coup à l'adversaire
    err=send(Jadverse->sockTrans,&req,sizeof(TCoupReq),0);
    if(err<=0){
      perror("(serveur) erreur sur le send");
      shutdown(Jadverse->sockTrans, SHUT_RDWR); close(Jadverse->sockTrans);
      return err;
    }
    
  }
  return 0;
}

int commencerPartie(struct Joueur *J1, struct Joueur *J2){
  bool partieEncours=true;
  initialiserPartie();
  while(partieEncours){
    //Verif Attente
    //Joueur 1
    if(traiteCoup(J1,J2)!=0) return -2;
    //Joueur 2
    if(traiteCoup(J2,J1)!=0) return -2;

    //
  }
  return 0;
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

    err=AttenteReq(&J1,&J2,sockConx);
    if(err<0){
      //ERROR
      printf("Error Attente Req main\n");
    }

    printf("Joueur 1 %s\n",J1.nomJoueur);
    err=LancerPartie(&J1,&J2,0);
    if(err<0){
      //ERROR
      printf("Error LancerPartie main ou Fin partie\n");
    }

   /* 
   * arret de la connexion et fermeture
   */
  shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
  shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
  close(sockConx);
  
  return 0;
}


