#define UNICODE
#include <windows.h>
#include <commctrl.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("SetOtherText");

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
	HWND hOther, hChild;
	LVITEM LI;
	TCHAR* Mes = TEXT("왼쪽: 리스트 박스에 문자열 추가, 오른쪽: 리스트 컨트롤에 항목 추가");

	switch(iMessage){
		case WM_LBUTTONDOWN:
			hOther = FindWindow(TEXT("My ListBox"), NULL);
			SendMessage(hOther, WM_SETTEXT, 0, (LPARAM)TEXT("남의 타이틀 바"));
			if(hOther){
				hChild = GetWindow(hOther, GW_CHILD);
				SendMessage(hChild, LB_ADDSTRING, 0, (LPARAM)TEXT("장난꾸러기"));
			}
			return 0;

		case WM_RBUTTONDOWN:
			hOther = FindWindow(TEXT("ListCtrl"), NULL);
			if(hOther){
				hChild = GetWindow(hOther, GW_CHILD);
				LI.mask = LVIF_TEXT;
				LI.iSubItem = 0;
				LI.iItem = 0;
				LI.pszText = TEXT("욕심꾸러기");
				SendMessage(hChild, LVM_INSERTITEM, 0, (LPARAM)&LI);
			}
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 10, 10, Mes,lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

