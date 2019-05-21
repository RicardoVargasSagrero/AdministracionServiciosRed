#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>		/* superset of previous */
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
void serchData();
char *bin2hex(const unsigned char *input, size_t len);
void Query(unsigned char []);
int main (){
	struct sockaddr_in servidor, cliente;
	int udp_socket, lrecv, tam, lbind, bandera;
	unsigned char request[512];
	struct timeval start, end;
	long mtime = 0, seconds, useconds;
	/*IP from google DNS*/
	unsigned char ip_google[15] = "8.8.8.8";
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
	  		while (mtime < 50000){
	      		tam =recvfrom (udp_socket, request, 512, MSG_DONTWAIT,(struct sockaddr *) &cliente, &lrecv);
	     		if (tam == -1){
		  			//perror ("\nError al recibir");
				}
	      		else{
	      			printf("Exito al recibir query\n");
	      			Query(request);
		  			printf ("%s", request);
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

void serchData(unsigned char request[]){

}
char *bin2hex(const unsigned char *input, size_t len){
	char *result;
	char *hexits = "0123456789ABCDEF";
	int i;
	if(input == NULL || len <= 0)
		return NULL;

	//(2 hexits+space)/chr + NULL
	int resultlength = (len*3)+1;

	result = malloc(resultlength);
	bzero(result, resultlength);

	for(i = 0; i < len; i++){
		result[i*3] 	= hexits[input[i] >> 4];
		result[(i*3)+1]	= hexits[input[i] & 0x0F];
		result[(i*3)+2] = ' '; //for readability
	}
	return result;
}
void Query(unsigned char message[]){
	//Algoritmo para tranducir una petición DNS
	//Esta apartir del message[12] y termina hasta el 
	//strlen(message[12])
	//Necesitamos sacar el tamaño de todo el mensaje para saber hasta donde
	//se parara
	/*This function will help us to create a string with the query that the client send,
	thus we can ask to the data base if it exits*/
	char *str = (char*)malloc(sizeof(char));

	printf("------Consultas (Queries)------\n");
	printf("\tNombre (name): ");
	int i=13,lenght = strlen(message+12)+1,label,j,k = 1,l = 0;
	label = (int) message[12];
	for(j = 0;j <= label & i < lenght+12;j++,i++,k++,l++){
		if(message[i] == 0x00){
			break;
		}
		else if(j == label){
			label = message[i];
			str = (char*)realloc(str,k*sizeof(char));
			str[l] = '.';
			j = -1;
			printf(".");
		}
		else{
			str = (char*)realloc(str,k*sizeof(char));
			str[l] = message[i];
			printf("%c",message[i]);
		}
	}
	printf("\n\tTipo (Type): ");
	if(message[15+lenght] == 0x01){
		printf("\tClase: IN (0x0001)------\n");
	}else{
		printf("\tClase no definida------\n");
	}
	printf("Str = %s\n",str);
	free(str);
}
