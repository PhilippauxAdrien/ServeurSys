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
  printf("%s\n", request_line);
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
	      //request->url = mot;  
        request->url = rewrite_url(mot);

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
/* Ignore l'entete */
void skip_headers(FILE *client){
  char * buff = malloc(256);
  while(strcmp(buff,"\n") !=0 && strcmp(buff,"\r\n")!=0 ){
    fgets(buff,256,client);
  }
}

void send_status(FILE *client , int code , const char *reason_phrase){
  fprintf(client, "HTTP/1.1 %d %s\n", code, reason_phrase);
}

void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body){
  send_status(client, code, reason_phrase);
  fprintf(client, "Content-Length : %d\r\n\r\n%s", (int)strlen(message_body),message_body);
  fflush(client);  
}

void send_response_fd(FILE * client, int code, const char * reason_phrase, int fd, char *mime, char *contenu){
  send_status(client, code, reason_phrase);
  fprintf(client, "Connection: closed\r\nContent-Length: %d\n\rContent-type:%s\n\r\n%s\r\n",get_file_size(fd),mime,contenu);
  fflush(client);
}
/* Supprime les caractères suivant '?' dans l'url */
char * rewrite_url (char * url) {

  if(strcmp(url,"/")==0){
    return "/index.html";
  }

  return strtok(url, "?");
}

/* Vérifie si le chemin est correct */
int verif_chemin(const char * url){
  if(access(url, F_OK) == -1){
    perror("Unexistant file");
    return 0;
  }
  if(access(url, X_OK) == -1){
    perror("Unreachable file");
    return 0;
  }
  struct stat statdata;
  stat(url, &statdata);
  if(!S_ISDIR(statdata.st_mode)){
    perror("not a directory");
    return 0;
  }
  return 1;
}

/* Vérifie la validité du fichier et l'ouvre ensuite */
int check_and_open(const char * url, const char * document_root){
  char * chemin = malloc(strlen(url)+strlen(document_root)+1);
  strcpy(chemin, document_root);
  strcat(chemin, url);
  printf("%s\n", url);
  FILE *fichier = NULL;
  fichier = fopen(chemin,"r");
 
    if (fichier == NULL){
      perror("le fichier n'existe pas !");
      return -1;
    }
  
  struct stat statdata; 
  stat(chemin, &statdata);

  if (!S_ISREG(statdata.st_mode)){
    perror("le fichier n'es pas un fichier régulier");
    return -1;
  }
  int fd = open(chemin,O_RDONLY);
  return fd;
}

/* Retourne la taille du fichier avec lstat */
int get_file_size(int fd){
  struct stat statbuf;
  fstat(fd,&statbuf);
  return statbuf.st_size;
}

/* Copie le fichier depuis in vers out */
int copy(int in, int out){

  char * buff = malloc(get_file_size(in));
  if(read(in,buff,get_file_size(in))==-1){
    perror("read error");
    return 0;
  }
  if(write(out,buff,get_file_size(in))==-1){
    perror("write error");
    return 0;
  }
  return 1;
}

char * gettype(char  nom[]){
  int k=strlen(nom);
  int i=0;
  
  while(i<k && nom[i]!='.'){
    i++;
  }
  if(nom[i]=='.'){
    i++;
  }
  int j=0;
  char ext[strlen(nom)-i];
  while(i+j<=k  ){
    ext[j]=nom[i+j];
    j++;
  }
  printf("ext : %s\n",ext);
  fflush(stdout);
  if(strcmp(ext,"jpg")==0){
    return "image/jpeg";
  }
  if(strcmp(ext,"jpeg")==0){
    return "image/jpeg";
  }
  if(strcmp(ext,"html")==0){
    return "text/html";
  }
  if(strcmp(ext,"htm")==0){
    return "text/html";
  }
  return "text/plain";
}