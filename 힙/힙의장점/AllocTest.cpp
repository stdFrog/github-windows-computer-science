#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("AllocTest");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
	WNDCLASS wc = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,0,
		hInstance,
		NULL, LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		lpszClass
	};
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(
				lpszClass,
				lpszClass,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL,
				(HMENU)NULL,
				hInstance,
				NULL
			);
	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	while(GetMessage(&msg, nullptr, 0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

#define MAXALLOC 10000
void *arHeap[MAXALLOC];
void *arVirt[MAXALLOC];

void DeallocAll(){
	int i;

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	for(i=0; i<MAXALLOC; i++){
		if(arHeap[i]){
			HeapFree(GetProcessHeap(), 0, arHeap[i]);
			arHeap[i]=NULL;
		}
	}

	for(i=0; i<MAXALLOC; i++){
		if(arVirt[i]){
			VirtualFree(arVirt[i], sizeof(int), MEM_DECOMMIT);
			VirtualFree(arVirt[i], 0, MEM_RELEASE);
			arVirt[i] = NULL;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR* Mes = TEXT("왼쪽 버튼: 힙에서 할당, 오른쪽 버튼: 가상 메모리에서 할당");
	int i;

	switch(iMessage){
		case WM_LBUTTONDOWN:
			// 힙에서 할당 시: 40K만큼의 힙 소요
			DeallocAll();
			for(i=0; i<MAXALLOC; i++){
				arHeap[i] = HeapAlloc(GetProcessHeap(), 0, sizeof(int));
			}
			return 0;

		case WM_RBUTTONDOWN:
			// 가상 메모리에서 할당 시: 40M만크므이 가상 메모리 소요, 주소 공간은 660M 소요
			DeallocAll();
			for(i=0; i<MAXALLOC; i++){
				arVirt[i] = VirtualAlloc(NULL, sizeof(int), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			}
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 50,50, Mes,lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			DeallocAll();
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
