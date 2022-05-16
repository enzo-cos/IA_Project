import java.net.*;
import java.net.UnknownHostException;
import java.util.Scanner;

import java.io.*;
import java.io.IOException;

public class IA {
    
    /**
     * Mettre à jour le plateau de jeu
     * @param plateau plateau de jeu
     * @param typeCoup type de coup joué
     * @param ligne numéro de ligne d'arrivée
     * @param colonne numéro de colonne d'arrivée
     * @param lDepart numéro de ligne de départ
     * @param colDepart numéro de colonne de départ
     * @param joueur joueur courant
     */
    static void UpdatePlateau(Plateau plateau, int typeCoup, int ligne, int colonne,int lDepart, int colDepart, Joueur joueur){
        if(typeCoup==1) plateau.retirerPion(lDepart, colDepart, joueur);
        plateau.ajouterPion(ligne, colonne, joueur);
        System.out.println("Affichage du plateau : \n"+plateau);
    }

    /**
     * Chercher un coup de type position
     * @param plateau plateau de jeu
     * @param joueur joueur courant
     * @return Case trouvée
     */
    static Case chercherCoupPosition(Plateau plateau, Joueur joueur){
        AlphaBeta alphaBeta=new AlphaBeta();
        return alphaBeta.getResPosition(plateau, joueur);
    }
    /**
     * Chercher un coup de type Déplacement
     * @param plateau plateau de jeu
     * @param joueur joueur courant
     * @return Case trouvée
     */
    static Case[] chercherCoupDeplacement(Plateau plateau, Joueur joueur){
        AlphaBeta alphaBeta=new AlphaBeta();
        return alphaBeta.getResDeplacement(plateau, joueur);
    }

    /**
     * Envoyer un coup au joueur 
     * @param sock sock de com
     * @param plateau plateau de jeau
     * @param joueur Joueur 
     * @param oos
     * @return code erreur
     */
    static int EnvoyerCoup(Socket sock, Plateau plateau, Joueur joueur,DataOutputStream oos){
        int typeCoup,ligne,colonne;
        typeCoup=0;//Position
        //if(joueur.pionRestant==0) typeCoup=1;
        colonne=1;//colonne B
        ligne=1;//ligne 2
        int lDepart=0, colDepart=0;
        //Le Premier coup sera joué au milieu
        if(joueur.nbCoup>0){
            if(joueur.pionRestant>0){
                Case caseP=chercherCoupPosition(plateau,joueur);
                if(caseP==null){
                    System.out.println("Aucune case trouvée");
                    typeCoup=2;
                }else{
                    colonne=caseP.nbColonne;
                    ligne=caseP.nbLigne;
                }
                
            }else{
                typeCoup=1;
                Case[] casesRes=chercherCoupDeplacement(plateau, joueur);
                if(casesRes[0]==null || casesRes[1]==null){
                    System.out.println("Aucune case trouvée");
                    typeCoup=2;
                }else{
                    colDepart=casesRes[0].nbColonne;
                    lDepart=casesRes[0].nbLigne;
                    colonne=casesRes[1].nbColonne;
                    ligne=casesRes[1].nbLigne;
                }
                
            }
            
        }        
        
        //Envoie du coup
        try{
            System.out.println(" coup à Envoyer : \nType Coup : "+typeCoup+", Colonne : "+colonne+", Ligne : "+ligne+"\n");
            System.out.println(joueur);
            // Scanner sca=new Scanner(System.in);
            // sca.nextLine();
            oos.writeInt(typeCoup);//typeCoup
            if(typeCoup==1){
                oos.writeInt(colDepart);
                oos.writeInt(lDepart);
            }else if(typeCoup==2) return 0; //Passe
			oos.writeInt(colonne);//colonne
            oos.writeInt(ligne);//ligne
            UpdatePlateau(plateau, typeCoup, ligne, colonne, lDepart, colDepart, joueur);
		} catch (IOException e) {
		    System.out.println("IO exception1" + e);
            return -1;
		}catch (Exception e){
			System.out.println("Exception1" + e);
            return -1;
		}
        return 0;
    }

    /**
     * Recevoir un coup du client
     * @param sock socket de communication
     * @param plateau plateau de jeu
     * @param joueur joueur courant
     * @param dis
     * @return code d'erreur
     */
    public static int RecevoirCoup(Socket sock, Plateau plateau, Joueur joueur,DataInputStream dis){
        int typeCoup,ligne,colonne;
        int lDepart=-1, colDepart=-1;
        try{
            //Recevoir typeCoup
            typeCoup=dis.readInt();
            if(typeCoup==2) return 0; //Passe
            if(typeCoup==1){ //DEPL
                colDepart=dis.readInt();
                lDepart=dis.readInt();
                System.out.println("Réception coup : \nColonne Départ: "+colDepart+", Ligne Départ: "+lDepart+"\n");
            }
            if(typeCoup>2){
                //FIN Parti
                System.err.println("Fin partie");
                return 3;
            }
            colonne=dis.readInt();
            ligne=dis.readInt();
            System.out.println("Réception coup : \nType Coup : "+typeCoup+", Colonne : "+colonne+", Ligne : "+ligne+"\n");
            UpdatePlateau(plateau, typeCoup, ligne, colonne, lDepart, colDepart, joueur);
		} catch (IOException e) {
		    System.out.println("IO exception2" + e);
            return -2;
		}catch (Exception e){
			System.out.println("Exception2" + e);
            return -2;
		}

        return 0;
    }

    /**
     * Recevoir la couleur de jeu
     * @param sock
     * @param dis
     * @return couleur
     */
    public static int RecevoirCouleur(Socket sock,DataInputStream dis){
        int res=-1;
        try{
            res=dis.readInt()+1;

        } catch (IOException e) {
            System.out.println("IO exception" + e);
        }catch (Exception e){
            System.out.println("Exception1" + e);
        }
        return res;
    }
    public static void main(String[] args) {

        if (args.length != 2){
		    System.out.println("usage : java IA nom/IPServ port\n");
		    System.exit(1);
		}
		Socket socket ;
		// References de la socket
		String hote = args[0] ;
		int port = Integer.parseInt(args[1]);
		int err=0;
        Joueur myJoueur=new Joueur(1);
        Joueur advJoueur=new Joueur(2);
        Plateau plateau= new Plateau();
        plateau.setJoueur(myJoueur, advJoueur);
        
        try {
		    socket = new Socket(hote, port) ;
            InputStream is = socket.getInputStream();
			DataInputStream dis = new DataInputStream(is);
            OutputStream os = socket.getOutputStream();
			DataOutputStream oos = new DataOutputStream(os);
            myJoueur.numJoueur=RecevoirCouleur(socket,dis);
            if(myJoueur.numJoueur<0){
                System.err.println("Erreur dans la couleur reçue");
                return;
            }
            System.out.println(myJoueur);
            if(myJoueur.numJoueur==2) advJoueur.numJoueur=1;
            //1ère parti
            System.out.println("Première Partie");
            if(myJoueur.numJoueur==1){ 
                err=EnvoyerCoup(socket,plateau,myJoueur,oos);
            }
            while(err==0){
                err=RecevoirCoup(socket, plateau, advJoueur,dis);
                //if(err<0) break;
                if(err==3){
                    //Fin partie
                    System.out.println("Plateau fin de partie : \n"+plateau);
                    //break;
                }else{
                    err=EnvoyerCoup(socket,plateau,myJoueur,oos);
                }
                //if(err<0) break;
            }
            //Deuxième partie
            System.out.println("Deuxième Partie");
            err=0;
            plateau.clear();
            myJoueur.clear();
            advJoueur.clear();
            if(myJoueur.numJoueur==2) err=EnvoyerCoup(socket,plateau,myJoueur,oos);
            while(err==0){
                err=RecevoirCoup(socket, plateau, advJoueur,dis);
                //if(err<0) break;
                if(err==3){
                    //Fin parti
                    System.out.println("Plateau fin de partie : \n"+plateau);
                    //break;
                }else{
                    err=EnvoyerCoup(socket,plateau,myJoueur,oos);
                }
                //if(err<0) break;
            }
		} catch (UnknownHostException e) { 
		    System.out.println("Unknown host" + e);
		} catch (IOException e) {
		    System.out.println("IO exception" + e);
		} catch (Exception e){
			System.out.println("Exception" + e);
		}
    
    }
}

//Class Joueur
class Joueur{
    int numJoueur;
    int nbCoup;
    int pionRestant;

    //Constructeur
    public Joueur(int num){
        this.numJoueur=num;
        nbCoup=0;
        pionRestant=8;
    }
    //Constructeur copie
    public Joueur(Joueur j){
        this.numJoueur=j.numJoueur;
        this.nbCoup=j.nbCoup;
        this.pionRestant=j.pionRestant;
    }

    public void clear(){
        nbCoup=0;
        pionRestant=8;
    }

    public String toString(){
        return "Joueur "+this.numJoueur+", nbCoup = "+nbCoup+", pionRestants = "+pionRestant;
    }
}

//Class Case
class Case{
    int nbLigne;
    int nbColonne;
    int tabPion[];
    int nbPion;
    /* Constructeur  */
    public Case(int lg,int col){
        nbLigne=lg;
        nbColonne=col;
        tabPion=new int[3];  
        nbPion=0;  
    }

    /**
     * Ajouter pion dans la case
     * @param joueur joueur actif
     * @return code erreur
     */
    public int ajouterPion(Joueur joueur){
        int i=0;
        //vérification 
        while(tabPion[i]!=0){
            i++;
            if(i>2){
                System.out.println("Case rempli");
                return -1;
            }
        }
        tabPion[i]=joueur.numJoueur;
        joueur.nbCoup++;
        if(joueur.pionRestant>0) joueur.pionRestant--;
        nbPion++;
        return 0;
    }

    /**
     * Retirer pion dans la case
     * @param joueur joueur actif
     * @return code erreur
     */
    public int retirerPion(Joueur joueur){
        int i=2;
        while(tabPion[i]==0){
            i--;
            if(i<0){
                System.out.println("Case Vide");
                return -1;
            }
        }
        tabPion[i]=0;
        nbPion--;
        return 0;
    }


    /**
     * Calculer la valeur (Poids) d'une case
     * @param joueur courant
     * @param typeCoup type du coup à jouer
     * @param plateau plateau de la partie en cours
     * @return Valeur de la case
     */
    public int getValeurCase(Joueur joueur, int typeCoup, Plateau plateau){
        int res=10000; //initialisation du poids
        int nbPionJoueur=0; //nbPionsJoueur
        boolean joueurTop=true;
        boolean caseVide=false;
        //parcours de la case
        if(nbPion==0) caseVide=true;
        //Parcours case
        for(int k=0;k<nbPion;k++){
            if(tabPion[k]==joueur.numJoueur){
                nbPionJoueur++;
                if(k==nbPion-1) joueurTop=true;
            } 
        }

        if(caseVide){
            if(typeCoup==0){ //Position
                res+=5000;
                if(nbLigne!=1 || nbColonne!=1) res+=500; //Pas d'angle
            }else{ //déplacement
                if(plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), nbLigne, nbColonne)!=null){ //Si on peut déplacer un pion dans cette case
                    res+=1000;
                    if(nbLigne!=1 || nbColonne!=1) res+=4000;
                }else res-=1000;
                
            }
        }else{
            if(nbPionJoueur==2 && nbPion==2){ //si on a deux pions sur une case contenant seulement deux pions
                if(typeCoup==0 || plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), nbLigne, nbColonne)!=null){ //Si on peut déplacer un pion dans cette case
                    res+=250000;
                }else res+=200000;
            }
            if(nbPionJoueur==1 && nbPion==1){ //si on a un pion seul
                if(typeCoup==0 || plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), nbLigne, nbColonne)!=null){ //Si on peut déplacer un pion dans cette case
                    res+=50000;
                }else res+=30000;
            }
            if(joueurTop){ //si le joueur a une case au dessus
                res+=10000;
            }
            if(nbPion==2 && (typeCoup==0 || plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), nbLigne, nbColonne)!=null)) res+=3000;
            if(!joueurTop){
                if(nbPion==2 && nbPionJoueur==0){ //Il ne faut pas que l'adversaire ait 2 pions dans la même case
                    if(typeCoup==0 || plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), nbLigne, nbColonne)!=null){ //Si on peut déplacer un pion dans cette case
                        res-=250000; //Bloquer l'adversaire
                    }else res-=300000;
                }
                if(nbPion==2 && nbPionJoueur==1){
                    res+=5000;
                }
                res-=5000;
            }
            if(nbPion==3){
                if(joueurTop) res+=50000;
                else res-=10000;
            }
            if(nbPion==3 && nbPionJoueur==3) res+=3700000; //Victoire
            if(nbPion==3 && nbPionJoueur==0) res-=2000000; //Défaite
            if(nbPion==3 && nbPionJoueur==1 && joueurTop) res+=300000; //Bloquer

        } 
        return res;
    }

    /**
     * Affichage case
     */
    public String toString(){
        int i=0;
        String s="";
        if(nbLigne<0){
            return "Case inexistante";
        }
        while(i<3){
            if(tabPion[i]==1){
                s+="o";
            }else if(tabPion[i]==2){
                s+="x";
            }else if(tabPion[i]==0){
                s+=" ";
            }
            i++;
        }
        return s;
    }


}

//class Plateau
class Plateau{
    Case plateau[][];
    int taille=3;
    Joueur myJoueur;
    Joueur advJoueur;

    //Constructeur
    public Plateau(){
        plateau = new Case[taille][taille];
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                plateau[i][j]=new Case(i, j);
            }
        }
    }
    //Constructeur copie
    public Plateau(Plateau plat){
        this.taille=3;
        plateau = new Case[taille][taille];
        this.myJoueur=new Joueur(plat.myJoueur);
        this.advJoueur=new Joueur(plat.advJoueur);
        //plateau=plat.plateau.clone();
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                //plateau[i][j]=plat.plateau[i][j].clone();
                plateau[i][j]=new Case(i, j);
                plateau[i][j].tabPion[0]=plat.plateau[i][j].tabPion[0];
                plateau[i][j].tabPion[1]=plat.plateau[i][j].tabPion[1];
                plateau[i][j].tabPion[2]=plat.plateau[i][j].tabPion[2];
                plateau[i][j].nbPion=plat.plateau[i][j].nbPion;
            }
        }
    }

    //Set les joueur 1 et 2
    public void setJoueur(Joueur joueur1,Joueur joueur2){
        this.myJoueur=joueur1;
        this.advJoueur=joueur2;
    }

    /**
     * Initialisation
     */
    public void clear(){
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                plateau[i][j]=new Case(i, j);
            }
        }
    }

    public Joueur getAdversaire(){
        return new Joueur(advJoueur);
    }

    /**
     * Ajouter un pion dans une case
     * @param lg ligne de la case
     * @param col colonne de la case
     * @param joueur joueur actif
     */
    public void ajouterPion(int lg, int col, Joueur joueur){
        plateau[lg][col].ajouterPion(joueur);
    }
    /**
     * Retirer un pion d'une case
      * @param lg ligne de la case
     * @param col colonne de la case
     * @param joueur joueur actif
     */
    public void retirerPion(int lg, int col, Joueur joueur){
        plateau[lg][col].retirerPion(joueur);
    }

    /**
     * Obtenir les cases où nos pions sont déplaçables
     * @param joueur joueur demandant
     * @return Tableau de cases
     * Utilisation : parcourir le tableau en vérifiant que case!=null
     */
    public Case[] pionDeplacable(Joueur joueur){
        int cpt=0;
        Case[] cases=new Case[taille*taille+1];
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                int nb=plateau[i][j].nbPion;
                //Pion placé au dessus
               if(nb>0 && plateau[i][j].tabPion[nb-1]==joueur.numJoueur){
                    cases[cpt]=plateau[i][j];
                    cpt++;
               }
            }
        }
        return cases;
    }

    /**
     * Obtenir les cases disponibles non pleines pour coup Positon
     * @return Tableau de cases
     * Utilisation : parcourir le tableau en vérifiant que case!=null
     */
    public Case[] caseAjouerPos(){
        int cpt=0;
        Case[] cases=new Case[taille*taille+1];
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
               if(plateau[i][j].nbPion<3){
                    cases[cpt]=plateau[i][j];
                    cpt++;
               }
            }
        }

        return cases;
    }

    /**
     * Obtenir les cases disponibles pour coup Déplacement, avec un pion déplaçable à côté
     * @param joueur Joueur voulant déplacer un pion
     * @return Tableau de cases
     * Utilisation : parcourir le tableau en vérifiant que case!=null
     */
    public Case[] caseAjouerDepl(Joueur joueur){
        int cpt=0;
        Case[] cases=new Case[taille*taille+1];
        Case[] pions=pionDeplacable(joueur);

        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
               if(plateau[i][j].nbPion<3){
                   //Case non pleine
                   if(verifCaseVoisine(pions,i, j)!=null){
                        cases[cpt]=plateau[i][j];
                        cpt++;
                   }
                    
               }
            }
        }
        return cases;
    }
    

    /**
     * Vérifier si une case a un pion déplaçable à coté d'elle
     * @param l ligne de la case
     * @param c colonne de la case
     * @return la case où le pion peut être retiré, null si il n'y en a pas
     */
    public Case verifCaseVoisine(Case[] pions, int l, int c){
        //boolean b=false;
        Case res=null;
        int k=0;
        //ligne=l && col 0 -> 2
        while(pions[k]!=null){
            for(int ligne=l-1;ligne<l+2;ligne++){
                for(int col=c-1;col<c+2;col++){
                    if(ligne<0 || ligne >2 || col <0 || col>2) continue;
                    if((ligne==l && col==c) || (ligne!=l && col!= c)) continue;
                    if(pions[k].nbLigne==ligne && pions[k].nbColonne==col) res=pions[k];
                }
            }
            
            k++;
        }
        return res;
    }
    /**
     * Affichage du plateau de jeu
     */
    public String toString(){
        String res="    A   B   C  \n";
        int col=0;
        res+="  _____________\n";
        for(int lg=0;lg<3;lg++){
            res+="  |   |   |   |\n"+lg+" ";
            while(col<3){
                res+="|"+plateau[lg][col].toString();
                col++;
            }
            col=0;
            res+="|\n  |___|___|___|\n";
        }

        return res;
    }

}

//Class AlphaBeta
class AlphaBeta{
    private static final int PROFONDEUR_MAX = 4;

    /** Constructeur */
	public AlphaBeta() {
	}

    /**
     * Obtenir le résultat d'un coup position
     * @param plateau plateau de jeu
     * @param joueur joueur courant
     * @return Casse trouvée
     */
    public Case getResPosition(Plateau plateau, Joueur joueur){
        System.out.println("Préparation position");
        Joueur JoueurCopie=new Joueur(joueur);
        Plateau platCopie2=new Plateau(plateau);
        Case[] cases=platCopie2.caseAjouerPos();
        int valJouer=0;
        int i=0;
        int nbCase=0;
        while(cases[i]!=null){
			try {
                Plateau platCopie=new Plateau(platCopie2);
                platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, JoueurCopie);
                int curr=alphabeta(platCopie,JoueurCopie,PROFONDEUR_MAX,0);
                if(curr>=valJouer){
                    valJouer=curr;
                    nbCase=i;
                }
				i++;
			} catch (Exception e) {
                System.err.println("ErrGetRes");
				e.printStackTrace();
			}
		}
        if(i==0) return null;
        return cases[nbCase];
    }

    /**
     * Obtenir le résultat d'un coup déplacement
     * @param plateau plateau de jeu
     * @param joueur joueur courant
     * @return Casse trouvée
     */
    public Case[] getResDeplacement(Plateau plateau, Joueur joueur){
        System.out.println("Préparation déplacement");
        Joueur JoueurCopie=new Joueur(joueur);
        Plateau platCopie2=new Plateau(plateau);
        //pionD
        Case[] cases=platCopie2.caseAjouerDepl(joueur);
        Case[] resCase=new Case[2];
        int valJouer=0;
        int i=0;
        int nbCase=0;
        while(cases[i]!=null){
			try {
                Plateau platCopie=new Plateau(platCopie2);
                
                platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, JoueurCopie);
                int curr=alphabeta(platCopie,JoueurCopie,PROFONDEUR_MAX,1);
                if(curr>=valJouer){
                    valJouer=curr;
                    nbCase=i;
                }
				i++;
               
			} catch (Exception e) {
                System.err.println("ErrGetRes");
				e.printStackTrace();
			}
		}
        if(i==0) return resCase;
        //Case départ
        resCase[0]=plateau.verifCaseVoisine(plateau.pionDeplacable(joueur), cases[nbCase].nbLigne,  cases[nbCase].nbColonne);
        //Case arrivée
        resCase[1]=cases[nbCase];
        return resCase;
    }

    //alphabeta
    private int alphabeta(Plateau plat, Joueur joueur, int profondeur, int typeCoup){
		int alpha =0;
		int beta=10000000;
		return this.min(plat, joueur, profondeur, alpha, beta,typeCoup);
	}

    //Algo Min 
    private int min(Plateau plateau, Joueur joueur,  int profondeur, int alpha, int beta,int typeCoup){
        Case[] cases;
        if(typeCoup==0) cases=plateau.caseAjouerPos();
        else cases = plateau.caseAjouerDepl(joueur);
        Joueur adversaireCopie=plateau.getAdversaire();
		if(profondeur != 0){
			int valeurDeJeu = 10000000;
			int i=0;
            while(cases[i]!=null){
				try {
                    Plateau platCopie=new Plateau(plateau);
                    platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, adversaireCopie);

					valeurDeJeu = Math.min(valeurDeJeu, this.max(platCopie, joueur, profondeur-1, alpha, beta,typeCoup));
						
                    if(alpha >= valeurDeJeu){
                        return valeurDeJeu; // Coupure
                    }
						
		            beta = Math.min(beta, valeurDeJeu);
						
				} catch (Exception e) {
                    System.out.println("minError");
					e.printStackTrace();
				}
                i++;
			}
			return valeurDeJeu;
		}else{
			return this.getVal(plateau,joueur,typeCoup);
		}
	}

    //algo max
    private int max(Plateau plateau, Joueur joueur, int profondeur, int alpha, int beta,int typeCoup){
		Case[] cases;
        if(typeCoup==0) cases=plateau.caseAjouerPos();
        else cases = plateau.caseAjouerDepl(joueur);
		if(profondeur != 0){
			int valeurDeJeu = 10;
			int i=0;
            while(cases[i]!=null){
				try {
                    Plateau platCopie=new Plateau(plateau);
                    platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, joueur);

					valeurDeJeu = Math.max(valeurDeJeu, this.min(platCopie, joueur, profondeur-1, alpha, beta,typeCoup));
						
                    if(valeurDeJeu >= beta){
                        return valeurDeJeu; // Coupure
                    }
						
		            alpha = Math.max(alpha, valeurDeJeu);
						
				} catch (Exception e) {
                    System.out.println("maxError");
					e.printStackTrace();
				}
                i++;
			}
			return valeurDeJeu;
		}else{
			return this.getVal(plateau,joueur, typeCoup);
		}
	}

    //Obtenir le résultat
    public int getVal(Plateau plat, Joueur joueur, int typeCoup){
        int res=0;
       
        for(int i=0;i<plat.taille;i++){
            for(int j=0;j<plat.taille;j++){
                res+=plat.plateau[i][j].getValeurCase(joueur, typeCoup,plat);
            }
        }
        return res;
    }
}