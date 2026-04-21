#ifndef _BLOCKS_H_
#define _BLOCKS_H_
#include <Windows.h>

//定义一个抽象类，用于后续产生各种方块组合
class Blocks{
public:
	virtual ~Blocks();

public:
	virtual void draw() = 0;        //画出对象图案
	virtual void left_move() = 0;   //左移操作
	virtual void right_move() = 0;  //右移操作
	virtual void revolve() = 0;     //旋转操作
	virtual bool down() = 0;        //下落操作
	virtual void wipe() = 0;        //擦除当前方块组合

protected:
	COORD block1;
	COORD block2;
	COORD block3;
	COORD block4;
	int color;
	int status;     //标记方块组合的状态
};

//■■
//■■ 类
class Square :public Blocks{
public:
	Square();
	~Square();
	
public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();
};

//■■■■类
class Bar :public Blocks{
public:
	Bar();
	~Bar();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

//■
//■■
//  ■ 类
class Rcrutch :public Blocks{
public:
	Rcrutch();
	~Rcrutch();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

//  ■
//■■
//■   类
class Lcrutch :public Blocks{
public:
	Lcrutch();
	~Lcrutch();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

//■
//■
//■■ 类
class L :public Blocks{
public:
	L();
	~L();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

//  ■
//  ■
//■■ 类
class RevL :public Blocks{
public:
	RevL();
	~RevL();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

//  ■
//■■■ 类
class T :public Blocks{
public:
	T();
	~T();

public:
	virtual void draw();
	virtual void left_move();
	virtual void right_move();
	virtual void revolve();
	virtual bool down();
	virtual void wipe();

};

#endif