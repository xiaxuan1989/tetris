#include "Blocks.h"
#include "OtherFuncs.h"
#include "ColorAndKey.h"
#include <cstdio>

extern int bg_status[11][21];

Bar::Bar()
{
	block1 = { 4, 0 };
	block2 = { 5, 0 };
	block3 = { 6, 0 };
	block4 = { 7, 0 };

	color = MakeColor();
	status = 1;   //以1记为横
}

void Bar::draw()
{
	SetBlockColor(color);

	//如果长条进入边框内，则画出
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("■");
		bg_status[block1.X - 1][block1.Y - 1] = color;

		GotoXY(block2);
		printf("■");
		bg_status[block2.X - 1][block2.Y - 1] = color;

		GotoXY(block3);
		printf("■");
		bg_status[block3.X - 1][block3.Y - 1] = color;

		GotoXY(block4);
		printf("■");
		bg_status[block4.X - 1][block4.Y - 1] = color;
	}
}

void Bar::left_move()
{
	//横着状态下，如果左移后越界或者左侧有其它方块，则函数返回
	if (status == 1)
	{
		if (block1.X - 1 <= 0 || (bg_status[block1.X - 2][block1.Y - 1] != BLANK))
			return;

	}  //竖着状态下，如果左移后没有超出左侧边框,并且左侧四格全为空，则左移
	else{
		if (block1.X - 1 <= 0
			|| (bg_status[block1.X - 2][block1.Y - 1] != BLANK)
			|| (bg_status[block2.X - 2][block2.Y - 1] != BLANK)
			|| (bg_status[block3.X - 2][block3.Y - 1] != BLANK)
			|| (bg_status[block4.X - 2][block4.Y - 1] != BLANK)) {

			return;
			}
		
	}

	wipe();
	block1.X--;
	block2.X--;
	block3.X--;
	block4.X--;
	draw();
}

void Bar::right_move()
{
	//横着状态下,如果右移后越界或者右侧有其它方块，则函数返回
	if (status == 1){
		if (block4.X + 1 >= 12 || (bg_status[block4.X][block4.Y - 1] != BLANK))
			return;
	}//竖着状态下，如果右移后越界或者右侧有其它方块,则函数返回
	else{
		if (block1.X + 1 >= 12
			|| (bg_status[block1.X][block1.Y - 1] != BLANK)
			|| (bg_status[block2.X][block2.Y - 1] != BLANK)
			|| (bg_status[block3.X][block3.Y - 1] != BLANK)
			|| (bg_status[block4.X][block4.Y - 1] != BLANK)){

			return;
		}
	}

	wipe();
	block1.X++;
	block2.X++;
	block3.X++;
	block4.X++;
	draw();
}

void Bar::revolve()
{
	//如果当前状态横着，则旋转为竖状态
	if (status == 1)
	{
		//如果旋转后仍在边框内，并且不与其它方块重合，则旋转
		if (block1.Y > 0 && block4.Y + 3 < 22 
			&& (bg_status[block2.X - 2][block2.Y] == BLANK)
			&& (bg_status[block3.X - 3][block3.Y + 1]) == BLANK
			&& (bg_status[block4.X - 4][block4.Y + 2]) == BLANK)
		{
			wipe();

			block2.X--;
			block2.Y++;
			block3.X -= 2;
			block3.Y += 2;
			block4.X -= 3;
			block4.Y += 3;

			status = 2;
			draw();
		}
	}
	//否则旋转为横状态
	else
	{
		if (block4.X + 3 < 12
			&& (bg_status[block2.X][block2.Y - 2] == BLANK)
			&& (bg_status[block3.X + 1][block3.Y - 3] == BLANK)
			&& (bg_status[block4.X + 2][block4.Y - 4] == BLANK))
		{
			wipe();

			block2.X++;
			block2.Y--;
			block3.X += 2;
			block3.Y -= 2;
			block4.X += 3;
			block4.Y -= 3;

			status = 1;
			draw();
		}
	}
}

bool Bar::down()
{
	//横着状态下，如果长条越界或者下面有其它方块，则函数返回
	if (status == 1){
		if (block4.Y - 1 > 19
			|| (bg_status[block1.X - 1][block1.Y] != BLANK)
			|| (bg_status[block2.X - 1][block2.Y] != BLANK)
			|| (bg_status[block3.X - 1][block3.Y] != BLANK)
			|| (bg_status[block4.X - 1][block4.Y] != BLANK)){

			return false;
		}
	}//竖着状态下，如果长条底部越界或者下面有其它方块，则函数返回
	else{
		if (block4.Y - 1 > 19
			|| (bg_status[block4.X - 1][block4.Y]))

			return false;
	}
	
	wipe();
	block1.Y++;
	block2.Y++;
	block3.Y++;
	block4.Y++;
	draw();
	return true;
}

void Bar::wipe()
{
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("  ");
		bg_status[block1.X - 1][block1.Y - 1] = BLANK;

		GotoXY(block2);
		printf("  ");
		bg_status[block2.X - 1][block2.Y - 1] = BLANK;

		GotoXY(block3);
		printf("  ");
		bg_status[block3.X - 1][block3.Y - 1] = BLANK;

		GotoXY(block4);
		printf("  ");
		bg_status[block4.X - 1][block4.Y - 1] = BLANK;
	}
}

Bar::~Bar()
{
}