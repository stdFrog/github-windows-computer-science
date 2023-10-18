#include <stdio.h>
#include <conio.h>

int main()
{
	int i=0x12345678,j;
	char *p=(char *)&i;

	// 빅 엔디안
	for (j=0;j<sizeof(int);j++) {
		printf("%x ",p[j]);
	}

	printf("\n");

	// 리틀 엔디안
	for (j=sizeof(int)-1;j>=0;j--) {
		printf("%x ",p[j]);
	}
	putch('\n');

	int x = 1;
	char *y = (char*)&x;
	printf("%c\n",*y+48);
	// 1이면 LE, 0이면 BE

	int point = 0xFEFF0000;
	
	for(int i=0; i<sizeof(int); i++){
		printf("%02X ", *(((unsigned char*)&point)+i));
	}
}
