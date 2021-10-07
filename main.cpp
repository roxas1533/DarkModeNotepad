#include <iostream>
#include <filesystem>
#include <Windows.h>


int main() {
	STARTUPINFOA  tStartupInfo = { 0 };
	PROCESS_INFORMATION tProcessInfomation = { 0 };
	bool isSuccess = CreateProcessA(0, (LPSTR)"\"C:\\Windows\\System32\\notepad.exe\""
		, NULL
		, NULL
		, FALSE
		, 0
		, NULL
		, "C:\\Windows"
		, &tStartupInfo
		, &tProcessInfomation
	);
	std::string path = std::filesystem::absolute("forcedark.dll").string();
	LPVOID remoteLibPath = VirtualAllocEx(tProcessInfomation.hProcess, NULL, path.length(), MEM_COMMIT, PAGE_READWRITE);
	if (remoteLibPath) {
		bool isWrite = WriteProcessMemory(tProcessInfomation.hProcess, remoteLibPath, path.c_str(), path.length(), NULL);
		HANDLE newHandle = CreateRemoteThread(tProcessInfomation.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, remoteLibPath, 0, NULL);
	}
	::CloseHandle(tProcessInfomation.hProcess);
	::CloseHandle(tProcessInfomation.hThread);
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	main();
	return 0;
}
