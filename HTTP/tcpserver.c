#include "common.h"

int main(int argc, char **argv){
	//variables to recieve data
	int 				listenfd, connfd,n;
	struct sockaddr_in 	servaddr;
	uint8_t 			buff[MAXLINE+1];
	uint8_t 			recvline[MAXLINE+1];

	/*trying to create a new socket*/
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		err_n_die("socket error ");

	/*Setting up the address*/
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family 		= AF_INET;
	servaddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	servaddr.sin_port 			= htons(SERVER_PORT);

	/*this part binds or listening socket to the address*/
	if((bind(listenfd, (SA *)&servaddr,sizeof(servaddr))) < 0)
		err_n_die("bind error.");

	if((listen(listenfd,10)) < 0)
		err_n_die("listen error.");

	/*Loop of connection*/

	for( ; ; ){
		struct sockaddr_in addr;
		socklen_t  addr_len;

		//Accp=ept blocks until an incoming connection arrives
		//it returns a "file descriptor" to the connection
		printf("waiting for a connection on port %d\n",SERVER_PORT);
		fflush(stdout);
		/*This is the socket that we use to talk to the client*/
		connfd = accept(listenfd,(SA *)NULL,NULL);

		//zero put the receive buffer to make sure it ends up null
		//terminated
		memset(recvline, 0, MAXLINE);
		//Now read the client's message
		while ((n = read(connfd, recvline, MAXLINE-1)) > 0){
			fprintf(stdout,"\n%s\n\n%s",bin2hex(recvline,n),recvline);

			//hacky way to detectt the end of the message
			if (recvline[n-1] == '\n'){
				break;
			}
			memset(recvline,0,MAXLINE);
		}
		if(n < 0)
			err_n_die("read error");

		//now send a response
		snprintf((char*)buff,sizeof(buff),"HTTP/1.0 200 OK\r\n\r\n Hello");
		//note: normally, you may want to check the result from write and close 
		//in case errror occur, For now, I'm ingring them. 
		write(connfd, (char*)buff,strlen((char*)buff));
		close(connfd); 
	}

}