#include <stdio.h>
#include <string.h>

int main(){
	unsigned char filename[] = "Hola.txt";
	unsigned char trama[100];
	FILE *file = NULL;
	file = fopen(filename,"rb");
	if(file == NULL){
		printf("El archivo no existe\n");
	}
	printf("Armemos la trama\n");
	int opcode = 0x02,i;
	int Error = 0x01;
	trama[0] = 0x00;
	trama[1] = opcode;
	memcpy(trama+2,&Error,2);
	printf("a\n");
	strcpy(trama+4,filename);
	printf("%s\n",trama+4);
	int lenght = strlen(filename)+4;
	printf("%d\n",lenght);
	for(i = 0; i < lenght; i++)
		printf("%c ",trama[i]);
	printf("\n");
	return 0;
}