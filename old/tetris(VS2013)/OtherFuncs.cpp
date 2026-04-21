#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include "ColorAndKey.h"
#include "OtherFuncs.h"
#include "Blocks.h"

extern int bg_status[11][21] = { BLANK };
HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
COORD position;
unsigned int score;

LPCWSTR background = 

L"■■■■■■■■■■■■■                "
L"■                      ■    ＮＥＸＴ    "
L"■                      ■  ┏━━━━┓  "
L"■                      ■  ┃　　　　┃  "
L"■                      ■  ┃　　　　┃  "
L"■                      ■  ┗━━━━┛  "
L"■                      ■                "
L"■                      ■   ＳＣＯＲＥ   "
L"■                      ■    00000000    "
L"■                      ■                "
L"■                      ■    操作方法    "
L"■                      ■                "
L"■                      ■       ↑       "
L"■                      ■    ← ↓ →    "
L"■                      ■                "
L"■                      ■   Enter:开始   "
L"■                      ■                "
L"■                      ■   空格：暂停   "
L"■                      ■                "
L"■                      ■  任意键：继续  "
L"■                      ■                "
L"■                      ■   by  易水寒   "
L"■■■■■■■■■■■■■                ";

//此处定义Blocks类的虚析构函数纯粹是为了能够析构其它子类对象，
//将它写在这里的确很异样，但实属无奈之举呐。。 -_-||
Blocks::~Blocks()
{
}

void HideCursor()
{
	CONSOLE_CURSOR_INFO cur_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cur_info);
}

void SetWindowRect()
{
	SMALL_RECT wrt = { 0, 0, 41, 22 };
	SetConsoleWindowInfo(hStdOutput, TRUE, &wrt);
}

void SetBufSize()
{
	COORD coord = { 42, 23 };
	SetConsoleScreenBufferSize(hStdOutput, coord);
}

void Interface()
{
	COORD coord = { 0, 0 };
	WriteConsoleOutputCharacter(hStdOutput, background, wcslen(background), coord, NULL);
}

void GotoXY(const COORD &obj)
{
	position.X = 2*obj.X;
	position.Y = obj.Y;

	SetConsoleCursorPosition(hStdOutput, position);   //将光标定位到指定坐标
}

int MakeColor()
{
	int rad;
	srand((unsigned int)time(NULL));
	rad = rand() % 6;

	switch (rad)
	{
	case 0:return BLUE;

	case 1:return GREEN;

	case 2:return LGREEN;

	case 3:return RED;

	case 4:return PURPLE;

	case 5:return YELLOW;
	}
}

void SetBlockColor(int color)
{
	SetConsoleTextAttribute(hStdOutput, color);
}

int GetKey()
{
	char ch1;
	ch1 = getch();

	if (ch1 == ENTER || ch1 == SPACE)
		return ch1;
	else if (ch1 == -32)    //方向键每敲击一次，可以看成输入两个字符，第一个字符的键值是-32
		return getch();
	else
		return 0;
}

Blocks *MakeBlocks()
{
	int rad;
	Blocks *pB = NULL;
	srand((unsigned int)time(NULL));

	rad = rand() % 7;

	switch (rad)
	{
	case 0:pB = new Square;
		break;

	case 1:pB = new Bar;
		break;

	case 2:pB = new Rcrutch;
		break;

	case 3:pB = new Lcrutch;
		break;

	case 4:pB = new L;
		break;

	case 5:pB = new RevL;
		break;

	case 6:pB = new T;
		break;
	}

	pB->draw();
	return pB;
}

void BlocksControl(Blocks *pB)
{
	pB->draw();

	while (pB->down()){
		for (int i = 0; i < 10; i++){
			if (_kbhit())
			{
				int key = GetKey();

				switch (key)
				{
				case LEFT:pB->left_move(); break;

				case RIGHT:pB->right_move(); break;

				case UP:pB->revolve(); break;

				case DOWN:pB->down(); break;

				case SPACE:MessageBox(NULL, TEXT("请确认继续"), TEXT("游戏暂停"), MB_ICONWARNING); break;
				}
			}
			Sleep(55);
		}
	}
	delete pB;

	if (!IsEmpty(0)){
		MessageBox(NULL, TEXT("游戏结束！"), TEXT("温馨提示"), MB_ICONWARNING);
		exit(0);
	}
}

Blocks *MakeNext()
{
	//用于擦除NEXT框里上次的图案
	COORD tmp1 = { 15, 3 };
	COORD tmp2 = { 15, 4 };
	GotoXY(tmp1);
	printf("        ");
	GotoXY(tmp2);
	printf("        ");
	SetBlockColor(0x3);

	int rad;
	Blocks *pBN = NULL;
	srand((unsigned int)time(NULL));

	rad = rand() % 7;

	switch (rad)
	{
	case 0:pBN = new Square;
		GotoXY(tmp1); printf("  ■■");
		GotoXY(tmp2); printf("  ■■");
		break;

	case 1:pBN = new Bar;
		GotoXY(tmp1); printf("■■■■");
		break;

	case 2:pBN = new Rcrutch;
		GotoXY(tmp1); printf("    ■■");
		GotoXY(tmp2); printf("  ■■");
		break;

	case 3:pBN = new Lcrutch;
		GotoXY(tmp1); printf("  ■■");
		GotoXY(tmp2); printf("    ■■");
		break;

	case 4:pBN = new L;
		GotoXY(tmp1); printf("      ■");
		GotoXY(tmp2); printf("  ■■■");
		break;

	case 5:pBN = new RevL;
		GotoXY(tmp1); printf("  ■■■");
		GotoXY(tmp2); printf("      ■");
		break;

	case 6:pBN = new T;
		GotoXY(tmp1); printf("   ■");
		GotoXY(tmp2); printf(" ■■■");
		break;
	}
	
	return pBN;
}

bool IsFilled(int line)
{
	for (int i = 0; i < 11; i++){
		if (!bg_status[i][line]){
			return false;
		}
	}
	return true;
}

bool IsEmpty(int line)
{
	for (int i = 0; i < 11; i++){
		if (bg_status[i][line]){
			return false;
		}
	}
	return true;
}

void Remove(int line)
{
	if (!IsEmpty(line)){
		for (int i = 0; i < 11; i++){
			bg_status[i][line] = bg_status[i][line - 1];
			COORD tmp = { i + 1, line + 1 };
			GotoXY(tmp);

			if (bg_status[i][line]){
				SetBlockColor(bg_status[i][line]);
				printf("■");
			}
			else{
				printf("  ");
			}
		}
		Remove(line - 1);
	}
	return;
}

void flush()
{
	int count = 0;
	COORD PrtScore = { 15, 8 };

	for (int line = 20; line > 0; line--){
		if (IsFilled(line)){
			Remove(line);
			count++;
			line = 21;
		}
	}

	switch (count)
	{
	case 1:score += 10; break;

	case 2:score += 30; break;

	case 3:score += 60; break;

	case 4:score += 100; break;
	}

	GotoXY(PrtScore);
	SetBlockColor(RED);
	printf("%8d", score);
}