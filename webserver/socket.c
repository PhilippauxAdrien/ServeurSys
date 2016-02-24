#include "socket.h"

const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\nCe serveur est un serveur test\nDonc s'il ne fonctionne pas encore correctement,\nNe vous inquietez pas\nNous allons régler cela dans les plus brefs délais\nEn attendant, vous pouvez regarder ce joli message défiler\nEnfin, si tout marche bien!\n" ;
const char *server_name = "<ServeurSys>";
const char *error_msg = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Lenght: 17\r\n\r\n400 Bad Request\r\n";
const char *mess_ok = "HTTP/1.1 200 Ok\r\nConnection: close\r\nContent-Length: ";
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

int verif_requete(FILE *file){
	char buf[4096];

    if (fgets(buf, 4096, file) != NULL) {
      char *chaine = buf;
      int nbMots = 0;
      char *mot[2];
   		
   	  /* Vérifie que le premier mot est "GET" */	
      if((chaine[0]!='G' || chaine[1]!='E' || chaine[2]!='T')){
      	return -1;
      }
	  /*Compte le nombre de mots sur la 1ere ligne*/
      while(chaine[0]!='\0'){
      	if(chaine[0] == ' '){
	  		if(nbMots == 2) {
	   			 return -1;
	  		}
	 		 mot[nbMots] = chaine;
	  		 nbMots++;
		}
		chaine++;
      }
      	printf("%s\n",mot[1]);
      /*Vérification du mot*/
     if (strcmp(" HTTP/1.1\r\n", mot[1]) != 0 && strcmp(" HTTP/1.0\r\n", mot[1]) != 0) {
	     return -1;
      }
   }
  	else{
		return -1;
	}

	return 0;
}
int accept_client(int sock_serveur){
	int socket_client;
	FILE *file;
	socket_client = accept(sock_serveur, NULL, NULL);
	int error;
	if(socket_client == -1){
		perror("accept_client");
		return -1;
	}

	if(fork() == 0){
		file= fdopen(socket_client, "w+");
		char buf[4096];
		error = verif_requete(file);
		//fprintf(file, "%s %s\n",server_name,message_bienvenue);
		
		int fini = 0;

    	while (fini == 0 && fgets(buf, 4096, file) != NULL) {
     		 if (strcmp(buf, "\r\n") == 0 || strcmp(buf, "\n") == 0) {
	     		fini = 1;
      		}
    	}
    	if(error < 0){ /*En cas d'erreur de l'entete : Error 400 */
     		 fprintf(file, " %s", error_msg);
    	} 
    	else { /* Sinon on renvoie le message et sa taille */
      		fprintf(file, "%s%d\r\n\r\n%s %s",mess_ok,(int) (strlen(server_name) + strlen(message_bienvenue)), server_name, message_bienvenue);
    	}
		
		fflush(file);   
    	close(socket_client);
    	close(sock_serveur);
    	exit(1);
    	}

	close(socket_client);
	return 0;
}