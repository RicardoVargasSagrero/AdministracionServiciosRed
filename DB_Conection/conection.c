#include <stdio.h>
#include "mysql/mysql.h" //main header for SQLAPI
#include <stdlib.h>
#include <string.h>

static char *host = "localhost";
static char *user = "root";
static char *pw = "Speck3220#";
static char *proxyDB = "proxy";
unsigned int port = 3306;
static char *unix_socket = NULL;
unsigned int flag = 0;

int main(int argc, char const *argv[])
{
	MYSQL *conn; /*Connection varible*/
	MYSQL_RES *res;  /*Result set variable*/
	MYSQL_ROW row; /*This variable will content all the sets of every register*/
	conn = mysql_init(NULL); 
	char statment[18] = "www.steren.com.mx";
	char query[36] = "select url from pages where url = '";
	char finish [3] = "';";
	char concat[60];

	strcat(concat,query);
	strcat(concat,statment);
	strcat(concat,finish);
	//choice = "SELECT id_Cliente,cliente,domicilio,num_casa,fecha_evento FROM cliente WHERE cliente LIKE '%"+busqueda+"%';";
	printf("Result = %s\n\n\n",concat );

	if(!(mysql_real_connect(conn,host,user,pw,proxyDB,port,unix_socket,flag))){
		fprintf(stderr,"\nError: %s [%d]\n",mysql_error(conn),mysql_errno(conn));
	}else{
		perror("Connection successfull\n");	
		/*We sent the Query consult*/
		if(mysql_query(conn,concat)){
			fprintf(stderr, "%s\n",mysql_error(conn));
			exit(1);
		}
		res = mysql_use_result(conn);
		printf("ID\tURL Name \tURL\tIP\tname\n");
		if((row = mysql_fetch_row(res))== NULL)
			printf("Sin resultados\n");
		else{
			printf("Resultados\n");
		}
		while((row = mysql_fetch_row(res))!= NULL)
			printf("%s\t%s\t%s\t%s\n",row[0],row[1],row[2],row[3]);
		/*We free the varibale res and close the connection*/
		mysql_free_result(res);
		mysql_close(conn);
	}
	return 0;
}