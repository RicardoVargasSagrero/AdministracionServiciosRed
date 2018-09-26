#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>

/*
 * Mismo archivo, cambiamos las variables a cliente y servidor
 *
 *
 */
unsigned char mensaje[512];
unsigned char envia[]="Genaro SA";

int main(){
    struct sockaddr_in servidor, cliente, remota;
    int udp_socket, lbind, tam, ptam;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if( udp_socket == -1 ){
        perror("Error al abrir el socket");
        exit(1);
    }
    else{
        perror("Exito al abrir el socket");
        memset(&servidor,0x00, sizeof(servidor));
        servidor.sin_family=AF_INET;
        servidor.sin_port=htons(8000);
        servidor.sin_addr.s_addr=INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&servidor, sizeof(servidor));
        if(lbind == -1){
            perror("\nError en bind");
            exit(1);
        }
        else{
            perror("\nExito en bind");
            ptam = sizeof(cliente);
            tam = recvfrom(udp_socket, mensaje, 512, 0, (struct sockaddr *)&cliente, &ptam);

            if(tam ==-1){
                perror("\nError al recibir");
                exit(1);
            }else{
                printf("\nExito al recibir %s", mensaje);
                printf("\nConectado por el puerto: %d", ntohs(cliente.sin_port));
                printf("\nCliente: %s", inet_ntoa(cliente.sin_addr));
                puts("\n");
                tam = sendto(udp_socket, envia, 100, 0, (struct sockaddr *)&remota, sizeof(remota));
                if(tam ==-1){
                    perror("\nError al enviar");
                    exit(1);
                }else{
                    printf("\nExito al enviar: '%s'", envia);
                    ptam = sizeof(cliente);
                    tam = recvfrom(udp_socket, envia, 512, 0, (struct sockaddr *)&cliente, &ptam);
                    if(tam ==-1){
                        perror("\nError al recibir");
                        exit(1);
                    }else{
                        printf("\nExito al recibir %s", envia);
                        printf("\nConectado por el puerto: %d", ntohs(cliente.sin_port));
                        printf("\nCliente: %s", inet_ntoa(cliente.sin_addr));
                        puts("\n");
                    }
                }
            }
        }
    }
    close(udp_socket);
    return 1;
}
