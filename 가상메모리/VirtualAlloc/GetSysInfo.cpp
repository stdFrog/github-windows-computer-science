#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("GetSysInfo PageSize");

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
HWND hWndMain;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	SYSTEM_INFO si;
	TCHAR str[128];

	switch(iMessage){
		case WM_CREATE:
			hWndMain = hWnd;
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			GetSystemInfo(&si);
			wsprintf(str, TEXT("PageSize: %lu"), si.dwPageSize);
			TextOut(hdc, 10,10, str,lstrlen(str));
			wsprintf(str, TEXT("Min Addr : %lu"), si.lpMinimumApplicationAddress);
			TextOut(hdc, 10,30, str,lstrlen(str));
			wsprintf(str, TEXT("Max Addr : %lu"), si.lpMaximumApplicationAddress);
			TextOut(hdc, 10,50, str,lstrlen(str));
			wsprintf(str, TEXT("Alloc Gra : %lu"), si.dwAllocationGranularity);
			TextOut(hdc, 10,70, str,lstrlen(str));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
