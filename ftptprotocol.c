/**
Con fopen copiar un archivo, de la entrada a la salida
ese archivo  guardarlo en un arreglo de [512] y 
despues enviarla por un socket
*/

#include <stdio.h>

FILE *fileCopy(FILE *f,char *filename);
int main(int argc, char const *argv[])
{
	FILE *file,*file2;
	char filename[100],c;
	printf("Enter the file name to copy: \n");
	scanf("%s",filename);
	file = fopen(filename,"r");	
	if(file != NULL){
		file2 = fileCopy(file,filename);		
	}else
		printf("Error al abrir el archivo");	
	fclose(file);
	fclose(file2);
	return 0;
}
FILE *fileCopy(FILE *f,char *filename){
	FILE *fc;
	char c;
	printf("\tFunction cpFile\n");
	printf("Enter a name for the copy: \n");
	scanf("%s",filename);
	fc = fopen(filename,"w");
	if(fc != NULL){
		while((c = fgetc(f))!= EOF){
			fputc(c,fc);		
		}
		printf("File copied successfully.\n");
	}

	return fc;
}
