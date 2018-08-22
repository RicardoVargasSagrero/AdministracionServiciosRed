 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/ip.h> /* superset of previous */
 #include <stdlib.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <arpa/inet.h>
 #include <sys/time.h>

 int main()
 {
 struct sockaddr_in local, remota;    
 int udp_socket,lbind,tam,lrecv,bandera;
 unsigned char mensaje[1514];
 struct timeval start, end;
 long mtime=0, seconds, useconds; 

 udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
 if(udp_socket==-1)
   {
    perror("\nError al abrir el socket");
    exit(1);
   }
 else
   {
    perror("\nExito al abrir el socket");
    local.sin_family=AF_INET;
    local.sin_port=htons(8000);
    local.sin_addr.s_addr=INADDR_ANY;
    lbind=bind(udp_socket,(struct sockaddr *)&local,sizeof(local));
    if(lbind==-1)
      {
      perror("\nError en bind");
      exit(1);
      }
    else
      {
       printf("\nExito en bind");
       lrecv=sizeof(remota);
       gettimeofday(&start, NULL);
       bandera=0;
       while(mtime<5000)
       {
       tam=recvfrom(udp_socket,mensaje,512,MSG_DONTWAIT,(struct sockaddr*)&remota,&lrecv);
       if(tam==-1)
        {
         perror("\nError al recibir");
        }
       else
        {
         printf("\nExito al recibir: %s",mensaje);
         bandera=1;
        }
        gettimeofday(&end, NULL);
        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        printf("Elapsed time: %ld milliseconds\n", mtime);
        if(bandera==1)
            break;
       }
      }
   }
 
 close(udp_socket);
 return 0;
 }
