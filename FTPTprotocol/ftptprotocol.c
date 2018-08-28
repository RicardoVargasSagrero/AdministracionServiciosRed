/**
Con fopen copiar un archivo, de la entrada a la salida
ese archivo  guardarlo en un arreglo de [512] y 
despues enviarla por un socket
*/

#include <stdio.h>

FILE *fileCopy(FILE *f);
int main(int argc, char const *argv[])
{
	FILE *file,*file2;
	char filename[100],c;
	*file2 = *fileCopy(file);
	
	return 0;
}
FILE *fileCopy(FILE *f){
	printf("function cpFile\n");
	return f;
}