#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<stdbool.h>
#include "projet.h"
#include"mpi.h"

//allocation dynamique d'une liste chainée
struct liste* creer_liste(){
	struct liste* l = (struct liste*) malloc(sizeof(struct liste));
	if(l != NULL){
		//si l'allocation a réussit, initialisé la tête à null
		//NULL = liste vide
		l->tete = NULL;
	}
	return l;
}

struct noeud* creer_noeud(int ident){
	struct noeud* tmp = (struct noeud*) malloc (sizeof(struct noeud));
	if(tmp != NULL){
		tmp->identifiant = ident;
		tmp->suivant = NULL;
	}
	return tmp;
}


int ajouter_liste(struct liste* l, int ident){
	struct noeud* tmp = l->tete;
	struct noeud* noeud = creer_noeud(ident);
	if(noeud != NULL){
		l->tete = noeud;
		noeud->suivant = tmp;
		return EXIT_SUCCESS;
	}else{
		return EXIT_FAILURE;
	}
}




int main(int argc, char** argv){
	MPI_Init(NULL, NULL);
	int identifiant;
	MPI_Comm_rank(MPI_COMM_WORLD, &identifiant);
	int nb_processus;
	MPI_Comm_size(MPI_COMM_WORLD, &nb_processus);

	MPI_Status status;
	
	
	int pere = 0;
	int message1;
	int message2;
	int accrecep = 2;
	int c = 0;
	int i;

	struct liste* fils = creer_liste(); 
	int fin = 0;

	if (identifiant == 0)
	{
		message1 = 11709949;
		int deg = 0;
		for(i = 1; i < nb_processus; i++){
			MPI_Send(&message1, 1, MPI_INT, i, DONNEE, MPI_COMM_WORLD);
			printf("p(%d) a envoyer (%d, donnée) à p(%d)\n", identifiant, message1, i);			
			deg++;
		}
		c = 2 * deg;
	}else{
		int deg = 0;

		MPI_Recv(&message1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);		
		deg++;
		printf("p(%d) a recu (message : %d) de son pere p(%d)\n", identifiant, message1, status.MPI_SOURCE);
		for(i = 0; i < nb_processus; i++){
			if(i != identifiant && i != status.MPI_SOURCE){
				MPI_Send(&message1, 1, MPI_INT, i, DONNEE, MPI_COMM_WORLD);
				printf("p(%d) a envoyer (%d, donnée) à p(%d)\n", identifiant, message1, i);
				deg++;
			}
			
		}
		pere = status.MPI_SOURCE;
		c = 2 * deg - 2;
	}
	
	printf("C initial de p(%d) est %d\n", identifiant, c);
	
	//MPI_Barrier(MPI_COMM_WORLD);

	while(c > 0){
		MPI_Recv(&message2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		if(status.MPI_TAG == DONNEE){
			printf("p(%d) a recu (%d, donnée) du p(%d)\n", identifiant, message2, status.MPI_SOURCE);
			MPI_Send(&accrecep, 1, MPI_INT, status.MPI_SOURCE, ACCRECEP, MPI_COMM_WORLD);
			printf("p(%d) a envoyer (accusé) à p(%d)\n", identifiant, status.MPI_SOURCE);
			c = c - 1;
		}
		
		if(status.MPI_TAG == PERE){
			printf("p(%d) a recu (pere  ) du p(%d)\n", identifiant, status.MPI_SOURCE);
			ajouter_liste(fils, status.MPI_SOURCE);
			c = c - 2;
		}

		if(status.MPI_TAG == ACCRECEP){
			printf("p(%d) a recu (accusé) du p(%d)\n", identifiant, status.MPI_SOURCE);
			c = c - 1;
		}
		printf("c(%d) = %d\n", identifiant, c);
	}

	if(identifiant != 0){
		MPI_Send(&pere, 1, MPI_INT, pere, PERE, MPI_COMM_WORLD);
		printf("p(%d) a envoyé (PERE) au p(%d)\n", identifiant, pere);
		MPI_Recv(&fin , 1, MPI_INT, pere, FIN , MPI_COMM_WORLD, &status);
		printf("p(%d) a recu (fin   ) du p(%d)\n", identifiant, pere);
	}

	fin = 1;
	
	struct noeud* ptr = fils->tete;

	while(ptr){
		MPI_Send(&pere, 1, MPI_INT, ptr->identifiant, FIN, MPI_COMM_WORLD);
		printf("p(%d) a envoyé (fin ) au p(%d)\n", identifiant, ptr->identifiant);
		ptr = ptr->suivant;
	}

	printf("fin de l'execution pour p(%d)\n", identifiant);


	MPI_Finalize();

	return EXIT_SUCCESS;
}