#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>

unsigned char mensaje[512] = "adios";
unsigned char respuesta[512] = "";

int main(){
    struct sockaddr_in local, remota;
    int udp_socket, lbind, tam,ptam,lrecv;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //MOmento de abrir el socket
    unsigned char ip[15] = "192.168.0.4";//Dirección local
    int length = 0;
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
        /*Condición para abrir el socket*/
        lbind = bind(udp_socket, (struct sockaddr *)&local, sizeof(local) );

        if( lbind == -1 ){
          perror("\nError en bind");
          exit(1);
        }

        else{ //Parte del SENDTO
          perror("\nExito en el bind");
          memset(&remota,0x00, sizeof(remota));
          remota.sin_family=AF_INET;
          remota.sin_port=htons(8000);
          remota.sin_addr.s_addr=inet_addr(ip);
          tam = sendto(udp_socket, mensaje, 512, 0, (struct sockaddr *)&remota, sizeof(remota));
          printf("Mensaje enviado\n");
          if( tam == -1 ){
            perror("\nError a enviar el mensaje");
            exit(1);
          }

          else{
            perror("\n------Esperando respuesta");
            tam = recvfrom(udp_socket,respuesta,512,0,(struct sockaddr*)&remota,&ptam);
            printf("-----Mensaje recibido: \n%s\n",respuesta);  
          }

        }

        close(udp_socket);
        return 1;
    }
}
