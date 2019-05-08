#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>		/* superset of previous */
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main (){
	struct sockaddr_in servidor, cliente;
	int udp_socket, lrecv, tam, lbind, bandera;
	unsigned char paquete[512];
	struct timeval start, end;
	long mtime = 0, seconds, useconds;
  	udp_socket = socket (AF_INET, SOCK_DGRAM, 0);
  	if (udp_socket == -1){

      	perror ("\nError al recibir");
      	exit (1);
    }
    /*After openning the socket() we are gonna add the REUSE clause*/
	int enable = 1; 
	if(setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");


  	else{

	    printf ("\nExito al abrir el socket\n");
	    memset (&servidor, 0x00, sizeof (servidor));
	    servidor.sin_family = AF_INET;
	    servidor.sin_port = htons (53);
	    servidor.sin_addr.s_addr = INADDR_ANY;
	    lbind = bind (udp_socket, (struct sockaddr *) &servidor, sizeof (servidor));
      	if(lbind == -1){
	  		perror ("\nError en el bind");
	  		printf("\n");
	  		exit (1);
		}
      	else{
	  		lrecv = sizeof (cliente);
	  		gettimeofday (&start, NULL);
	  		bandera = 0;
	  		while (mtime < 2000){
	      		tam =recvfrom (udp_socket, paquete, 512, MSG_DONTWAIT,(struct sockaddr *) &cliente, &lrecv);
	     		if (tam == -1){
		  			//perror ("\nError al recibir");
				}
	      		else{
		  			printf ("%s", paquete);
		  			bandera = 1;
				}
	      		gettimeofday (&end, NULL);
	      		seconds = end.tv_sec - start.tv_sec;
	     		useconds = end.tv_usec - start.tv_usec;
	    		mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
	      		//printf ("Elapsed time: %ld milliseconds\n", mtime);
	      		if(bandera == 1)
					break;
	    	}
		}
    }
  close (udp_socket);
  return 1;
}
