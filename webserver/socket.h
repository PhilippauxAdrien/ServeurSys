#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>

#ifndef __SOCKET_H__
#define __SOCKET_H__
#define NO_ERROR 200
#define ERROR_400 400
#define ERROR_404 404

enum http_method {
HTTP_GET ,
HTTP_UNSUPPORTED ,
};

typedef struct{
enum http_method method ;
int major_version ;
int minor_version ;
char * url ;
}http_request ;

/* * Crée une socket serveur qui écoute sur toute les interfaces IPv4
de la machine sur le port passé en paramètre . La socket retournée
doit pouvoir être utilisée directement par un appel à accept .
La fonction retourne -1 en cas d ’ erreur ou le descripteur de la
socket créée . */
int creer_serveur(int port);
int accept_client(int sock_serveur);
#endif