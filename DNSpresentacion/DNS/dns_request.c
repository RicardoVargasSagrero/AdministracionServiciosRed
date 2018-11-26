/**Ricardo Vargas Sagrero DNS request
DNS(Domain Name System) uses UDP (User Datagram Protocol) by the port 53
Consult DNS message

Google DNS (8.8.8.8)
Consulta 
http://www.newdevices.com/tutoriales/dns/3.html
http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
unsigned char message[516];
unsigned char recibe[516];
int answerRRS = 0; 
void FirstMessage(int,unsigned char[],int,struct sockaddr_in);
void opcodeOption();
void flags();
void contadores();
void Query();
void Answer();
int type(int);
void urlPrint(int,int,int);
int main(){
    struct sockaddr_in local, remota, cliente;
    int udp_socket, lbind, tam, ptam,opcode,lrecv;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char filename[100] = "prueba.c";
    unsigned char ip[15] = "8.8.8.8";
    int lenght;
    struct timeval start, end;
    long mtime=0, seconds, useconds;
    system("clear"); 
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
            remota.sin_port=htons(53);
            remota.sin_addr.s_addr=inet_addr(ip);
            gettimeofday(&start, NULL);
            //FirstMessage(opcode,filename,udp_socket,remota);
            FirstMessage(0,"",udp_socket,remota);
            printf("message send\n");
            //while(mtime < 10000){
            //system("clear");
            printf("************RESPUESTA DNS DEL SERVIDOR********************\n");
            	recvfrom(udp_socket,message,516,0,(struct sockaddr*)&remota,&lrecv);
 
              	printf("-----ID de transacción-----\n\t\t%.2X%.2X\n",message[0],message[1]);
              	printf("-----Flags-----\n");
              	if(message[2] & 0x80){
              		printf("\tRespuesta\n");
              	}else{
              		printf("\tSolicitud\n");
              	}
              	opcodeOption();
              	flags();
              	contadores();
              	Query();
              	Answer();
              	gettimeofday(&end, NULL);
            	seconds  = end.tv_sec  - start.tv_sec;
            	useconds = end.tv_usec - start.tv_usec;
            	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;	
            //}
            printf("Waiting for while\n");
            
        }
    }
    close(udp_socket);
    return 1;
}
void FirstMessage(int opcode,unsigned char filename[],int udp_socket,struct sockaddr_in remota){
  /*consult DNS Struct 
                1 bit        1 nibble             1 bit             1 bit         1 bit
     -------------------------------------------------------------------------------------------
    | Solicitud/Respuesta |  Opcode  |   Respuesta de autoridad  | Truncado |Se desea recursion |
     -------------------------------------------------------------------------------------------
              1 bit          3 bits(Reservado)    1 nibble
     -------------------------------------------------------------
    | Recursion disponible |  0  |   0  |   0  | Codigo de retorno | 
     -------------------------------------------------------------
  */
  //Armamos la primer trama de prueba con DNS
  //00 00 03 77 77 77 03 69 70 6e 02 6d 78 00 00 
  message[0] = 0x00;
  message[1] = 0x3d; //Le asignamos el ID 3D
  //Flags
  message[2] = 0x01;
  message[3] = 0x00;
  //Contador de peticiones (Questions), debe cambiar
  message[4] = 0x00;
  message[5] = 0x01;
  //Answer RRS (Contador de RR(Resource Record) de respuesta)
  //Cuando lleguen las respuestas este contador debe ah
  message[6] = 0x00;
  message[7] = 0x00;
  //Authority RRs (Contador RR de autoridad)
  message[8] = 0x00;
  message[9] = 0x00;
  //Addtional RRs (Contador Adicional de RRs)
  message[10] = 0x00;
  	message[11] = 0x00;
	char queryRequestMessage[100];
	printf("Ingrese el nombre de dominio a buscar\n");
	gets(queryRequestMessage);
	printf("Query entered: %s\n",queryRequestMessage);
	int cont = 1,i = 0,j = 0,k = 0,poss;
	poss = 12;
	while(i < strlen(queryRequestMessage)-1){
		cont++;
		i++;
		if(queryRequestMessage[i] == '.'){
			cont--;
			printf("Dentro del IF = %d\n",cont);
			message[poss+j] = cont;
			printf("message[%d] = %.2X\n",poss+j,cont);
			for(k = 0; k < cont; j++,k++){
				message[poss+j+1] = queryRequestMessage[j];
				printf("message[%d] = %c\n",poss+j+1,queryRequestMessage[j]);
			}
			j++;
			cont = 0;
			printf("\n");
			//printf("k = %d,i = %d\n",k,i);
		}else if(i == strlen(queryRequestMessage)-1){
			printf("Dentro del IF = %d\n",cont);
			message[poss+j] = cont;
			printf("message[%d] = %.2X\n",poss+j,cont);
			for(k = 0; k < cont; j++,k++){
				message[poss+j+1] = queryRequestMessage[j];
				printf("message[%d] = %c\n",poss+j+1,queryRequestMessage[j]);
			}
			poss = poss+j+1;
			j++;
			cont = 0;
			printf("\n");
		}
		printf("cont dentro %d\n",cont);
	}
	
  //Querties
  printf("\nFin de message\n");
  message[poss] = 0x00;
  printf("message[%d] = %.2X\n",poss,0x00);
  poss++;
  //Type (Tipo)
  message[poss] = 0x00;
  printf("message[%d] = %.2X\n",poss,0x00);
  poss++;
  message[poss] = 0x01;
  printf("message[%d] = %.2X\n",poss,0x01);
  //Class (Clase)
  poss++;
  message[poss] = 0x00;
  printf("message[%d] = %.2X\n",poss,0x00);
  poss++;
  message[poss] = 0x01;
  printf("message[%d] = %.2X\n",poss,0x01);
  //Records adicionales 

  sendto(udp_socket,message,poss+1,MSG_DONTWAIT,(struct sockaddr *) &remota,sizeof(remota));
  //memset(&message,0x00,sizeof(message));
}
void opcodeOption(){
	//4 bits de codigo de operacion 
	//Se encuentra en el la posicion 2 
	/*0x0 Solicitud (Query)
	  0x1 Solicitud inversa (!Query)
	  0x2 Solicitud del estado del servidor (Status)
	  0x3 Reservado, no utilizado (Reserverd)
	  0x4 Notificación  (Notify)
	  0x5 Actualización (Update)
	  else not define
		*/
	printf("\tOpcode: ");
	int opcode = 3;
	opcode = (int) ((message[2] & 0x78) >> 3);
	switch(opcode){
		case 0:
			printf("Solicitud (Standard Query)\n");
			break;
		case 1:
			printf("Solicitud inversa\n");
			break;
		case 2: 
			printf("Solicitud del estado del servidor\n");
			break;
		case 3:
			printf("Reservado\n");
			break;
		case 4:
			printf("Notificación\n");
			break;
		case 5:
			printf("Actualización\n");
			break;
		default:
			printf("No definido\n");
	}
}
void flags(){
	/*
	Respuesta de autoridad bandera(Authoritative Answer Flag)
		This bit is set to indicate that server that created the response
		is authoritative for the zone in wich the domain name 
		specified in the Question section is located. if is 0, the 
		responce is non-authoritative

	Truncado (Truncation Flag)
		1 si # total de respuestas no podía incluirse 

	Se desea recursión (Recursion Desired)
		1 Solicitud recursiva

	| -------------- message[3]------| ----message[4]
		             1 bit        1 nibble             1 bit             1 bit         1 bit
     -------------------------------------------------------------------------------------------
    | Solicitud/Respuesta |  Opcode  |   Respuesta de autoridad  | Truncado |Se desea recursion |
     -------------------------------------------------------------------------------------------
              1 bit          3 bits(Reservado)    1 nibble
     -------------------------------------------------------------
    | Recursion disponible |  0  |   0  |   0  | Codigo de retorno | 
     -------------------------------------------------------------
	*/
	int auxFlag;
	if((message[2] >> 2) & 1)
		printf("\tAutoridad: Es Respuesta de Autoridad\n");
	else{
		printf("\tAutoridad: No es Respuesta de Autoridad (Server is not an authority for domain)\n");
	}
	if((message[2] >> 1) & 1)
		printf("\tTruncado: El mensaje de respuesta esta truncado\n");
	else{
		printf("\tTruncado: El mensaje de respuesta NO esta truncado\n");
	}
	if(message[2] & 1)
		printf("\tRecursión: Es una solicitud recursiva\n");
	else{
		printf("\tRecursión: NO es una solicitud recursiva\n");
	}
	if(message[3] & 0x80)
		printf("\tRecursión disponible: El servidor PUEDE hacer consultas recurivas\n");
	else{
		printf("\tRecursión disponible: El servidor no puede hacer consultas recurivas \n");
	}
	if(message[3] & 0x20)
		printf("\tRespuesta auntenticada: la porcion de Respuesta/Autoridad fue auntenticada por el servidor\n");
	else{
		printf("\tRespuesta auntenticada: la porcion de Respuesta/Autoridad NO fue auntenticada por el servidor\n");
	}
	if(message[3] & 0x10)
		printf("\tDatos no autenticados: Aceptable\n");
	else{
		printf("\tDatos no autenticados: NO Aceptable\n");
	}
	printf("\tCódigo de retorno: ");
	switch((int)(message[3] & 0x0F)){
		case 0:
			printf("No ocurrio Error (No error occurred)\n");
			break;
		case 1:
			printf("Error de formato (Format Error)\n");
			break;
		case 2:
			printf("Error en servidor (Server Failure)\n");
			break;
		case 3:
			printf("Error de nombre (Name Error)\n");
			break;
		case 4:
			printf("No implementado, el tipo de consulta noesta implementado (Not implemented)\n");
			break;
		case 5:
			printf("Rechazado (Refused)\n");
			break;
		case 6: 
			printf("Dominio YX \n");		
			break;
		case 7:
			printf("YX RR Set\n");
			break;
		case 8:
			printf("NX RR set\n");	
			break;
		case 9:
			printf("No autorizado por la zona especificada\n");
			break;
		case 10:
		    printf("Not zone: Un nombre especificado en el mensaje no esta en la zona especificada\n");
		    break;
		default:
			printf("ERROR NO DEFINIDO\n");
			break;
	}
}
void contadores(){
	printf("-----Contadores------\n");
	printf("\tContador de peticiones: %d\n", (int)((message[4]<<8)+(message[5])));
	answerRRS = (int)((message[6]<<8)+(message[7]));
	printf("\tContador de RR de respuesta: %d\n",answerRRS);
	printf("\tContador de RR de Autoridad: %d\n",(int)((message[8]<<8)+(message[9])));
	printf("\tContador de RR adicionales: %d\n",(int)((message[10]<<8)+(message[11])));	
}
void Query(){
	//Algoritmo para tranducir una petición DNS
	//Esta apartir del message[12] y termina hasta el 
	//strlen(message[12])
	//Necesitamos sacar el tamaño de todo el mensaje para saber hasta donde
	//se parara
	printf("------Consultas (Queries)------\n");
	printf("\tNombre (name): ");
	int i=13,lenght = strlen(message+12)+1,label,j;
	label = (int) message[12];
	for(j = 0;j <= label & i < lenght+12;j++,i++){
		if(message[i] == 0x00){
			break;
		}
		else if(j == label){
			label = message[i];
			j = -1;
			printf(".");
		}
		else{
			printf("%c",message[i]);
		}
	}
	printf("\n\tTipo (Type): ");
	i = (int)message[13+lenght];
	type(i);
	if(message[15+lenght] == 0x01){
		printf("\tClase: IN (0x0001)------\n");
	}else{
		printf("\tClase no definida------\n");
	}
}
void Answer(){
	printf("------Answer------\n");
	int i,startAns = strlen(message+12)+1+4+12,t;
	//Falta la parte de imprimir los valores anteriores
	int lenght = (int)((message[startAns]&0x3F)<< 8) + (message[startAns+1]);
	for(i = 0; i < answerRRS; i++){
		//printf("%.2X %.2X %.2X \n",message[startAns],message[startAns+1],message[startAns+2] );
		printf("\tNombre (Name): ");
		if(message[startAns]>>6 == 3){
			lenght = (int)((message[startAns]&0x3F)<< 8) + (message[startAns+1]);
			urlPrint(lenght+1,strlen(message+lenght)+1,message[lenght]);
		}
		startAns = startAns + 2;
		printf("\tTipo (Type): ");
		t = type((int)((message[startAns] << 8) +(message[startAns+1])));
		startAns = startAns + 2;
		if(message[startAns+1] == 0x01){
			printf("\tClase: IN (0x0001)\n");
		}else{
			printf("\tClase no definida------\n");
		}
		startAns = startAns + 2;
		printf("\tTiempo de vida (Time to live): %d\n",(int)((message[startAns]<<32)+(message[startAns+1]<<16)+(message[startAns+2]<<8)+(message[startAns+3])));
		startAns = startAns + 4;
		printf("\tLongitud de datos(Data lenght): %d\n",(int)((message[startAns]<<8)+(message[startAns+1])));
		//type(t);
		if(t == 1){
			printf("\tAddress = %d.%d.%d.%d\n",message[startAns+2],message[startAns+3],message[startAns+4],message[startAns+5]);
			startAns = startAns + 6;
		}
		else if(t == 5){
			printf("\tCNAME = ");
			if(message[startAns+2] >>6 == 3){
				lenght = (int)((message[startAns+2]&0x3F)<< 8) + (message[startAns+3]);
				urlPrint(lenght+1,strlen(message+lenght)+1,message[lenght]);
				startAns = startAns+4;
			}
			else{
				lenght = strlen(message+startAns+2) + 1;
				urlPrint(startAns+2+1,lenght,message[startAns+2]);
				startAns = startAns + lenght+2;
			}
		}
		printf("\n\n");
	}

}
int type(int index){
	switch(index){
		case 1: 
			printf("Registro host \n");
			break;
		case 2: 
			printf("Registro (A) servidor de nombres (Host Address) \n");
			break;
		case 5:
			printf("Registro alisas (CNAME) \n");
			break;
		case 12:
			printf("Registro de búsqueda inversa \n");
			break;	
		default:
			printf("Registro no definido \n");
			break;
	}
	return index;
}
void urlPrint(int start,int length,int label){
	int j = 0;
	int i=start;
	for(j = 0;j <= label & i < length+start;j++,i++){
		if(message[i] == 0x00){
			break;
		}
		else if(j == label){
			label = message[i];
			j = -1;
			printf(".");
		}
		else{
			printf("%c",message[i]);
		}
	}
	label = (int) message[start];
	printf("\n");
}