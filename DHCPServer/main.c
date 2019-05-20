#include "common.h"
/*The following program is an implamentation of a DHCP server (Dynamic Host Configuration Protocol)*/
int main(int argc, char const *argv[])
{
	int 		       listenfd,connfd,n;
	struct sockaddr_in servaddr;
	uint8_t 			buff[MAXLINE+1];
	uint8_t 			recvline[MAXLINE+1];

	/*Trying to create a new socket*/
	if((listenfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		err_n_die("socket error");

	/*Setting up the address*/
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family 		= AF_INET;
	servaddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	servaddr.sin_port 			= htons(SERVER_PORT);

	/*In this part, the funcion binds and listen socket to the address*/
	if((bind(listenfd), (SA *)&servaddr,sizeof(servaddr))<0)
		err_n_die("bind error");

	if((listen(listenfd,10)) < 0)
		err_n_die("listen error");

	/*loop of connection
	for( ; ; ){
		struct sockaddr_in addr;
		socklen_t addr_len;

	}*/
	printf("Exito en abrir el socket!\n");
	close(listenfd);
	return 0;
}