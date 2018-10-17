//
// Created by dany on 4/09/18.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <glob.h>
#include <fcntl.h>
#include <sys/mman.h>

unsigned char buff[512];

int main(){
    struct sockaddr_in local, remota;
    int udp_socket, lbind, tam,contTrama=0;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    size_t n;
    FILE *exein, *exeout;

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
            //setters de puerto e ip
            remota.sin_port=htons(69);
            remota.sin_addr.s_addr=inet_addr("10.100.68.242");

            //se abre el archivo a leer
            exein = fopen("panda.jpg", "rb");
            if(exein == NULL) {
                perror("file open for reading");
                exit(EXIT_FAILURE);
            }

            do{
                n = fread(buff, 1, sizeof buff, exein);
                if (n){
                    tam = sendto(udp_socket, buff, 512, 0, (struct sockaddr *) &remota, sizeof(remota));
                    contTrama++;
                    // esta parte es la que va en el servidor, metiendolo en un bucle, mientras sea menor de 512, es bloque del archivo
                    //m = fwrite(buff, 1, n, exeout);
                    //printf("%d -- %d \n", tam, n);
                    if (tam == -1) {
                        perror("\nError al enviar");
                        exit(1);
                    } else {
                        printf("Exito al enviar trama\n");
                        printf("Trama: %d \n",contTrama);
                    }
                }else {
                    tam = 0;
                }
            } while ((n > 0) && (n == tam));
            if (fclose(exein)) perror("close input file");
        }
    }
    close(udp_socket);
    return 1;
}
