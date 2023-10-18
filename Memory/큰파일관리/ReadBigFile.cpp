#define UNICODE
#include <windows.h>
#define min(a,b) (((a) > (b)) ? (b) : (a))
#define max(a,b) (((a) > (b)) ? (a) : (b))

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Read Big File");

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
HANDLE hFile, hMapp;
TCHAR* base;
int line;

#define LINE 600000
#define PAGE 50

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR buf[256];
	DWORD dwWritten;
	int Length;
	int i;

	switch(iMessage){
		case WM_CREATE:
			hWndMain = hWnd;
			hFile = CreateFile(
					TEXT("BigFile.dat"),
					GENERIC_WRITE,
					0, NULL,
					CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);

			if(hFile != INVALID_HANDLE_VALUE){
				for(i=0; i<LINE; i++){
					wsprintf(
							buf,
							TEXT("Read Very BigFile Using Memory Mapped File")
							TEXT(" - %06d Lines\r\n"),
							i
							);
					Length = lstrlen(buf);
					unsigned short header=0xFEFF;
					WriteFile(hFile, &header, sizeof(header), &dwWritten, NULL);
					WriteFile(hFile, buf, lstrlen(buf) * sizeof(wchar_t), &dwWritten, NULL);
				}
				MessageBox(hWnd, TEXT("BigFile.dat를 생성했습니다."), TEXT("알림"), MB_OK);
				CloseHandle(hFile);
			}

			// Read
			hFile = CreateFile(
					TEXT("BigFile.dat"),
					GENERIC_READ,
					0, NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);

			hMapp = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,0, NULL);
			base = (TCHAR*)MapViewOfFile(hMapp, FILE_MAP_READ, 0,0,0);
			return 0;

		case WM_KEYDOWN:
			switch(wParam){
				case VK_DOWN:
					line = min(line+1, LINE-1);
					break;

				case VK_UP:
					line = max(line-1, 0);
					break;

				case VK_NEXT:
					line = min(line+PAGE, LINE-1);
					break;

				case VK_PRIOR:
					line = max(line-PAGE, 0);
					break;
			}
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;


		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			for(i=0; i<PAGE; i++){
				if(line+i < LINE){
					// \r\n까지 세었을 때 길이가 59이고
					// ANSI가 아닌 UTF-16으로 변환하기 때문에 + 1 추가
					// 총 60 글자 중 57개까지만 출력 (\r\n\0) - 널 종료 문자가 아니라 BOM일 수도 있음
					// 위에 보니까 모든 글자 선두에 BOM 박아뒀음
					TextOut(hdc, 10, i*16, base + (line+i)*60, 57);
					// 16px가 기본 Font의 Height값이다
				}
			}
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			UnmapViewOfFile(base);
			CloseHandle(hMapp);
			CloseHandle(hFile);
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
