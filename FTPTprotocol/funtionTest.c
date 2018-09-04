#include <stdio.h>
#include <string.h>

int main(){
	unsigned char message[] = "Holacomoesta.txt";
	char filename[100];
	memcpy(filename, message+2,strlen(message));
	printf("%s\n%c %X\n",filename,message[0],message[1]);

	return 0;
}