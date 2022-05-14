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
#include "fctServ.h"
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
            bool contiParti =true;
            
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
                        contiParti =false;
                        printf("vous avez gagné \n");
                        break;
                    case NULLE : 
                        contiParti =false; 
                        printf("Match Nulle \n");
                        break;
                    case PERDU : 
                        contiParti =false; 
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
                       contiParti =false;
                        printf("l'adversaire a gagné vous avez perdu \n");
                        break;
                    case NULLE : 
                       contiParti =false;
                        printf("Match Nulle \n");
                        break;
                    case PERDU : 
                        contiParti =false;
                        printf("l'adversaire a Perdu vous avez gagné \n");
                        break;
                
                }

            }
            return  contiParti;
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
 * @brief Envoie du coup jouer par l'adversaire à l'IA
 * 
 * @param sockIA socket de communication avec l'IA
 * @param coup  coup de l'avdersaire
 * @return int  Code d'erreur
 */
int EnvoieIA(int sockIA, TCoupReq *coup ){
    int err=0;
    //Envoie typeCoup
    int n=htonl(coup->typeCoup);
    err = send(sockIA,&n,sizeof(int),0);
    if (err <= 0) {
        perror("erreur dans l'envoie du typeCoup");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -1;
    }
    if(coup->typeCoup==DEPL_PION){
        //Envoie case départ Colonne
        n=htonl(coup->action.deplPion.caseDep.col);
        err = send(sockIA,&n,sizeof(int),0);
        if (err <= 0) {
            perror("erreur dans l'envoie de la ligne");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -2;
        }
        //Envoie case départ Ligne
        n=htonl(coup->action.deplPion.caseDep.lg);
        err = send(sockIA,&n,sizeof(int),0);
        if (err <= 0) {
            perror("erreur dans l'envoie de la ligne");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -2;
        }
    }else if(coup->typeCoup==PASSE){
        return 1;
    }
    //Envoie Colonne arrivée
    if(coup->typeCoup==DEPL_PION) n=htonl(coup->action.deplPion.caseArr.col);
    else n=htonl(coup->action.posPion.col);
    err = send(sockIA,&n,sizeof(int),0);
    if (err <= 0) {
        perror("erreur dans l'envoie de la colonne");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -4;
    }    

    //Envoie Ligne arrivée
    if(coup->typeCoup==DEPL_PION) n=htonl(coup->action.deplPion.caseArr.lg);
    else n=htonl(coup->action.posPion.lg);

    err = send(sockIA,&n,sizeof(int),0);
    if (err <= 0) {
        perror("erreur dans l'envoie de la ligne");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -3;
    }
    printf("FIN3\n");
    //envoie couleur
    // n=htonl(coup->coul);
    // err = send(sockIA,&n,sizeof(int),0);
    // if (err <= 0) {
    //     perror("erreur dans l'envoie de la couleur");
    //     shutdown(sockIA, SHUT_RDWR); close(sockIA);
    //     return -5;
    // }

    return 0;
}

/**
 * @brief Recevoir le Coup par l'IA
 * 
 * @param sockIA socket de communication avec l'IA
 * @param coup   coup à jouer
 * @return int code d'erreur
 */
int RecevoirIA(int sockIA, TCoupReq *coup){
    int tc,l,c,lD,cD; //TypeCoup, Ligne, Colonne, LigneDépart, ColonneDépart
    int nb=0;
    printf("Attente du coup par l'IA\n");
   
   // int msec = 0;
    //  clock_t before = clock();
    //  clock_t difference = clock() - before;
    //     msec=difference * 1000 / CLOCKS_PER_SEC;
    //     printf("msec : %d\n",msec);
    //     if(msec>4){
    //         printf("TimeOut\n");
    //         return 6;
    //     }
    
    //Récepetion TypeCoup
    while(tc!=sizeof(int)){
        tc = recv(sockIA,&nb,sizeof(int),MSG_PEEK);
        if (tc <= 0) {
            perror("erreur dans la reception");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -6;
        }
    }
    tc = recv(sockIA,&nb,sizeof(int),0);
    if (tc <= 0) {
        perror("erreur dans la reception");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -6;
    }
    coup->typeCoup=ntohl(nb);
    printf("reçu typeCoup : %d\n",coup->typeCoup);
    //Recevoir Les cases de départ si déplacement
    if(tc==DEPL_PION){
        //Recevoir Colonne Départ
        //printf("Recevoir Colonne Départ : \n");
        while(cD!=sizeof(int)){
            cD = recv(sockIA,&nb,sizeof(int),MSG_PEEK);
            if (cD <= 0) {
                perror("erreur dans la reception");
                shutdown(sockIA, SHUT_RDWR); close(sockIA);
                return -6;
            }
        }
        cD = recv(sockIA,&nb,sizeof(int),0);
        if (cD <= 0) {
            perror("erreur dans la reception");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -6;
        }
        coup->action.deplPion.caseDep.col=ntohl(nb);
        printf("reçu Colonne Départ : %d\n",coup->action.deplPion.caseDep.col);
        
        //Recevoir Ligne Départ
        //printf("Recevoir Ligne Départ : \n");
        while(lD!=sizeof(int)){
            lD = recv(sockIA,&nb,sizeof(int),MSG_PEEK);
            if (lD <= 0) {
                perror("erreur dans la reception");
                shutdown(sockIA, SHUT_RDWR); close(sockIA);
                return -6;
            }
        }
        lD = recv(sockIA,&nb,sizeof(int),0);
        if (lD <= 0) {
            perror("erreur dans la reception");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -6;
        }
        coup->action.deplPion.caseDep.lg=ntohl(nb);
        printf("reçu Ligne Départ : %d\n",coup->action.deplPion.caseDep.lg);
        
    }else if(tc==PASSE){ //Passer le tour
        printf("Tour passé\n");
        return 2;
    } 
    //Recevoir les lignes et colonnes d'arrivées
    //printf("Recevoir colonne \n");
    while(c!=sizeof(int)){
        c = recv(sockIA,&nb,sizeof(int),MSG_PEEK);
        if (c <= 0) {
            perror("erreur dans la reception");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -6;
        }
    }
    c = recv(sockIA,&nb,sizeof(int),0);
    if (c <= 0) {
        perror("erreur dans la reception");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -6;
    }
    //Adresser les valeurs à la bonne variable en fonction du type de coup
    if(coup->typeCoup==POS_PION){
        coup->action.posPion.col=ntohl(nb);
        printf("reçu colonne : %d \n",coup->action.posPion.col);
    }else{ //DEPL
        coup->action.deplPion.caseArr.col=ntohl(nb);
        printf("reçu colonne arrivée: %d \n",coup->action.deplPion.caseArr.col);
    }

    //printf("Recevoir ligne \n");
    while(l!=sizeof(int)){
        l = recv(sockIA,&nb,sizeof(int),MSG_PEEK);
        if (l <= 0) {
            perror("erreur dans la reception");
            shutdown(sockIA, SHUT_RDWR); close(sockIA);
            return -6;
        }
    }
    l = recv(sockIA,&nb,sizeof(int),0);
    if (l <= 0) {
        perror("erreur dans la reception");
        shutdown(sockIA, SHUT_RDWR); close(sockIA);
        return -6;
    }
    //Adresser les valeurs à la bonne variable en fonction du type de coup
    if(coup->typeCoup==POS_PION){
        coup->action.posPion.lg=ntohl(nb);
        printf("reçu ligne : %d \n",coup->action.posPion.lg);
    }else{//DEPL
        coup->action.deplPion.caseArr.lg=ntohl(nb);
        printf("reçu ligne arrivée: %d \n",coup->action.deplPion.caseArr.lg);
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
    //err=ChoisirCoup(&coup);
       // if(err<0) printf("Erreur Choix Coup\n");
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
    
    return 0;
}

/**
 * @brief Recevoir coup de l'IA et l'envoyer ou gérer le timeOut reçu du serveur 
 * 
 * @param sockServeur socket de communication avec le serveur
 * @param sockIA      socket de communication avec l'IA
 * @param coup        coup à jouer
 * @return int  code erreur
 */
int RecvIA_Envoi_select(int sockServ, int sockIA,TCoupReq coup){
    int err=0;
    //Recevoir Coup de l'IA
    fd_set readSet; 
   /* preparation du fd_set et select pour recevoir timeOut si besoin */
    FD_ZERO(&readSet);
    FD_SET(sockServ, &readSet);
    FD_SET(sockIA, &readSet);
    err = select(FD_SETSIZE, &readSet, NULL, NULL, NULL);
    //Traitement du select
    if (err < 0) {
      perror("(Joueur) error in select recvIA"); 
      return err;
    }
    if(err>=0){
        //Activité avec serveur => timeOut
        if (FD_ISSET(sockServ, &readSet)) { 
            printf("TimeOut : vous avez mis trop de temps à répondre\n");
            //Envoi de la réponse au joueur 
            TCoupRep repCoup;
            err = recv(sockServ, &repCoup, sizeof(TCoupRep), 0);
            ReponseCoup(repCoup,0);
            return 1;
        }
        if(FD_ISSET(sockIA, &readSet)){
            err=RecevoirIA(sockIA,&coup);
            if(err!=0) return err; //Ou établir coup par défault ?
            err=EnvoieCoup(sockServ,coup);
        }
    }
    return err;
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
    //Envoi du coup de l'adversaire à l'IA
    err=EnvoieIA(sockIA, &coupAdv);
    //affichage coup addversaire
    RequeteADV(coupAdv);
    //Recevoir Coup IA et envoyer Coup
    err=RecvIA_Envoi_select(sock,sockIA,coup);

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
        err=RecvIA_Envoi_select(sock,sockIA,coup);
    }
    while(err==0){
        err=RecevoirEtEnvoyerCoup(sock,coup, sockIA);
    }
    //Contacter l'IA pour signifier la fin de la partie
    int n=14;
    n=htonl(n);
    err=send(sockIA,&n,sizeof(int),0);
    if(err<0){
        perror("Erreur Envoie Fin de partie");
        return err;
    }
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

/**
 * @brief Connexion avec l'IA
 * 
 * @param sockIA  socket de communication avec l'IA
 * @param portIA    numéro de port de l'IA
 * @param couleur couleur du joueur
 */
void ConnectIA(int *sockIA, int portIA, int couleur){
    //Faire Connexion avec l'IA, Après avoir fais la demande de partie pour couleur
    struct sockaddr_in addClient;	/* adresse de la socket client connectee */
    int err=0;
    int sockConx;
    sockConx=socketServeur(portIA);
    if(sockConx<1){ 
        return ;
    }
    int sizeAddr = sizeof(struct sockaddr_in);
    *sockIA = accept(sockConx, 
                (struct sockaddr *)&addClient, 
                (socklen_t *)&sizeAddr);
    if (*sockIA < 0) {
        perror(" erreur sur accept Connexion avec l'IA");
        return ;
    }
    //Envoie de la couleur à l'IA
    couleur=htonl(couleur);
    err = send(*sockIA,&couleur,sizeof(int),0);
    if (err <= 0) {
        perror("erreur dans l'envoie de la couleur");
        shutdown(*sockIA, SHUT_RDWR); close(*sockIA);
        return ;
    }
    //*************** FIN Connexion ***********************
}


int main(int argc, char** argv) {

    int sock,                /* descripteur de la socket locale */
        port,                /* port du serveur */
        portIA;              /* port de l'IA */

      char* ipMachServ;        /* pour solution inet_aton */    

    /* verification des arguments */
    // if (argc != 5) {
    //     printf("usage : %s nom/IPServ nomJoueur portServeur portIA \n", argv[0]);
    //     return -1;
    // }
    if (argc != 4) {
        printf("usage : %s nomJoueur portServeur portIA \n", argv[0]);
        return -1;
    }
    char* nom = argv[1];
    ipMachServ="127.0.0.1";
    //ipMachServ = argv[1];
    port = atoi(argv[2]);
    sock = socketClient(ipMachServ,port);
    portIA=atoi(argv[3]);
    if(sock<1){
        return -1;
    }
    
    int couleur = DemandePartie(sock, nom);
    if(couleur<0) return -1;
    int sockIA;
    ConnectIA(&sockIA, portIA, couleur);
    Jouer(sock, couleur, sockIA);

    /* 
    * fermeture connexion et sockets
    */
    shutdown(sockIA, SHUT_RDWR); 
    close(sockIA);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    
    return 0;

}