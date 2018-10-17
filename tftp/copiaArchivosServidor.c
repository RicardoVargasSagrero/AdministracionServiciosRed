//
// Created by dany on 4/09/18.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h> //
#include <arpa/inet.h> 
#include <glob.h> //
#include <fcntl.h> //
#include <sys/mman.h> //

unsigned char buff[512];

int main(){
    int contTrama = 0;
    struct sockaddr_in servidor, cliente;
    int udp_socket, lbind, tam, ptam;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    FILE *fout;
    if( udp_socket == -1 ){
        perror("Error al abrir el socket");
        exit(1);
    }
    else{
        perror("Exito al abrir el socket");
        memset(&servidor,0x00, sizeof(servidor));
        servidor.sin_family=AF_INET;
        servidor.sin_port=htons(69);
        servidor.sin_addr.s_addr=INADDR_ANY;
        lbind = bind(udp_socket, (struct sockaddr *)&servidor, sizeof(servidor));
        if(lbind == -1){
            perror("\nError en bind");
            exit(1);
        }
        else{
            perror("\nExito en bind");
            ptam = sizeof(cliente);

            //se abre el archivo para escribir, esta parte va en el servidor....
            fout = fopen("imagenCliente.jpg", "wb");
            if(fout == NULL) {
                perror("file open for writing");
                exit(EXIT_FAILURE);
            }

            while(1) {
                //recibimos el bloque
                tam = recvfrom(udp_socket, buff, 512, 0, (struct sockaddr *) &cliente, &ptam);
                //si el bloque es positivo, significa que esta recibiendo algo
		        contTrama++;
                if (tam == -1) {
                    perror("\nError al recibir");
                    exit(1);
                } else {
                    if (tam < 512) {
                        //es el ultimo bloque de la trama
                        fwrite(buff, 1, tam, fout);
                        break;
                        //lo guardamos, y luego salimos
                    } else {
                        //aqui recibimos los bloques anteriores al final
                        fwrite(buff, 1, tam, fout);
                    }
                    printf("Exito al recibir trama #: %d, tam = %d\n ",contTrama,tam);
                }
            }
            printf("Archivo recibidos");
        }
    }
    puts("\n");
    close(udp_socket);
    if (fclose(fout)) perror("close output file");
    return 1;
}
