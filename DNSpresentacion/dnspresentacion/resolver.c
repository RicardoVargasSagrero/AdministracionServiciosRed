/*
  Este programa utiliza el API ofrecido por el resolver para obtener las direcciones de todos los hosts de una zona
*/
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

typedef union
{
  HEADER cab;
  u_char dat[PACKETSZ]
} tmsg;

int main(int argc, char **argv)
{
  int s;  /*socket para establecer la conexion con el servidor*/
  tmsg sol; /*mensaje de solicitud*/
  u_short cnsol;  /*longitud mensaje de solicitud en modo caracter*/
  int nsol; /*longitud del mensaje de solicitud como entero*/
  struct sockaddr_in sin; /*datos para realizar la conexion con el servidor*/
  struct in_addr dir; /*direccion IP de los hosts del dominio*/
  HEADER *cab;  /*ṕuntero a la cabecera del mensaje*/
  char nombre[MAXDNAME] /*nombre expandido del dominio*/
  int i;  /*variable auxiliar para bucles*/
  u_short tipo; /*tipo de un registro*/
  u_char *res; /*buffer para el mensaje de respuesta*/
  u_char *fin;  /*puntero al final del mensaje*/
  u_char *p;  /*puntero a la posicion actual dentro del mensaje*/
  u_short cnres;  /*longitud del mensaje de respuesta*/
  u_short ndat; /*longitud del campo de datos del registro*/
  int nres; /*longitud del mensaje de respuesta*/
  char soa; /*contador de los registros SOA*/
  int nnom; /*longitud del nombre de dominio*/
  res_init(); /*inicializar el resolver para que tome al menos uno de los servidores de nombres que se hayan configurad en el archivo de sistema /etc/resolv.conf*/
  /*A continuacion se crea un mensaje de peticion para que el servidor de nombres nos transfiera toda una zona*/
  if((nsol = res_mkquery(QUERY,argv[1],C_IN, T_AXFR, NULL, 0, NULL, sol.dat, sizeof(sol))) < 0)
  {
    herror("res_mkquery()");
    exit(1);
  }
  /*A continuacion se crea un socket y se establece la conexion con el servidor*/
  sin.sin_family = AF_INET;
  sin.sin_port = htons(NAMESERVER_PORT);
  sin.sin_addr = _res.naddr_list[0].sin_addr;
  if((s = socket(AF_INET,SOCK_STREAM,0)) < 0)
  {
    perror("socket()");
    exit(1);
  }
  if(connect(s,(struct sockaddr *)&sin,sizeof(sin)) < 0)
  {
    perror("connect()");
    exit(1);
  }
  /*Se envia la longitud del mensaje de peticion y el propio mensaje que ha sido creado con anterioridad con la funcion res_mkquery()*/
  __putshort(nsol,(u_char *)&cnsol);
  if(write(s, (char *)&cnsol,INT16SZ) != INT16SZ)
  {
    perror("write()");
    exit(1);
  }
  if(write(s, (char *)&sol,nsol) != nsol)
  {
    perror("write()");
    exit(1);
  }
  soa = 0;
  while(soa != 2)
  {
    /*Se lee en primer lugar el tamanio en caracteres de la respuesta*/
    if(read(s,(u_char *)&cnres, INT16SZ) != INT16SZ)
    {
      perror("read()");
      exit(1);
    }
    nres = _getshort((u_char *)&cnres);
    /*Se reserva un buffer de suficiente tamanio para almacenar la respuesta y se realiza la lectura del mismo*/
    
  }
  return 0;
}
