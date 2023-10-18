#include <stdio.h>
#include <windows.h>

int main()
{
	const char* FilePath = "test.txt";
	
	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		DWORD dwSize = GetFileSize(hFile, NULL);
		DWORD dwRead;
		unsigned short header;
		
		if(!(ReadFile(hFile, &header, sizeof(header), &dwRead, NULL))){
			printf("Read File failed");
		}else{
			char *p = (char*)&header;
			for(int i=0; i<sizeof(short); i++){
				printf("%x ", p[i]);
			}

			if(header == 0xFEFF){printf("header = 0xFEFF\n");}
			else if(header == 0xFFFE){printf("header = 0xFFFE\n");}
		}
	}else{
		printf("Failed");
	}

	CloseHandle(hFile);

	return 0;
}
