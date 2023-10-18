#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("VMemB");

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR Mes[256];
	static TCHAR *str;
	HWND hB;

	/*
		페이지 테이블에 의해 관리되는 가상 메모리 공간에 대하여 알아보기 위한 실험이다.

		응용 프로그램의 주소와 가상 메모리의 주소가 각각 분리된 공간에 존재한다는 것을 알아보기 위해
		메세지를 이용한 통신 프로그램을 작성했다.

		응용 프로그램이 가지는 주소는 우연하게도 동일할 수 있으나, 이를 관리하는 페이지 테이블에 의해
		각기 다른 프로세스 즉, 가상메모리 공간을 사용한다는 것을 간단하게 입증해보자. 
	 */

	switch(iMessage){
		case WM_USER+1:
			hdc = GetDC(hWnd);
			str = (TCHAR*)lParam;
			wsprintf(Mes, TEXT("%p 번지의 문자열을 받았습니다."), str);
			MessageBox(hWnd, Mes, TEXT("알림"), MB_OK);
			TextOut(hdc, 10, 30, str, lstrlen(str));
			ReleaseDC(hWnd, hdc);
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			wsprintf(Mes, TEXT("VMemA가 보내주는 문자열을 출력합니다."));
			TextOut(hdc, 10,10, Mes,lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
