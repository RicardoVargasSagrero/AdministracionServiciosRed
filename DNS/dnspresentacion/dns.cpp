#include "dns.h"

using namespace std;

bool enviarTrama(struct host *local, struct host *remoto, int bandera)
{
  bool flg = false;
  int n = sendto(local->descriptorSocket, local->trama, local->longitudTrama, bandera, (struct sockaddr *)&remoto->dispositivoRed, sizeof(struct sockaddr_in));
  if(n == -1) cerr << "sendto(): " << strerror(errno) << endl;
  else  flg = true;
  return flg;
}

bool recibirTrama(struct host *local, struct host *remoto, int bandera, int *res)
{
  bool flg = false;
  socklen_t sa_len = sizeof(struct sockaddr_in);
  *res = recvfrom(local->descriptorSocket, remoto->trama, MAX_BUFFER, bandera, (struct sockaddr *)&remoto->dispositivoRed, &sa_len);
  if(*res == -1)  cerr << "recvfrom(): " << strerror(errno) << endl;
  else
  {
    remoto->longitudTrama = *res;
    flg = true;
  }
  return flg;
}

uint16_t obtenerDesplazamiento(uint16_t puntero)
{
  uint16_t desp = 0, comp = 0x3fff;
  desp = puntero&comp;
  return desp;
}

bool esPuntero(uint16_t puntero)
{
  bool flg = false;
  uint16_t comp = 0xc000;
  if(puntero >= comp)
    flg = true;
  return flg;
}

bool determinarDatos(char *datos, int partida, string &resultado, int *posFin, bool *fuePuntero)
{
  bool flg = false, stop = false;
  int i, finLoc;
  string loc = "", forn = "";
  uint16_t puntero;i = partida;
  char aux[4];
  memset(aux, 0x00, 4*sizeof(char));
  do
  {
    memcpy(&puntero, datos+partida, sizeof(uint16_t));
    puntero = ntohs(puntero);
    sprintf(aux,"%x",puntero);
    if(esPuntero(puntero))
    {
      if(determinarDatos(datos, obtenerDesplazamiento(puntero), forn, &finLoc, &stop))
      {
        loc += forn;
        *posFin = finLoc;
        flg = true;
      }
      else  *posFin = partida;
      *fuePuntero = true;
      break;
    }
    else
    {
      for(i = partida; datos[i] != '\0'; i++)
      {
        memcpy(&puntero, datos+i, sizeof(uint16_t));
        puntero = ntohs(puntero);
        if(esPuntero(puntero))
        {
          forn = "";
          if(determinarDatos(datos, obtenerDesplazamiento(puntero), forn, &finLoc, &stop))
          {
            loc += forn;
            i += 2;
          }
          else
          {
            cout << "ERROR: NOMBRE COMPUESTO, ERROR EN PUNTERO" << endl;
            exit(1);
          }
        }
        else  loc += datos[i];
      }
      if(loc.length() > 0)
      {
        flg = true;
        *posFin = i;
        break;
      }
      else
      {
        *posFin = partida;
        break;
      }
      *fuePuntero = false;
    }
  }while(true);
  resultado = loc;
  return flg;
}

bool determinarDatosQuery(char *recursos, int posicionInic, int *posicionSig, struct dns_query *query, bool *fuePuntero)
{
  bool flg = false, stop = false;
  string nombreRecurso;
  if(determinarDatos(recursos, posicionInic, nombreRecurso, posicionSig, fuePuntero))
  {
    query->nombres.push_back(nombreRecurso);
    *posicionSig += 1;
    memcpy(&query->informacionRecurso.tipo, recursos+*posicionSig, sizeof(uint16_t));
    query->informacionRecurso.tipo = ntohs(query->informacionRecurso.tipo);
    *posicionSig += sizeof(uint16_t);
    memcpy(&query->informacionRecurso.clase, recursos+*posicionSig, sizeof(uint16_t));
    query->informacionRecurso.clase = ntohs(query->informacionRecurso.clase);
    *posicionSig += sizeof(uint16_t);
    flg = true;
  }
  return flg;
}

bool determinarDatosRecord(char *recursos, int posicionInic, int *posicionSig, struct dns_record *record)
{
  bool flg = false, fuePuntero;
  int sigLocal;
  if(determinarDatosQuery(recursos, posicionInic, &sigLocal, &record->detallesRecurso, &fuePuntero))
  {
    if(fuePuntero)
    {
      sigLocal = posicionInic + 2;
      memcpy(&record->detallesRecurso.informacionRecurso.tipo, recursos+sigLocal, sizeof(uint16_t));
      record->detallesRecurso.informacionRecurso.tipo = ntohs(record->detallesRecurso.informacionRecurso.tipo);
      sigLocal += sizeof(uint16_t);
      memcpy(&record->detallesRecurso.informacionRecurso.clase, recursos+sigLocal, sizeof(uint16_t));
      record->detallesRecurso.informacionRecurso.clase = ntohs(record->detallesRecurso.informacionRecurso.clase);
      sigLocal += sizeof(uint16_t);
      memcpy(&record->ttl, recursos+sigLocal, sizeof(int));
      record->ttl = ntohl(record->ttl);
      sigLocal += sizeof(int);
      memcpy(&record->lenDatos, recursos+sigLocal, sizeof(uint16_t));
      record->lenDatos = ntohs(record->lenDatos);
      sigLocal += sizeof(uint16_t);
      record->offsetInicioDatos = sigLocal;
      if((record->datos = (char *)calloc(record->lenDatos+1,sizeof(char))) == NULL)
      {
        cout << "calloc()" << endl;
        exit(1);
      }
      memcpy(record->datos, recursos+sigLocal, record->lenDatos*sizeof(char));
      sigLocal += record->lenDatos*sizeof(char);
      if(sigLocal > posicionInic)
      {
        *posicionSig = sigLocal;
        flg = true;
      }
    }
    else  *posicionSig = posicionInic;
  }
  return flg;
}

void interpretarTrama(char *trama, unsigned int longitudTrama)
{
  int l_h = sizeof(struct dns_header), i, inic_peticiones, inic_respuestas, inic_autoridad, inic_adicionales, posicionAct, posicionSig;
  char recursos[longitudTrama-l_h];
  struct dns_header cabecera;
  memcpy(&cabecera, trama, l_h);
  cout << "-------------------------------------" << endl;
  cout << "CABECERA RESPUESTA DNS" << endl;
  cout << "-------------------------------------" << endl;
  interpretarCabecera(&cabecera);
  struct dns_query queries[cabecera.contadorPeticiones];
  struct dns_record answers[cabecera.contadorRRrespuestas];
  struct dns_record auth[cabecera.contadorRRautoridad];
  struct dns_record adic[cabecera.contadorRRadicionales];
  cout << "-------------------------------------" << endl;
  cout << "QUERIES RESPUESTA DNS" << endl;
  cout << "-------------------------------------" << endl;
  inic_peticiones = l_h;
  posicionAct = inic_peticiones;
  for(i = 0; i < cabecera.contadorPeticiones; i++)
  {
    cout << "QUERY[" << i << "]: " << endl;
    if(!determinarDatosQuery(trama, posicionAct, &posicionSig, &queries[i], NULL))
    {
      cout << "ERROR: QUERY MAL FORMADO" << endl;
      exit(1);
    }
    if(posicionSig >= (longitudTrama - l_h))
    {
      cout << "ERROR: NUMERO DE QUERIES MAL FORMADO" << endl;
      exit(1);
    }
    else
    {
      interpretarQuery(&queries[i]);
      posicionAct = posicionSig;
    }
    cout << "-------------------------------------" << endl;
  }
  cout << "RESPUESTAS DNS" << endl;
  cout << "-------------------------------------" << endl;
  for(i = 0; i < cabecera.contadorRRrespuestas; i++)
  {
    cout << "RESPUESTA[" << i << "]: " << endl;
    if(!determinarDatosRecord(trama, posicionAct, &posicionSig, &answers[i]))
    {
      cout << "ERROR: RESPUESTA MAL FORMADA(" << i << ")" << endl;
      exit(1);
    }
    else
    {
        interpretarRecord(&answers[i],trama);
        posicionAct = posicionSig;
    }
    cout << "-------------------------------------" << endl;
  }
  cout << "AUTORIDAD DNS" << endl;
  cout << "-------------------------------------" << endl;
  for(i = 0; i < cabecera.contadorRRautoridad; i++)
  {
    if(!determinarDatosRecord(trama, posicionAct, &posicionSig, &auth[i]))
    {
      cout << "ERROR: AUTORIDAD MAL FORMADA" << endl;
      exit(1);
    }
    interpretarRecord(&auth[i],trama);
    posicionAct = posicionSig;
    cout << "-------------------------------------" << endl;
  }
  cout << "ADICIONALES DNS" << endl;
  cout << "-------------------------------------" << endl;
  for(i = 0; i < cabecera.contadorRRadicionales; i++)
  {
    if(!determinarDatosRecord(trama, posicionAct, &posicionSig, &adic[i]))
    {
      cout << "ERROR: ADICIONAL MAL FORMADA" << endl;
      exit(1);
    }
    interpretarRecord(&adic[i],trama);
    posicionAct = posicionSig;
    cout << "-------------------------------------" << endl;
  }
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
  string cero = "0000";
  string uno = "0001";
  string dos = "0010";
  string tres = "0011";
  string cuatro = "0100";
  string cinco = "0101";
  string seis = "0110";
  string siete = "0111";
  string ocho = "1000";
  string nueve = "1001";
  string a = "1010";
  string b = "1011";
  string c = "1100";
  string d = "1101";
  string e = "1110";
  string f = "1111";
  string cad = "";
  uint16_t sr, codigoSolicitud, ra, tr, rs, rd, codigoRespuesta;
  sprintf(aux,"%x",indicadores);
  for(i = 0; i < strlen(aux);i++)
  {
    switch(aux[i])
    {
      case '0': cad += cero; break;
      case '1': cad += uno; break;
      case '2': cad += dos; break;
      case '3': cad += tres; break;
      case '4': cad += cuatro; break;
      case '5': cad += cinco; break;
      case '6': cad += seis; break;
      case '7': cad += siete; break;
      case '8': cad += ocho; break;
      case '9': cad += nueve; break;
      case 'a': cad += a; break;
      case 'b': cad += b; break;
      case 'c': cad += c; break;
      case 'd': cad += d; break;
      case 'e': cad += e; break;
      case 'f': cad += f; break;
    }
  }
  memcpy(cadena, cad.c_str(), 16*sizeof(char));
  string indic(aux);
  cout << "Indicadores[0x" << indic << "]: " << cadena << endl;
  switch(cadena[0])
  {
    case '0': cout << "Solicitud de informacion" << endl; break;
    case '1': cout << "Respuesta a solicitud" << endl; break;
  }
  memcpy(aux,cadena+1,4*sizeof(char));
  codigoSolicitud = bin_String_to_uint(aux,strlen(aux));
  switch(codigoSolicitud)
  {
    case 0: cout << "Resolucion Directa" << endl; break;
    case 1: cout << "Resolucion Inversa" << endl; break;
    case 2: cout << "Solicitud del estado del servidor" << endl; break;
    default: cout << "Codigo de Solicitud invalido[" << codigoSolicitud << "]" << endl; break;
  }
  memset(aux, 0x00, 4*sizeof(char));
  switch(cadena[5])
  {
    case '0': cout << "Respuesta Autorizada" << endl; break;
    case '1': cout << "Respuesta No Autorizada" << endl; break;
  }
  switch(cadena[6])
  {
    case '0': cout << "Mensaje no Truncado" << endl; break;
    case '1': cout << "Mensaje Truncado" << endl; break;
  }
  switch(cadena[7])
  {
    case '0': cout << "No se desea recursion" << endl; break;
    case '1': cout << "Si se desea recursion" << endl; break;
  }
  switch(cadena[8])
  {
    case '0': cout << "No se permite recursion" << endl; break;
    case '1': cout << "Si se permite recursion" << endl; break;
  }
  memcpy(aux,cadena+12,4*sizeof(char));
  codigoRespuesta = bin_String_to_uint(aux,strlen(aux));
  switch(codigoRespuesta)
  {
    case 0: cout << "No hubo error" << endl; break;
    case 1: cout << "Hubo un error de formato" << endl; break;
    case 2: cout << "Hubo un fallo en el servidor" << endl; break;
    case 3: cout << "Error en el nombre: no existe" << endl; break;
    case 4: cout << "Servidor de nombres no sabe como tratar solicitud" << endl; break;
    case 5: cout << "Servidor de nombres rechazo peticiones" << endl; break;
    default: cout << "Codigo de Respuesta invalido[" << codigoRespuesta << "]" << endl; break;
  }
}

void interpretarCabecera(struct dns_header *cabecera)
{
  uint16_t uno = 0x001;
  cout << "ID Transaccion: " << cabecera->idTransaccion << endl;
  interpretarIndicadores(cabecera->indicadores);
  cabecera->contadorPeticiones = ntohs(cabecera->contadorPeticiones);
  cout << "Numero Peticiones: " << cabecera->contadorPeticiones << endl;
  cabecera->contadorRRrespuestas = ntohs(cabecera->contadorRRrespuestas);
  cout << "Numero Respuestas: " << cabecera->contadorRRrespuestas << endl;
  cabecera->contadorRRautoridad = ntohs(cabecera->contadorRRautoridad);
  cout << "Numero Autoridades: " << cabecera->contadorRRautoridad << endl;
  cabecera->contadorRRadicionales = ntohs(cabecera->contadorRRadicionales);
  cout << "Numero Adicionales: " << cabecera->contadorRRadicionales << endl;
}

void interpretarQuery(struct dns_query * query)
{
  int i;
  cout << "Nombre:\t";
  for(i = 0; i < query->nombres.size(); i++)  cout << interpretarNombre(query->nombres[i]) << endl;
  cout << "Tipo:\t";
  switch(query->informacionRecurso.tipo)
  {
    case 1: cout << "A (Address IPv4): Direccion Host" << endl; break;
    case 2: cout << "NS (Name Server): Un servidor de nombres con autoridad" << endl; break;
    case 5: cout << "CNAME (Cannonical Name): Un alias" << endl; break;
    case 6: cout << "SOA (Start Of Authority): Comienzo de una zona de autoridad" << endl; break;
    case 11: cout << "WKS (Well Known Service): Descripcion de un servicio estandar" << endl; break;
    case 12: cout << "PTR (Pointer): Puntero a un nombre de dominio" << endl; break;
    case 13: cout << "HINFO (Host Information): Informacion sobre el host" << endl; break;
    case 14: cout << "MINFO (Mail Information): Informacion sobre un buzon de mail" << endl; break;
    case 15: cout << "MX (Mail Exchange): Servidor de Correo asociado al dominio" << endl; break;
    case 16: cout << "TXT (Text): Cadenas de texto informativas" << endl; break;
    case 28: cout << "AAAA (Address IPv6): Direccion Host" << endl; break;
    default: cout << "NO SOPORTADA" << endl; break;
  }
  cout << "Clase:\t";
  switch(query->informacionRecurso.clase)
  {
    case 1: cout << "IN: Internet" << endl; break;
    case 2: cout << "CS: CSNET (obsoleto)" << endl; break;
    case 3: cout << "CH: CHAOS" << endl; break;
    case 4: cout << "HS: Hesiod" << endl; break;
    default: cout << "NO SOPORTADA" << endl; break;
  }
}

void interpretarRecord(struct dns_record *record, char *trama)
{
  int i;
  char aux[4];
  interpretarQuery(&record->detallesRecurso);
  cout << "TTL:\t" << dec << record->ttl << endl;
  cout << "Longitud de datos:\t" << record->lenDatos << endl;
  cout << "Datos:\t" << endl;
  switch(record->detallesRecurso.informacionRecurso.tipo)
  {
    case 1: mostrarA(record); break;
    case 2: mostrarNS(record,trama); break;
    case 5: mostrarCNAME(record,trama); break;
    case 6: mostrarSOA(record,trama); break;
    case 11: mostrarWKS(record); break;
    case 12: mostrarPTR(record,trama); break;
    case 13: mostrarHINFO(record); break;
    case 14: mostrarMINFO(record,trama); break;
    case 15: mostrarMX(record,trama); break;
    case 16: mostrarTXT(record); break;
    case 28: mostrarAAAA(record); break;
    default:
      for(i = 0; i < record->lenDatos; i++)
      {
        sprintf(aux,"%x \0",record->datos[i]);
        string str(aux);
        cout << str;
      }
      cout << endl;
    break;
  }
}

void mostrarA(struct dns_record *record)
{
  /*A (Address): Direccion IPv4*/
  int i;
  cout << "Direccion IPv4:\t";
  for(i = 0; i < record->lenDatos; i++) cout << dec << (int)((unsigned char)record->datos[i]) << ".";
  cout << endl;
}

void mostrarNS(struct dns_record *record, char *trama)
{
  /*NS (Name Server): Un servidor de nombres con autoridad*/
  bool fuePuntero;
  int posFin;
  string ns;
  if(determinarDatos(trama, record->offsetInicioDatos, ns, &posFin, &fuePuntero))  cout << "NS:\t" << interpretarNombre(ns) << endl;
  else cout << "ERROR: AL DETERMINAR NS" << endl;
}

void mostrarCNAME(struct dns_record *record, char *trama)
{
  /*CNAME (Cannonical Name): Un alias*/
  bool fuePuntero;
  string cname = "";
  int posFin;
  if(determinarDatos(trama, record->offsetInicioDatos, cname, &posFin, &fuePuntero))  cout << "CNAME:\t" << interpretarNombre(cname) << endl;
  else cout << "ERROR: AL DETERMINAR CNAME" << endl;
}

void mostrarSOA(struct dns_record *record, char *trama)
{
  /*SOA (Start Of Authority): Comienzo de una zona de autoridad*/
  bool fuePuntero;
  int i, posFin;
  uint32_t serial, refresh, retry, expire, minimum;
  string nameser;
  if(determinarDatos(trama, record->offsetInicioDatos, nameser, &posFin, &fuePuntero))
  {
    cout << "SOA NAME SERVER:\t" << interpretarNombre(nameser) << endl;
    i = sizeof(uint16_t);
  }
  else
  {
    for(i = 0; i < record->lenDatos && record->datos[i] != '\0'; i++) nameser += record->datos[i];
    if(i == record->lenDatos - 1)
    {
      cout << "ERROR: AL OBTENER SOA NAME SERVER:\t";
      return;
    }
    else  cout << interpretarNombre(nameser) << endl;
  }
  cout << "SOA EMAIL:\t";
  for(i += 1; i < record->lenDatos && record->datos[i] != '\0'; i++) cout << record->datos[i];
  cout << endl;
  i++;
  memcpy(&serial, record->datos+i, sizeof(uint32_t));
  i += sizeof(uint32_t);
  cout << "SOA SERIAL:\t" << serial << endl;
  memcpy(&refresh, record->datos+i, sizeof(uint32_t));
  i += sizeof(uint32_t);
  cout << "SOA REFRESH:\t" << refresh << endl;
  memcpy(&retry, record->datos+i, sizeof(uint32_t));
  i += sizeof(uint32_t);
  cout << "SOA RETRY:\t" << retry << endl;
  memcpy(&expire, record->datos+i, sizeof(uint32_t));
  i += sizeof(uint32_t);
  cout << "SOA EXPIRE:\t" << expire << endl;
  memcpy(&minimum, record->datos+i, sizeof(uint32_t));
  i += sizeof(uint32_t);
  cout << "SOA MINIMUM:\t" << minimum << endl;
}

void mostrarWKS(struct dns_record *record)
{
  /*WKS (Well Known Service): Descripcion de un servicio estandar*/
  int i;
  char aux[4];
  cout << "WKS Direccion IPv4:\t";
  for(i = 0; i < 4; i++) cout << dec << (int)((unsigned char)record->datos[i]) << ".";
  cout << endl;
  i++;
  cout << "WKS Protocolo:\t" << (uint32_t)record->datos[i] << endl;
  for(i += sizeof(uint32_t); i < record->lenDatos; i++)
  {
    sprintf(aux,"%x \0",record->datos[i]);
    string str(aux);
    cout << str;
  }
}

void mostrarPTR(struct dns_record *record, char *trama)
{
  /*PTR (Pointer): Puntero a un nombre de dominio*/
  bool fuePuntero;
  int posFin;
  string ptr;
  if(determinarDatos(trama, record->offsetInicioDatos, ptr, &posFin, &fuePuntero))  cout << "PTR:\t" << interpretarNombre(ptr) << endl;
  else cout << "ERROR: AL DETERMINAR PTR" << endl;
}

void mostrarHINFO(struct dns_record *record)
{
  /*HINFO (Host Information): Informacion sobre el host*/
  int i;
  string cpu = "", os = "";
  for(i = 0; record->datos[i] != '\0' && i < record->lenDatos; i++) cpu += record->datos[i];
  if(record->datos[i] == '\0' && i < record->lenDatos)
    for(i = i+1; i < record->lenDatos; i++) os += record->datos[i];
  cout << "HINFO CPU:\t" << cpu << endl;
  cout << "HINFO OS:\t" << os << endl;
}

void mostrarMINFO(struct dns_record *record, char *trama)
{
  /*MINFO (Mail Information): Informacion sobre un buzon de mail*/
  bool fuePuntero;
  int i, posFin;
  string rmailbx = "", emailbx = "";
  if(determinarDatos(trama, record->offsetInicioDatos, rmailbx, &posFin, &fuePuntero))
  {
    cout << "MINFO RMAILBX:\t" << rmailbx << endl;
    if(fuePuntero)  i = record->offsetInicioDatos + 2;
    else
    {
      for(i = 0; i < record->lenDatos; i++)
        if(record->datos[i] == '\0')  break;
      if(i == record->lenDatos - 1)
      {
        cout << "ERROR: AL DETERMINAR MINFO RMAILBX" << endl;
        return;
      }
      i++;
    }
    if(determinarDatos(trama, i, emailbx, &posFin, &fuePuntero)) cout << "MINFO EMAILBX:\t" << emailbx << endl;
    else
    {
      cout << "ERROR: AL DETERMINAR MINFO EMAILBX" << endl;
      return;
    }
  }
  else
  {
    cout << "ERROR: AL DETERMINAR MINFO RMAILBX" << endl;
    return;
  }
  /*
  for(i = 0; record->datos[i] != '\0' && i < record->lenDatos; i++) rmailbx += record->datos[i];
  if(record->datos[i] == '\0' && i < record->lenDatos)
    for(i = i+1; i < record->lenDatos; i++) emailbx += record->datos[i];
  cout << "MINFO RMAILBX:\t" << rmailbx << endl;
  cout << "MINFO EMAILBX:\t" << emailbx << endl;
  */
}

void mostrarMX(struct dns_record *record, char *trama)
{
  /*MX (Mail Exchange): Servidor de Correo asociado al dominio*/
  int i, posFin;
  uint16_t preference, puntero;
  string exchange = "";
  memcpy(&preference, record->datos, sizeof(uint16_t));
  cout << "MX PREFERENCE:\t" << preference << endl;
  if(record->lenDatos == 4)
  {
    memcpy(&puntero, record->datos+sizeof(uint16_t), sizeof(uint16_t));
    puntero = ntohs(puntero);
    if(esPuntero(puntero))
      if(determinarDatos(trama, obtenerDesplazamiento(puntero), exchange, &posFin, NULL))  cout << "MX EXCHANGE:\t" << interpretarNombre(exchange) << endl;
  }
  else
  {
    for(i = sizeof(uint16_t); i < record->lenDatos; i++) exchange += record->datos[i];
    cout << "MX EXCHANGE:\t" << exchange  << endl;
  }
}

void mostrarTXT(struct dns_record *record)
{
  /*TXT (Text): Cadenas de texto informativas*/
  cout << "TXT:\t" << string(record->datos) << endl;
}

void mostrarAAAA(struct dns_record *record)
{
    /*AAAA (Address): Direccion IPv6*/
    int i;
    uint8_t num1, num2;
    cout << "Direccion IPv6:\t";
    for(i = 0; i < record->lenDatos/2; )
    {
        memcpy(&num1, record->datos+i, sizeof(uint8_t));
        i += sizeof(uint8_t);
        memcpy(&num2, record->datos+i, sizeof(uint8_t));
        i += sizeof(uint8_t);
        printf("%.2x%.2x:",num1,num2);
    }
    cout << endl;
}

string interpretarNombre(string entrada)
{
  int i, l;
  char cadena[entrada.length()+1];
  memset(cadena, 0x00, (entrada.length()+1)*sizeof(char));
  memcpy(cadena, entrada.c_str(), entrada.length()*sizeof(char));
  i = 0;
  do
  {
    l = (int)((unsigned char)cadena[i]);
    cadena[i] = '.';
    i += l + 1;
  }while(i < entrada.length());
  string salida(cadena);
  salida = salida.substr(1);
  return salida;
}

void modoCliente(struct host *local, struct host *servidor, char *serv, char *recurso)
{
  struct dns_header cabecera;
  struct dns_request cuerpoRecurso;
  uint16_t i, r;
  char rec, *nombreRecurso;
  int l, res;
  cout << "Ejecutando en modo Cliente." << endl;
  do
  {
    cout << "¿Desea buscar recursivamente? (s/n)" << endl;
    cin >> rec;
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
  if((nombreRecurso = (char *)calloc((strlen(recurso)+2),sizeof(char))) == NULL)
  {
    cerr << "calloc(): " << strerror(errno) << endl;
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
  servidor->dispositivoRed.sin_family = AF_INET;
  servidor->dispositivoRed.sin_addr.s_addr = inet_addr(serv);
  servidor->dispositivoRed.sin_port = htons(puertoEscucha);
  if(establecerPuntoComunicacion(local,AF_INET,SOCK_DGRAM,0,0))
  {
    l = 0;
    memcpy(local->trama, &cabecera, sizeof(struct dns_header));
    l += sizeof(struct dns_header);
    memcpy(local->trama+l, nombreRecurso, (strlen(recurso)+2)*sizeof(char));
    l += (strlen(recurso)+2)*sizeof(char);
    memcpy(local->trama+l, &cuerpoRecurso, sizeof(struct dns_request));
    l += sizeof(struct dns_request);
    local->longitudTrama = l;
    if(enviarTrama(local,servidor,0))
      if(recibirTrama(local, servidor, 0, &res))
        interpretarTrama(servidor->trama, servidor->longitudTrama);
  }
}

bool establecerPuntoComunicacion(struct host *d, int dominio, int tipo, int protocolo, int puerto)
{
  bool flg = false;
  d->dispositivoRed.sin_family = dominio;
  d->dispositivoRed.sin_addr.s_addr = INADDR_ANY;
  d->dispositivoRed.sin_port = htons(puerto);
  d->descriptorSocket = socket(dominio, tipo, protocolo);
  if(d->descriptorSocket == -1) cerr << "socket(): " << strerror(errno) << endl;
  else
    if(bind(d->descriptorSocket, (struct sockaddr *)&d->dispositivoRed, sizeof(struct sockaddr_in)) < 0)  cerr << "bind(): " << strerror(errno) << endl;
    else  flg = true;
  return flg;
}

bool validarArgumentos(int argc, char **argv)
{
  bool flg = false;
  if(argc == 3)
    if(inet_addr(argv[1]) != INADDR_NONE)
      if(strlen(argv[2]) > 0) flg = true;
  return flg;
}
