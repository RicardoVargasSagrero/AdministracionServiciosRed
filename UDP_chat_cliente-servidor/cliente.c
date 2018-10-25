#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <string.h>
unsigned char mensaje[516];
unsigned char recibe[516];
int counterInteger = 1;
void FirstMessage();
int main(){
    struct sockaddr_in local, remota, cliente;
    int udp_socket, lbind, tam, ptam,opcode;
    udp_socket = udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    unsigned char filename[100] = "prueba.c";
    unsigned char ip[15] = "192.168.1.65";
    int lenght;
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
            remota.sin_port=htons(69);
            remota.sin_addr.s_addr=inet_addr(ip);

            for(;;){
              opcode = (recibe[0] >> 8)+recibe[1];
              switch(opcode){
                case 1:
                    mensaje[0] = 0x00;
                    mensaje[1] = 0x01;
                    strcpy(mensaje+2,filename);
                    lenght = strlen(filename)+3;
                    mensaje[lenght] = 0x00;
                    strcpy(mensaje+lenght,"octet");
                    lenght = lenght + strlen("octet");
                    mensaje[lenght+1] = 0x00;
                    tam = sendto(udp_socket, mensaje, lenght+1, 0, (struct sockaddr *)&remota, sizeof(remota));
                    break;
                case 2:
                    mensaje[0] = 0X00;
                    mensaje[1] = 0X02;
                    strcpy(mensaje+2,filename);
                    lenght = strlen(filename)+3;
                    mensaje[lenght] = 0x00;
                    strcpy(mensaje+lenght,"octet");
                    lenght = lenght + strlen("octet");
                    mensaje[lenght+1] = 0x00;
                    tam = sendto(udp_socket, mensaje, lenght+1, 0, (struct sockaddr *)&remota, sizeof(remota));
                    break;
                case 3:
                    
              }
              printf("We send the FirstMessage");
              if(!(recibe[1] == 0x04)){
                mensaje[0] = 0X00;
                mensaje[1] = 0X02;
                strcpy(mensaje+2,filename);
                lenght = strlen(filename)+3;
                mensaje[lenght] = 0x00;
                strcpy(mensaje+lenght,"octet");
                lenght = lenght + strlen("octet");
                mensaje[lenght+1] = 0x00;
                tam = sendto(udp_socket, mensaje, lenght+1, 0, (struct sockaddr *)&remota, sizeof(remota));
              }
              else{
                mensaje[0] = 0X00;
                mensaje[1] = 0X03;
                mensaje[2] = counterInteger >> 8;
                mensaje[3] = counterInteger;
                tam = sendto(udp_socket, mensaje, 516, 0, (struct sockaddr *)&remota, sizeof(remota));
                counterInteger++;
                if(counterInteger == 10)
                  exit(0);
              }
              if(tam ==-1){
                  perror("\nError al enviar");
                  exit(1);
              }else{
                  printf("\nYo digo: '%s'", mensaje);
                  ptam = sizeof(remota);
                  tam = recvfrom(udp_socket, recibe, 512, 0, (struct sockaddr *)&remota, &ptam);
                  if(tam ==-1){
                      perror("\nError al recibir");
                      exit(1);
                  }else{
                      printf("\nEl dice: %s", recibe);
                      printf("\nConectado por el puerto: %d", ntohs(remota.sin_port));
                      //printf("\nCliente: %s", inet_ntoa(remota.sin_addr));
                      puts("\n");
                  }
              }
            }

        }
    }
    close(udp_socket);
    return 1;
}
void FirstMessage(){

}
