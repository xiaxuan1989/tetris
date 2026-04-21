#include "Blocks.h"
#include "OtherFuncs.h"
#include "ColorAndKey.h"
#include <cstdio>

extern int bg_status[11][21];

Square::Square()
{
	block1 = { 5, 0 };
	block2 = { 6, 0 };
	block3 = { 5, 1 };
	block4 = { 6, 1 };

	color = MakeColor();
	status = 0;
}

void Square::draw()
{
	SetBlockColor(color);

	//如果方块上半部分尚未进入边框内，则不画出上半部分
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("■");
		bg_status[block1.X - 1][block1.Y - 1] = color;   //将数组对应的元素设置相应值

		GotoXY(block2);
		printf("■");
		bg_status[block2.X - 1][block2.Y - 1] = color;
	}

	GotoXY(block3);
	printf("■");
	bg_status[block3.X - 1][block3.Y - 1] = color;

	GotoXY(block4);
	printf("■");
	bg_status[block4.X - 1][block4.Y - 1] = color;
}

void Square::left_move()
{
	//如果左移后没有超出左侧区域，并且左侧两格皆为空，则左移
	if (block1.X - 1 > 0 && (bg_status[block1.X - 2][block1.Y - 1] == BLANK)
		&& (bg_status[block3.X - 2][block3.Y - 1] == BLANK))
	{
		wipe();

		block1.X--;
		block2.X--;
		block3.X--;
		block4.X--;

		draw();
	}
}

void Square::right_move()
{
	//如果右移后没有超出右侧区域，并且右侧两格皆为空，则右移
	if (block2.X + 1 < 12 && (bg_status[block2.X][block2.Y - 1] == BLANK)
		&& (bg_status[block4.X][block4.Y - 1] == BLANK))
	{
		wipe();

		block1.X++;
		block2.X++;
		block3.X++;
		block4.X++;

		draw();
	}
}

void Square::revolve()
{

}

bool Square::down()
{
	//如果方块底部高于边框底部，并且方块下面没有其它方块，则下落
	if (block3.Y <= 20 && (bg_status[block3.X - 1][block3.Y]) == BLANK
		&& (bg_status[block4.X - 1][block4.Y]) == BLANK)
	{
		wipe();
		block1.Y++;
		block2.Y++;
		block3.Y++;
		block4.Y++;
		draw();
		return true;
	}
	return false;
}

void Square::wipe()
{
	//如果方块上半部分在边框内，则擦除
	if (block1.Y > 0)
	{
		GotoXY(block1);
		printf("  ");
		bg_status[block1.X - 1][block1.Y - 1] = BLANK;

		GotoXY(block2);
		printf("  ");
		bg_status[block2.X - 1][block2.Y - 1] = BLANK;
	}

	GotoXY(block3);
	printf("  ");
	bg_status[block3.X - 1][block3.Y - 1] = BLANK;

	GotoXY(block4);
	printf("  ");
	bg_status[block4.X - 1][block4.Y - 1] = BLANK;
}

Square::~Square()
{
}