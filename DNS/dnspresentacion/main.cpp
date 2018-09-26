#include "dns.h"

using namespace std;

int main(int argc, char **argv)
{
  if(!validarArgumentos(argc, argv))
  {
    cout << argv[0] << "<servidor dns> <dominio>" << endl;
    exit(1);
  }
  int dominio = AF_INET, tipo = SOCK_DGRAM, protocolo = 0, puerto;
  struct host local, remoto;
  memset(&local, 0x00, sizeof(struct host));
  memset(&remoto, 0x00, sizeof(struct host));
  modoCliente(&local,&remoto,argv[1],argv[2]);
  return 0;
}
