import java.net.*;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.io.*;
import java.io.IOException;

public class IA {
    
    static void UpdatePlateau(Plateau plateau, int typeCoup, int ligne, int colonne,int lDepart, int colDepart, Joueur joueur){
        if(typeCoup==1) plateau.retirerPion(lDepart, colDepart, joueur);
        plateau.ajouterPion(ligne, colonne, joueur);
        System.out.println("Affichage du plateau : \n"+plateau);
    }

    static Case chercherCoupPosition(Plateau plateau, Joueur joueur){
        AlphaBeta alphaBeta=new AlphaBeta();
        return alphaBeta.getResPosition(plateau, joueur);
    }
    static Case[] chercherCoupDeplacement(Plateau plateau, Joueur joueur){
        AlphaBeta alphaBeta=new AlphaBeta();
        return alphaBeta.getResDeplacement(plateau, joueur);
    }

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

    public static int RecevoirCoup(Socket sock, Plateau plateau, Joueur joueur,DataInputStream dis){
        int typeCoup,ligne,colonne;
        int lDepart=-1, colDepart=-1;
        try{
            //Recevoir typeCoup
            typeCoup=dis.readInt();
            System.out.println("réception typeCoup : "+typeCoup);
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

    // public Case copie(){
    //     return this.clone();
    // }

    /**
     * Ajouter pion dans la case
     * @param joueur joueur actif
     * @return code erreur
     */
    public int ajouterPion(Joueur joueur){
        //System.out.println("ajout Pion dans la case c "+this.nbColonne+ ", l "+this.nbLigne);
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
                s+="x";
            }else if(tabPion[i]==2){
                s+="o";
            }else if(tabPion[i]==0){
                s+=" ";
            }
            i++;
        }
        return s;
    }


}

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
    public Case[] pionDeplacable(int joueur){
        int cpt=0;
        Case[] cases=new Case[taille*taille+1];
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                int nb=plateau[i][j].nbPion;
                //Pion placé au dessus
               if(nb>0 && plateau[i][j].tabPion[nb-1]==joueur){
                    cases[cpt]=plateau[i][j];
                    cpt++;
               }
            }
        }
        // for(int i=0;i<10;i++){
        //     System.out.println(cases[i]);
        // }
        return cases;
    }

    /**
     * Obtenir les cases disponibles non pleines
     * @return Tableau de cases
     * Utilisation : parcourir le tableau en vérifiant que case!=null
     */
    public Case[] caseAjouer(){
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

class AlphaBeta{
    private static final int PROFONDEUR_MAX = 3;

    /** Constructeur */
	public AlphaBeta() {
	}

    public Case getResPosition(Plateau plateau, Joueur joueur){
        System.out.println("Préparation position");
        Joueur JoueurCopie=new Joueur(joueur);
        int res=0;
        Plateau platCopie2=new Plateau(plateau);
        Case[] cases=platCopie2.caseAjouer();
        int valJouer=0;
        int i=0;
        int nbCase=0;
        while(cases[i]!=null){
			try {
                Plateau platCopie=new Plateau(platCopie2);
                platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, JoueurCopie);
                int curr=alphabeta(platCopie,JoueurCopie,PROFONDEUR_MAX);
                if(curr>=valJouer){
                    valJouer=curr;
                    nbCase=i;
                }
				i++;
			} catch (Exception e) {
                System.out.println("ErrGetRes");
				e.printStackTrace();
			}
		}
        return cases[nbCase];
    }

    public Case[] getResDeplacement(Plateau plateau, Joueur joueur){
        System.out.println("Préparation déplacement");
        Joueur JoueurCopie=new Joueur(joueur);
        int res=0;
        Plateau platCopie2=new Plateau(plateau);
        Case[] cases=platCopie2.caseAjouer();
        Case[] resCase=new Case[2];
        int valJouer=0;
        int i=0;
        int nbCase=0;
        while(cases[i]!=null){
			try {
                Plateau platCopie=new Plateau(platCopie2);
                platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, JoueurCopie);
                int curr=alphabeta(platCopie,JoueurCopie,PROFONDEUR_MAX);
                if(curr>=valJouer){
                    valJouer=curr;
                    nbCase=i;
                }
				i++;
			} catch (Exception e) {
                System.out.println("ErrGetRes");
				e.printStackTrace();
			}
		}
        resCase[0]=platCopie2.plateau[0][0];//CHANGE
        resCase[1]=cases[nbCase];
        return resCase;
    }

    private int alphabeta(Plateau plat, Joueur joueur, int profondeur){
		int alpha =0;
		int beta=300;
		return this.min(plat, joueur, profondeur, alpha, beta);
	}

    private int min(Plateau plateau, Joueur joueur,  int profondeur, int alpha, int beta){
        Case[] cases=plateau.caseAjouer();
        Joueur adversaireCopie=plateau.getAdversaire();
		if(profondeur != 0){
			int valeurDeJeu = 300;
			int i=0;
            while(cases[i]!=null){
				try {
                    Plateau platCopie=new Plateau(plateau);
                    platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, adversaireCopie);

					valeurDeJeu = Math.min(valeurDeJeu, this.max(platCopie, joueur, profondeur-1, alpha, beta));
						
                    if(alpha >= valeurDeJeu){
                        return valeurDeJeu; // Coupure alpha
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
			return this.getVal(plateau,joueur);
		}
	}

    private int max(Plateau plateau, Joueur joueur, int profondeur, int alpha, int beta){
		Case[] cases=plateau.caseAjouer();
		if(profondeur != 0){
			int valeurDeJeu = 300;
			int i=0;
            while(cases[i]!=null){
				try {
                    Plateau platCopie=new Plateau(plateau);
                    platCopie.ajouterPion(cases[i].nbLigne,cases[i].nbColonne, joueur);

					valeurDeJeu = Math.max(valeurDeJeu, this.min(platCopie, joueur, profondeur-1, alpha, beta));
						
                    if(valeurDeJeu >= beta){
                        return valeurDeJeu; // Coupure alpha
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
			return this.getVal(plateau,joueur);
		}
	}

    public int getVal(Plateau plat, Joueur joueur){
        int res=0;
        for(int i=0;i<plat.taille;i++){
            for(int j=0;j<plat.taille;j++){
                res+=joueur.numJoueur*plat.plateau[i][j].nbPion;
            }
        }
       // System.out.println("Valeur retournée : "+res);
        return res;
    }
}