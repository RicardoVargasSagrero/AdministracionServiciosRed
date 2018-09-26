#ifndef DNS_H
#define DNS_H

  #include <sys/wait.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/ipc.h>
  #include <sys/shm.h>
  #include <sys/time.h>
  #include <sys/sendfile.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <semaphore.h>
  #include <pthread.h>
  #include <fcntl.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <errno.h>
  #include <iostream>
  #include <iomanip>
  #include <cstdio>
  #include <cstdlib>
  #include <cstdint>
  #include <cstring>
  #include <ctime>
  #include <cmath>
  #include <string>
  #include <vector>
  #include <fstream>

  #define MAX_BUFFER 1024
  #define puertoEscucha 53
  #define numNombres 20
  #define longitudNombres 128

  using namespace std;
  /*
  Tipos de Recursos: (- Experimentales, *mas de un elemento de informacion) <arpa/nameser.h>

  A 1 (Address) Direccion de un host
  NS  2 (Name Server) Un servidor de nombres con autoridades
  CNAME 5 (Canonical Name)  Un alias
  SOA 6 (Start of Authority) Indica el comienzo de una zona de autoridades
  WKS 11  (Well known service)  Indica el comienzo de una zona de autoridad
  PTR 12  (Pointer) Puntero a un nombre de dominio
  HINFO 13  (Host Information)  Informacion sobre el host
  MINFO 14  (Mail Information)  Informacion sobre un buzon o lista de correo
  MX  15  (Mail exchange) Servidor de correo asociado al dominio
  TXT 16  (Text)  Cadenas de texto informativas
  -MB 7 (MailBox) Nombre de dominio para un buzon de correo
  -MG 8 (Mail Group) Miembro de un grupo de correo
  -MR 9 (Mail Rename) Nombre de dominio de correo renombrado
  -NULL 10  Registro Nulo
  *AXFR 252 La zona entera
  *MAILB  253 Registros relacionados con el buzon de correo (MB,MG,MR)
  *MAILA  254 Registros relaciones con el agente de correo
  * * 255 Todos los registros

  Clases de recursos: <arpa/nameser.h>
  IN  1 internet
  CS  2 CSNET(obsoleto)
  CH  3 CHAOS
  HS  4 Hesiod
  * 255 ALL
  */

  struct dns_header
  {
    uint16_t idTransaccion;
    uint16_t indicadores;
    uint16_t contadorPeticiones;
    uint16_t contadorRRrespuestas;
    uint16_t contadorRRautoridad;
    uint16_t contadorRRadicionales;
  };

  struct dns_request
  {
    uint16_t tipo;
    uint16_t clase;
  };

  struct dns_query
  {
    vector<string> nombres;
    struct dns_request informacionRecurso;
  };

  struct dns_record
  {
    struct dns_query detallesRecurso;
    int ttl;
    uint16_t lenDatos;
    char *datos;
    int offsetInicioDatos;
  };

  /*
  IDENTIFICADOR -> 2 Bytes
  *************************************************************************************
  INDICADORES -> 2 Bytes::
  ------------------------------- 1er Byte
  SR ->1 bit (Sort Request) 0->RQ, 1->RS
  CodSol -> 4 bit (Codigo Solicitud) 0->Resolucion directa, 1->Resolucion Inversa, 2->Solicitud de estado del servidor, 3-15->reservado futuro
  RA ->1 bit (Respuesta autorizada) 0-> no, 1-> si
  TR-> 1 bit (Truncado) indica si el mensaje fue truncado 0->no, 1->si
  RS -> 1 bit (Respuesta a la solicitud) indica si se desea recursividad
  ------------------------------- 2do Byte
  RD -> 1 bit (Permiso busqueda recursiva) Indicado por el servidor si este permite o no busqueda recursividad
  Reservado -> 3 bit (Campo reservado para usos futuros) se pone a 0
  CR -> 4 bit (Codigo respuesta) indica el tipo de respuesta:
    0-> sin error, 1->error de formato y servidor de nombres no pudo interpretar RQ
    2->fallo en servidor por lo que la RQ no pudo llevarse a cabo
    3->error en el nombre indica que el nombre de dominio referenciado no existe (solo lo responde servidor con autoridad)
    4->Se realizo una solicitud que el servidor no sabe como tratar
    5->servidor rechazo RQ
    6-15->Reservados para usos futuros
  *************************************************************************************
  NUMERO RRs EN EL CAMPO (Solicitud de informacion) -> 2 Bytes [Contador peticiones]
  *************************************************************************************
  NUMERO RRs EN EL CAMPO (Respuesta a la solicitud) -> 2 Bytes [Contador de respuestas]
  *************************************************************************************
  NUMERO RRs EN EL CAMPO (Servidor con Autoridad) -> 2 Bytes [Contador autoridades]
  *************************************************************************************
  NUMERO RRs EN EL CAMPO (Información Adicional) -> 2 Bytes [Contador adicionales]
  *\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\
  CADA RR tiene la siguiente composicion
  Nombre del RR -> N Bytes (ultimo bit en 0)
  Tipo de Registro -> 2 Bytes indica el tipo de solicitud
  Clase de Registro -> 2 Bytes
  TTL -> 2 Bytes tiempo de vida
  Longitud -> 2 Bytes indica la longitud del recurso
  Datos del recurso -> N bytes
  */

  struct host
  {
    int descriptorSocket;
    struct sockaddr_in dispositivoRed;
    char trama[MAX_BUFFER];
    unsigned int longitudTrama;
  };

  uint16_t bin_String_to_uint(char *cadena, int longitudCadena);
  bool enviarTrama(struct host *local, struct host *remoto, int bandera);
  bool recibirTrama(struct host *local, struct host *remoto, int bandera, int *res);
  uint16_t obtenerDesplazamiento(uint16_t puntero);
  bool esPuntero(uint16_t puntero);
  bool determinarDatos(char *datos, int partida, string &resultado, int *posFin, bool *fuePuntero);
  void interpretarTrama(char *trama, unsigned int longitudTrama);
  void interpretarCabecera(struct dns_header *cabecera);
  void interpretarIndicadores(uint16_t indicadores);
  void interpretarQuery(struct dns_query * query);
  void interpretarRecord(struct dns_record *record, char *trama);
  bool determinarDatosQuery(char *recursos, int posicionInic, int *posicionSig, struct dns_query * query, bool *fuePuntero);
  bool determinarDatosRecord(char *recursos, int posicionInic, int *posicionSig, struct dns_record *record);
  void modoCliente(struct host *local, struct host *servidor, char *serv, char *recurso);
  bool establecerPuntoComunicacion(struct host *d, int dominio, int tipo, int protocolo, int puerto);
  bool validarArgumentos(int argc, char **argv);
  string interpretarNombre(string entrada);

  void mostrarA(struct dns_record *record);
  void mostrarNS(struct dns_record *record, char *trama);
  void mostrarCNAME(struct dns_record *record, char *trama);
  void mostrarSOA(struct dns_record *record, char *trama);
  void mostrarWKS(struct dns_record *record);
  void mostrarPTR(struct dns_record *record, char *trama);
  void mostrarHINFO(struct dns_record *record);
  void mostrarMINFO(struct dns_record *record, char *trama);
  void mostrarMX(struct dns_record *record, char *trama);
  void mostrarTXT(struct dns_record *record);
  void mostrarAAAA(struct dns_record *record);
#endif
