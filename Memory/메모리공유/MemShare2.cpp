#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MemShare2");

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

#define WM_SYNCSHAREMEMORY WM_USER+1
#define MAXSHAREMEMORY 1024
#define ID_EDIT 100
HWND hEdit;
HANDLE hMapp;
TCHAR* ptrFile;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hTarget;
	TCHAR* Mes = TEXT("메모리 맵 파일을 사용하여 두 프로세스가 메모리를 공유하는 예제");

	switch(iMessage){
		case WM_CREATE:
			hEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE, 10, 10, 500, 200, hWnd, (HMENU)ID_EDIT, g_hInst, NULL);
			SendMessage(hEdit, EM_LIMITTEXT, MAXSHAREMEMORY, 0);
			hMapp = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAXSHAREMEMORY, TEXT("MEMSHAREMAPPING"));
			ptrFile = (TCHAR*)MapViewOfFile(hMapp, FILE_MAP_ALL_ACCESS, 0,0, MAXSHAREMEMORY);	
			return 0;


		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case ID_EDIT:
				switch(HIWORD(wParam)){
					case EN_CHANGE:
					GetWindowText(hEdit, ptrFile, MAXSHAREMEMORY);
					hTarget = FindWindow(NULL, TEXT("MemShare1"));
					if(hTarget){
						SendMessage(hTarget, WM_SYNCSHAREMEMORY, 0,0);
					}
					break;
				}
			}
			return 0;

		case WM_SYNCSHAREMEMORY:
			SetWindowText(hEdit, ptrFile);
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 10, 220, Mes,lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;
		case WM_DESTROY:
			UnmapViewOfFile(ptrFile);
			CloseHandle(hMapp);
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
