#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>

int main(void){

	int socket_serveur = creer_serveur(8080);
		if(socket_serveur == -1){
			perror("Création du serveur");
			return -1;
		}

	int socket_client ;
	const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\nCe serveur est un serveur test\nDonc s'il ne fonctionne pas encore correctement,\nNe vous inquietez pas\nNous allons régler cela dans les plus brefs délais\nEn attendant, vous pouvez regarder ce joli message défiler\nEnfin, si tout marche bien!" ;
	
	while(1){
	
		socket_client = accept_client(socket_serveur,NULL,NULL);
	
		if(socket_client == -1){
			perror("accept");
			/* traitement d ’ erreur */
			return -1;
		}
		/* On peut maintenant dialoguer avec le client */
		
		while(1){
		sleep(1); /* Attente d'une seconde avant l'envoi du message */
		if(write(socket_client,message_bienvenue,strlen(message_bienvenue)) == -1){
			perror("write");
			break;
			} 
		}
	}

	return 0;
}