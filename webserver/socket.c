#include "socket.h"

void traitement_signal(int sig){
	int status;
	printf("Signal %d reçu\n",sig);
	waitpid(-1, &status, WNOHANG);
	if (WIFSIGNALED(status))
	{
		switch (WTERMSIG(status))
		{
			case SIGSEGV:
				fprintf(stderr, "Erreur de segmentation\n");
				break;
			default:
				fprintf(stderr, "Fils arrété par signal %d\n", WTERMSIG(status));
				break;
		}
	}
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

/* Créer le serveur sur le port spécifié et attends la connexion des clients */
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
	printf("Serveur connecté\n");
	
	return socket_serveur;
}
