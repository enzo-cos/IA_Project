/*
 **********************************************************
 *
 *  Programme : validation.h
 *
 *  ecrit par : FB / VF
 *
 *  resume : entete pour la validation des coups
 *
 *  date :      07 / 03 / 22
 *  
 ***********************************************************
 */

#ifndef _VALIDATION_H
#define _VALIDATION_H
#include <stdbool.h>

/* Validation d'un coup :
 * parametres : 
 *    le numero du joueur courant : 1 (le premier qui a commence a jouer) ou 
 *                                  2 (le deuxieme)
 *    le coup (TCoupReq)
 * resultat : type bool (coup valide ou non) 
 *            propriete du coup  (GAGNE, PERDU, NUL - le coup rend le joueur gagnant, perdant ou la partie est ex aequo, ou CONT si aucune des autres)
*/
bool validationCoup(int joueur, TCoupReq coup, TPropCoup *propCoup);

/* Initialiser une partie : informer du demarrage d'une nouvelle partie avant son debut */
void initialiserPartie();

#endif
