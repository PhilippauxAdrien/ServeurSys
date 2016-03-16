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