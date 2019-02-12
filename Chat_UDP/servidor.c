#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>		/* superset of previous */
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>

unsigned char mensaje[512] = "";
unsigned char respuesta[512] = "";

int main (){
  struct sockaddr_in local, remota;
  int udp_socket, lbind, tam,lrecv;
  udp_socket = socket (AF_INET, SOCK_DGRAM, 0);	//MOmento de abrir el socket
  int length = 0;
  if (udp_socket == -1){
      perror ("\nError al abrir el socket");
      exit (1);
    }

  else{				//Parte del BIND
      perror ("\nExito al abrir el socket");
      memset (&local, 0x00, sizeof (local));	//Resetear la memoria
      local.sin_port = htons (8000);
      local.sin_family = AF_INET;
      local.sin_addr.s_addr = INADDR_ANY;
      /*Condición para abrir el socket */
      lbind = bind (udp_socket, (struct sockaddr *) &local, sizeof (local));

      if (lbind == -1){
        perror ("\nError en bind");
	      exit (1);
	    }
      else{			//Parte del SENDTO
	      perror ("\nExito en el bind");
        while(tam != -1 || strcmp(respuesta,"adios\n") == 0){
          tam = recvfrom(udp_socket,respuesta,512,0,(struct sockaddr*)&remota,&lrecv);
	        printf("-----Mensaje recivido: \n%s\n",respuesta);  
          printf("-----Escribir respuesta:\n");
          gets(mensaje);
          tam = sendto(udp_socket, mensaje, 512, 0, (struct sockaddr *)&local, sizeof(local));;
          printf("-----Mensaje enviado\ntam = %d\nport = %d\nip = %d",tam,local.sin_port);
          perror("Mesaje en curso");
        }
      }  
  }
  close (udp_socket);
  return 1;
}
