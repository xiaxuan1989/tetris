#include "Blocks.h"
#include "OtherFuncs.h"
#include "ColorAndKey.h"
#include <cstdio>

extern int bg_status[11][21];

T::T()
{
	block1 = { 6, 0 };
	block2 = { 5, 1 };
	block3 = { 6, 1 };
	block4 = { 7, 1 };

	color = MakeColor();
	status = 1;
}

void T::draw()
{
	SetBlockColor(color);

	if (block1.Y > 0){
		GotoXY(block1);
		printf("¡ö");
		bg_status[block1.X - 1][block1.Y - 1] = color;
	}

	GotoXY(block2);
	printf("¡ö");
	bg_status[block2.X - 1][block2.Y - 1] = color;

	GotoXY(block3);
	printf("¡ö");
	bg_status[block3.X - 1][block3.Y - 1] = color;

	GotoXY(block4);
	printf("¡ö");
	bg_status[block4.X - 1][block4.Y - 1] = color;
}

void T::left_move()
{
	if (status == 1){
		if (block2.X - 1 <= 0
			|| bg_status[block1.X - 2][block1.Y - 1] != BLANK
			|| bg_status[block2.X - 2][block2.Y - 1] != BLANK){

			return;
		}
	}
	else if (status == 2){
		if (block1.X - 1 <= 0
			|| bg_status[block1.X - 2][block1.Y - 1] != BLANK
			|| bg_status[block2.X - 2][block2.Y - 1] != BLANK
			|| bg_status[block4.X - 2][block4.Y - 1] != BLANK){

			return;
		}
	}
	else if (status == 3){
		if (block4.X - 1 <= 0
			|| bg_status[block1.X - 2][block1.Y - 1] != BLANK
			|| bg_status[block4.X - 2][block4.Y - 1] != BLANK){

			return;
		}
	}
	else{
		if (block2.X - 1 <= 0
			|| bg_status[block2.X - 2][block2.Y - 1] != BLANK
			|| bg_status[block3.X - 2][block3.Y - 1] != BLANK
			|| bg_status[block4.X - 2][block4.Y - 1] != BLANK){

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

void T::right_move()
{
	if (status == 1){
		if (block4.X + 1 >= 12
			|| bg_status[block1.X][block1.Y - 1] != BLANK
			|| bg_status[block4.X][block4.Y - 1] != BLANK){

			return;
		}
	}
	else if (status == 2){
		if (block2.X + 1 >= 12
			|| bg_status[block2.X][block2.Y - 1] != BLANK
			|| bg_status[block3.X][block3.Y - 1] != BLANK
			|| bg_status[block4.X][block4.Y - 1] != BLANK){

			return;
		}
	}
	else if (status == 3){
		if (block2.X + 1 >= 12
			|| bg_status[block1.X][block1.Y - 1] != BLANK
			|| bg_status[block2.X][block2.Y - 1] != BLANK){

			return;
		}
	}
	else{
		if (block1.X + 1 >= 12
			|| bg_status[block1.X][block1.Y - 1] != BLANK
			|| bg_status[block2.X][block2.Y - 1] != BLANK
			|| bg_status[block4.X][block4.Y - 1] != BLANK){

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

void T::revolve()
{
	if (status == 1){
		if (block1.Y > 1
			&&bg_status[block4.X - 1][block4.Y - 3] == BLANK
			&&bg_status[block3.X][block3.Y - 2] == BLANK){

			wipe();
			block2.X += 2;
			block3.X++;
			block3.Y--;
			block4.Y -= 2;
			draw();
			status = 2;
		}
	}
	else if (status == 2){
		if (block4.X-2>0
			&&bg_status[block4.X - 3][block4.Y - 1] == BLANK
			&& bg_status[block3.X - 2][block3.Y - 2] == BLANK) {

			wipe();
			block2.Y -= 2;
			block3.X--;
			block3.Y--;
			block4.X -= 2;
			draw();
			status = 3;
		}
	}
	else if (status == 3){
		if (bg_status[block3.X - 2][block3.Y] == BLANK
			&&bg_status[block4.X - 1][block4.Y + 1] == BLANK) {

			wipe();
			block2.X -= 2;
			block3.X--;
			block3.Y++;
			block4.Y += 2;
			draw();
			status = 4;
		}
	}
	else{
		if (block4.X+2 < 12
			&&bg_status[block3.X][block3.Y] == BLANK
			&&bg_status[block4.X + 1][block4.Y - 1] == BLANK){

			wipe();
			block2.Y += 2;
			block3.X++;
			block3.Y++;
			block4.X += 2;
			draw();
			status = 1;
		}
	}
}

bool T::down()
{
	if (status == 1){
		if (block2.Y + 1 > 21
			|| bg_status[block2.X - 1][block2.Y] != BLANK
			|| bg_status[block3.X - 1][block3.Y] != BLANK
			|| bg_status[block4.X - 1][block4.Y] != BLANK){

			return false;
		}
	}
	else if (status == 2){
		if (block2.Y + 1 > 21
			|| bg_status[block1.X - 1][block1.Y] != BLANK
			|| bg_status[block2.X - 1][block2.Y] != BLANK){

			return false;
		}
	}
	else if (status==3){
		if (block1.Y + 1 > 21
			|| bg_status[block1.X - 1][block1.Y] != BLANK
			|| bg_status[block2.X - 1][block2.Y] != BLANK
			|| bg_status[block4.X - 1][block4.Y] != BLANK){

			return false;
		}
	}
	else{
		if (block4.Y + 1 > 21
			|| bg_status[block1.X - 1][block1.Y] != BLANK
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

void T::wipe()
{
	if (block1.Y > 0){
		GotoXY(block1);
		printf("  ");
		bg_status[block1.X - 1][block1.Y - 1] = BLANK;
	}

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

T::~T()
{
}