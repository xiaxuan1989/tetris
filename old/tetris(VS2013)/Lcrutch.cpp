#include "Blocks.h"
#include "OtherFuncs.h"
#include "ColorAndKey.h"
#include <cstdio>

extern int bg_status[11][21];

Lcrutch::Lcrutch()
{
	block1 = { 6, -1 };
	block2 = { 6, 0 };
	block3 = { 5, 0 };
	block4 = { 5, 1 };

	color = MakeColor();
	status = 1;   //以1记为竖着状态
}

void Lcrutch::draw()
{
	SetBlockColor(color);

	//如果block1进入边框，则画出
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("■");
		bg_status[block1.X - 1][block1.Y - 1] = color;
	}

	//如果block2,3进入边框，则画出
	if (block2.Y > 0)
	{
		GotoXY(block2);
		printf("■");
		bg_status[block2.X - 1][block2.Y - 1] = color;

		GotoXY(block3);
		printf("■");
		bg_status[block3.X - 1][block3.Y - 1] = color;
	}

	GotoXY(block4);
	printf("■");
	bg_status[block4.X - 1][block4.Y - 1] = color;
}

void Lcrutch::left_move()
{
	//竖着状态下，如果左移后越界或者左侧有其它方块，则函数返回
	if (status == 1){
		if (block3.X - 1 <= 0
			|| (bg_status[block1.X - 2][block1.Y - 1] != BLANK)
			|| (bg_status[block3.X - 2][block3.Y - 1] != BLANK)
			|| (bg_status[block4.X - 2][block4.Y - 1] != BLANK))
			return;

	}//横着状态下，如果左移后没有超出左侧边框,并且左侧一格全为空，则左移
	else{
		if (block1.X - 1 <= 0
			|| (bg_status[block1.X - 2][block1.Y - 1] != BLANK)
			|| (bg_status[block3.X - 2][block3.Y - 1] != BLANK))
			return;
	}

	wipe();
	block1.X--;
	block2.X--;
	block3.X--;
	block4.X--;
	draw();
}

void Lcrutch::right_move()
{
	//竖着状态下,如果右移后越界或者右侧有其它方块，则函数返回
	if (status == 1){
		if (block1.X + 1 >= 12
			|| (bg_status[block1.X][block1.Y - 1] != BLANK)
			|| (bg_status[block2.X][block2.Y - 1] != BLANK)
			|| (bg_status[block4.X][block4.Y - 1] != BLANK))
			return;

	}//横着状态下，如果右移后越界或者右侧有其它方块,则函数返回
	else{
		if (block4.X + 1 >= 12
			|| (bg_status[block2.X][block2.Y - 1] != BLANK)
			|| (bg_status[block4.X][block4.Y - 1] != BLANK))
			return;
	}

	wipe();
	block1.X++;
	block2.X++;
	block3.X++;
	block4.X++;
	draw();
}

void Lcrutch::revolve()
{
	//如果当前状态竖着，则旋转为横状态
	if (status == 1){
		if (block1.Y > 0 && block4.X + 2 < 12
			&& bg_status[block3.X][block3.Y] == BLANK
			&&bg_status[block4.X + 1][block4.Y - 1] == BLANK) {

			wipe();
			block1.X--;
			block1.Y++;
			block3.X++;
			block3.Y++;
			block4.X += 2;
			draw();
			status = 2;
		}
	}//否则转换为竖状态
	else{
		if (bg_status[block1.X][block1.Y - 2] == BLANK
			&&bg_status[block4.X - 3][block4.Y] == BLANK){

			wipe();
			block1.X++;
			block1.Y--;
			block3.X--;
			block3.Y--;
			block4.X -= 2;
			draw();
			status = 1;
		}
	}

}

bool Lcrutch::down()
{
	//竖着状态下，如果长条越界或者下面有其它方块，则函数返回
	if (status == 1){
		if (block4.Y + 1 > 21
			|| bg_status[block2.X - 1][block2.Y] != BLANK
			|| bg_status[block4.X - 1][block4.Y] != BLANK){

			return false;

		}
	}
	//横着状态下，如果长条越界或者下面有其它方块，则函数返回
	else{
		if (block3.Y + 1 > 21
			|| bg_status[block1.X - 1][block1.Y] != BLANK
			|| bg_status[block3.X - 1][block3.Y] != BLANK
			|| bg_status[block4.X - 1][block4.Y] != BLANK){

			return false;
		}
	}

	wipe();
	block1.Y++;
	block2.Y++;
	block3.Y++;
	block4.Y++;
	draw();
	return true;
}

void Lcrutch::wipe()
{
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("  ");
		bg_status[block1.X - 1][block1.Y - 1] = BLANK;
	}

	if (block2.Y > 0)
	{
		GotoXY(block2);
		printf("  ");
		bg_status[block2.X - 1][block2.Y - 1] = BLANK;

		GotoXY(block3);
		printf("  ");
		bg_status[block3.X - 1][block3.Y - 1] = BLANK;
	}

	GotoXY(block4);
	printf("  ");
	bg_status[block4.X - 1][block4.Y - 1] = BLANK;
}

Lcrutch::~Lcrutch()
{
}