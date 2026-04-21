#include "OtherFuncs.h"
#include "ColorAndKey.h"
#include "Blocks.h"
#include "ColorAndKey.h"
#include <Windows.h>
#include <cstdio>

int main()
{
	SetConsoleTitle(L"俄罗斯方块");
	HideCursor();
	SetWindowRect();
	SetBufSize();
	Interface();

	//按回车开始
	while (GetKey() != ENTER)
		;
	
	Blocks *pB = NULL;
	Blocks *pBN = NULL;
	int flag = 1;

	do
	{
		if (flag){
			pB = MakeBlocks();
			flag = 0;
		}

		Sleep(600);

		pBN = MakeNext();
		BlocksControl(pB);
		Sleep(100);
		flush();
		pB = pBN;
	} while (1);

	
	return 0;
}