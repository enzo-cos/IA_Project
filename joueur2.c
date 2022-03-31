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

    err = send(sock, &reqPartie, sizeof(struct TPartieReq*), 0);
    if (err <= 0) { 
        perror("(client) erreur sur le send");
        shutdown(sock, SHUT_RDWR); close(sock);
        return -5;
    }

    TPartieRep repPartie;
    err = recv(sock, &repPartie, sizeof(struct TPartieRep*), 0);
        if (err <= 0) {
            perror("(Client) erreur dans la reception repPartie");
            shutdown(sock, SHUT_RDWR); 
            close(sock);
            return -6;
        }

    if(repPartie.coul == BLANC){
        printf("couleur Blanc");
    }else{
        printf("couleur Noir");
    }

return 0;

}


int EnvoyerCoup(int sock){
    TCoupReq coup;
    int err;
    coup.action.posPion.col = DEUX;
    coup.action.posPion.lg = B;
    coup.coul  = NOIR;
    coup.idRequest = COUP;
    coup.typeCoup = POS_PION;
   
    while(1){
        err =  send(sock, &coup, sizeof(struct TCoupReq*), 0);
        if (err <= 0) { 
            perror("(client) erreur sur le send");
            shutdown(sock, SHUT_RDWR); close(sock);
            return -5;
        }

        TCoupRep repCoup;
        err = recv(sock, &repCoup, sizeof(struct TCoupRep*), 0);
            if (err <= 0) {
                perror("(Client) erreur dans la reception repCOUP");
                shutdown(sock, SHUT_RDWR); 
                close(sock);
                return -6;
            }

        TCoupReq coupAdv;
        err = recv(sock, &coupAdv, sizeof(struct TCoupReq*), 0);
            if (err <= 0) {
                perror("(Client) erreur dans la reception coupADV");
                shutdown(sock, SHUT_RDWR); 
                close(sock);
                return -6;
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

    DemandePartie(sock);
    EnvoyerCoup(sock);
    
    return 0;

}