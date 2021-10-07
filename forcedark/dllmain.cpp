#include "pch.h"
#include <thread>


LONG_PTR OriginalProc;
LRESULT CALLBACK StaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CTLCOLOREDIT:
		SetTextColor((HDC)wp, COLORREF(RGB(241, 241, 241)));
		SetBkColor((HDC)wp, COLORREF(RGB(37, 37, 38)));
		return (LRESULT)(HBRUSH)CreateSolidBrush(COLORREF(RGB(37, 37, 38)));
	default:
		break;
	}
	return CallWindowProc((WNDPROC)OriginalProc, hwnd, msg, wp, lp);
}

struct Handle_data {
	unsigned long process_id;
	HWND window_handle;
};
BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	Handle_data& data = *(Handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id )
		return TRUE;
	char* name = new char[20];
	GetClassNameA(handle, name, 20);
	if (std::string(name) != "Notepad")
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

void setColor();
void NotepadHook() {

	std::thread th(setColor);
	th.detach();
}

void setColor() {
	Handle_data data{ GetCurrentProcessId(),0 };
	while (true)
	{
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		if (data.window_handle != 0)break;
	}

	if (data.window_handle) {
		OriginalProc = GetWindowLongPtr(data.window_handle, GWLP_WNDPROC);
		SetWindowLongPtr(data.window_handle, GWLP_WNDPROC, (__int3264)(LONG_PTR)StaticProc);
		//SendMessageA(data.window_handle, WM_CTLCOLOREDIT, 0, 0);
	}
	
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	std::thread th_a;
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