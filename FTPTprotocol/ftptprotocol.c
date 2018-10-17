/**
Con fopen copiar un archivo, de la entrada a la salida
ese archivo  guardarlo en un arreglo de [512] y 
despues enviarla por un socket
Se agrerga funcion para copiar un archivo

Para mayor informacion del protocolo consultar
https://tools.ietf.org/html/rfc1350
The following limitations apply in RFC 783; the maximum size of a data block is 
512 bytes, block number is represented by a two byte unsigned integer. 
This means the maximum number of blocks that can be transferred is 65,535 and the maximum file size that can be transferred is 65,535 x 512 bytes, 
or about 32 megabytes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>

FILE *fileCopy(FILE *f,char *filename);
/*Operacion con los oparation codes, simple construccion, del 1 al 5*/
int ReadRequest(unsigned char[],int);
int WriteRequest(unsigned char [],int, int, struct sockaddr_in,int);
int Data(unsigned char [],int,int,struct sockaddr_in,int,int);
int Acknowledment(int, int, struct sockaddr_in);
int Error(unsigned char [],int);
FILE *filein,*fileout;
int counterInteger = 0;
unsigned char buffer[516];
int WRFlag; 
int main(int argc, char const *argv[])
{
	int opcode,i,contTrama = 0;
	FILE *file,*file2;
	char filename[100],c,mode[10];

 	struct sockaddr_in local, remota;    
	int udp_socket,lbind,tam,lrecv,bandera;
	unsigned char message[516];
	struct timeval start, end;
	long mtime=0, seconds, useconds; 
	int fff = 0 ;

	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket==-1){
	    perror("\nError al abrir el socket");
	    exit(1);
	}
	else{
	    perror("\nExito al abrir el socket");
	    local.sin_family=AF_INET;
	    local.sin_port=htons(69);
	    local.sin_addr.s_addr=INADDR_ANY;
	    lbind=bind(udp_socket,(struct sockaddr *)&local,sizeof(local));
	    if(lbind==-1)
	      {
	      perror("\nError en bind");
	      exit(1);
	      }
	    else
	      {
	       perror("\nExito en bind");
	       lrecv=sizeof(remota);
	       gettimeofday(&start, NULL);
	       bandera=0;
	       while(mtime<500000)
	       {
	       tam=recvfrom(udp_socket,message,512,MSG_DONTWAIT,(struct sockaddr*)&remota,&lrecv);
	       //printf("TAM inicial = %d",tam);
	       if(tam==-1){
	       	//perror("\nError al recibir");
	       }
	       else
	        {
	         printf("\nExito al recibir: %s",message);
	         /*Se hace lectura de los codigos de operacion dados en los primeros 2 bytes del message*/
	         opcode = message[1] + 0x0000;
	         printf("%d\n",opcode);
	         do{
	         	sleep(1);
	         	tam=recvfrom(udp_socket,message,516,MSG_DONTWAIT,(struct sockaddr*)&remota,&lrecv);
	         	printf("tam %d\n", tam);
	         	opcode = message[1] + 0x0000;
	        	printf("%d\n",opcode);
	        	fff = tam;
	         	switch(opcode){
	         		case 1:
	         			WRFlag = 0;
	         			tam = ReadRequest(message,opcode);
	         			printf("TAM after ReadRequest = %d",tam);
	         			break;
	         		case 2:
	         			/*2 bytes     string    1 byte     string   1 byte
    				 	------------------------------------------------
    					| Opcode |  Filename  |   0  |   Mode    |   0  |
    				 	------------------------------------------------*/
	         			WRFlag = 1;
	         			tam = WriteRequest(message,opcode,udp_socket,remota,lrecv);
	         			printf("TAM after WriteRequest = %d\n\n",tam);
	         			break;
	         		case 3:
	         			Data(message,opcode,udp_socket,remota,lrecv,tam);
	         			printf("TAM after Data = %d\n",tam);
	         			break;
	         		case 4:
	         			tam = Acknowledment(opcode,udp_socket,remota);
	         			printf("TAM after Acknowledment = %d\n",tam);
	         			break;
	         		case 5:
	         			tam = Error(message,opcode);
	         			printf("TAM after Error = %d\n",tam);
	         			break;
	         		default:
	         			/*Se debe esperar nueva respuesta*/
	         			tam = 0;
	         			printf("Error en codigo de operacion, esperando nueva respuesta\n");
	         			break;
	        	}
	        }while(tam >= 512);
	        fclose(filein);
	        bandera=1;
	         /*EN ESTA PARTE SE REALIZAN LA VALIDACION DE LOS CODIGOS
	         DE OPERACION PARA ENTRAR A LAS FUNCIONES*/
	        }
	        gettimeofday(&end, NULL);
	        seconds  = end.tv_sec  - start.tv_sec;
	        useconds = end.tv_usec - start.tv_usec;
	        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	        //printf("Elapsed time: %ld milliseconds\n", mtime);
	        if(bandera==1)
	            break;
	        }
	    }
	}
	 
	close(udp_socket);
	return 0;
}
FILE *fileCopy(FILE *f,char *filename){
	FILE *fc;
	char c;
	printf("\tFunction cpFile\n");
	printf("Enter a name for the copy: \n");
	scanf("%s",filename);
	fc = fopen(filename,"a");
	if(fc != NULL){
		while((c = fgetc(f))!= EOF){
			fputc(c,fc);		
		}
		printf("File copied successfully.\n");
	}

	return fc;
}
int WriteRequest(unsigned char message[],int opcode,int udp_socket,struct sockaddr_in remota,int lrecv){
	/*From the side of the server we recived a Write Request
	so the client is tring to Send a file or write in one.

	2 bytes     string    1 byte     string   1 byte
     ------------------------------------------------
    | Opcode |  Filename  |   0  |   Mode    |   0  |
     ------------------------------------------------*/
	int tam = 0;
	char filename[80],mode[15];
	strcpy(filename,message+2);
	printf("WriteRequest Function\nFilename: %s\ntamano del msj: %ld\n",filename,strlen(message));
 	//Once we recive the opcode to recive a file we open the  file and wait for the data
 	filein = fopen(filename,"wb+");
 	//We add a condition, if the file name is null, we cannot create the file and send and send an error
 	tam = Acknowledment(opcode,udp_socket,remota);
 	return tam; 
}
int ReadRequest(unsigned char message[],int opcode){
	/*2 bytes     string    1 byte     string   1 byte
     ------------------------------------------------
    | Opcode |  Filename  |   0  |    Mode    |   0  |
     ------------------------------------------------ */
	int tam = 0;
	buffer[0] = 0x00;
	buffer[1] = 0x01;
	printf("ReadRequest Function in Acction\n");
	return tam;
}
int Data(unsigned char message[],int opcode,int udp_socket,struct sockaddr_in remota,int lrecv,int tam){
	/*2 bytes     2 bytes      n bytes
     ----------------------------------
    | Opcode |   Block #  |   Data     |
     ----------------------------------*/
	/*To know if we're gonna write or read a file we use the opcode
	knowing that 1 =  Read Request and 2 = Write Request 
	We'll use a flag WRFlag*/
	//strcpy(buffer,message+4);
	//printf("%s\n",buffer);
	//printf("DATA tam: %d\n",tam );
	if(tam != 512){
		tam = tam - 4;
		printf("DATA tam: %d\n",tam );
	}
	printf("%s\n",message+4);
	if(WRFlag){
		fwrite(message+4,1,tam,filein);
	}
	printf("We got a DATA package\nstrlen BUFFER: %ld\n",strlen(buffer));
	Acknowledment(opcode,udp_socket,remota);
	tam = strlen(message+4);

	return tam;
}
int Acknowledment(int opcode,int udp_socket,struct sockaddr_in remota){
	/* 2 bytes     2 bytes
      ---------------------
     | Opcode |   Block #  |
      ---------------------*/
	/**We send the Acknowledment of the package recive
	So what we need is the number of the block we got
	We add a counter.. 
	*/
	int tam = 0;
	printf("Acknowledment #:%d\n",counterInteger);
	memset(buffer,0,sizeof(buffer));
	buffer[0] = 0x00;
	buffer[1] = 0x04;
	buffer[2] = counterInteger >> 8;
	buffer[3] = counterInteger;
	tam = sendto(udp_socket,buffer,512,MSG_DONTWAIT,(struct sockaddr *)&remota,sizeof(remota));
	printf("We have sented the Acknowledment #:%d\n",counterInteger);
	memset(buffer,0,sizeof(buffer));
	counterInteger++;
	return tam;
}
int Error(unsigned char message[],int opcode){
	/*2 bytes     2 bytes      string    1 byte
    -----------------------------------------
   | Opcode |  ErrorCode |   ErrMsg   |   0  |
    -----------------------------------------*/
    int tam = 0;
    buffer[0] = 0x00;
    buffer[1] = 0x05;
    printf("Error ocurr\n");
    return tam;
}