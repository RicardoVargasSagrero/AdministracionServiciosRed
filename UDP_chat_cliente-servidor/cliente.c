#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>

unsigned char mensaje[512];
unsigned char recibe[512];

int main(){
    struct sockaddr_in local, remota, cliente;
    int udp_socket, lbind, tam, ptam;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if( udp_socket == -1 ){
        perror("Error al abrir el socket");
        exit(1);
    }
    else{
        perror("Exito al abrir el socket");
        memset(&local,0x00, sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(0);
        local.sin_addr.s_addr=INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&local, sizeof(local));
        if(lbind == -1){
            perror("\nError en bind");
            exit(1);
        }
        else{
            perror("\nExito en bind");
            memset(&remota,0x00, sizeof(remota));
            remota.sin_family=AF_INET;
            remota.sin_port=htons(8000);
            remota.sin_addr.s_addr=inet_addr("8.12.0.229");

            for(;;){
              printf("Escribe tu mensaje: ");
              gets(mensaje);
              tam = sendto(udp_socket, mensaje, 512, 0, (struct sockaddr *)&remota, sizeof(remota));

              if(tam ==-1){
                  perror("\nError al enviar");
                  exit(1);
              }else{
                  printf("\nYo digo: '%s'", mensaje);
                  ptam = sizeof(remota);
                  tam = recvfrom(udp_socket, recibe, 512, 0, (struct sockaddr *)&remota, &ptam);
                  if(tam ==-1){
                      perror("\nError al recibir");
                      exit(1);
                  }else{
                      printf("\nEl dice: %s", recibe);
                      //printf("\nConectado por el puerto: %d", ntohs(remota.sin_port));
                      //printf("\nCliente: %s", inet_ntoa(remota.sin_addr));
                      puts("\n");
                  }
              }
            }

        }
    }
    close(udp_socket);
    return 1;
}
