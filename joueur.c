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
    char nomJoueur[TNOM] = "j1";
    
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
    coup.action.posPion.col = A;
    coup.action.posPion.lg = DEUX;
    coup.coul  = BLANC;
    coup.idRequest = COUP;
    coup.typeCoup = POS_PION;
   int i=0;
    while(1){

        
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

        TCoupReq coupAdv;
        err = recv(sock, &coupAdv, sizeof(TCoupReq), 0);
        if (err <= 0) {
            perror("(Client) erreur dans la reception coupADV");
            shutdown(sock, SHUT_RDWR); 
            close(sock);
            return -6;
        }
        i++;
        printf(" i : %d\n", i);
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