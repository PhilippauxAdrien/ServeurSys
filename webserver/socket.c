#include "socket.h"

const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\nCe serveur est un serveur test\nDonc s'il ne fonctionne pas encore correctement,\nNe vous inquietez pas\nNous allons régler cela dans les plus brefs délais\nEn attendant, vous pouvez regarder ce joli message défiler\nEnfin, si tout marche bien!\n\n" ;
const char *server_name = "<ServeurSys>";
const char *error_400 = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Lenght: 17\r\n\r\n400 Bad Request\r\n";
const char *mess_ok = "HTTP/1.1 200 Ok\r\nConnection: close\r\nContent-Length: ";
const char *error_404 = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Lenght: 17\r\n\r\n<h1>404 Not Found</h1>\r\n";

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
/* Lit les données en provenance du client et quitte si une erreur survient */
char *fgets_or_exit(char *buffer, int size, FILE *stream){
  if(fgets(buffer, size, stream) == NULL){
    perror("Stream problem");
    exit(1);
  }
  printf("%s", buffer);
  return buffer;
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

	
	return socket_serveur;
}

/* Analyse la premiere ligne de la requête */
int parse_http_request(const char *request_line, http_request * request){
	int n = 0, boolean = 1;
	char *copy = strdup(request_line);
	char *mot = strtok(copy," ");

	while(mot != NULL){
		n++;
		if(n == 1){
		 	if(strcmp(mot,"GET") != 0){ /* Methode different de GET */
				boolean = 0;
				request->method = HTTP_UNSUPPORTED;
			}
		 	else{
		 		request->method = HTTP_GET;
			 }	
		}
		if(n == 2){
			printf("url : %s\n",mot);
			request->url = mot;
			if(strcmp(mot,"/") != 0){ /* URL != "/" */
				boolean = 0;
				//printf("URL OK\n");
			}
		}
		if(n == 3){
			request->major_version = mot[5] - '0';
			request->minor_version = mot[7] - '0';
			if(strcmp(mot,"HTTP/1.0\r\n") != 0 && strcmp(mot,"HTTP/1.1\r\n") != 0){ /* Controle de la version */
				boolean = 0;
				//printf("VERSION OK\n");
			}
		}
		if(n>3){ /* Nombre de mots supérieur à 3 */
			boolean = 0;
		}
		mot = strtok(NULL, " "); /* On avance d'un mot */
	}

	if(n<3){ /* Manque des arguments dans la requete */
		boolean = 0;
	}
	  
	free(copy);
	return boolean;
}

/* Vérifie si la requête reçue est bien correcte */
int verif_requete(FILE *file){
	char buf[4096];
	fgets_or_exit(buf, 4096, file);
    
      char *chaine = buf;
      int i = 0;
      char *car[2];
   		// printf("chaine : %s\n", chaine);
      http_request req ;
   	  parse_http_request(chaine,&req);
   	  printf("method : %d maj : %d min : %d url : %s\n",req.method,req.major_version,req.minor_version,req.url);
   	  /* Vérifie que le premier mot est "GET" */	
      if((chaine[0]!='G' || chaine[1]!='E' || chaine[2]!='T')){
      	return ERROR_400;
      }
	  /*Compte le nombre de mots sur la 1ere ligne*/
      while(chaine[0]!='\0'){
      	if(chaine[0] == ' '){
	  		if(i == 2) {
	   			 return ERROR_400;
	  		}
	 		 car[i] = chaine;
	  		 i++;
		}
		chaine++;
      }

     //printf(" : %c   %c\n", mot[0][1],mot[0][2]);
     if(car[0][1] != '/' || car[0][2] != ' '){
      	return ERROR_404;
      } 
     
      /*Vérification du mot*/
     if (strcmp(" HTTP/1.1\r\n", car[1]) != 0 && strcmp(" HTTP/1.0\r\n", car[1]) != 0) {
	     return ERROR_400;
      }

	return NO_ERROR;
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
    	if(error == ERROR_400){ /*En cas d'erreur de l'entete : Error 400 */
     		 fprintf(file, "%s", error_400);
    	} 
    	else if(error == ERROR_404){ /*En cas d'erreur de l'entete : Error 400 */
     		 fprintf(file, "%s", error_404);
    	}
    	else if(error == NO_ERROR){ /* Sinon on renvoie le message et sa taille */
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