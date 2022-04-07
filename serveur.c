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
  char nomJoueur[32]; 
  int score;
};


/**
 * @brief Traite la requête partie d'un joueur
 * 
 * @param J Joueur conecerné
 * @param req requête Partie
 * @param cpt Numéro de connexion
 * @return int code d'erreur
 */
int traiteReqPartie(struct Joueur *J, TPartieReq req ,int cpt){
  int err=recv(J->sockTrans,&req,sizeof(TPartieReq),0);
  if(err<=0){
    perror("(serveurTCP) erreur dans la reception");
    return err;
  }
  if(req.idRequest!=PARTIE) return -8;
  if(cpt==1){
    //Premier Reçu
    J->couleur=BLANC;
  }else{
    J->couleur=NOIR;
  }
  strcpy(J->nomJoueur,req.nomJoueur);
  printf("J%d : %s connected\n",cpt,J->nomJoueur);
  return 0;
}
/**
 * @brief Attente des requêtes des joueurs
 * 
 * @param J1 Joueur 1
 * @param J2 Joueur 2
 * @return int code d'erreur
 */
int AttenteReq(struct Joueur *J1, struct Joueur *J2){
  TPartieReq req;
  TPartieRep rep;
  int err=0;
  fd_set readSet; 
   /* preparation du fd_set et select */
   int cpt=0;
  while(cpt<2){
    FD_ZERO(&readSet);
    FD_SET(J1->sockTrans, &readSet);
    FD_SET(J2->sockTrans, &readSet);
    err = select(FD_SETSIZE, &readSet, NULL, NULL, NULL);
    //Traitement du select
    if (err < 0) {
      perror("(servSelect) error in select"); 
      return err;
    }

    if(err>=0){
      //Vérification de l'activité
      if (FD_ISSET(J1->sockTrans, &readSet)) { 
        cpt++;
        err=traiteReqPartie(J1,req,cpt);
        if(err<0) return err;
      }

      if(FD_ISSET(J2->sockTrans, &readSet)){
        cpt++;
        err=traiteReqPartie(J2,req,cpt);
        if(err<0) return err;
        //if err==-8 -> rep.err=ERR_TYP
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

/**
 * @brief Fin de partie et afficher les résultats
 * 
 */
void finPartie(struct Joueur *Jcoup, struct Joueur *Jadv,TCoupRep rep){
  printf("Fin de la partie .\n");
  //Afficher les résultats
}

/**
 * @brief Traiter un coup reçu par le client S
 * 
 * @param Jcoup Joueur ayant envoyé le coup
 * @param Jadverse Joueur adverse
 * @param nJoueur1 Numéro du joueur effectuant le premier coup de la partie
 * @return int code d'erreur
 */
int traiteCoup(struct Joueur *Jcoup,struct Joueur *Jadverse, int nJoueur1){
  TCoupReq req;
  TCoupRep rep;
  int err;
  //Joueur 1 ou 2 pour validation
  int nj=2;
  if(Jcoup->couleur+1==nJoueur1) nj=1;

  //Réception du coup
  err=recv(Jcoup->sockTrans,&req,sizeof(TCoupReq),0);
  if(err<=0){
    perror("(serveurTCP) erreur dans la reception");
    shutdown(Jcoup->sockTrans, SHUT_RDWR); close(Jcoup->sockTrans);
    return err;
  }
  printf("Réception du coup de %s : %d\n",Jcoup->nomJoueur,req.typeCoup);
  printf("Coup des %d : %d\n",Jcoup->couleur,req.coul);
  if(req.idRequest!=COUP){
    //Error : rep.err=ERR_TYP, rep.validCoup ?
    //Send ?
  }else{
    //Vérification du coup
    if(!validationCoup(nj,req,&rep.propCoup)){
      //A faire après ??
      //A nous d'initaliser cces valeurs ? pareil pour req.idRequest
      rep.err=ERR_COUP; 
      rep.validCoup=TRICHE;
    }else{
      rep.err=ERR_OK;
      rep.validCoup=VALID;
    }
  }
  
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
    finPartie(Jcoup,Jadverse,rep);
    return 1;
  }
  //Envoie du coup si requête Typ Incorrecte ??

  //Si tout est valide, Envoie du coup à l'adversaire
  err=send(Jadverse->sockTrans,&req,sizeof(TCoupReq),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(Jadverse->sockTrans, SHUT_RDWR); close(Jadverse->sockTrans);
    return err;
  }
    
  
  return 0;
}

/**
 * @brief Lancer une partie
 * 
 * @param J1 Joueur 1
 * @param J2 Joueur 2
 * @return int code d'erreur
 */
int LancerPartie(struct Joueur *J1, struct Joueur *J2){
  int err;
  int nbJoueur1=J1->couleur+1;
  printf("J1 : %s - %d\n",J1->nomJoueur,J1->couleur);
  printf("J2 : %s - %d\n",J2->nomJoueur,J2->couleur);

  initialiserPartie();
  while(1){
    //Verif Attente
    //Joueur 1
    err=traiteCoup(J1,J2,nbJoueur1);
    if(err!=0) return err;
    //Joueur 2
    err=traiteCoup(J2,J1,nbJoueur1);
    if(err!=0) return err;

  }
  return 0;
}



int main(int argc, char** argv) {
  int  sockConx,        /* descripteur socket connexion */
       port;       /* numero de port */

  int err=0;    //code erreur
  int nbCl=0;     //nombre de client

  //Joueur 1
  struct Joueur J1;
  //Joueur 2
  struct Joueur J2;

  
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

    err=AttenteReq(&J1,&J2);
    if(err<0){
      printf("Error Attente Req main\n");
      shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
      shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
      close(sockConx);
      return -1;
    }
    if(J1.couleur==NOIR){
      //Si le J1 n'est pas le premier à avoir envoyé une requête
      struct Joueur Tmp=J1;
      J1=J2;
      J2=Tmp;
    }
    //Initialisation des scores
    J1.score=0;
    J2.score=0;
    
    err=LancerPartie(&J1,&J2);
    if(err<0){
      printf("Error LancerPartie main ou Fin partie\n");
    }else{
      err=LancerPartie(&J2,&J1);
      if(err<0){
        printf("Error LancerPartie main ou Fin partie\n");
      }
    }
    

   /* 
   * arret de la connexion et fermeture
   */
  shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
  shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
  close(sockConx);
  
  return 0;
}


