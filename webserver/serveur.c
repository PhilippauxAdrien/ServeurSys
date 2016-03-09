#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include "serveur.h"

/* Lit les données en provenance du client et quitte si une erreur survient */
char *fgets_or_exit(char *buffer , int size , FILE *stream){
  char* s = fgets(buffer, size, stream);
  if(s==NULL){
      perror("Stream problem");
      exit(1);
    }
  return s;
}

/* Analyse la premiere ligne de la requête */
int parse_http_request(const char *request_line , http_request *request){
  char* mot ="";
  char* req = strdup(request_line);
  int b = 1,i = 0;
  mot = strtok(req, " ");
  request->method = HTTP_UNSUPPORTED;

  while(mot){
    ++i;
    if(i==1 && strcmp(mot,"GET")==0){
	     request->method = HTTP_GET;
	   }
      if(i==2){
	      request->url = mot;
	     }	      
      if(i > 3){
	       b = 0;
	   }
      if(i == 3){
	       if(strcmp(mot,"HTTP/1.0\r\n")==0){
	      request->major_version = 1;
	      request->minor_version = 0;
	    }
	    else if (strcmp(mot,"HTTP/1.1\r\n")==0){
	      request->major_version = 1;
	      request->minor_version = 1;
	    }
	  else{
	      b = 0;
	    }
	}
      mot=strtok(NULL," ");
    } 
    if(i < 3 ){
        b = 0;
    }
  return b;
}

void skip_headers(FILE *client)
{
  char * buff = malloc(256);
  while(strcmp(buff,"\n") !=0 && strcmp(buff,"\r\n")!=0 ){
    fgets(buff,256,client);
  }
}

void send_status(FILE *client , int code , const char *reason_phrase)
{
  fprintf(client, "HTTP/1.1 %d %s\n", code, reason_phrase);
}

void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body)
{
  send_status(client, code, reason_phrase);
  fprintf(client, "Content-Length : %d\r\n\r\n", (int)strlen(message_body));
  fprintf(client, message_body);
  fflush(client);  
}

