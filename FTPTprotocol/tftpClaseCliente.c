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
void ReadRequest(unsigned char[],int);
void WriteRequest(unsigned char [],int, int, struct sockaddr_in,int);
void Data(unsigned char [],int,int,struct sockaddr_in,int);
void Acknowledment(int, int, struct sockaddr_in);
void Error(unsigned char [],int);
FILE *filein,*fileout;
int counterInteger = 0;
unsigned char buffer[512];
int main(int argc, char const *argv[])
{
	int opcode,i,contTrama = 0;
	FILE *file,*file2;
	char filename[100],c,mode[10];

	/*
	printf("Enter the file name to copy: \n");
	scanf("%s",filename);
	file = fopen(filename,"r");	
	if(file != NULL){
		file2 = fileCopy(file,filename);		
	}else
		printf("Error al abrir el archivo");	
	fclose(file);
	fclose(file2);
	*/
	/*Funcion para crear un servidor */

 	struct sockaddr_in local, remota;    
	int udp_socket,lbind,tam,lrecv,bandera;
	unsigned char message[512];
	struct timeval start, end;
	long mtime=0, seconds, useconds; 

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
	       if(tam==-1){
	       	perror("\nError al recibir");
	       }
	       else
	        {
	         printf("\nExito al recibir: %s",message);
	         /*Se hace lectura de los codigos de operacion dados en los primeros 2 bytes del message*/
	         opcode = message[1] + 0x0000;
	         printf("%d\n",opcode);
	         switch(opcode){
	         	case 1:
	         		ReadRequest(message,opcode);
	         		break;
	         	case 2:
	         		/*2 bytes     string    1 byte     string   1 byte
    				 ------------------------------------------------
    				| Opcode |  Filename  |   0  |   Mode    |   0  |
    				 ------------------------------------------------*/
		         		printf("*******************\n");
		         		for(i = 0; i < sizeof(message); i++){
		         			printf("%c",message[i]);
		         		}
	         		printf("\n");
	         		WriteRequest(message,opcode,udp_socket,remota,lrecv);
	         		break;
	         	case 3:
	         		Data(message,opcode,udp_socket,remota,lrecv);
	         		break;
	         	case 4:
	         		Acknowledment(opcode,udp_socket,remota);
	         		break;
	         	case 5:
	         		Error(message,opcode);
	         		break;
	         	default:
	         		/*Se debe esperar nueva respuesta*/
	         		printf("Error en codigo de operacion, esperando nueva respuesta\n");
	         		break;
	        }
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
int estructuraPeticion(unsigned char *msj){
	int tam;
	char nombre[50];
	msj[0] = 0x00;
	msj[1] = 0x01;
	printf("Inserte el nombre del archivo\n");
	scanf("%s",nombre);
	strcpy(msj+2,nombre);
	strcpy(msj+strlen(nombre)+3,modo);
	tam = strlen(nombre)+strlen(modo)+4;
	return tam;
}
int acuse(){
	msj[0] = 0x00;
	msj[1] = 0x04;
	msj[2] = nblock >> 8;
	msj[3] = nblock;
	tam = 4;
	return tam;
}
int bloque(){
	msj[0] = 0x00;
	msj[1] = 0x01;
	msj[2] = nblock >>8;
	msj[3] = nblock;
	tam = ;
	//El tamanio se manda se manda con el tamanio de los datos + 4 para saber cuando se
	//Termino la comunicacion
	return tam;
}
FILE *fileCopy(FILE *f,char *filename){
	FILE *fc;
	char c;
	printf("\tFunction cpFile\n");
	printf("Enter a name for the copy: \n");
	scanf("%s",filename);
	fc = fopen(filename,"w");
	if(fc != NULL){
		while((c = fgetc(f))!= EOF){
			fputc(c,fc);		
		}
		printf("File copied successfully.\n");
	}

	return fc;
}
void WriteRequest(unsigned char message[],int opcode,int udp_socket,struct sockaddr_in remota,int lrecv){
	/*From the side of the server we recived a Write Request
	so the client is tring to Send a file or write in one.

	2 bytes     string    1 byte     string   1 byte
     ------------------------------------------------
    | Opcode |  Filename  |   0  |   Mode    |   0  |
     ------------------------------------------------*/
	char filename[80],mode[15];
	strcpy(filename,message+2);
	printf("WriteRequest Function\nFilename: %s\ntamano del msj: %ld\n",filename,strlen(message));
 	int i,tam = 0;
 	//Once we recive the opcode to recive a file we open the  file and wait for the data
 	filein = fopen(filename,"wb");
 	//We add a condition, if the file name is null, we cannot create the file and send and send an error
 	if(filename != NULL){
 		while(counterInteger <= 512){
 			Acknowledment(opcode,udp_socket,remota);
 			tam = recvfrom(udp_socket,message,512,0,(struct sockaddr*)&remota,&lrecv);
 			//Mientras el bloque sea positivo estamos reciviendo algo
 			if(tam < 512){
	 			fwrite(buffer,1,tam,filein);
	 			break;
	 			//Se escribe el ultimo bloque
 			}
 			else{
 				//Se reciben todos los bloques
 				fwrite(buffer,1,tam,filein);
 			}
 		}
 		if(tam == -1){
 			perror("\nError: ");
 			exit(1);
 		}
 	} 
}
void ReadRequest(unsigned char message[],int opcode){
	/*2 bytes     string    1 byte     string   1 byte
     ------------------------------------------------
    | Opcode |  Filename  |   0  |    Mode    |   0  |
     ------------------------------------------------ */
	printf("ReadRequest Function in Acction\n");
}
void Data(unsigned char message[],int opcode,int udp_socket,struct sockaddr_in remota,int lrecv){
	/*2 bytes     2 bytes      n bytes
     ----------------------------------
    | Opcode |   Block #  |   Data     |
     ----------------------------------*/
	
	printf("Data packing sending\n");
}
void Acknowledment(int opcode,int udp_socket,struct sockaddr_in remota){
	/* 2 bytes     2 bytes
      ---------------------
     | Opcode |   Block #  |
      ---------------------*/
	/**We send the Acknowledment of the package recive
	So what we need is the number of the block we got
	We add a counter.. 
	*/
	printf("Acknowledment #;%d\n",counterInteger);
	memset(buffer,0,sizeof(buffer));
	buffer[0] = 0;
	buffer[1] = 4;
	memcpy(buffer+2,&counterInteger,2);
	sendto(udp_socket,buffer,512,0,(struct sockaddr *)&remota,sizeof(remota));
	printf("We have sented the Acknowledment #:%d\n",counterInteger);
	counterInteger++;
}
void Error(unsigned char message[],int opcode){
	/*2 bytes     2 bytes      string    1 byte
    -----------------------------------------
   | Opcode |  ErrorCode |   ErrMsg   |   0  |
    -----------------------------------------*/
    printf("Error ocurr\n");
}