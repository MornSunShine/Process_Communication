#include<windows.h>
#include<stdio.h>
#include <tchar.h>
#include"resource.h"
#define BUF_SIZE_MAX 1000
#define R_ACTION	 0
#define	F_MAPPING	 2
#define	T_MESSAGE	 1
#define	C_BOARD		 3
HINSTANCE proHandle;
HWND hwnd;
HWND dialog;
HWND nextHwnd;
LPVOID pBuf = NULL;
HANDLE hMapFile = NULL;
wchar_t data[BUF_SIZE_MAX];
wchar_t myName[] = L"Child Process";
wchar_t yuName[] = L"Parent Process";
/*Define The Function To Realize Converting Char To LPWSTR*/
LPWSTR ConvertCharToLPWSTR(char * szString)
{
	/*Set The Size Of szString*/
	int dwLen = strlen(szString) + 1;
	/*Calculate The Byte Size Of szString*/
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);
	/*Initializate The LPWSTR Object*/
	LPWSTR lpszPath = new WCHAR[dwLen];
	/*Fill The Content Of szString Into lpszPath*/
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return lpszPath;//Return The Result
}
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){//Unscramble The Message
		/*When Choosing Window Menu,Send The Message*/
	case WM_SYSCOMMAND:{
						   if (wParam == SC_CLOSE){
							   /*Close The Window*/
							   DestroyWindow(hdlg);
							   /*Send Quit Request To System*/
							   PostQuitMessage(0);
						   }
						   return 0;
	}
		/*When Clicking The Button,Send The Message*/
	case WM_COMMAND:
		switch (LOWORD(wParam)){//Unscramble The wParam	
			/*Button2 Action,Sharing Memory,File Mapping*/
		case IDC_BUTTON2:{
							 /*Get The Handle to Parent Process's Window*/
							 nextHwnd = FindWindow(yuName, yuName);
							 hMapFile = CreateFileMappingA(
								 INVALID_HANDLE_VALUE,// use paging file
								 NULL,// default security
								 PAGE_READWRITE | SEC_COMMIT,// read/write access
								 0,// maximum object size (high-order DWORD)
								 BUF_SIZE_MAX,// maximum object size (low-order DWORD)
								 "MyFileMappingObject");// name of mapping object
							 if (!hMapFile){
								 MessageBox(hwnd, TEXT("Could not create file mapping object:") + GetLastError(), L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
								 return -1;
							 }
							 pBuf = MapViewOfFile(hMapFile,// handle to map object
								 FILE_MAP_READ | FILE_MAP_WRITE,// read/write permission
								 0,
								 0,
								 BUF_SIZE_MAX);
							 if (!pBuf){
								 MessageBox(hwnd, TEXT("Could not map view of file:") + GetLastError(), L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
								 /*Close The Handl To Created Mapping Space*/
								 CloseHandle(hMapFile);
								 return -1;
							 }
							 ZeroMemory(pBuf, sizeof(pBuf));//Clear The Memory
							 /*Make Data From Window's Edit Control Into Mapping View*/
							 SendMessage(GetDlgItem(dialog, IDC_EDIT1), WM_GETTEXT, BUF_SIZE_MAX, (LPARAM)pBuf);
							 /*Send Data To Child Process To Show Mapping Successfully*/
							 SendMessage(nextHwnd, WM_SETTEXT, F_MAPPING, NULL);
							 UnmapViewOfFile(pBuf);
							 CloseHandle(nextHwnd);
							 break;
		}
			/*Button3 Action,Transfer Message*/
		case IDC_BUTTON3:{
							 /*Get The Handle to Parent Process's Window*/
							 nextHwnd = FindWindow(yuName, yuName);
							 /*Make Data Into Array*/
							 SendMessage(GetDlgItem(dialog, IDC_EDIT2), WM_GETTEXT, BUF_SIZE_MAX, (LPARAM)data);
							 /*Send Array(data) To Child Process*/
							 SendMessage(nextHwnd, WM_SETTEXT, T_MESSAGE, (LPARAM)data);
							 CloseHandle(nextHwnd);
							 break;
		}
			/*Button4 Action,Clipboard*/
		case IDC_BUTTON4:{
							 /*Judge Whether Succeed To Open And Empty The ClipBoard*/
							 if (!OpenClipboard(NULL) && EmptyClipboard()){
								 MessageBox(hwnd, L"ChilpBoard Opreation Failed!", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
								 return -1;
							 }
							 /*Make Data Into Array*/
							 SendMessage(GetDlgItem(dialog, IDC_EDIT3), WM_GETTEXT, BUF_SIZE_MAX, (LPARAM)data);
							 /*Achieve The Length Of Data*/
							 size_t number = (wcslen(data) + 1) * sizeof(wchar_t);
							 /*Alloc Global Space*/
							 HGLOBAL hData = GlobalAlloc(GHND, number);
							 if (!hData){
								 MessageBox(hwnd, L"Failed To Alloc!", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
								 CloseClipboard();
								 return -1;
							 }
							 /*Lock The Space*/
							 LPTSTR lpDest = (LPTSTR)GlobalLock(hData);
							 /*Set Data Into Space*/
							 memcpy(lpDest, data, number);
							 /*Unlock The Space*/
							 GlobalUnlock(hData);
							 /*Set Data Into ClipBoard*/
							 if (!(SetClipboardData(CF_TEXT, hData))){
								 MessageBox(hwnd, L"ChilpBoard Setting Failed", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
								 return -1;
							 }
							 /*Close The ClipBoard*/
							 CloseClipboard();
							 /*Release The Space*/
							 GlobalFree(hData);
							 /*Get The Handle to Parent Process's Window*/
							 nextHwnd = FindWindow(yuName, yuName);
							 /*Send Array(data) To Child Process*/
							 SendMessage(nextHwnd, WM_SETTEXT, C_BOARD, NULL);
							 CloseHandle(nextHwnd);
							 break;
		}
		default:
			break;
		}
		return 0;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	switch (uMsg){
	case WM_DESTROY:{//Close The Window
						PostQuitMessage(0);//Send Quit Request To System
						return 0;
	}
	case WM_CREATE:{
					   HWND hdlg = CreateDialog(proHandle, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc);//Create Dialog
					   if (hdlg){
						   dialog = hdlg;//Save The Handle To Window
						   ShowWindow(hdlg, SW_NORMAL);//Show The Window
					   }
					   CloseHandle(hdlg);
					   return 0;
	}
	case WM_SETTEXT:{
						switch (wParam){
							/*Reaction The Successful Operation*/
						case R_ACTION:{
										  SendMessage(GetDlgItem(dialog, IDC_LIST1), LB_ADDSTRING, NULL, lParam);
										  return 0;
						}
							/*Recation Of File Mapping*/
						case F_MAPPING:{
										   /*Open The File Mapping*/
										   hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, "MyFileMappingObject");
										   if (!hMapFile){
											   MessageBox(hwnd, TEXT("Could not open mapping of file:") + GetLastError(), L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
											   return -1;
										   }
										   /*Map The View Of File*/
										   pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
										   if (!pBuf){
											   MessageBox(hwnd, TEXT("Could not map view of file:") + GetLastError(), L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
											   CloseHandle(hMapFile);
											   return -1;
										   }
										   /*Get Data From File Mapping Memory*/
										   SendMessage(GetDlgItem(dialog, IDC_LIST2), LB_ADDSTRING, NULL, (LPARAM)pBuf);
										   /*Get The Handle to Parent Process's Window*/
										   nextHwnd = FindWindow(yuName, yuName);
										   /*Successful And Recation*/
										   SendMessage(nextHwnd, WM_SETTEXT, R_ACTION, LPARAM(L"Succeed To Map File!"));
										   /*Release The View*/
										   UnmapViewOfFile(pBuf);
										   /*Close The Mapping*/
										   CloseHandle(hMapFile);
										   CloseHandle(nextHwnd);
										   return 0;
						}
							/*Reaction To Transfer Message*/
						case T_MESSAGE:{
										   SendMessage(GetDlgItem(dialog, IDC_LIST2), LB_ADDSTRING, NULL, lParam);
										   /*Get The Handle to Parent Process's Window*/
										   nextHwnd = FindWindow(yuName, yuName);
										   /*Successful And Recation*/
										   SendMessage(nextHwnd, WM_SETTEXT, R_ACTION, (LPARAM)(L"Succeed To Transfer Message!"));
										   CloseHandle(nextHwnd);
										   return 0;
						}
							/*Reaction To Clipboard Operation*/
						case C_BOARD:{
										 /*Open The ClipBoard*/
										 if (!OpenClipboard(NULL)){
											 MessageBox(hwnd, L"Could not open clipboard", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
											 return -1;
										 }
										 /*Get Data From ClipBoard*/
										 HGLOBAL hMem = GetClipboardData(CF_TEXT);
										 if (!hMem){
											 MessageBox(hwnd, L"Failed to Get Data", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
											 return -1;
										 }
										 /*Lock The Space*/
										 LPCTSTR lpDest = (LPCTSTR)GlobalLock(hMem);
										 SendMessage(GetDlgItem(dialog, IDC_LIST2), LB_ADDSTRING, NULL, (LPARAM)lpDest);
										 GlobalUnlock(hMem);
										 /*Get The Handle to Parent Process's Window*/
										 nextHwnd = FindWindow(yuName, yuName);
										 /*Successful And Recation*/
										 SendMessage(nextHwnd, WM_SETTEXT, R_ACTION, (LPARAM)(L"Succeed To Get Data From ClipBoard!"));
										 CloseHandle(nextHwnd);
										 CloseClipboard();
										 return 0;
						}
						}
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow)
{
	WNDCLASS wndclass = { CS_HREDRAW | CS_VREDRAW,
		WindowProc,
		NULL,
		NULL,
		hInstance,
		NULL,
		NULL,
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		myName
	};
	RegisterClass(&wndclass);//Register The Class
	/*Save The Handle To Current Process*/
	proHandle = hInstance;
	/*Create The Window*/
	hwnd = CreateWindow(
		myName,           //Class Name,The Same As Previously Registered Class
		myName,          //The Title Of Window
		WS_OVERLAPPEDWINDOW,        //Style of Window's Appearance  
		0,             //X coordinates Of Window Relative to the Parent's
		0,             //Y coordinates Of Window Relative to the Parent's
		0,                //Width Of Window  
		0,                //Height Of Window  
		NULL,               //Without Parent Window
		NULL,               //Without Menu 
		proHandle,          //The Handle To Current Process
		NULL);              //Without Additional data
	if (!hwnd){                //Check Whether Creating Successfully
		MessageBox(hwnd, L"Fail to Create The Window!", L"WARMING", MB_OKCANCEL | MB_ICONEXCLAMATION);
		return -1;
	}
	ShowWindow(hwnd, SW_HIDE);//Show The Window
	UpdateWindow(hwnd);/*Update The Handle To Current Window*/
	MSG msg;
	/*Get All Message Under Current Process*/
	while (GetMessage(&msg, NULL, 0, 0))
	{
		/*Translate Virtual Message Into Character Message*/
		TranslateMessage(&msg);
		/*Unscramble The Message*/
		DispatchMessage(&msg);
	}
	CloseHandle(hwnd);
	return 0;
}