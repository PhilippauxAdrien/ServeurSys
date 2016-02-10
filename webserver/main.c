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
	
	while(1){
		accept_client(socket_serveur);
	}

	return 0;
}