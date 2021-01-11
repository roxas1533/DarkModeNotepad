// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include<fstream>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <richedit.h>
//#pragma comment(lib, "RICHED32.DLL")

#include "rewrite.h"

//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")
LONG_PTR DefStaticProc[20] = {NULL};
HHOOK g_hHook = NULL;
HWND noteWnd[20] = { 0 };
bool readyUnhook = false;
fpCreateWindowEx createWindowExWPtr = NULL;
fpHWriteFile back = NULL;
#pragma data_seg()

int myNum = 0;
LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_DESTROY:
		DefStaticProc[myNum] = NULL;
		noteWnd[myNum] = NULL;
		PostQuitMessage(0);
		return 0;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wp, COLORREF(RGB(241, 241, 241)));
		SetBkColor((HDC)wp, COLORREF(RGB(37, 37, 38)));
		return (LRESULT)(HBRUSH)CreateSolidBrush(COLORREF(RGB(37, 37, 38)));
	default:
		break;
	}
	return CallWindowProc((WNDPROC)DefStaticProc[myNum], hwnd, msg, wp, lp);
}

HWND WINAPI MyCreateWindowEx(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCWSTR lpClassName,
	_In_opt_ LPCWSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam){
	if (std::wstring(lpClassName) == L"Edit") {
		LoadLibrary(L"RICHED20.DLL");
		lpClassName = L"RICHEDIT20A";
	};
	return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle,X,Y,nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

}

BOOL WINAPI HWriteFile(
	_In_ HANDLE hFile,
	_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite,
	_Out_opt_ LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
) {
	std::cout << std::string((char*)lpBuffer);
	return back(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}
LRESULT WINAPI HookProc(int code, WPARAM wParam, LPARAM lParam) {
	WCHAR fileNameBuf2[1024];

	//std::cout << ((CWPSTRUCT*)lParam)->message << "\n";
	if (((CWPRETSTRUCT*)lParam)->message == WM_CREATE) {
		if (GetClassName(((CWPRETSTRUCT*)lParam)->hwnd, fileNameBuf2, 1024) > 0)
			if (_tcsstr(fileNameBuf2, TEXT("Notepad")) != NULL) {
				for (int i = 0; i < 20; i++) {
					if (DefStaticProc[i] == NULL) {
						myNum = i;
						break;
					}
				}
				DefStaticProc[myNum] = GetWindowLongPtr(((CWPRETSTRUCT*)lParam)->hwnd, GWLP_WNDPROC);
				SetWindowLongPtr(((CWPRETSTRUCT*)lParam)->hwnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
				noteWnd[myNum] = ((CWPRETSTRUCT*)lParam)->hwnd;
			}
		//if (_tcsstr(fileNameBuf2, TEXT("RichEdit20A")) != NULL) {
		//	SendMessage(((CWPRETSTRUCT*)lParam)->hwnd, EM_SETBKGNDCOLOR, 0,(LPARAM)COLORREF(RGB(37, 37, 38)));
		//}
	}

	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

void NotepadHook() {
	WCHAR fileNameBuf[MAX_PATH];
	readyUnhook = false;
	if (GetModuleFileName(GetModuleHandle(NULL), fileNameBuf, MAX_PATH) > 0) {
		if (_tcsstr(fileNameBuf, TEXT("notepad.exe")) != NULL) {
			//AllocConsole();
			//FILE* fpOut = NULL;
			//::freopen_s(&fpOut, "CONOUT$", "w", stdout);
			//PrintFunctions();
			//createWindowExWPtr = (fpCreateWindowEx)RewriteFunction("USER32.dll", "CreateWindowExW", MyCreateWindowEx);
			//back=(fpHWriteFile)RewriteFunction("kernel32.dll", "WriteFile", HWriteFile);
		}
	}
}
void Inject() {
	g_hHook = SetWindowsHookEx(WH_CALLWNDPROCRET, HookProc, GetModuleHandle(__TEXT("forcedark")), 0);
}

LRESULT WINAPI UnHookProc(int code, WPARAM wParam, LPARAM lParam) {
	WCHAR fileNameBuf2[1024];
	if (GetModuleFileName(GetModuleHandle(NULL), fileNameBuf2, MAX_PATH) > 0) {
		if (_tcsstr(fileNameBuf2, TEXT("notepad.exe")) != NULL) {
			SetWindowLongPtrW(noteWnd[myNum], GWLP_WNDPROC, (__int3264)(LONG_PTR)DefStaticProc[myNum]);
			noteWnd[myNum] = 0;
			DefStaticProc[myNum] = 0;
		}
	}
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

void UnInject() {
	g_hHook = SetWindowsHookEx(WH_GETMESSAGE, UnHookProc, GetModuleHandle(__TEXT("forcedark")), 0);
	while (!readyUnhook) {
		for (int i = 0; i < 20; i++) {
			if (DefStaticProc[i] != NULL|| noteWnd[myNum] != 0) {
				PostMessage(noteWnd[i], NULL, 0, 0);
				break;
			}
			if (i == 19)
				readyUnhook = true;
		}
	}
	UnhookWindowsHookEx(g_hHook);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		NotepadHook();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		FreeConsole();
		break;
	}
	return TRUE;
}