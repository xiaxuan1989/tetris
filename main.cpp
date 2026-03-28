#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <stdio.h>

const int BACK = 34;
const int SPEED[] = { 0,12,9,6,3,1 };
const int NUMBER[] = { 0,2,3,5,7,11,15 };
const int COL[] = { 11,15,12,12,13,13,14 };

const int WIDTH = 15;
const int HEIGHT = 25;

int rank = 1;
int level = 0;	//0表示既不是向右也不是向左，1表示向左，2表示向右
int kind;
int pri_kind;	//7大类，颜色标记，调用COL[col_kind];
int revolve = 0;	//旋转状态标记

struct {
	int x;
	int y;
} current, before;

struct {
	int x;
	int y;
	int color;
	bool having;
} board[WIDTH][HEIGHT];

struct {
	int vary_x[4];
	int vary_y[4];
} vary[] = {
	{{0, 2, 4, 6},{0, 0, 0, 0}},{{0, 0, 0, 0},{0,-1,-2,-3}},
	{{0, 2, 2, 0},{0, 0,-1,-1}},{{0,-2,-2,-4},{0 ,0,-1,-1}},
	{{0, 0, 2, 2},{0,-1,-1,-2}},{{0, 2, 2, 4},{0, 0,-1,-1}},
	{{0, 0,-2,-2},{0,-1,-1,-2}},{{0, 0, 2, 4},{0,-1, 0, 0}},
	{{0, 0, 0, 2},{0,-1,-2,-2}},{{0, 0,-2,-4},{0,-1,-1,-1}},
	{{0, 2, 2, 2},{0, 0,-1,-2}},{{0, 2, 4, 4},{0, 0, 0,-1}},
	{{0,-2,-2,-2},{0, 0,-1,-2}},{{0, 0, 2, 4},{0,-1,-1,-1}},
	{{0, 0, 0,-2},{0,-1,-2,-2}},{{0 ,2, 4, 2},{0, 0, 0,-1}},
	{{0, 0, 2, 0},{0,-1,-1,-2}},{{0,-2, 0, 2},{0,-1,-1,-1}},
	{{0, 0,-2, 0},{0,-1,-1,-2}},
};

struct {
	int sum;
	int connection_x[5];
	int connection_y[5];
} connection[] = {
	{2,{-2,2},{0,1}},
	{1,{0},{0}},
	{2,{2,-2},{0,0}},
	{2,{-2,2},{0,0}},
	{4,{-2,0,4,-2},{0,0,-1,1}},
	{4,{-4,2,-2,4},{0,0,-1,1}},
	{4,{-2,2,0,0},{0,0,0,0}},
};

void gotoxy(int x, int y) {
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void color(int b) {
	HANDLE hConsole = GetStdHandle((STD_OUTPUT_HANDLE));
	SetConsoleTextAttribute(hConsole, b);
}

void PrintGarphy() {
	int i, j;
	color(288); gotoxy(2, 1);
	for (i = 0; i < WIDTH; ++i) {
		printf("□");
	}
	printf("□□□□□□□□□□□"); //上面的横边
	for (i = 0;i < HEIGHT + 1;i++) {
		gotoxy(2, 2 + i);
		puts("□"); //左边的竖边
		gotoxy(WIDTH + 19, 2 + i);
		if (i == 8 || i == 16)
			printf("□□□□□□□□□□");  //右半部的两条横边
		else
			puts("□"); //中间的竖边
		gotoxy(WIDTH + 37, 2 + i);
		puts("□"); //右边的竖边
	}
	gotoxy(2, 3 + HEIGHT);
	for (i = 0; i < WIDTH; ++i) {
		printf("□");
	}
	printf("□□□□□□□□□□□"); //下面的横边

	int t;
	FILE* fp = fopen("score.txt", "r");
	fscanf(fp, "%d", &t);
	fclose(fp);
	color(43);
	gotoxy(22 + WIDTH, 12);
	printf("BEST      %d", t);
	color(46);
	gotoxy(22 + WIDTH, 14);
	puts("SCORE");
	color(44);
	gotoxy(22 + WIDTH, 16);
	puts("MEDAL");
	color(39);
	gotoxy(22 + WIDTH, 20);
	puts("↑ ↓ ←  →");
	gotoxy(22 + WIDTH, 22);
	puts("[SPACE] pause");
	gotoxy(22 + WIDTH, 24);
	puts("twice[ESC] exit");
	gotoxy(22 + WIDTH, 3);
	puts("NEXT");

	color(BACK);
	for (i = 0; i < WIDTH; ++i) {
		for (j = 0; j < HEIGHT; ++j) {
			board[i][j].x = 4 + 2 * i;
			board[i][j].y = 3 + j;
			board[i][j].having = 0;
			gotoxy(board[i][j].x, board[i][j].y);
			puts("  ");
		}
	}
}

//游戏结束后作出清屏效果
void GameOver() {
	color(46);
	gotoxy(47, 14);
	puts("0    ");    //清空Socre分数为0

	gotoxy(42, 16);
	while (rank--)
		printf("  ");   //清空星星

	color(391);
	for (int i = HEIGHT - 1; i >= 0; i--)
		for (int j = 0; j < WIDTH; j++) {
			gotoxy(32 - j * 2, 3 + i);
			puts("██");   //清屏的刷涂
			Sleep(1);
		}
	color(BACK);
	for (int j = 0; j < HEIGHT; j++)
		for (int i = 0;i < WIDTH; i++) {
			board[i][j].having = 0;
			gotoxy(4 + 2 * i, j + 3);
			puts("  ");
			Sleep(1);
		}
}
void GetDepth(int* pdepth) {
	(*pdepth) = 24;
	for (int j = 0;j < 4;j++) {
		int sum = 0;
		int n = (current.x + vary[kind].vary_x[j] - 4) / 2;
		int m = current.y + vary[kind].vary_y[j] - 3;
		if (m < 0)
			m = 0;
		for (++m; m < 25; m++)
			if (board[n][m].having == 0 && n >= 0 && n < WIDTH && m >= 0 && m < HEIGHT)
				sum++;
			else
				break;
		if (sum < (*pdepth))
			(*pdepth) = sum;
	}
}
void CheckFull(int* pscore) {
	int i, j;
	int sum = 0;
	int s[] = { 0,1,3,6,10 };
	for (i = 0; i < HEIGHT; i++) {
		bool mark = 1;
		for (j = 0; j < WIDTH; j++) {
			if (board[j][i].having == 0) {
				mark = 0;
				board[j][i].color = BACK;
			}
		}
		if (mark) {
			sum++;
			for (int t = i; t > 0;t--) {
				gotoxy(4, 3 + t);
				for (j = 0; j < WIDTH; j++) {
					if (board[j][t - 1].having) {
						color(board[j][t - 1].color);
						printf("■");
						board[j][t].color = board[j][t - 1].color;
						board[j][t].having = 1;
						board[j][t - 1].having = 0;
					}
					else {
						color(BACK);
						printf("  ");
						board[j][t].color = board[j][t].color;
						board[j][t].having = 0;
						board[j][t - 1].having = 0;
					}
				}
			}
		}
	}
	(*pscore) += s[sum];
	color(46);
	gotoxy(47, 14);
	printf("%d", *pscore);  //Socre分数
	if (*pscore >= 140)
		rank = 5;
	else if (*pscore >= 90)
		rank = 4;
	else if (*pscore >= 50)
		rank = 3;
	else if (*pscore >= 20)
		rank = 2;
	else if (*pscore >= 1)
		rank = 1;
	color(44);
	gotoxy(43, 16);
	for (i = 0;i < rank && *pscore;i++)
		printf("★");
	int t;
	FILE* fp = fopen("score.txt", "r");
	fscanf(fp, "%d", &t); //历史记录t
	fclose(fp);
	//如果Socre分数大于t历史纪录
	if (*pscore > t) {
		fp = fopen("score.txt", "w");
		fprintf(fp, "%d", *pscore); //将Socre分数保存在文件中
		fclose(fp);
		color(43);   //颜色函数43
		gotoxy(47, 12); //在位置47列 12行
		printf("%d", *pscore); //输出Score在历史记录的位置
	}
	fclose(fp);
}

void PrintNext(int pkind) {
	int i;
	int x = 43, y = 6;
	color(34);
	for (i = 0; i < 4; i++) {
		gotoxy(x - 3, y + i - 2);
		puts("        ");
	}
	pri_kind = pkind;
	if (pkind == 6)
		pkind = 15, x--;
	else if (pkind == 5)
		pkind = 11, x--;
	else if (pkind == 4)
		pkind = 7, x--;
	else if (pkind == 3)
		pkind = 5, x -= 2;
	else if (pkind == 2)
		pkind = 3, x += 2;
	else if (pkind == 1)
		pkind = 2, x--;
	else
		pkind = 0, x -= 3;
	color(COL[pri_kind]);
	for (i = 0; i < 4; i++) {
		gotoxy(x + vary[pkind].vary_x[i], y + vary[pkind].vary_y[i]);
		puts("■");
	}
}
void CheckBoundary() {
	int i, n, m;
	for (i = 0; i < 4; i++) {
		m = current.x + vary[kind].vary_x[i];
		if (m < 4 || m > 32) {
			current.x += (m < 4 ? 2 : -2);
			i = 0;
		}
	}
	// 对四个方块分别检查
	for (i = 0; i < 4; i++)	{
		m = current.x + vary[kind].vary_x[i];
		n = current.y + vary[kind].vary_y[i];
		m = (m - 4) / 2;
		n -= 3;
		if (n < 0 || n>24 || board[m][n].having) {
			if (level == 1)		// 左移
				current.x += 2;   //复原
			else if (level == 2)	// 右移
				current.x -= 2;	//复原
			level = 0;
			break;
		}
	}
}
void LevelMove() {
	CheckBoundary();
	if (level == 0) return;
	int i;
	color(BACK);
	for (i = 0; i < 4; i++) {
		if (current.y + vary[kind].vary_y[i] > 2) {
			gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
			puts("  ");
		}
	}
	color(COL[pri_kind]);
	for (i = 0;i < 4;i++) {
		if (current.y + vary[kind].vary_y[i] > 2) {
			gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
			puts("■");
		}
	}
	before.x = current.x;
}

//旋转函数
void Revolve(int** prev_count) {
	int i;
	before.x = current.x;
	before.y = current.y;
	(**prev_count)++;
	(**prev_count) %= connection[pri_kind].sum;
	current.x = current.x + connection[pri_kind].connection_x[**prev_count];
	current.y = current.y + connection[pri_kind].connection_y[**prev_count];
	CheckBoundary();
	for (i = 0; i < 4; i++) {
		int m = (current.x - 4 + vary[NUMBER[pri_kind] + **prev_count].vary_x[i]) / 2;
		int n = current.y + vary[NUMBER[pri_kind] + **prev_count].vary_y[i] - 3;
		if (board[m][n].having || m > 14 || m < 0 || n>24) {
			current.x = current.x - connection[pri_kind].connection_x[**prev_count];
			current.y = current.y - connection[pri_kind].connection_y[**prev_count];
			revolve = 0;
			(**prev_count)--;
			before.x = current.x;
			before.y = current.y;
			return;
		}
	}
	color(BACK);
	for (i = 0;i < 4;i++) {
		if (before.y + vary[kind].vary_y[i] > 2) {
			gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
			puts("  ");
		}
	}
	kind = NUMBER[pri_kind] + **prev_count;
	color(COL[pri_kind]);
	for (i = 0;i < 4;i++) {
		if (current.y + vary[kind].vary_y[i] > 2) {
			gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
			puts("■");
		}
	}
	Sleep(SPEED[rank]);
	before.x = current.x;
	before.y = current.y;
}

void ChoiceDirection(int* prev_count) {
	int t = 50 / rank;
	while (t--) {
		if (_kbhit()) {
			char c = _getch();
			switch (c) {
			case 0x48:// up
				revolve = 1;
				Revolve(&prev_count);
				break;
			case 0x50: //down
				level = 0;
				return;
			case 0x4b: //left
				before.x = current.x;
				current.x -= 2;
				level = 1;
				LevelMove();
				break;
			case 0x4d:  //right
				before.x = current.x;
				current.x += 2;
				level = 2;
				LevelMove();
				break;
			case ' ':
				_getch();
				break;
			case 27:
				color(BACK);
				exit(1);
			default:break;
			}
		}
		Sleep(SPEED[rank]);
	}
}

void StraightFall(int depth) {
	int i;
	int rev_count = 0;
	before.x = current.x;
	before.y = current.y;
	while (GetDepth(&depth), depth--) {
		ChoiceDirection(&rev_count);
		GetDepth(&depth);
		if (depth == 0) {
			revolve = 0;
			before.x = current.x;
			before.y = current.y;
			break;
		}
		color(BACK);
		for (i = 0;i < 4;i++) {
			if (before.y + vary[kind].vary_y[i] > 2) {
				gotoxy(before.x + vary[kind].vary_x[i], before.y + vary[kind].vary_y[i]);
				puts("  ");
			}
		}
		current.y++;
		color(COL[pri_kind]);
		for (i = 0;i < 4;i++) {
			if (current.y + vary[kind].vary_y[i] > 2) {
				gotoxy(current.x + vary[kind].vary_x[i], current.y + vary[kind].vary_y[i]);
				puts("■");
			}
		}
		revolve = 0;
		before.x = current.x;
		before.y = current.y;
	}
	ChoiceDirection(&rev_count);
	if (level || revolve) {
		level = 0;
		revolve = 0;
		ChoiceDirection(&rev_count);
		StraightFall(depth);		 //递归调用
	}
}
int main() {
	srand((unsigned long)time(0));
	system("mode con cols=56 lines=30");
	system("color 24");

	color(46);
	gotoxy(47, 14);
	puts("0");

	int score = 0;
	FILE* fp = fopen("score.txt", "r");
	if (fp == NULL) {
		fp = fopen("score.txt", "w");
		fprintf(fp, "%d", score);
		fclose(fp);
	}
	PrintGarphy();
	kind = rand() % 7;
	int depth;
	while (1) {
		int pkind = rand() % 7;
		PrintNext(pkind);
		current.x = 18;
		current.y = 2;		// 方块初始出现的坐标
		GetDepth(&depth);   // 初始深度
		if (depth == 0) {
			GameOver();
			score = 0;
			rank = 1;
		}
		pri_kind = kind;
		if (kind == 6)
			kind = 15, current.x -= 2;
		else if (kind == 5)
			kind = 11;
		else if (kind == 4)
			kind = 7;
		else if (kind == 3)
			kind = 5;
		else if (kind == 2)
			kind = 3, current.x += 2;
		else if (kind == 1)
			kind = 2;
		else
			kind = 0, current.x -= 2;
		StraightFall(depth);
		for (int i = 0;i < 4;i++) {
			int m = (current.x - 4 + vary[kind].vary_x[i]) / 2;
			int n = current.y - 3 + vary[kind].vary_y[i];
			board[m][n].having = 1;
			board[m][n].color = COL[pri_kind];
		}
		CheckFull(&score);
		kind = pkind;
	}
	return 0;
}