#include "dns.h"

int main(int argc, char **argv)
{
  bool inv;
  if(!validarArgumentos(argc, argv,&inv))
  {
    printf("%s -s | -c <servidor dns> <dominio> \n",argv[0]);
    exit(1);
  }
  int dominio = AF_INET, tipo = SOCK_DGRAM, protocolo = 0, puerto;
  struct host local, remoto;
  memset(&local, 0x00, sizeof(struct host));
  memset(&remoto, 0x00, sizeof(struct host));
  if(strcmp(argv[1], "-s") == 0)  modoServidor(&local);
  else  modoCliente(&local,&remoto,inv,argv[2],argv[3]);
  return 0;
}
