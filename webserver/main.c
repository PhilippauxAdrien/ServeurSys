#include "main.h"

int main(int argc, char *argv[]){
	
	
	if(argc != 2){
		perror("arguments");
		return 0;
	}

	const char * chemin=argv[1];
	if(!verif_chemin(chemin)){
		return 0;
	}
  
  int fd = 0;
  int socket_serveur, socket_client;
  FILE *file;
  http_request request;
  char buffer[256];
  initialiser_signaux();
  socket_serveur=creer_serveur(8080);
  
  while(1){
      socket_client = accept(socket_serveur , NULL , NULL );
      if(socket_client == -1){	 
       perror ("accept");
	  /* traitement d ' erreur */
	 }
      
      file = fdopen(socket_client, "w+");
      if(file==NULL){
	  perror ("fdopen");
	  /* traitement d ' erreur */
	 }
      
      int pid = fork();
      if(pid==-1){
	  perror("fork");
	  /* traitement d ' erreur */
	  }
      if(pid==0){	 

	  char *req_line = fgets_or_exit(buffer, 256, file);
	  fflush(file);
	  int bad_request = parse_http_request(req_line, &request);
	  skip_headers(file);

	  if (bad_request == 0){
	    send_response(file , 400, "Bad Request", "Bad Request\r\n");
		}
	  else if (request.method  ==  HTTP_UNSUPPORTED){
	    send_response(file , 405, "Method Not Allowed", "Method Not Allowed\r\n");
		}
	  else if((fd=check_and_open(request.url, chemin)) != -1){
	  	//send_response(file , 200, "OK", message_bienvenue);
	  	printf("Length file %d\n", get_file_size(fd));

		send_response_fd(file , 200 , "OK" , fd, gettype(request.url),socket_client);
	  }
	  else{
	    send_response(file , 404, "Not Found", "Not Found\r\n");
	  }
	  close(fd);
	  exit(0);		  
	}
      else{
	  close(socket_client);
	  }

    }
  return 0; 
}


