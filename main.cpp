#include <iostream>
#include <Windows.h>
#include "forcedark\force.h"
#pragma comment(lib,"forcedark.lib")


int main() {
	std::cout << "���̃v���O�����I������Ƃ��͉����L�[������";
	Inject();
	getchar();
	UnInject();
	return 0;
}