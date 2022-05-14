import java.net.*;
import java.net.UnknownHostException;
import java.io.*;
import java.io.IOException;

public class IA {

    static void UpdatePlateau(Plateau plateau, int typeCoup, int ligne, int colonne,int lDepart, int colDepart, int joueur){
        if(typeCoup==1) plateau.retirerPion(lDepart, colDepart, joueur);
        plateau.ajouterPion(ligne, colonne, joueur);
        System.out.println("Affichage du plateau : \n"+plateau);
    }

    static int EnvoyerCoup(Socket sock, Plateau plateau, int joueur,DataOutputStream oos){
        int typeCoup,ligne,colonne;
        typeCoup=0;//Position
        colonne=0;//colonne A
        ligne=2;//ligne 3
        int lDepart=0, colDepart=0;
        try{
            oos.writeInt(typeCoup);//typeCoup
            if(typeCoup==1){
                oos.writeInt(colDepart);
                oos.writeInt(lDepart);
            }else if(typeCoup==2) return 0; //Passe
			oos.writeInt(colonne);//colonne
            oos.writeInt(ligne);//ligne
            System.out.println(" coup Envoyé : \nType Coup : "+typeCoup+",Ligne : "+ligne+", Colonne : "+colonne+"\n");
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

    public static int RecevoirCoup(Socket sock, Plateau plateau, int joueur,DataInputStream dis){
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
            //joueur=dis.readInt()+1;//couleur
            System.out.println("Réception coup : \nType Coup : "+typeCoup+"Colonne : "+colonne+", Ligne : "+ligne+"\n");
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

        int myJoueur=1;
        int advJoueur=2;
        if (args.length != 2){
		    System.out.println("usage : java IA nom/IPServ port\n");
		    System.exit(1);
		}
		Socket socket ;
		// References de la socket
		String hote = args[0] ;
		int port = Integer.parseInt(args[1]);
		int err=0;
        Plateau plateau= new Plateau();
        try {
		    socket = new Socket(hote, port) ;
            InputStream is = socket.getInputStream();
			DataInputStream dis = new DataInputStream(is);
            OutputStream os = socket.getOutputStream();
			DataOutputStream oos = new DataOutputStream(os);
            myJoueur=RecevoirCouleur(socket,dis);
            if(myJoueur<0){
                System.err.println("Erreur dans la couleur reçue");
                return;
            }
            System.out.println(myJoueur);
            if(myJoueur==2) advJoueur=1;
            //1ère parti
            System.out.println("Première Partie");
            if(myJoueur==1) err=EnvoyerCoup(socket,plateau,myJoueur,oos);
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
            if(myJoueur==2) err=EnvoyerCoup(socket,plateau,myJoueur,oos);
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

class Case{
    int nbLigne;
    int nbColonne;
    int tabPion[];
    int nbPion;
    /* Constructeur  */
    // public Case(){
    //     nbLigne=-1;
    //     nbColonne=-1;
    //     tabPion=new int[3];  
    //     nbPion=0;  
    // }
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
    public int ajouterPion(int joueur){
        int i=0;
        //vérification 
        while(tabPion[i]!=0){
            i++;
            if(i>2){
                System.out.println("Case rempli");
                return -1;
            }
        }
        tabPion[i]=joueur;
        nbPion++;
        return 0;
    }

    /**
     * Retirer pion dans la case
     * @param joueur joueur actif
     * @return code erreur
     */
    public int retirerPion(int joueur){
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

    public Plateau(){
        plateau = new Case[taille][taille];
        //utile ?
        for(int i=0;i<taille;i++){
            for(int j=0;j<taille;j++){
                plateau[i][j]=new Case(i, j);
            }
        }
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

    /**
     * Ajouter un pion dans une case
     * @param lg ligne de la case
     * @param col colonne de la case
     * @param joueur joueur actif
     */
    public void ajouterPion(int lg, int col, int joueur){
        plateau[lg][col].ajouterPion(joueur);
    }
    /**
     * Retirer un pion d'une case
      * @param lg ligne de la case
     * @param col colonne de la case
     * @param joueur joueur actif
     */
    public void retirerPion(int lg, int col, int joueur){
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
               if(nb>0 && plateau[i][j].tabPion[nb-1]==joueur){
                    cases[cpt]=plateau[i][j];
                    cpt++;
               }
            }
        }
        for(int i=0;i<10;i++){
            System.out.println(cases[i]);
        }
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