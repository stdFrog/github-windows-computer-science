#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("VirtualProtect - AccViolation");

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

int i;
TCHAR* str=TEXT("string");
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR* Mes = TEXT("왼쪽 버튼: 정수값 변경, 오른쪽 버튼: 문자열 상수 변경");

	switch(iMessage){
		case WM_LBUTTONDOWN:
			i=1234;
			return 0;

		case WM_RBUTTONDOWN:
			str[0]='a';
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 10,10, Mes, lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
