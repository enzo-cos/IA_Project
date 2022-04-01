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


int DemandePartie(int sock){
    TPartieReq reqPartie;
    int err;
    char nomJoueur[TNOM] = "j2";
    
    reqPartie.idRequest = PARTIE;
    strcpy(reqPartie.nomJoueur,nomJoueur);

    printf("nom joueur : %s \n", reqPartie.nomJoueur);
    printf("id : %d \n", reqPartie.idRequest);

    err = send(sock, &reqPartie, sizeof(TPartieReq), 0);
    if (err <= 0) { 
        perror("(client) erreur sur le send");
        shutdown(sock, SHUT_RDWR); close(sock);
        return -5;
    }

    TPartieRep repPartie;
    err = recv(sock, &repPartie, sizeof(TPartieRep), 0);
        if (err <= 0) {
            perror("(Client) erreur dans la reception repPartie");
            shutdown(sock, SHUT_RDWR); 
            close(sock);
            return -6;
        }
    printf("Mon advsersaire est %s\n",repPartie.nomAdvers);

    if(repPartie.coul == BLANC){
        printf("Je joue les pions Blanc \n");
    }else{
        printf("Je joue les pions Noir \n");
    }

   return repPartie.coul;
// couleur blanc = 0 ; couleur noir =1;


}

bool ReponseCoup(TCoupRep repCoup, int moi){
            bool premiereManche =true;
            
            if(moi ==0){
                switch (repCoup.validCoup) {
                    case 0 :
                        printf("Coup valide \n");
                        break;
                    case 1 : 
                        printf("Coup timeout \n");
                        break;
                    case 2 : 
                        printf("Coup triche \n");
                        break;
                    default:
                        break;
                }
            
                switch (repCoup.propCoup) {
                    case 0 :
                        printf("Continiue\n");
                        break;
                    case 1 : 
                        premiereManche =false;
                        printf("vous avez gagné \n");
                        break;
                    case 2 : 
                        premiereManche =false; 
                        printf("Match Nulle \n");
                        break;
                    case 3 : 
                        premiereManche =false; 
                        printf("Vous avez Perdu \n");
                        break;
                
                }
            }else{
                switch (repCoup.validCoup) {
                    case 0 :
                        printf("Le coup de l'adversaire est valide \n");
                        break;
                    case 1 : 
                        printf("Le coup de l'adversaire est timeout \n");
                        break;
                    case 2 : 
                        printf("Le coup de l'adversaire est triche \n");
                        break;
                    default:
                        break;
                }
            
                switch (repCoup.propCoup) {
                    case 0 :
                        printf("La partie Continiue\n");
                        break;
                    case 1 : 
                       
                        printf("l'adversaire à gagné vous avez perdu \n");
                        break;
                    case 2 : 
                       
                        printf("Match Nulle \n");
                        break;
                    case 3 : 
                        printf("l'adversaire à Perdu vous avez gagné \n");
                        break;
                
                }


            }
            return  premiereManche;
}

void RequeteADV(TCoupReq coupAdv){

    char lg;
    char col;
    char colArr;
    char lgArr;
    switch (coupAdv.action.posPion.lg){
        case 0 :
            lg = '1';
            break;
        case 1 :
            lg ='2';
            break;
        case 2 :
            lg='3';
            break;
    }

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

    switch (coupAdv.action.deplPion.caseDep.lg){
        case 0 :
            lg = '1';
            break;
        case 1 :
            lg ='2';
            break;
        case 2 :
            lg='3';
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

    switch (coupAdv.action.deplPion.caseArr.lg){
        case 0 :
            lgArr = '1';
            break;
        case 1 :
            lgArr ='2';
            break;
        case 2 :
            lgArr='3';
            break;
    }

    switch (coupAdv.typeCoup){
        case 0 :
            printf("l'adversaire à placer un pion en (%c,%c) \n",col,lg);
            break;
        case 1 :
            printf("l'adversaire à déplacer un pion de (%c ,%c) en (%c,%c) \n",col,lg,colArr,lgArr);
            break;
        case 2 :
            printf("l'adversaire à passé son tour \n");
            break;
    }

}


int EnvoyerCoup(int sock, int couleur){
    TCoupReq coup;
    int err;
    coup.action.posPion.col = B;
    coup.action.posPion.lg = DEUX;
    coup.coul  = couleur;
    coup.idRequest = COUP;
    coup.typeCoup = POS_PION;
    char chaine[20];
    bool premiereManche = true; 

    
    while(1){
      


        if(couleur==BLANC){
            printf("appuyer pour envoyer requete \n ");
            scanf("%s",chaine);
            err =  send(sock, &coup, sizeof(TCoupReq), 0);
            if (err <= 0) { 
                perror("(client) erreur sur le send");
                shutdown(sock, SHUT_RDWR); close(sock);
                return -5;
            }

            TCoupRep repCoup;
            err = recv(sock, &repCoup, sizeof(TCoupRep), 0);
                if (err <= 0) {
                    perror("(Client) erreur dans la reception repCOUP");
                    shutdown(sock, SHUT_RDWR); 
                    close(sock);
                    return -6;
                }

           premiereManche= ReponseCoup(repCoup,0);
            printf("premiere manche %d \n",premiereManche);
            if(!premiereManche){
                couleur=NOIR;
                printf("coul blanc %d \n", couleur);
            }
                
                TCoupRep repAdv;
            err = recv(sock, &repAdv, sizeof(TCoupRep), 0);
                if (err <= 0) {
                    perror("(Client) erreur dans la reception coupADV");
                    shutdown(sock, SHUT_RDWR); 
                    close(sock);
                    return -6;
                }

             ReponseCoup(repAdv,1);

            TCoupReq coupAdv;
            err = recv(sock, &coupAdv, sizeof(TCoupReq), 0);
                if (err <= 0) {
                    perror("(Client) erreur dans la reception coupADV");
                    shutdown(sock, SHUT_RDWR); 
                    close(sock);
                    return -6;
                }
            RequeteADV(coupAdv);
            
       
                
            
        } 
        if(couleur==NOIR ){
            TCoupRep repAdv;
            err = recv(sock, &repAdv, sizeof(TCoupRep), 0);
            if (err <= 0) {
                perror("(Client) erreur dans la reception coupADV");
                shutdown(sock, SHUT_RDWR); 
                close(sock);
                return -6;
            }

             ReponseCoup(repAdv,1);
            TCoupReq coupAdv;
            err = recv(sock, &coupAdv, sizeof(TCoupReq), 0);
            if (err <= 0) {
                perror("(Client) erreur dans la reception coupADV");
                shutdown(sock, SHUT_RDWR); 
                close(sock);
                return -6;
            }

            RequeteADV(coupAdv);
                
            printf("appuyer pour envoyer requete \n ");
            scanf("%s",chaine);
           
             err =  send(sock, &coup, sizeof(TCoupReq), 0);
            if (err <= 0) { 
                perror("(client) erreur sur le send");
                shutdown(sock, SHUT_RDWR); close(sock);
                return -5;
            }

            TCoupRep repCoup;
            err = recv(sock, &repCoup, sizeof(TCoupRep), 0);
            if (err <= 0) {
                perror("(Client) erreur dans la reception repCOUP");
                shutdown(sock, SHUT_RDWR); 
                close(sock);
                return -6;
            }
            premiereManche =ReponseCoup(repCoup,0);
             if(!premiereManche){
                couleur=NOIR;
                printf("coul blanc %d \n", couleur);
            }
        }
        
       
    }
    

    return 0;

}

int main(int argc, char** argv) {

    int sock,                /* descripteur de la socket locale */
        port;                /* variables de lecture */
       
      char* ipMachServ;        /* pour solution inet_aton */
    //char* nomMachServ;       /* pour solution getaddrinfo */
    

      /* verification des arguments */
    if (argc != 3) {
        printf("usage : %s nom/IPServ port\n", argv[0]);
        return -1;
    }
    ipMachServ = argv[1]; //nomMachServ = argv[1];
    port = atoi(argv[2]);

    sock = socketClient(ipMachServ,port);

    if(sock<1){
        return -1;
    }
    int couleur;
    couleur = DemandePartie(sock);
    EnvoyerCoup(sock, couleur);
    
    return 0;

}