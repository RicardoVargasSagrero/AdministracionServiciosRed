#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

unsigned char mensaje[512];

int main(){
    //Declaramos las estructuras de los sockets
    struct sockaddr_in servidor,cliente;
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
        memset(&servidor,0x00,sizeof(servidor));
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(8000); //Puerto 8000
        //Direccion IP 
        servidor.sin_addr.s_addr=INADDR_ANY; 
        //Bind asocia la direccion con el socket que este conectado
        //A cada socket se le asigna una direccion diferente
        lbind = bind(udp_socket, (struct sockaddr *)&servidor,sizeof(servidor));
        if(lbind == -1){
        	perror("\nError en bind");
        	exit(1);
        }
        else{
        	perror("\nExito en bind");
        	//Como somos el servidor necesita recibir 
        	//Colocamos el ciclo
        	while(strncmp(mensaje,"adios",6)){
        	int ptam = sizeof(cliente);//Puntero al tamanio
    	    tam = recvfrom(udp_socket,mensaje,512,0,(struct sockaddr *)&cliente,&ptam); //Lo ultimo es porque se pide un apuntador
    		if(tam == -1){
    			perror("\nError al enviar recibir");
    			exit(1);
    		}
    		else{
    			printf("\nExito al recibir: %s\n",mensaje);
    			//printf("ip: %s\n", inet_ntoa(cliente.sin_addr));
    			//printf("Puerto: %d\n",cliente.sin_port);
    		}
    		//Se enviara un mensaje ahora *************
    		//cliente.sin_family = AF_INET;
        	//cliente.sin_port = htons(8000); //Puerto remoto
        	//Direccion IP, aqui se debe declarar
    	    //***cliente.sin_addr.s_addr=inet_addr("10.100.78.142"); 
    	    //Regresa la cantidad de byte que envio, si regresa -1 
    	    //quiere decir que hubo un error
    	    printf("Responder: ");
    	    gets(mensaje);
    	    printf("\n");
    	    tam = sendto(udp_socket,mensaje,25,0,(struct sockaddr *)&cliente,sizeof(cliente));
    		if(tam == -1){
    			perror("\nError al enviar mensaje");
    			exit(1);
    		}
    		else
    			perror("\nExito al enviar");
    	}
    	}
    }
    close(udp_socket);
    return 1;
}
