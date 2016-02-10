#include "socket.h"

const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\nCe serveur est un serveur test\nDonc s'il ne fonctionne pas encore correctement,\nNe vous inquietez pas\nNous allons régler cela dans les plus brefs délais\nEn attendant, vous pouvez regarder ce joli message défiler\nEnfin, si tout marche bien!\n" ;

void traitement_signal(int sig){
	printf("Signal %d reçu\n",sig);
	waitpid(-1, NULL, WNOHANG);
}

void initialiser_signaux(void) {
	struct sigaction sa;

	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	
	if(sigaction(SIGCHLD,&sa,NULL) == -1){
	perror("sigaction(SIGCHLD)");
	}
 	
 	if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){
		perror("signal");
	}
}

int creer_serveur(int port){
	int socket_serveur ;

	socket_serveur = socket(AF_INET,SOCK_STREAM,0);
	if(socket_serveur == -1){
		perror("socket_serveur");
		/* traitement de l ’ erreur */
		return -1;
	}

	int optval = 1;
 	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
     	perror("Can not set SO_REUSEADDR option");
 	}
 	
 	initialiser_signaux();

	/* Utilisation de la socket serveur */
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d ’ écoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
	
	

	if(bind(socket_serveur,(struct sockaddr *)&saddr , sizeof(saddr)) == -1){
		perror ("bind socket_serveur");
		/* traitement de l’erreur */
		return -1;
	}	
	if(listen(socket_serveur,10) == -1){
		perror("listen socket_serveur");
		/* traitement d ’ erreur */
		return -1;
	}

	
	return socket_serveur;
}


int accept_client(int sock_serveur){
	int socket_client;
	socket_client = accept(sock_serveur, NULL, NULL);

	if(socket_client == -1){
		perror("accept_client");
		return -1;
	}

	if(fork() == 0){
		write(socket_client, message_bienvenue, strlen(message_bienvenue));
		char buf[4096];

		while(1){
			bzero(buf,4096);
			sleep(1); /* Attente d'une seconde avant l'envoi du message */
			if(read(socket_client , buf ,sizeof(buf)) == -1){
				perror("read");
				break;
			}
			if(write(socket_client,buf,strlen(buf)) == -1){
				perror("write");
				break;
			} 
		}
		close(sock_serveur);
		close(socket_client);
		exit(1);
	}

	close(socket_client);
	return 0;
}