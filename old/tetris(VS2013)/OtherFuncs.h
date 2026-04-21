#ifndef _OTHER_H_
#define _OTHER_H_
#include <Windows.h>
#include "Blocks.h"

void HideCursor();      //隐藏光标

void SetWindowRect();   //设置窗口尺寸

void SetBufSize();      //设置输出缓冲区尺寸

void Interface();       //打印游戏初始界面

int MakeColor();        //随机产生一种颜色

void GotoXY(const COORD &obj);   //将光标跳到指定坐标

void SetBlockColor(int color);   //设置方块颜色

int GetKey();                    //接收键盘输入

Blocks *MakeBlocks();            //随机生成一种方块组合

Blocks *MakeNext();              //产生下一个方块的预览

void BlocksControl(Blocks *pB);  //控制方块组合

bool IsFilled(int line);         //判断某一行是否被填满

bool IsEmpty(int line);          //判断某一行是否全为空

void Remove(int line);           //清除某一行

void flush();                    //刷新屏幕

#endif