#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

unsigned char mensaje[] = "Ricardo Vargas Sagrero\n";

int main(){
    //Declaramos las estructuras de los sockets
    struct sockaddr_in local,remota;
    int udp_socket, lbind,tam;
    //Verificar que este descriptor de socket es correcto
    udp_socket = socket(AF_INET,SOCK_DGRAM,0);
    if(udp_socket == -1){
        //Es una funcion que siempre llamara a erno
        perror("\nError al abrir el socket");
        exit(1);
    }
    else{
        perror("\nExito al abrir el socket");
        //La siguiete funcion borra todo lo que hay dentro de la estrutura
        //Borra la estrutura, llena con 0, y el tamanio
        memset(&local,0x00,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(0);
        //Direccion IP 
        local.sin_addr.s_addr=INADDR_ANY; 
        //Bind asocia la direccion con el socket que este conectado
        //A cada socket se le asigna una direccion diferente
        lbind = bind(udp_socket, (struct sockaddr *)&local,sizeof(local));
        if(lbind == -1){
        	perror("\nError en bind");
        	exit(1);
        }
        else{
        	perror("\nExito en bind");
        	memset(&remota,0x00,sizeof(remota));
        	remota.sin_family = AF_INET;
        	remota.sin_port = htons(8000); //Puerto remoto
        	//Direccion IP, aqui se debe declarar
    	    remota.sin_addr.s_addr=inet_addr("10.100.78.205"); 
    	    //Regresa la cantidad de byte que envio, si regresa -1 
    	    //quiere decir que hubo un erro
    	    tam = sendto(udp_socket,mensaje,25,0,(struct sockaddr *)&remota,sizeof(remota));
    		if(tam == -1){
    			perror("\nError al enviar mensaje");
    			exit(1);
    		}
    		else
    			perror("\nExito al enviar");
            //********Recibe mensaje 
            tam = recvfrom(udp_socket,mensaje,512,0,(struct sockaddr *)&cliente,&ptam); //Lo ultimo es porque se pide un apuntador
            if(tam == -1){
                perror("\nError al enviar recibir");
                exit(1);
            }
            else{
                printf("\nExito al recibir: %s\n",mensaje);
                printf("ip: %s\n", inet_ntoa(cliente.sin_addr));
                printf("Puerto: %d\n",cliente.sin_port);
            }
    	}
    }
    close(udp_socket);
    return 1;
}
