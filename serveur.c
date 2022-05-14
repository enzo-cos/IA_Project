#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "fonctionsTCP.h"
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
static bool isTimeOut=true;
const int TIME_MAX=6;

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
  printf("J%d : %s is connected\n",cpt,J->nomJoueur);
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

  //Envoie à J1
  rep.coul=J1->couleur;
  rep.err=ERR_OK;
  strcpy(rep.nomAdvers,J2->nomJoueur);
  err=send(J1->sockTrans,&rep,sizeof(TPartieRep),0);
  if(err<=0){
    perror("(serveur) erreur sur le send");
    shutdown(J1->sockTrans, SHUT_RDWR); close(J1->sockTrans);
    return err;
  }

  //Envoie à J2
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
  printf("Fin de la partie.\n");
  if(rep.propCoup==GAGNE){
    printf("Match terminé !\nLe joueur %s gagne la partie.\n",Jcoup->nomJoueur);
    Jcoup->score+=1;
  }else if(rep.propCoup==PERDU){
    printf("Match terminé !\nLe joueur %s gagne la partie.\n",Jadv->nomJoueur);
    Jadv->score+=1;
  }else if(rep.propCoup==NULLE){
    printf("Match terminé !\nEgalité entre les deux joueurs.\n");
  }
  //Afficher les résultats
  printf("Tableaux des scores : \n%s : %d\n%s : %d\n",Jcoup->nomJoueur,Jcoup->score,Jadv->nomJoueur,Jadv->score);
}

/**
 * @brief Traiter un coup reçu par le client
 * 
 * @param Jcoup Joueur ayant envoyé le coup
 * @param Jadverse Joueur adverse
 * @param nJoueur1 Numéro du joueur effectuant le premier coup de la partie
 * @return int code d'erreur
 */
int traiteCoup(struct Joueur *Jcoup,struct Joueur *Jadverse, int nJoueur1){
  TCoupReq req;
  TCoupRep rep;
  int err=0;
  bool timeOut=false;
  //Joueur 1 ou 2 pour validation
  int nj=2;
  if(Jcoup->couleur+1==nJoueur1) nj=1;
  printf("Attente du coup de %s\n",Jcoup->nomJoueur);
  if(isTimeOut){
    struct timeval timev = {TIME_MAX, 0};
    fd_set readSet; 
    FD_ZERO(&readSet);
    FD_SET(Jcoup->sockTrans, &readSet);
    err = select(FD_SETSIZE, &readSet, NULL, NULL, &timev);
    //Traitement du select
    if (err < 0) {
      perror(" error in select TraiteCoup"); 
      return err;
    }
    if(err>=0){
      //Vérification de l'activité
      if(FD_ISSET(Jcoup->sockTrans, &readSet)) { 
        //Réception du coup
        err=recv(Jcoup->sockTrans,&req,sizeof(TCoupReq),0);
        if(err<=0){
          perror("(serveur) erreur dans la reception");
          shutdown(Jcoup->sockTrans, SHUT_RDWR); close(Jcoup->sockTrans);
          return err;
        }
        printf("Réception du coup de %s : %d\n",Jcoup->nomJoueur,req.typeCoup);
        printf("Coup des %d : %d\n",Jcoup->couleur,req.coul);

      }else timeOut=true;
    }

  }else{
    //Réception du coup
    err=recv(Jcoup->sockTrans,&req,sizeof(TCoupReq),0);
    if(err<=0){
      perror("(serveur) erreur dans la reception");
      shutdown(Jcoup->sockTrans, SHUT_RDWR); close(Jcoup->sockTrans);
      return err;
    }
    printf("Réception du coup de %s : %d\n",Jcoup->nomJoueur,req.typeCoup);
    printf("Coup des %d : %d\n",Jcoup->couleur,req.coul);
  }
  
  //Vérification coup valide
  if(timeOut){
      printf("Le temps d'attente est dépassé pour le joueur %s\n",Jcoup->nomJoueur);
      rep.propCoup=PERDU;
      rep.validCoup=TIMEOUT;
      rep.err=ERR_COUP;
  }else if(req.idRequest!=COUP){
    printf("Erreur type de requête\n");
    rep.err=ERR_TYP;
    rep.propCoup=PERDU;
  }else{
    //Vérification du coup
    if(!validationCoup(nj,req,&rep.propCoup)){
      if(Jcoup->couleur==NOIR) printf("Le coup joué par les NOIRS est INVALIDE\n");
      else printf("Le coup joué par les BLANCS est INVALIDE\n");
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

  //Si tout est valide et que la partie continue, Envoie du coup à l'adversaire
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
  // printf("J1 : %s - %d\n",J1->nomJoueur,J1->couleur);
  // printf("J2 : %s - %d\n",J2->nomJoueur,J2->couleur);

  initialiserPartie();
  while(1){
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
   * verification des arguments, 2 argument ou 3 avec l'option "noTimeOut"
   */
  if (argc != 2 && ( argc!=3 || (argc==3 && strcmp(argv[1],"--noTimeout")!=0))) {
    printf ("usage : %s [--noTimeout] port\n", argv[0]);
    return -1;
  }
  //Affectation du port
  if(argc==3){ 
    isTimeOut=false;
    port  = atoi(argv[2]);
  }else{
    port  = atoi(argv[1]);
  }

  //Création socket de connexion
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

  //Attente des requêtes TPartie
  err=AttenteReq(&J1,&J2);
  if(err<0){
    printf("Erreur lors de la requête de demande de partie\nFermeture du serveur\n");
    shutdown(J1.sockTrans, SHUT_RDWR); close(J1.sockTrans);
    shutdown(J2.sockTrans, SHUT_RDWR); close(J2.sockTrans);
    close(sockConx);
    return -1;
  }

  if(J1.couleur==NOIR){
    //Si le J1 n'est pas le premier à avoir envoyé une requête TPartie
    struct Joueur Tmp=J1;
    J1=J2;
    J2=Tmp;
  }
  //Initialisation des scores
  J1.score=0;
  J2.score=0;
  
  //Lancement Première partie
  err=LancerPartie(&J1,&J2);
  if(err<0){
    printf("Error LancerPartie main ou Fin partie\n");
  }else{
    //Lancement Deuxième Partie
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


