#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>

unsigned char mensaje[] = "Hola red";

int main(){
    struct sockaddr_in local, remota;
    int udp_socket, lbind, tam;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //MOmento de abrir el socket

    if( udp_socket == -1 ){
        perror("\nError al abrir el socket");
        exit(1);
    }

    else{ //Parte del BIND
        perror("\nExito al abrir el socket");
        memset(&local, 0x00, sizeof(local) ); //Resetear la memoria
        local.sin_port = htons(0);
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&local, sizeof(local) );

        if( lbind == -1 ){
          perror("\nError en bind");
          exit(1);
        }

        else{ //Parte del SENDTO
          perror("\nExito en el bind");
          memset(&remota, 0x00, sizeof(remota) ); //Resetear la memoria
          remota.sin_family = AF_INET;
          remota.sin_port = htons(8000);
          remota.sin_addr.s_addr = inet_addr("10.100.78.205");
          tam = sendto(udp_socket, mensaje, 100, 0, (struct sockaddr *)&remota, sizeof(remota) );

          if( tam == -1 ){
            perror("\nError a enviar el mensaje");
            exit(1);
          }

          else{
            perror("\nExito al enviar el mensaje: Genaro SA");
          }

        }

        close(udp_socket);
        return 1;
    }
}
