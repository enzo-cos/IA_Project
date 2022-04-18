import java.net.*;
import java.net.UnknownHostException;
import java.io.*;
import java.io.IOException;

public class IA {

    static void UpdatePlateau(Plateau plateau, int typeCoup, int ligne, int colonne,int lDepart, int colDepart, int joueur){
        if(typeCoup==1) plateau.retirerPion(lDepart, colDepart, joueur);
        System.out.println("up l +"+ligne+", c +"+colonne);
        plateau.ajouterPion(ligne, colonne, joueur);
        //Else : passe
    }

    static int EnvoyerCoup(Socket sock, Plateau plateau, int joueur,DataOutputStream oos){
        int typeCoup,ligne,colonne;
        typeCoup=colonne=0;
        ligne=2;
        int lDepart=-1, colDepart=-1;

        try{
			// OutputStream os = sock.getOutputStream();
			// DataOutputStream oos = new DataOutputStream(os);
            System.out.println("envoie typeCoup : "+typeCoup);
            oos.writeInt(typeCoup);//typeCoup
            if(typeCoup==1){
                System.out.println("Depl");
                oos.writeInt(lDepart);
                oos.writeInt(colDepart);
            }else if(typeCoup==2) return 1; //Passe
            System.out.println("envoie Ligne : "+ligne);
			oos.writeInt(ligne);//ligne
            System.out.println("envoie colonne : "+colonne);
			oos.writeInt(colonne);//colonne
            System.out.println(" coup Envoyé : \nType Coup : "+typeCoup+",Ligne : "+ligne+", Colonne : "+colonne+"\n");

            //UpdatePlateau(plateau, typeCoup, ligne, colonne, lDepart, colDepart, joueur);

            // os.close();
            // oos.close();
		} catch (IOException e) {
		    System.out.println("IO exception1" + e);
		}catch (Exception e){
			System.out.println("Exception1" + e);
		}
        return 0;
    }

    public static int RecevoirCoup(Socket sock, Plateau plateau, int joueur,DataInputStream dis){
        int typeCoup,ligne,colonne;
        int lDepart=-1, colDepart=-1;
        try{
            // InputStream is = sock.getInputStream();
			// DataInputStream dis = new DataInputStream(is);
            //Recevoir typeCoup
            typeCoup=dis.readInt();
            if(typeCoup==2) return 1; //Passe
            if(typeCoup==1){
                lDepart=dis.readInt();
                colDepart=dis.readInt();
            }
            if(typeCoup>2){
                //FIN Parti
                System.err.println("Fin partie");
                return 3;
            }
            ligne=dis.readInt();
            colonne=dis.readInt();
            System.out.println("Réception coup : \nType Coup : "+typeCoup+",Ligne : "+ligne+", Colonne : "+colonne+"\n");
            joueur=dis.readInt()+1;//couleur
            UpdatePlateau(plateau, typeCoup, ligne, colonne, lDepart, colDepart, joueur);
            // is.close();
            // dis.close();
		} catch (IOException e) {
		    System.out.println("IO exception2" + e);
		}catch (Exception e){
			System.out.println("Exception2" + e);
		}

        return 0;
    }

    public static int RecevoirCouleur(Socket sock,DataInputStream dis){
        int res=-1;
        try{
            // InputStream is = sock.getInputStream();
            // DataInputStream dis = new DataInputStream(is);
            res=dis.readInt()+1;
            // is.close();
            // dis.close();
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
            while(true){
                err=RecevoirCoup(socket, plateau, advJoueur,dis);
                if(err==3){
                    //Fin parti
                    break;
                }
                err=EnvoyerCoup(socket,plateau,myJoueur,oos);
            }
            //Deuxième partie
            System.out.println("Deuxième Partie");
            if(myJoueur==2) err=EnvoyerCoup(socket,plateau,myJoueur,oos);
            while(true){
                err=RecevoirCoup(socket, plateau, advJoueur,dis);
                if(err==3){
                    //Fin parti
                    break;
                }
                err=EnvoyerCoup(socket,plateau,myJoueur,oos);
            }
		} catch (UnknownHostException e) { 
		    System.out.println("Unknown host" + e);
		} catch (IOException e) {
		    System.out.println("IO exception" + e);
		} catch (Exception e){
			System.out.println("Exception" + e);
		}
        

        //Création plateau
        //Ligne 0,1,2 | Colonne 0(A),1(B),2(C)
        
        plateau.ajouterPion(1, 1, 1);
        plateau.ajouterPion(1, 1, 2);
        plateau.ajouterPion(1, 1, 1);
        plateau.ajouterPion(1, 2, 2);
        plateau.ajouterPion(2, 1, 1);plateau.ajouterPion(2, 1, 1);
        System.out.println(plateau);

    }
}

class Case{
    int nbLigne;
    int nbColonne;
    int tabPion[];
    int nbPion;

    public Case(){
        nbLigne=0;
        nbColonne=0;
        tabPion=new int[3];  
        nbPion=0;  
    }
    public Case(int lg,int col){
        nbLigne=lg;
        nbColonne=col;
        tabPion=new int[3];  
        nbPion=0;  
    }

    public int ajouterPion(int joueur){
        int i=0;
        while(tabPion[i]!=0){
            i++;
            if(i>2){
                System.out.println("Case rempli");
                return -1;
            }
        }
        if(joueur!=1 && joueur!=2) System.out.println("JOUEUR INCONNU ");//Impossible
        tabPion[i]=joueur;
        return 0;
    }

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
        return 0;
    }

    public String toString(){
        int i=0;
        String s="";
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

    public void ajouterPion(int lg, int col, int joueur){
        plateau[lg][col].ajouterPion(joueur);
    }
    public void retirerPion(int lg, int col, int joueur){
        plateau[lg][col].retirerPion(joueur);
    }

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