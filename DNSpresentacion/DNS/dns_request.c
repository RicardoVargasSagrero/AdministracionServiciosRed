/**Ricardo Vargas Sagrero DNS request
DNS(Domain Name System) uses UDP (User Datagram Protocol) by the port 53
Consult DNS message

Google DNS (8.8.8.8)
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
unsigned char mensaje[516];
unsigned char recibe[516];
int counterInteger = 1;
void FirstMessage(int,unsigned char[],int,struct sockaddr_in);
int main(){
    struct sockaddr_in local, remota, cliente;
    int udp_socket, lbind, tam, ptam,opcode;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char filename[100] = "prueba.c";
    unsigned char ip[15] = "8.8.8.8";
    int lenght;
    struct timeval start, end;
    long mtime=0, seconds, useconds; 
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
            while(mtime < 100000){
              
            }
            gettimeofday(&end, NULL);
            seconds  = end.tv_sec  - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
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
  mensaje[0] = 0x00;
  mensaje[1] = 0x3d; //Le asignamos el ID 3D
  //Flags
  mensaje[2] = 0x00;
  mensaje[3] = 0x01;
  //Contador de peticiones (Questions), debe cambiar
  mensaje[4] = 0x00;
  mensaje[5] = 0x01;
  //Answer RRS (Contador de RR(Resource Record) de respuesta)
  //Cuando lleguen las respuestas este contador debe ah
  mensaje[6] = 0x00;
  mensaje[7] = 0x00;
  //Authority RRs (Contador RR de autoridad)
  mensaje[8] = 0x00;
  mensaje[9] = 0x00;
  //Addtional RRs (Contador Adicional de RRs)
  mensaje[10] = 0x00;
  mensaje[11] = 0x01;
  mensaje[12] = 0x03;
  mensaje[13] = 0x77;
  mensaje[14] = 0x77;
  mensaje[15] = 0x77;
  mensaje[16] = 0x03;
  mensaje[17] = 0x69;
  mensaje[18] = 0x70;
  mensaje[19] = 0x6e;
  mensaje[20] = 0x02;
  mensaje[21] = 0x6d;
  mensaje[22] = 0x78;
  mensaje[23] = 0x00;
  mensaje[24] = 0x00;
  sendto(udp_socket,mensaje,25,MSG_DONTWAIT,(struct sockaddr *) &remota,sizeof(remota));
}
