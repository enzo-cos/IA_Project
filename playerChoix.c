#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include "fonctionsTCP.h"
#include "protocolColonne.h"
#include "validation.h"

int nbPartie=0;

/**
 * @brief Rejoindre la partie
 * 
 * @param sock socket de communication
 * @param nom  nom du joueur
 * @return int Couleur ou Erreur si <0
 */
int DemandePartie(int sock, char nom[TNOM]){
    TPartieReq reqPartie;
    int err;
    //char nomJoueur[TNOM] = nom;
    //char ch[12];
    
    reqPartie.idRequest = PARTIE;
    strcpy(reqPartie.nomJoueur,nom);

    printf("nom joueur : %s \n", reqPartie.nomJoueur);
    printf("id : %d \n", reqPartie.idRequest);

  //  printf("Taper une touche pour envoyer la req partie\n");
    //scanf("%s",ch);

    err = send(sock, &reqPartie, sizeof(TPartieReq), 0);
    if (err <= 0) { 
        perror("(client) erreur sur le send");
        shutdown(sock, SHUT_RDWR); close(sock);
        return -1;
    }

    TPartieRep repPartie;
    err = recv(sock, &repPartie, sizeof(TPartieRep), 0);
    if (err <= 0) {
        perror("(Client) erreur dans la reception repPartie");
        shutdown(sock, SHUT_RDWR); 
        close(sock);
        return -2;
    }

    printf("Mon advsersaire est %s\n",repPartie.nomAdvers);

    if(repPartie.coul == BLANC){
        printf("Je joue les pions Blanc \n");
    }else{
        printf("Je joue les pions Noir \n");
    }

   return repPartie.coul;

}

/**
 * @brief Savoir si le coup finit la partie
 * 
 * @param repCoup Réponse du serveur 
 * @param moi Nombre associé à mon joueur
 * @return true si la partie doit continuer
 * @return false si le coup arrête la partie
 */
bool ReponseCoup(TCoupRep repCoup, int moi){
            bool premiereManche =true;
            
            if(moi ==0){
                switch (repCoup.validCoup) {
                    case VALID :
                        printf("Coup valide \n");
                        break;
                    case TIMEOUT : 
                        printf("Coup timeout \n");
                        break;
                    case TRICHE : 
                        printf("Coup triche \n");
                        break;
                    default:
                        break;
                }
            
                switch (repCoup.propCoup) {
                    case CONT :
                        printf("Continue\n");
                        break;
                    case GAGNE : 
                        premiereManche =false;
                        printf("vous avez gagné \n");
                        break;
                    case NULLE : 
                        premiereManche =false; 
                        printf("Match Nulle \n");
                        break;
                    case PERDU : 
                        premiereManche =false; 
                        printf("Vous avez Perdu \n");
                        break;
                
                }
            }else{
                switch (repCoup.validCoup) {
                    case VALID :
                       
                        printf("Le coup de l'adversaire est valide \n");
                        break;
                    case TIMEOUT : 
                        
                        printf("Le coup de l'adversaire est timeout \n");
                        break;
                    case TRICHE : 
                        printf("Le coup de l'adversaire est triche \n");
                        break;
                    default:
                        break;
                }
            
                switch (repCoup.propCoup) {
                    case CONT :
                        printf("La partie Continue\n");
                        break;
                    case GAGNE : 
                       premiereManche =false;
                        printf("l'adversaire a gagné vous avez perdu \n");
                        break;
                    case NULLE : 
                       premiereManche =false;
                        printf("Match Nulle \n");
                        break;
                    case PERDU : 
                        premiereManche =false;
                        printf("l'adversaire a Perdu vous avez gagné \n");
                        break;
                
                }

            }
            return  premiereManche;
}

/**
 * @brief Affichage du coup de l'adversaire
 * @param coupAdv 
 */
void RequeteADV(TCoupReq coupAdv){
    char col;
    char colArr;

    switch (coupAdv.typeCoup){
        case 0 :
            switch (coupAdv.action.posPion.col){
                case 0 :
                    col = 'A';
                    break;
                case 1 :
                    col ='B';
                    break;
                case 2 :
                    col='C';
                    break;
            }
            printf("l'adversaire à placer un pion en (%c,%d) \n",col,coupAdv.action.posPion.lg+1);
            break;
        case 1 :
            switch (coupAdv.action.deplPion.caseDep.col){
                case 0 :
                    col = 'A';
                    break;
                case 1 :
                    col ='B';
                    break;
                case 2 :
                    col='C';
                    break;
            }
            switch (coupAdv.action.deplPion.caseArr.col){
                case 0 :
                    colArr = 'A';
                    break;
                case 1 :
                    colArr ='B';
                    break;
                case 2 :
                    colArr='C';
                    break;
            }
            printf("l'adversaire à déplacer un pion de (%c ,%d) en (%c,%d) \n",col,coupAdv.action.deplPion.caseDep.lg+1,colArr,coupAdv.action.deplPion.caseArr.lg+1);
            break;
        case 2 :
            printf("l'adversaire à passé son tour \n");
            break;
    }

}

/**
 * @brief Saisir le coup pour jouer manuellement
 * 
 * @param coup 
 * @return int Code d'erreur
 */
int ChoisirCoup(TCoupReq *coup){
    bool Bpos=false;
    bool Bdepl=false;
    char chaine[20];
    int nL=-1;
    printf("Choisir P(Position) D(Déplacement) ou S(Passe) \n ");
    scanf("%s",chaine);
    switch (chaine[0]){
    case 'p':
    case 'P':
        coup->typeCoup = POS_PION;
        Bpos=true;
        break;

    case 'd':
    case 'D':
        coup->typeCoup = DEPL_PION;
        Bdepl=true;
        break;
    
    case 's':
    case 'S':
        coup->typeCoup = PASSE;
        break;
    
    default:
            printf("MAUVAISE REPONSE, apprends à lire\n\n");
        break;
    }

    if(Bdepl || Bpos){
        if(Bdepl){
            printf("Choisir Case Départ Colonne  \n ");
            scanf("%s",chaine);
            switch (chaine[0]){
            case 'a':
            case 'A':
                coup->action.deplPion.caseDep.col = A;
                break;

            case 'b':
            case 'B':
                coup->action.deplPion.caseDep.col = B;
                break;

            case 'c':
            case 'C':
                coup->action.deplPion.caseDep.col = C;
                break;
            
            default:
                printf("MAUVAISE REPONSE, apprends à lire\n\n");
                break;
            }
            printf("Choisir Case Départ Ligne (1,2,3) \n ");
            scanf("%s",chaine);
            nL=atoi(chaine)-1;
            if(nL<0) printf("MAUVAISE REPONSE, apprends à lire\n\n");
            coup->action.deplPion.caseDep.lg=nL;
        }
        printf("Choisir Colonne (A,B,C) \n ");
        scanf("%s",chaine);
        switch (chaine[0]){
        case 'a':
        case 'A':
            if(Bpos){
                coup->action.posPion.col=A;
            }else{
                coup->action.deplPion.caseArr.col = A;
            }
            break;

        case 'b':
        case 'B':
            if(Bpos){
                coup->action.posPion.col=B;
            }else{
                coup->action.deplPion.caseArr.col = B;
            }
            break;

        case 'c':
        case 'C':
            if(Bpos){
                coup->action.posPion.col=C;
            }else{
                coup->action.deplPion.caseArr.col = C;
            }
            break;
        
        default:
            printf("MAUVAISE REPONSE, apprends à lire\n\n");
            break;
        }

        printf("Choisir Ligne (1,2,3) \n");
        scanf("%s",chaine);
        nL=atoi(chaine)-1;
        if(nL<0) printf("MAUVAISE REPONSE, apprends à lire\n\n");
        if(Bpos){
            coup->action.posPion.lg=nL;
        }else{
            coup->action.deplPion.caseArr.lg=nL;
        }
    }
    return 0;
}
/**
 * @brief Envoie d'un coup
 * 
 * @param sock socket de communication
 * @param coup Requête coup
 * @return int Code d'erreur, =0 si OK, <0 si Erreur, >0 si fin de partie
 */
int EnvoieCoup(int sock, TCoupReq coup){
    int err=0;
    err=ChoisirCoup(&coup);
     if(err<0) printf("Erreur Choix Coup\n");
    printf("Coup :\nligne %d, colonne %d\n",coup.action.posPion.lg,coup.action.posPion.col);
    err =  send(sock, &coup, sizeof(TCoupReq), 0);
    if (err <= 0) { 
        perror("(client) erreur sur le send Coup");
        //fermeture socket après
        return -1;
    }

    TCoupRep repCoup;
    err = recv(sock, &repCoup, sizeof(TCoupRep), 0);
    if (err <= 0) {
        perror("(Client) erreur dans la reception repCOUP");
        //fermeture socket après
        return -2;
    }
    //fin de partie si faux
    if(!ReponseCoup(repCoup,0)) return 1;
    printf("Coup envoyé \n");
    return 0;
}

/**
 * @brief Recevoir le coup de l'adversaire et envoyer son coup
 * 
 * @param sock   socket de communication avec le serveur
 * @param coup   Requête coup
 * @param sockIA socket de communication avec l'IA
 * @return int   Code d'erreur, =0 si OK, <0 si Erreur, >0 si fin de partie
 */
int RecevoirEtEnvoyerCoup(int sock, TCoupReq coup, int sockIA){
    int err=0;

    TCoupRep repAdv;
    err = recv(sock, &repAdv, sizeof(TCoupRep), 0);
    if (err <= 0) {
        perror("(Client) erreur dans la reception repAdv");
        //fermeture socket après
        return -2;
    }

    //Signifie fin de partie
    if(!ReponseCoup(repAdv,1)) return 1;

    TCoupReq coupAdv;
    err = recv(sock, &coupAdv, sizeof(TCoupReq), 0);
    if (err <= 0) {
        perror("(Client) erreur dans la reception coupADV");
        //fermeture socket après
        return -2;
    }

    //affichage coup
    RequeteADV(coupAdv);
    //Recevoir Coup de l'IA

    err=EnvoieCoup(sock,coup);
    
    return err;
}

/**
 * @brief Démarrer une partie
 * 
 * @param sock         socket de communication
 * @param coup         Requête coup
 * @param startToPlay  booleen, vrai si le joueur effectue le premier coup, faux sinon
 * @param sockIA       socket de communication avec l'IA
 * @return int 
 */
int DemarrerPartie(int sock, TCoupReq coup, bool startToPlay, int sockIA){
    int err=0;
    //Premier coup de la partie
    if(startToPlay){
        err=EnvoieCoup(sock,coup);
        if(err<0) return err;
    }
    while(err==0){
        err=RecevoirEtEnvoyerCoup(sock,coup, sockIA);
    }
    //Contacter l'IA pour signifier la fin de la partie
    printf("Fin Partie numéro %d\n",nbPartie);
    return err;
}

/**
 * @brief Démarrer le jeu
 * 
 * @param sock    socket de communication
 * @param couleur couleur du joueur
 * @param sockIA  socket de communication avec l'IA
 * @return int code d'erreur
 */
int Jouer(int sock, int couleur, int sockIA){
    TCoupReq coup;
    bool startToPlay = false; 
   // coup par défault
    coup.action.posPion.col = A;
    coup.action.posPion.lg = DEUX;
    coup.coul  = couleur;
    coup.idRequest = COUP;
    coup.typeCoup = POS_PION;
    //char chaine[20];
    if(couleur==BLANC) startToPlay=true;
    nbPartie=1;
    //Première partie
    if(DemarrerPartie(sock,coup,startToPlay, sockIA)<0){
        printf("Une erreur est survenue lors de la partie\n");
        return -1;
    }
    startToPlay=!startToPlay;
    nbPartie++;
    //2ème partie
    if(DemarrerPartie(sock,coup,startToPlay,sockIA)<0){
        printf("Une erreur est survenue lors de la partie\n");
        return -1;
    }
    printf("Fin du jeu\n");

    return 0;
}

int main(int argc, char** argv) {

    int sock,                /* descripteur de la socket locale */
        port;                /* variables de lecture */
       
      char* ipMachServ;        /* pour solution inet_aton */    

    /* verification des arguments */
    if (argc != 3) {
        printf("usage : %s nomJoueur port \n", argv[0]);
        return -1;
    }
    char* nom = argv[1];
    ipMachServ="127.0.0.1";
    //ipMachServ = argv[1];
    port = atoi(argv[2]);
    sock = socketClient(ipMachServ,port);
    
    if(sock<1){
        return -1;
    }
    
    int couleur = DemandePartie(sock, nom);
    if(couleur<0) return -1;
    int sockIA=0;
    Jouer(sock, couleur, sockIA);

    /* 
    * fermeture de la connexion et de la socket 
    */
    shutdown(sock, SHUT_RDWR);
    close(sock);
    
    return 0;

}