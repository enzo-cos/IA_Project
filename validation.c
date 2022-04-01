#include "validation.h"
#include "protocolColonne.h"


int nbCoup=0;
int nbPosBlanc=0;
int nbPosNoir=0;

/* Validation d'un coup :
 * parametres : 
 *    le numero du joueur courant : 1 (le premier qui a commence a jouer) ou 
 *                                  2 (le deuxieme)
 *    le coup (TCoupReq)
 * resultat : type bool (coup valide ou non) 
 *            propriete du coup  (GAGNE, PERDU, NUL - le coup rend le joueur gagnant, perdant ou la partie est ex aequo, ou CONT si aucune des autres)
*/
bool validationCoup(int joueur, TCoupReq coup, TPropCoup *propCoup){
    nbCoup++;
    if(nbCoup>40){
        printf("ARBITRE : 40 coups effectués, Match Null\n");
        *propCoup=NULLE;
        return false;
    }
    switch (coup.typeCoup)
    {
    case POS_PION:
        printf("ARBITRE : Joueur %d Placement\n",coup.coul);
        coup.coul==BLANC? nbPosBlanc++ : nbPosNoir++;
        if(nbPosBlanc>8 || nbPosNoir>8){
            printf("ARBITRE : Tous les pions sont déjà positionnés\n");
            *propCoup=PERDU;
            return false;
        }
        if(coup.action.posPion.col==A) printf("Col A , ");
        if(coup.action.posPion.col==B) printf("Col B , ");
        if(coup.action.posPion.col==C) printf("Col C , ");
        if(coup.action.posPion.lg==UN) printf("Ligne 1 ");
        if(coup.action.posPion.lg==DEUX) printf("Ligne 2 ");
        if(coup.action.posPion.lg==TROIS) printf("Ligne 3 ");
        printf(" \n");
        printf("ARBITRE : Position, col : %d, lg : %d \n",coup.action.posPion.col,coup.action.posPion.lg);
        // if(!coup.action.posPion.col || !coup.action.posPion.lg){
        //     printf("ARBITRE : Position du pion non définie\n");
        //     *propCoup=PERDU;
        //     return false;
        // }
        break;

    case DEPL_PION:
        if(nbPosBlanc<8 || nbPosNoir<8){
            printf("ARBITRE : Les pions ne sont pas tous positionnés sur le plateau\n");
            *propCoup=PERDU;
            return false;
        }
        break;

    case PASSE:
        if(nbPosBlanc<8 || nbPosNoir<8){
            printf("ARBITRE : Les pions ne sont pas tous positionnés sur le plateau\n");
            *propCoup=PERDU;
            return false;
        }
        break;
    
    default:
        printf("ARBITRE : Coup Non spécifié\n");
        *propCoup=PERDU;
        return false;
        break;
    }
    *propCoup=CONT;
    return true;
}

/* Initialiser une partie : informer du demarrage d'une nouvelle partie avant son debut */
void initialiserPartie(){
    
    nbCoup=0;
    nbPosBlanc=0;
    nbPosNoir=0;
    printf("ARBITRE : La partie peut commencer !\n");
}