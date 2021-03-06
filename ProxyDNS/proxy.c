#include "common.h"
int i; 
int main (){
	struct sockaddr_in servidor, cliente,servidor_google;
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
	  		for( ; ; ){
	      		tam =recvfrom (udp_socket, request, 512, MSG_DONTWAIT,(struct sockaddr *) &cliente, &lrecv);
	     		if (tam == -1){
		  			//perror ("\nError al recibir");
				}
	      		else{
	      			printf("Exito al recibir query\n");
	      			if(QueryAnalyzer(request)){
	      				printf("ACCEPTADO\n");
	      				memset(&servidor_google,0x00,sizeof(servidor_google));
	      				servidor_google.sin_family = AF_INET;
	      				servidor_google.sin_port = htons(53);
	      				servidor_google.sin_addr.s_addr = inet_addr(ip_google);
	      				sendto(udp_socket,request,512,MSG_DONTWAIT,(struct sockaddr *) &servidor_google,sizeof(servidor_google));
	      				recvfrom(udp_socket,google_answer,516,0,(struct sockaddr*)&servidor_google,&lrecv);
	      				sendto(udp_socket,google_answer,516,MSG_DONTWAIT,(struct sockaddr*)&cliente, sizeof(cliente));
	      			}else{
	      				printf("DENEGADO");
	      				printf("%d\n",cliente.sin_addr.s_addr );
	      				accessDenied(request);
	      				sendto(udp_socket,google_answer,i,MSG_DONTWAIT,(struct sockaddr*)&cliente, sizeof(cliente));
	      			}
				}
	      		gettimeofday (&end, NULL);
	      		seconds = end.tv_sec - start.tv_sec;
	     		useconds = end.tv_usec - start.tv_usec;
	    		mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
	      		//printf ("Elapsed time: %ld milliseconds\n", mtime);
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
bool QueryAnalyzer(unsigned char message[]){
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
	int lenght = strlen(message+12)+1,label,j,k = 1,l = 0;
	i = 13;
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

	/*Connecting to the database*/
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	conn = mysql_init(NULL);
	char query[37] = "select url from pages where url = '";
	char finish [3] = "';";
	char concat[200];
	memset(concat,0,sizeof(concat));
	strcat(concat,query);
	strcat(concat,str);
	strcat(concat,finish);
	printf("QUERY = %s\n",concat);
	if(!(mysql_real_connect(conn,host,user,pw,proxyDB,port,unix_socket,flag))){
		fprintf(stderr,"\nError: %s [%d]\n",mysql_error(conn),mysql_errno(conn));
		return 0;
	}else{
		perror("Connection successfull\n");
		if(mysql_query(conn,concat)){
			fprintf(stderr, "%s\n",mysql_error(conn));
			exit(1);
		}
		res = mysql_use_result(conn);
		if((row = mysql_fetch_row(res)) == NULL){
			printf("Permitir acceso a DNS de google para respuesta\n");
			return 1;
		}
		else{
			printf("DENEGAR ACCESO y mandar servicio HTTP\n");
			return 0;
		}
	}
	free(str);
}
void accessDenied(unsigned char response[]){
	memset(google_answer,0x00,sizeof(google_answer));
	for(i = 0; i < 2; i++){
		google_answer[i] = response[i];
	}
	google_answer[2] = 0x81;/*Flags*/
	google_answer[3] = 0x80;/*Flags*/
	google_answer[4] = 0x00;/*Questions*/
	google_answer[5] = 0x01;/*Questions*/
	google_answer[6] = 0x00;/*Answer RRs*/
	google_answer[7] = 0x01;/*Answer RRs*/
	google_answer[8] = 0x00;/*Authority*/
	google_answer[9] = 0x00;/*Authority*/
	google_answer[10] = 0x00;/*Additional RRs*/
	google_answer[11] = 0x00;/*Additional RRs*/
	i = 12;
	while(response[i] != 0x00){
		google_answer[i] = response[i];
		i++;
	}
	google_answer[i] = 0x00;
	google_answer[i+1] = 0x00;/*Type*/
	google_answer[i+2] = 0x01;/*Type*/
	google_answer[i+3] = 0x00;/*Class*/
	google_answer[i+4] = 0x01;/*Class*/
	i = i +5;
	/* http://localhost:18000/ */
	google_answer[i] = 0xc0;/*Apuntador a nombre*/
	google_answer[i+1] = 0x0c;/*Apuntador a nombre*/
	google_answer[i+2] = 0x00;/*Type*/
	google_answer[i+3] = 0x01;/*Type*/
	google_answer[i+4] = 0x00;/*Class*/
	google_answer[i+5] = 0x01;/*Class*/
	google_answer[i+6] = 0x00;/*Time to live*/
	google_answer[i+7] = 0x00;/*Time to live*/
	google_answer[i+8] = 0x00;/*Time to live*/
	google_answer[i+9] = 0xAA;/*Time to live*/
	google_answer[i+10] = 0x00;/*Data lenght*/
	google_answer[i+11] = 0x04;/*Data lenght*/

	google_answer[i+12] = 0x08;/*Address*/
	google_answer[i+13] = 0x0c;/*Address*/
	google_answer[i+14] = 0x00;/*Address*/
	google_answer[i+15] = 0xeb;/*Address*/
	i = i + 16;
	fprintf(stdout, "\n%s\n\n%s",bin2hex(google_answer,200),google_answer);

}