#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>

#include "socket.h"
#include "serveur.h"

const char *message_bienvenue = "Bonjour, bienvenue sur mon serveur\nCe serveur est un serveur test\nDonc s'il ne fonctionne pas encore correctement,\nNe vous inquietez pas\nNous allons régler cela dans les plus brefs délais\nEn attendant, vous pouvez regarder ce joli message défiler\nEnfin, si tout marche bien!\n\n" ;
const char *server_name = "<ServeurSys>";
const char *error_400 = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Lenght: 17\r\n\r\n400 Bad Request\r\n";
const char *mess_ok = "HTTP/1.1 200 Ok\r\nConnection: close\r\nContent-Length: ";
const char *error_404 = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Lenght: 17\r\n\r\n404 Not Found\r\n";

int main()
{
  int socket_serveur, socket_client;
  char buffer[80];
  FILE* file;
  char *req_line;
  
  http_request request;

  initialiser_signaux();
  socket_serveur=creer_serveur(8080);
  
  while(1){
      socket_client = accept ( socket_serveur , NULL , NULL );
      if ( socket_client == -1){	 
       perror ("accept");
	  /* traitement d ' erreur */
	 }
      
      file = fdopen(socket_client, "w+");
      if(file==NULL){
	  perror ("fdopen");
	 }
      
      int pid = fork();
      if(pid==-1){
	  perror("fork");
	  }
      if(pid==0){	  
	  req_line = fgets_or_exit(buffer, 80, file);
	  fflush(file);
	  int bad_request = parse_http_request(req_line, &request);
	  skip_headers(file);
	  if (bad_request == 0)
	    send_response(file , 400, "Bad Request", "Bad request\r\n");
	  else if (request.method  ==  HTTP_UNSUPPORTED)
	    send_response(file , 405, "Method Not Allowed", "Method Not Allowed\r\n");
	  else if (strcmp(request.url, "/") == 0)
	    send_response(file , 200, "OK", message_bienvenue);
	  else
	    send_response(file , 404, "Not Found", "Not Found\r\n");
	  exit(0);		  
	}
      else
	{
	  close(socket_client);
	}
    }
  return 0; 
}


