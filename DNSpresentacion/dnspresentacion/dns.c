#include "dns.h"

bool enviarTrama(struct host *local, struct host *remoto, int bandera)
{
  bool flg = false;
  int n = sendto(local->descriptorSocket, local->trama, local->longitudTrama, bandera, (struct sockaddr *)&remoto->dispositivoRed, sizeof(struct sockaddr_in));
  if(n == -1) perror("sendto()");
  else  flg = true;
  return flg;
}

bool recibirTrama(struct host *local, struct host *remoto, int bandera, int *res)
{
  bool flg = false;
  socklen_t sa_len = sizeof(struct sockaddr_in);
  *res = recvfrom(local->descriptorSocket, remoto->trama, MAX_BUFFER, bandera, (struct sockaddr *)&remoto->dispositivoRed, &sa_len);
  if(*res == -1)  perror("recvfrom()");
  else
  {
    remoto->longitudTrama = *res;
    flg = true;
  }
  return flg;
}

void imprimirTrama(char *trama, unsigned int longitudTrama)
{
  unsigned int i;
  printf("[%d]\t",longitudTrama);
  for(i = 0; i < longitudTrama; i++)
	{
		if(i % 16 == 0) printf("\n");
		printf("%.2x",trama[i]);
	}
	printf("\n");
}

void interpretarTrama(char *trama, unsigned int longitudTrama)
{
  int l_h = sizeof(struct dns_header);
  char recursos[longitudTrama-l_h];
  struct dns_header cabecera;
  memcpy(&cabecera, trama, l_h);
  interpretarCabecera(&cabecera);
  memcpy(recursos, trama+l_h, (longitudTrama - l_h)*sizeof(char));
  interpretarQuery(recursos, (longitudTrama - l_h), false);
}

uint16_t bin_String_to_uint(char *cadena, int longitudCadena)
{
  int i = 0;
  uint16_t r = 0;
  char tmp[longitudCadena+1];
  memset(tmp, 0x00, (longitudCadena+1)*sizeof(char));
  for(i = 0; i < longitudCadena; i++)
    tmp[longitudCadena-(i+1)] = cadena[i];
  for(i = 0; i < longitudCadena; i++)
    r += (uint16_t)(((int)tmp[i]-48)*pow(2,i));
  return r;
}

void interpretarIndicadores(uint16_t indicadores)
{
  int i;
  char aux[4], cadena[16];
  memset(aux,0x00,4*sizeof(char));
  memset(cadena,0x00,16*sizeof(char));
  char *cero = "0000";
  char *uno = "0001";
  char *dos = "0010";
  char *tres = "0011";
  char *cuatro = "0100";
  char *cinco = "0101";
  char *seis = "0110";
  char *siete = "0111";
  char *ocho = "1000";
  char *nueve = "1001";
  char *a = "1010";
  char *b = "1011";
  char *c = "1100";
  char *d = "1101";
  char *e = "1110";
  char *f = "1111";
  uint16_t sr, codigoSolicitud, ra, tr, rs, rd, codigoRespuesta;
  sprintf(aux,"%x",indicadores);
  for(i = 0; i < strlen(aux);i++)
  {
    switch(aux[i])
    {
      case '0': strcat(cadena,cero); break;
      case '1': strcat(cadena,uno); break;
      case '2': strcat(cadena,dos); break;
      case '3': strcat(cadena,tres); break;
      case '4': strcat(cadena,cuatro); break;
      case '5': strcat(cadena,cinco); break;
      case '6': strcat(cadena,seis); break;
      case '7': strcat(cadena,siete); break;
      case '8': strcat(cadena,ocho); break;
      case '9': strcat(cadena,nueve); break;
      case 'a': strcat(cadena,a); break;
      case 'b': strcat(cadena,b); break;
      case 'c': strcat(cadena,c); break;
      case 'd': strcat(cadena,d); break;
      case 'e': strcat(cadena,e); break;
      case 'f': strcat(cadena,f); break;
    }
  }
  memset(aux, 0x00, 4*sizeof(char));
  printf("Indicadores[%x]: %s\n",indicadores,cadena);
  switch(cadena[0])
  {
    case '0': printf("Solicitud de informacion\n"); break;
    case '1': printf("Respuesta a solicitud\n"); break;
  }
  memcpy(aux,cadena+1,4*sizeof(char));
  codigoSolicitud = bin_String_to_uint(aux,strlen(aux));
  switch(codigoSolicitud)
  {
    case 0: printf("Resolucion Directa\n"); break;
    case 1: printf("Resolucion Inversa\n"); break;
    case 2: printf("Solicitud del estado del servidor\n"); break;
    default: printf("Codigo de Solicitud invalido[%d]\n",codigoSolicitud); break;
  }
  memset(aux, 0x00, 4*sizeof(char));
  switch(cadena[5])
  {
    case '0': printf("Respuesta Autorizada\n"); break;
    case '1': printf("Respuesta No Autorizada\n"); break;
  }
  switch(cadena[6])
  {
    case '0': printf("Mensaje no Truncado\n"); break;
    case '1': printf("Mensaje Truncado\n"); break;
  }
  switch(cadena[7])
  {
    case '0': printf("No se desea recursion\n"); break;
    case '1': printf("Si se desea recursion\n"); break;
  }
  switch(cadena[8])
  {
    case '0': printf("No se permite recursion\n"); break;
    case '1': printf("Si se permite recursion\n"); break;
  }
  memcpy(aux,cadena+12,4*sizeof(char));
  codigoRespuesta = bin_String_to_uint(aux,strlen(aux));
  switch(codigoRespuesta)
  {
    case 0: printf("No hubo error\n"); break;
    case 1: printf("Hubo un error de formato\n"); break;
    case 2: printf("Hubo un fallo en el servidor\n"); break;
    case 3: printf("Error en el nombre: no existe"); break;
    case 4: printf("Servidor de nombres no sabe como tratar solicitud\n"); break;
    case 5: printf("Servidor de nombres rechazo peticiones\n"); break;
    default: printf("Codigo de Respuesta invalido[%d]\n",codigoRespuesta); break;
  }
}

void interpretarCabecera(struct dns_header *cabecera)
{
  uint16_t uno = 0x001;
  printf("ID Transaccion: %d\n",cabecera->idTransaccion);
  interpretarIndicadores(cabecera->indicadores);
  printf("Numero Peticiones: %d\n",ntohs(cabecera->contadorPeticiones));
  printf("Numero Respuestas: %d\n",ntohs(cabecera->contadorRRrespuestas));
  printf("Numero Autoridades: %d\n",ntohs(cabecera->contadorRRautoridad));
  printf("Numero Adicionales: %d\n",ntohs(cabecera->contadorRRadicionales));
}

void interpretarQuery(char *query, unsigned int longitudQuery, bool interpretar)
{
  char *nombreRecurso, *datRecurso;
  struct dns_request cuerpoRecurso;
  int lenNombre, i, acum, ttl;
  uint16_t lenDatos;
  /*Determinamos donde esta el nombre de recurso*/
  for(lenNombre = 0; lenNombre < longitudQuery; lenNombre++)
    if(query[lenNombre] == '\0') break;
  /*Extraemos el nombre de recurso*/
  if(lenNombre == 0 || lenNombre == longitudQuery)
  {
    printf("ERROR: Longitud de nombre de recurso incorrecta (%d).\n", lenNombre);
    exit(1);
  }
  if((nombreRecurso = (char *)calloc(lenNombre,sizeof(char))) == NULL)
  {
    perror("calloc()");
    exit(1);
  }
  memcpy(nombreRecurso, query+1, (lenNombre-1)*sizeof(char));
  lenNombre--;
  if(strlen(nombreRecurso) != lenNombre)
  {
    printf("ERROR: Determinacion de longitud del nombre de recurso incorrecta (%d != %d).\n", lenNombre, strlen(nombreRecurso));
    exit(1);
  }
  /*Acoplamos el nombre de recurso*/
  if((acum = (int)query[0]) <= 0)
  {
    printf("ERROR: Longitud de parte de nombre de recurso incorrecta (%d).\n",acum);
    exit(1);
  }
  i = acum;
  while(true)
  {
    if(i < lenNombre)
    {
      acum = (int)nombreRecurso[i];
      if(acum <= 0 || (i + acum) >= lenNombre)
      {
        printf("ERROR: Longitud de parte de nombre de recurso incorrecta (%d).\n", acum);
        exit(1);
      }
      nombreRecurso[i] = '.';
      i += acum + 1;
    }
    else if(i == lenNombre)  break;
    else
    {
      printf("ERROR: Nombre de recurso mal formado.\n");
      exit(1);
    }
  }
  /*Extraemos otros datos del recurso*/
  memcpy(&cuerpoRecurso.tipo, query+(lenNombre+2), sizeof(uint16_t));
  cuerpoRecurso.tipo = ntohs(cuerpoRecurso.tipo);
  memcpy(&cuerpoRecurso.clase, query+(lenNombre+4), sizeof(uint16_t));
  cuerpoRecurso.clase = ntohs(cuerpoRecurso.clase);
  /*Imprimimos datos del recurso*/
  printf("Recurso: %s\n",nombreRecurso);
  printf("Tipo:\t");
  switch(cuerpoRecurso.tipo)
  {
    case 1: printf("A (Address): Direccion Host\n");  break;
    case 2: printf("NS (Name Server): Un servidor de nombres con autoridad\n"); break;
    case 5: printf("CNAME (Cannonical Name): Un alias\n");  break;
    case 6: printf("SOA (Start Of Authority): Comienzo de una zona de autoridad\n");  break;
    case 11:  printf("WKS (Well Known Service): Descripcion de un servicio estandar\n");  break;
    case 12:  printf("PTR (Pointer): Puntero a un nombre de dominio\n");  break;
    case 13:  printf("HINFO (Host Information): Informacion sobre el host\n");  break;
    case 14:  printf("MINFO (Mail Information): Informacion sobre un buzon de mail\n"); break;
    case 15:  printf("MX (Mail Exchange): Servidor de Correo asociado al dominio\n"); break;
    case 16:  printf("TXT (Text): Cadenas de texto informativas\n");  break;
    default: printf("NO SOPORTADA\n"); break;
  }
  printf("Clase:\t");
  switch(cuerpoRecurso.clase)
  {
    case 1: printf("IN: Internet\n");  break;
    case 2: printf("CS: CSNET (obsoleto)\n");  break;
    case 3: printf("CH: CHAOS\n");  break;
    case 4: printf("HS: Hesiod\n");  break;
    default: printf("NO SOPORTADA\n"); break;
  }
  if(interpretar)
  {
    memcpy(&ttl, query+(lenNombre+6), sizeof(int));
    ttl = ntohs(ttl);
    memcpy(&lenDatos, query+(lenNombre+10), sizeof(uint16_t));
    printf("\nTTL:\t%d segundos.",ttl);
    printf("\nLongitud de Datos de Recurso:\t%d Bytes.",lenDatos);
    if((datRecurso = (char *)calloc(lenDatos, sizeof(char))) == NULL)
    {
      perror("calloc()");
      exit(1);
    }
    memcpy(datRecurso, query+(lenNombre+12), lenDatos*sizeof(char));
    switch(cuerpoRecurso.tipo)
    {
      case 1:
        interpretarAddress(datRecurso,lenDatos);
      break;
      default: printf("NO SOPORTADA\n"); break;
    }
    //En caso de que no se haya recorrido todo el buffer, llamamos recursivamente para interpretar siguiente recurso
  }
  else
  {
    uint16_t posRes;
    memcpy(&posRes, query+(lenNombre+6), sizeof(uint16_t));
    //Llamamos recursivamente para interpretar ahora un recurso
  }
}

void interpretarRespuesta(char *respuesta, unsigned int longitudRespuesta)
{

}

void interpretarAddress(char *datosRecurso, unsigned int longitudDatosRecurso)
{
  printf("Aqui se interpreta un Address\n");
}

bool acoplarRecurso(char *recurso, char *respuesta)
{
  bool flg = false;
  int i, l;
  if((respuesta = (char *)calloc((strlen(recurso)+2),sizeof(char))) == NULL)
  {
    perror("calloc()");
    exit(1);
  }
  respuesta[0] = '.';
  memcpy(respuesta+1, recurso, strlen(recurso));
  l = 0;
  for(i = strlen(recurso); i > 0; i--)
  {
    if(respuesta[i] != '.') l++;
    else
    {
      respuesta[i] = (char)l;
      l = 0;
    }
  }
  respuesta[0] = (char)l;
  flg = true;
  return flg;
}

void modoCliente(struct host *local, struct host *servidor, bool inv, char *serv, char *recurso)
{
  struct dns_header cabecera;
  struct dns_request cuerpoRecurso;
  uint16_t i, r;
  char rec, *nombreRecurso;
  int l, res;
  printf("Ejecutando en modo Cliente.\n");
  do
  {
    printf("¿Desea buscar recursivamente? (s/n)");
    scanf("%c",&rec);
  }while(rec != 's' && rec != 'n');
  if(rec == 's')  r = 0x0100;  //0000 0001 0000 0000
  else  r = 0x0000;
  cabecera.idTransaccion = 1;
  cabecera.indicadores = htons(r);
  cabecera.contadorPeticiones = htons(1);
  cabecera.contadorRRrespuestas = htons(0);
  cabecera.contadorRRautoridad = htons(0);
  cabecera.contadorRRadicionales = htons(0);
  cuerpoRecurso.tipo = htons(1); //IN
  cuerpoRecurso.clase = htons(1); //A (Address)
  /*
  if((nombreRecurso = (char *)calloc((strlen(recurso)+2),sizeof(char))) == NULL)
  {
    perror("calloc()");
    exit(1);
  }
  nombreRecurso[0] = '.';
  memcpy(nombreRecurso+1, recurso, strlen(recurso));
  l = 0;
  for(i = strlen(recurso); i > 0; i--)
  {
    if(nombreRecurso[i] != '.') l++;
    else
    {
      nombreRecurso[i] = (char)l;
      l = 0;
    }
  }
  nombreRecurso[0] = (char)l;
  */
  if(acoplarRecurso(recurso, nombreRecurso))
  {
    servidor->dispositivoRed.sin_family = AF_INET;
    servidor->dispositivoRed.sin_addr.s_addr = inet_addr(serv);
    servidor->dispositivoRed.sin_port = htons(puertoEscucha);
    if(establecerPuntoComunicacion(local,AF_INET,SOCK_DGRAM,0,0))
    {
      l = 0;
      memcpy(local->trama, &cabecera, sizeof(struct dns_header));
      l += sizeof(struct dns_header);
      memcpy(local->trama+l, nombreRecurso, (strlen(recurso)+2)*sizeof(char));
      printf("aqui\n");
      l += (strlen(recurso)+2)*sizeof(char);
      memcpy(local->trama+l, &cuerpoRecurso, sizeof(struct dns_request));
      l += sizeof(struct dns_request);
      local->longitudTrama = l;
      if(enviarTrama(local,servidor,0))
        if(recibirTrama(local, servidor, 0, &res))
          interpretarTrama(servidor->trama, servidor->longitudTrama);
    }
  }
  else
  {
    printf("ERROR: Acoplar nombre recurso.\n");
    exit(1);
  }
}

void modoServidor(struct host *local)
{
  printf("Ejecutando en modo Servidor.\n");
}

bool establecerPuntoComunicacion(struct host *d, int dominio, int tipo, int protocolo, int puerto)
{
  bool flg = false;
  d->dispositivoRed.sin_family = dominio;
  d->dispositivoRed.sin_addr.s_addr = INADDR_ANY;
  d->dispositivoRed.sin_port = htons(puerto);
  d->descriptorSocket = socket(dominio, tipo, protocolo);
  if(d->descriptorSocket == -1) perror("socket() [ERROR]");
  else
    if(bind(d->descriptorSocket, (struct sockaddr *)&d->dispositivoRed, sizeof(struct sockaddr_in)) < 0)  perror("bind() [ERROR]");
    else  flg = true;
  return flg;
}

bool validarArgumentos(int argc, char **argv, bool *inv)
{
  bool flg = false;
  if(argc == 2)
    if(strcmp(argv[1], "-s") == 0)
      flg = true;
  if(argc == 4)
    if(strcmp(argv[1], "-c") == 0)
      if(inet_addr(argv[2]) != INADDR_NONE)
        if(strlen(argv[3]) > 0 || inet_addr(argv[3]) != INADDR_NONE) flg = true;
  if(flg && inet_addr(argv[3]) != INADDR_NONE)
    *inv = true;
  else
    *inv = false;
  return flg;
}
