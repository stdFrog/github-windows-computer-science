#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Mem File Mapping");

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
	HANDLE hFile, hFMap;
	HDC hdc;
	char* ptrInTemp;
	TCHAR* ptrInFile;
	TCHAR* writeTemp;
	TCHAR readTemp[256];
	RECT rt;

	switch(iMessage){
		case WM_LBUTTONDOWN:
			hdc = GetDC(hWnd);
			// 파일을 연다
			hFile = CreateFile(
					TEXT("NaRu.txt"),
					GENERIC_READ,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);

			if(hFile == INVALID_HANDLE_VALUE){
				MessageBox(
						hWnd,
						TEXT("파일이 없습니다."),
						TEXT("에러"),
						MB_OK
						);
			}else{
				// 파일 맵핑 오브젝트를 만든다.
				hFMap = CreateFileMapping(
						hFile,
						NULL,
						PAGE_READONLY,
						0,
						0,
						NULL
						);
				//주소 공간에 맵한다.
				ptrInFile = (TCHAR*)MapViewOfFile(
						hFMap,
						FILE_MAP_READ,
						0,
						0,
						0
						);

				// ReadFile이란 함수는 쓸수 없고, 애초에 UNICODE 자체를 지원안함
				// 변환 함수 사용해서 읽고 화면에 출력
				// 이것도 임시방편이라 제대로 알아봐야 됨
				ptrInTemp = (char*)ptrInFile;
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, ptrInTemp,-1, readTemp,256);
				
				SetRect(&rt, 10,10, 640,400);
				DrawText(hdc, readTemp, -1/*GetFileSize(hFile, NULL)*/, &rt, DT_EXPANDTABS);

				// 원래 GetFileSize 함수 사용해서 파일 사이즈만큼 출력하면 되지만 유니코드라
				// 길이 계산이 어려우므로 char형 ptrInTemp 함수에 *2만큼 출력해줌
				// 그렇게 안하면 이상한 상형문자가 뒤에 출력됨
				UnmapViewOfFile(ptrInFile);
				CloseHandle(hFMap);
				CloseHandle(hFile);
			}
			ReleaseDC(hWnd, hdc);
			return 0;

		case WM_RBUTTONDOWN:
			hFile = CreateFile(TEXT("Alpha.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE){
				MessageBox(hWnd, TEXT("파일이 없습니다."), TEXT("에러"), MB_OK);
			}else{
				// 파일 맵핑 오브젝트를 만든다.
				hFMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0,0, NULL);
				ptrInFile = (TCHAR*)MapViewOfFile(hFMap, FILE_MAP_WRITE, 0,0,0);

				*ptrInFile = (TCHAR)0xFEFF;
				writeTemp = ptrInFile + 1;

				// 이거 길이를 한글 영어 상관없이 딱 맞춰서 주면 됨
				wcsncpy(writeTemp, TEXT("왜 안될까요?"), 8);
				/*
				writeTemp[4]=' ';
				*/
				UnmapViewOfFile(ptrInFile);
				CloseHandle(hFMap);
				CloseHandle(hFile);
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
