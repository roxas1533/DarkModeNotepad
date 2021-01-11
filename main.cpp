#include <iostream>
#include <Windows.h>
#include "forcedark\force.h"
#pragma comment(lib,"forcedark.lib")


int main() {
	std::cout << "このプログラム終了するときは何かキーを押す";
	Inject();
	getchar();
	UnInject();
	return 0;
}