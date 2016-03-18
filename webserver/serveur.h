#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stats.h"


enum  http_method {
  HTTP_GET ,
  HTTP_UNSUPPORTED ,
};

typedef  struct
{
  enum  http_method  method;
  int   major_version;
  int   minor_version;
  char *url;
} http_request;

void send_stats(FILE *client);
int copy(int in, int out);
char * gettype(char  nom[]);
void send_response_fd(FILE * client, int code, const char * reason_phrase, int fd, char mime[], int out);
int get_file_size(int fd);
int check_and_open(const char * url, const char * document_root);
int verif_chemin(const char * url);
char * rewrite_url (char * url);
void traiter_client(FILE *file);
char *fgets_or_exit(char *buffer , int size , FILE *stream);
int parse_http_request(const char *request_line , http_request *request);
void skip_headers(FILE *client);
void send_status(FILE *client , int code , const char *reason_phrase);
void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body);
