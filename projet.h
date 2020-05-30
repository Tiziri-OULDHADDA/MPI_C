#ifndef PROJET_H
#define PROJET_H


//Type de message
enum {
	DONNEE,
	PERE,
	ACCRECEP,
	FIN
};

//noeud de la liste chainé
struct noeud{
	int identifiant;		//identifiant du processus
	struct noeud* suivant;	//noeud suivant
};

//structure d'une liste chainé : 
//utilisé pour représenté les fils d'une entité de calcul
struct liste{
	struct noeud* tete;
};

//création de la liste
struct liste* creer_liste();
//creation et ajout d'un noeud à la liste l
int ajouter_liste(struct liste* l, int ident);



#endif