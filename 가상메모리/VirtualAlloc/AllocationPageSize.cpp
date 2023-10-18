#include <windows.h>
#include <stdio.h>

int main()
{
	int *ptr = (int*)VirtualAlloc(NULL, sizeof(int) * 10, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	printf("Virtual Alloc sizeof(int) * 10 Reserved\nReal Alloc Size = %d\n", sizeof(ptr)/sizeof(int));

	VirtualFree(ptr, sizeof(int) * 10, MEM_DECOMMIT);
	VirtualFree(ptr, 0, MEM_RELEASE);
	return 0;
}
