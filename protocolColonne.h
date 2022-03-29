/* 
 ******************************************************
 *
 * Programme : protocolColonne.h
 *
 * Synopsis : entete du protocole d'acces a l'arbitre
 *            pour le jeu Colonnes de trois
 *
 * Ecrit par : VF, FB
 * Date :  07 / 03 / 22
 * 
 *****************************************************/

#ifndef _protocolColonne_h
#define _protocolColonne_h

/* Taille des chaines de caracteres pour les noms */
#define TNOM 30

/* Identificateurs des requetes */
typedef enum { PARTIE, COUP } TIdRequest;

/* Types d'erreur */
typedef enum { ERR_OK,      /* Validation de la requete */
	       ERR_PARTIE,  /* Erreur sur la demande de partie */
	       ERR_COUP,    /* Erreur sur le coup joue */
	       ERR_TYP      /* Erreur sur le type de requete */
} TErreur;

/* 
 * Structures demande de partie
 */
typedef struct {
  TIdRequest idRequest;       /* Identificateur de la requete */
  char nomJoueur[TNOM];       /* Nom du joueur */
} TPartieReq;

typedef enum { BLANC, NOIR } TPion;

typedef struct {
  TErreur  err;               /* Code d'erreur */
  TPion    coul;              /* Couleur du pion */
  char nomAdvers[TNOM];       /* Nom du joueur */
} TPartieRep;


/* 
 * Definition d'une position de case
 */
typedef enum { UN, DEUX, TROIS } TLigne;
typedef enum { A, B, C } TCol;

typedef struct {
  TLigne lg;            /* La ligne de la position d'une case */
  TCol col;             /* La colonne de la position d'une case */
} TCase;

/* 
 * Definition d'un deplacement de pion
 */
typedef struct {
  TCase  caseDep;   /* Position de depart du pion */
  TCase  caseArr;   /* Position d'arrivee du pion */
} TDeplPion;


/* 
 * Structures coup du joueur 
 */

/* Precision des types de coups */
typedef enum { POS_PION, DEPL_PION, PASSE } TCoup;

typedef struct {
  TIdRequest idRequest;     /* Identificateur de la requete */
  TCoup      typeCoup;      /* Type du coup : positionnement ou deplacement */
  TPion      coul;          /* Couleur de pion */
  union {
    TCase     posPion;      /* Positionnement de pion */
    TDeplPion deplPion;     /* Deplacement de pion */
  } action;
} TCoupReq;

/* Validite du coup */
typedef enum { VALID, TIMEOUT, TRICHE } TValCoup;

/* Propriete des coups */
typedef enum { CONT, GAGNE, NULLE, PERDU } TPropCoup;

/* Reponse a un coup */
typedef struct {
  TErreur err;              /* Code d'erreur */
  TValCoup  validCoup;      /* Validite du coup */
  TPropCoup propCoup;       /* Propriete du coup */
} TCoupRep;

#endif

