#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>

#ifdef __cplusplus 
#define __CPPARGS ...
#else
#define __CPPARGS
#endif
#define MINBOXSIZE 15 /* 最小方块的尺寸 */
#define BGCOLOR 7 /* 背景着色 */
#define GX 200
#define GY 10
#define SJNUM 10000 /* 每当玩家打到一万分等级加一级*/

/* 按键码*/
#define VK_LEFT 0x4b00
#define VK_RIGHT 0x4d00
#define VK_DOWN 0x5000
#define VK_UP 0x4800
#define VK_HOME 0x4700
#define VK_END 0x4f00
#define VK_SPACE 0x3920
#define VK_ESC 0x011b
#define VK_ENTER 0x1c0d

/* 定义俄罗斯方块的方向（我定义他为4种）*/
#define F_DONG 0
#define F_NAN 1
#define F_XI 2
#define F_BEI 3

#define NEXTCOL 20 /* 要出的下一个方块的纵坐标*/
#define NEXTROW 12 /* 要出的下一个方块的横从标*/
#define MAXROW 14 /* 游戏屏幕大小*/
#define MAXCOL 20
#define SCCOL 100 /*游戏屏幕大显示器上的相对位置*/
#define SCROW 60


int gril[22][16]; /* 游戏屏幕坐标*/
int col=1,row=7; /* 当前方块的横纵坐标*/
int boxfx=0,boxgs=0; /* 当前寺块的形壮和方向*/
int nextboxfx=0,nextboxgs=0,maxcol=22;/*下一个方块的形壮和方向*/
int minboxcolor=6,nextminboxcolor=6;
int num=0; /*游戏分*/
int dj=0,gamedj[10]={18,16,14,12,10,8,6,4,2,1};/* 游戏等级*/
/* 以下我用了一个3维数组来纪录方块的最初形状和方向*/
int boxstr[7][4][16]={{
{1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
{0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0},
{1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
{0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0}},
{
{0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0},
{1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0},
{0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0},
{1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0}},
{
{1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0},
{1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
{1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0},
{0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0}},
{
{1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0},
{1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
{0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0},
{1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0}},
{
{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},
{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},
{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0}},
{
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0},
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0},
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0},
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0}},
{
{0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0},
{1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0},
{0,1,0,0,1,1,1,0,0,0,0,0.0,0,0,0},
{0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0}}

};

/* 随机得到当前方块和下一个方块的形状和方向*/
void boxrad(){
minboxcolor=nextminboxcolor;
boxgs=nextboxgs;
boxfx=nextboxfx;
nextminboxcolor=random(14)+1;
if(nextminboxcolor==4||nextminboxcolor==7||nextminboxcolor==8)
nextminboxcolor=9;
nextboxfx=F_DONG;
nextboxgs=random(7);
}
/*初始化图形模试*/
void init(int gdrive,int gmode){
int errorcode;
initgraph(&gdrive,&gmode,"D:\\tc\\");
errorcode=graphresult();
if(errorcode!=grOk){
printf("error of: %s",grapherrormsg(errorcode));
exit(1);
}
}

/* 在图形模式下的清屏 */
void cls()
{
setfillstyle(SOLID_FILL,0);
setcolor(0);
bar(0,0,640,480);
}
/*在图形模式下的高级清屏*/
void clscr(int a,int b,int c,int d,int color){
setfillstyle(SOLID_FILL,color);
setcolor(color);
bar(a,b,c,d);
}
/*最小方块的绘制*/
void minbox(int asc,int bsc,int color,int bdcolor){
int a=0,b=0;
a=SCCOL+asc;
b=SCROW+bsc;
clscr(a+1,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE,color);
if(color!=BGCOLOR){
setcolor(bdcolor);
line(a+1,b+1,a-1+MINBOXSIZE,b+1);
line(a+1,b+1,a+1,b-1+MINBOXSIZE);
line(a-1+MINBOXSIZE,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
line(a+1,b-1+MINBOXSIZE,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
}
}

/*游戏中出现的文字*/
void txt(int a,int b,char *txt,int font,int color){
setcolor(color);
settextstyle(0,0,font);
outtextxy(a,b,txt);
}
/*windows 绘制*/
void win(int a,int b,int c,int d,int bgcolor,int bordercolor){
clscr(a,b,c,d,bgcolor);
setcolor(bordercolor);
line(a,b,c,b);
line(a,b,a,d);
line(a,d,c,d);
line(c,b,c,d);

}

/* 当前方块的绘制*/
void funbox(int a,int b,int color,int bdcolor){
int i,j;
int boxz[4][4];
for(i=0;i<16;i++)
boxz[i/4][i%4]=boxstr[boxgs][boxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(boxz[i][j]==1)
minbox((j+row+a)*MINBOXSIZE,(i+col+b)*MINBOXSIZE,color,bdcolor);
}
/*下一个方块的绘制*/
void nextfunbox(int a,int b,int color,int bdcolor){
int i,j;
int boxz[4][4];
for(i=0;i<16;i++)
boxz[i/4][i%4]=boxstr[nextboxgs][nextboxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(boxz[i][j]==1)
minbox((j+a)*MINBOXSIZE,(i+b)*MINBOXSIZE,color,bdcolor);
}
/*时间中断定义*/
#define TIMER 0x1c
int TimerCounter=0;
void interrupt ( *oldhandler)(__CPPARGS);
void interrupt newhandler(__CPPARGS){
TimerCounter++;
oldhandler();
}
void SetTimer(void interrupt (*IntProc)(__CPPARGS)){
oldhandler=getvect(TIMER);
disable();
setvect(TIMER,IntProc);
enable();
}
/*由于游戏的规则，消掉都有最小方块的一行*/
void delcol(int a){
int i,j;
for(i=a;i>1;i--)
for(j=1;j<15;j++){
minbox(j*MINBOXSIZE,i*MINBOXSIZE,BGCOLOR,BGCOLOR);
gril[i][j]=gril[i-1][j];
if(gril[i][j]==1)
minbox(j*MINBOXSIZE,i*MINBOXSIZE,minboxcolor,0);
}
}

/*消掉所有都有最小方块的行*/
void delete(){
int i,j,zero,delgx=0;
char *nm="00000";
for(i=1;i<21;i++){
zero=0;
for(j=1;j<15;j++)
if(gril[i][j]==0)
zero=1;
if(zero==0){
delcol(i);
delgx++;
}
}
num=num+delgx*delgx*10;
dj=num/10000;

sprintf(nm,"%d",num);
clscr(456,173,500,200,4);
txt(456,173,"Number:",1,15);
txt(456,193,nm,1,15);
}
/*时间中断结束*/
void KillTimer(){
disable();
setvect(TIMER,oldhandler);
enable();
}
/* 测试当前方块是否可以向下落*/
int downok(){
int i,j,k=1,a[4][4];
for(i=0;i<16;i++)
a[i/4][i%4]=boxstr[boxgs][boxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(a[i][j] && gril[col+i+1][row+j])
k=0;
return(k);
}
/* 测试当前方块是否可以向左行*/
int leftok(){
int i,j,k=1,a[4][4];
for(i=0;i<16;i++)
a[i/4][i%4]=boxstr[boxgs][boxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(a[i][j] && gril[col+i][row+j-1])
k=0;
return(k);
}
/* 测试当前方块是否可以向右行*/
int rightok(){
int i,j,k=1,a[4][4];
for(i=0;i<16;i++)
a[i/4][i%4]=boxstr[boxgs][boxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(a[i][j] && gril[col+i][row+j+1])
k=0;
return(k);
}
/* 测试当前方块是否可以变形*/
int upok(){
int i,j,k=1,a[4][4];
for(i=0;i<4;i++)
for(i=0;i<16;i++)
a[i/4][i%4]=boxstr[boxgs][boxfx+1][i];
for(i=3;i>=0;i--)
for(j=3;j>=0;j--)
if(a[i][j] && gril[col+i][row+j])
k=0;
return(k);
}
/*当前方块落下之后，给屏幕坐标作标记*/
void setgril(){
int i,j,a[4][4];
funbox(0,0,minboxcolor,0);
for(i=0;i<16;i++)
a[i/4][i%4]=boxstr[boxgs][boxfx][i];
for(i=0;i<4;i++)
for(j=0;j<4;j++)
if(a[i][j])
gril[col+i][row+j]=1;
col=1;row=7;
}
/*游戏结束*/
void gameover(){
int i,j;

for(i=20;i>0;i--)
for(j=1;j<15;j++)
minbox(j*MINBOXSIZE,i*MINBOXSIZE,2,0);
txt(103,203,"Game Over",3,10);
}
/*按键的设置*/
void call_key(int keyx){
switch(keyx){
case VK_DOWN: { /*下方向键，横坐标加一。*/
if(downok()){
col++;
funbox(0,0,minboxcolor,0);}
else{
funbox(0,0,minboxcolor,0);
setgril();
nextfunbox(NEXTCOL,NEXTROW,4,4);
boxrad();
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0);
delete();
}
break;
}
case VK_UP: { /*上方向键，方向形状旋转90度*/
if(upok())
boxfx++;
if(boxfx>3)
boxfx=0;
funbox(0,0,minboxcolor,0);
break;
}
case VK_LEFT:{ /*左方向键，纵坐标减一*/
if(leftok())
row--;
funbox(0,0,minboxcolor,0);
break;
}
case VK_RIGHT:{ /*右方向键，纵坐标加一*/
if(rightok())
row++;
funbox(0,0,minboxcolor,0);
break;
}
case VK_SPACE: /*空格键，直接落到最后可以落到的们置*/
while(downok())
col++;
funbox(0,0,minboxcolor,0);
setgril();
nextfunbox(NEXTCOL,NEXTROW,4,4);
boxrad();
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0);
delete();
break;
default:
{
txt(423,53,"worng key!",1,4);
txt(428,80,"Plese Enter Anly Key AG!",1,4);
getch();
clscr(420,50,622,97,BGCOLOR);
}
}
}

/*时间中断开始*/
void timezd(void){
int key;
SetTimer(newhandler);
boxrad();
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0);
for(;;){
if(bioskey(1)){
key=bioskey(0);
funbox(0,0,BGCOLOR,BGCOLOR);
if(key==VK_ESC)
break;
call_key(key);
}
if(TimerCounter>gamedj[dj]){
TimerCounter=0;
if(downok()){
funbox(0,0,BGCOLOR,BGCOLOR);
col++;
funbox(0,0,minboxcolor,0);
}
else {
if(col==1){
gameover();
getch();
break;
}
setgril();
delete();
funbox(0,0,minboxcolor,0);
col=1;row=7;
funbox(0,0,BGCOLOR,BGCOLOR);
nextfunbox(NEXTCOL,NEXTROW,4,4);
boxrad();
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0);
}
}
}
}

/*主程序开始*/
void main(void){
int i,j;
char *nm="00000";
init(VGA,VGAHI);
cls();
/*屏幕坐标初始化*/
for(i=0;i<=MAXCOL+1;i++)
for(j=0;j<=MAXROW+1;j++)
gril[i][j]=0;
for(i=0;i<=MAXCOL+1;i++) {
gril[i][0]=1;
gril[i][15]=1;
}
for(j=1;j<=MAXROW;j++){
gril[0][j]=1;
gril[21][j]=1;
}
clscr(0,0,640,480,15);
win(1,1,639,479,4,15);
win(SCCOL+MINBOXSIZE-2,SCROW+MINBOXSIZE-2,SCCOL+15*MINBOXSIZE+2,SCROW+21*MINBOXSIZE+2,BGCOLOR,0);
nextboxgs=random(8);
nextboxfx=random(4);
sprintf(nm,"%d",num);
txt(456,173,"Number:",1,15);
txt(456,193,nm,1,15);
txt(456,243,"Next Box:",1,15);
timezd();
KillTimer();
closegraph();
}
56
回答者：热心网友 | 回答时间：2008-9-13 09:11 | 我来评论 
相关内容 
2011-5-15
VC6.0环境下的俄罗斯方块C语言源代码 
5 
2009-11-4
求俄罗斯方块C语言源代码，用Microsoft C++ 6.0能编译出来。 
9 
2008-6-19
俄罗斯方块的c语言源代码 api实现 
2 
2009-9-8
求用c语言写的俄罗斯方块源代码 
43 
2011-1-29
哪个才是俄罗斯方块源代码 C语言的 
更多关于俄罗斯方块源码的问题>> 
查看同主题问题： 俄罗斯方块 语言 语言 源代码 
等待您来回答
0回答
MiniGUI 程序中为什么我的鼠标无法正常工作

0回答
smdk2410 是什么，与S3C2410什么关系

1回答
那个开题报告也能发我一份么！谢谢！你有上市公司财务风险研究的开题...

0回答
50
关于百雀羚凡士林保湿润肤霜与大宝SOD蜜的市场份额比例的具体情况，大...

2回答
“MiniGUI图形库”这个词是怎么读的

0回答
20
谁有 s3c2410板子的wince6.0 的BSP包。

0回答
10
求运算放大器应用的开题报告，及运算放大器的现状，学习的重点和难点。

1回答
20
sod公司的电影有迅雷地址的，有的朋友发723453145@qq.com 好了给分

更多等待您来回答的问题>>
其他回答 共8条 
只有100分 
回答者： chenyilong58 | 一级 | 2008-9-13 09:04 
#include <graphics.h> 

#include <stdio.h> 

#include <dos.h> 

#include <conio.h> 

#include <time.h> 

#include <math.h> 

#include <stdlib.h> 

/*定义左上角点在屏幕上的位置*/ 

#define MAPXOFT     9 

#define MAPYOFT     5 

/*定义下一个方块显示的位置*/ 

#define MAPXOFT1     13 

#define MAPYOFT1     -2 

#define LEFT  0x4b00 

#define RIGHT 0x4d00 

#define DOWN  0x5000  /*此键为加速键*/ 

#define UP    0x4800  /*此键为变形键*/ 

#define ESC   0x011b  /*此键为退出键*/ 

#define ENTER 0x1c0d 

#define TIMER 0x1c /* 时钟中断的中断号 */ 

/* 中断处理函数在C和C++中的表示略有不同。 

如果定义了_cplusplus则表示在C++环境下，否则是在C环境下。 */ 

#ifdef __cplusplus 

#define __CPPARGS ... 

#else 

#define __CPPARGS 

#endif 

int TimerCounter=0; /* 计时变量，每秒钟增加18。 */ 

/* 指向原来时钟中断处理过程入口的中断处理函数指针(句柄) */ 

void interrupt ( *oldhandler)(__CPPARGS); 

/* 新的时钟中断处理函数 */ 

void interrupt newhandler(__CPPARGS) 

{ 

    /* increase the global counter */ 

    TimerCounter++; 

     

    /* call the old routine */ 

    oldhandler(); 

} 

/* 设置新的时钟中断处理过程 */ 

void SetTimer(void interrupt (*IntProc)(__CPPARGS)) 

{ 

    oldhandler=getvect(TIMER); 

    disable(); /* 设置新的时钟中断处理过程时，禁止所有中断 */ 

    setvect(TIMER,IntProc); 

    enable(); /* 开启中断 */ 

} 

/* 恢复原有的时钟中断处理过程 */ 

void KillTimer() 

{ 

    disable(); 

    setvect(TIMER,oldhandler); 

    enable(); 

} 

struct shape 

{ 

    int xy[8],next; 

}; 

struct shape shapes[19]= 

{ 

/*x1,y1,x2,y2,x3,y3,x4,y4 指四个小方块的相对坐标，next指此方块变形后应变为哪个小方块 

    { x1,y1,x2,y2,x3,y3,x4,y4,next}*/ 

    { 0,-2, 0,-1, 0, 0, 1, 0, 1}, 

    {-1, 0, 0, 0, 1,-1, 1, 0, 2}, 

    { 0,-2, 1,-2, 1,-1, 1, 0, 3}, 

    {-1,-1,-1, 0, 0,-1, 1,-1, 0}, 

    { 0,-2, 0,-1, 0, 0, 1,-2, 5}, 

    {-1,-1, 0,-1, 1,-1, 1, 0, 6}, 

    { 0, 0, 1,-2, 1,-1, 1, 0, 7}, 

    {-1,-1,-1, 0, 0, 0, 1, 0, 4}, 

    {-1, 0, 0,-1, 0, 0, 1, 0, 9}, 

    { 0,-2, 0,-1, 0, 0, 1,-1,10}, 

    {-1,-1, 0,-1, 1,-1, 0, 0,11}, 

    { 0,-1, 1,-2, 1,-1, 1, 0, 8}, 

    {-1, 0, 0,-1, 0, 0, 1,-1,13}, 

    { 0,-2, 0,-1, 1,-1, 1, 0,12}, 

    {-1,-1, 0,-1, 0, 0, 1, 0,15}, 

    { 0,-1, 0, 0, 1,-2, 1,-1,14}, 

    { 0,-3, 0,-2, 0,-1, 0, 0,17}, 

    {-1, 0, 0, 0, 1, 0, 2, 0,16}, 

    { 0,-1, 0, 0, 1,-1, 1, 0,18} 

}; 

int board[10][20]={0};/*定义游戏板初始化为0*/ 

char sp[]="0",le[]="0",sc[]="00000"; 

int speed,speed0,level,score; 

int sign,flag; 

int style,style1;  /*style为当前方块的种类，style1为即将输出的方块的种类*/ 

void draw_block(int x,int y,int style,int way); 

void draw_little_block(int x,int y); 

void init(); 

void initialize(); 

void speed_change(void); 

void score_change(int); 

void kill_line(int y); 

void fill_board(int x,int y, int style); 

int  change(int *i,int *j,int key); 

void renovate(void); 

void ajustment(void); 

void level_change(void); 

main() 

{ 

    int i,j,key,x0=5,y0=1; 

    randomize(); 

    while(!flag)                    /*flag为0表示重新开始游戏*/ 

    { 

        level=score=speed=0; 

        strcpy(le,"0"); 

        strcpy(sp,"0"); 

        strcpy(sc,"00000"); 

        for(i=0;i<10;i++) 

            for(j=0;j<20;j++) 

                board[i][j]=0;       /*初始化一些变量为0*/ 

            initialize();            /*初始化进入图形模式*/ 

            init();                  /*初始化游戏板记分器等*/ 

            SetTimer(newhandler);    /* 修改时钟中断 */ 

            ajustment();             /*开始游戏前调整速度和高度*/ 

             

            if(level>0) 

                level_change();       /*根据高度随机确定方块是否存在*/ 

             

            style=random(19);         /*随机确定方块种类*/ 

             

             

            while(1) 

            { 

                i=x0,j=y0; 

                 

                style1=random(19);    /*随机确定即将出现的方块种类*/ 

                setcolor(WHITE); 

                sign=1; 

                draw_block(MAPXOFT1,MAPYOFT1,style1,1); 

                /*画出即将出现的方块*/ 

                 

                for(j=y0;j<=20;j++)   /*使方块下降*/ 

                { 

                     

                     

                    if(!check_block(i,j,style)) 

                        break; 

                    draw_block(i,j,style,1); 

                     

                     

                    while(1) 

                    { 

                        if(speed0==0)                /*未按下加速键时的处理*/ 

                             

                        { 

                            if (TimerCounter>18/(speed+1)) 

                            { 

                                /* 恢复计时变量 */ 

                                TimerCounter=0; 

                                break; 

                            } 

                        } 

                        else if(TimerCounter>18/(9+1))/*按下加速键时的处理*/ 

                        { 

                            /* 恢复计时变量 */ 

                            TimerCounter=0; 

                            speed0=0; 

                            break; 

                        } 

                        if(bioskey(1)) 

                        { 

                            key=bioskey(0); 

                            if(change(&i,&j,key))/*根据按键值做调整*/ 

                            { 

                                flag=1; 

                                goto end; 

                            } 

                             

                        } 

                         

                         

                         

                    } 

                     

                    draw_block(i,j,style,0); 

                    renovate();                   /*刷新屏幕*/ 

                     

                     

                } 

                 

                if(j==y0) 

                    break; 

                j--; 

                 

                draw_block(i,j,style,1); 

                 

                fill_board(i,j,style); 

                 

                 

                sign=1; 

                draw_block(MAPXOFT1,MAPYOFT1,style1,0); 

                style=style1; 

                kill_line(j);     /*消去的函数，消去若干行并改变分数和速度*/ 

                 

                 

                while(bioskey(1))    /*清除内存中的按键*/ 

                    key=bioskey(0); 

            } 

            setcolor(CYAN); 

            settextstyle(0,0,2); 

            TimerCounter=0; 

             

            while(1) 

                if(TimerCounter>54) 

                { 

                    TimerCounter=0; 

                    break; 

                } 

                while(bioskey(1))     /*清除内存中的按键*/ 

                    key=bioskey(0); 

                outtextxy(400,340,"Game over!"); 

                outtextxy(360,360,"Enter to replay."); 

                outtextxy(360,380,"Esc to quit."); 

                while(bioskey(1)==0); 

                key=bioskey(0); 

                 

                 end:; 

                closegraph(); 

                KillTimer(); 

                if(key==ESC||flag) 

                    break; 

    } 

     

} 

void initialize()/*初始化进入图形模式*/ 

{ 

    int gdriver = VGA, gmode=VGAHI, errorcode; 

     

    /* initialize graphics mode */ 

     

    initgraph(&gdriver, &gmode, "c:\\Turboc2"); 

     

    /* read result of initialization */ 

    errorcode = graphresult(); 

     

    if (errorcode != grOk) /* an error occurred */ 

         

    { 

        printf("Graphics error: %s\n", grapherrormsg(errorcode)); 

        printf("Press any key to halt:"); 

        getch(); 

        exit(1); /* return with error code */ 

    } 

} 

void init(void)/*初始化游戏板记分器等*/ 

{ 

    int x1; 

    x1=5+MAPXOFT; 

    setcolor(GREEN); 

    circle((x1+0.5)*16,(MAPYOFT-2)*16,3*16); 

    setcolor(WHITE); 

    line((x1-0.6)*16,(MAPYOFT-3.2)*16,x1*16,(MAPYOFT-3.2)*16); 

    line((x1+1.4)*16,(MAPYOFT-3.2)*16,(x1+2.0)*16,(MAPYOFT-3.2)*16); 

    line((x1+0.5)*16,(MAPYOFT-2)*16,(x1+0.5)*16,(MAPYOFT-1.7)*16); 

    circle((x1+0.5)*16,(MAPYOFT-0.9)*16,0.3*16); 

    setcolor(CYAN); 

    line((MAPXOFT+3)*16,(MAPYOFT+21)*16,MAPXOFT*16,(MAPYOFT+23.5)*16); 

    line((MAPXOFT+5.5)*16,(MAPYOFT+21)*16,MAPXOFT*16,(MAPYOFT+26)*16); 

    line((MAPXOFT+9)*16,(MAPYOFT+21)*16,(MAPXOFT+12)*16,(MAPYOFT+23.5)*16); 

    line((MAPXOFT+6.5)*16,(MAPYOFT+21)*16,(MAPXOFT+12)*16,(MAPYOFT+26)*16); 

    setcolor(MAGENTA); 

    ellipse((MAPXOFT+1)*16,(MAPYOFT+11)*16,90,270,7*16,7*16); 

    ellipse((MAPXOFT+11)*16,(MAPYOFT+11)*16,-90,90,7*16,7*16); 

    circle((MAPXOFT+24)*16,(MAPYOFT+6)*16,6*16); 

    setcolor(WHITE); 

    rectangle((x1-0.5)*16,(MAPYOFT-2.9)*16,(x1-0.1)*16,(MAPYOFT-2.5)*16); 

    rectangle((x1+1.5)*16,(MAPYOFT-2.9)*16,(x1+1.9)*16,(MAPYOFT-2.5)*16); 

    setcolor(YELLOW); 

    rectangle(10*16,6*16,20*16,26*16); 

    settextstyle(0,0,2); 

    outtextxy(5*16,13.5*16,"level"); 

    outtextxy(20.4*16,13.5*16,"speed"); 

    outtextxy((MAPXOFT+22)*16,(MAPYOFT+3)*16,"score"); 

    settextstyle(0,0,4); 

    outtextxy(6.5*16,15*16,"0"); 

    outtextxy(21.9*16,15*16,"0"); 

    settextstyle(0,0,3); 

    outtextxy((MAPXOFT+21)*16,(MAPYOFT+6)*16,sc); 

} 

void ajustment(void)            /*开始游戏前调整速度和高度*/ 

{ 

    int key,boo=1,left=1; 

    setcolor(YELLOW); 

    settextstyle(0,0,2); 

    outtextxy(500,340,"PRESS"); 

    outtextxy(500,360,"ENTER"); 

    outtextxy(500,380," TO  "); 

    outtextxy(500,400,"START"); 

    while(1) 

    { 

        if(TimerCounter>8) 

        { 

            TimerCounter=0; 

            boo*=-1; 

            if(boo==-1) 

                setcolor(BLACK); 

            else 

                setcolor(YELLOW); 

            if(left==1) 

            { 

                line(6*16,17*16,8.5*16,17*16); 

                setcolor(BLACK); 

                line(21.4*16,17*16,23.9*16,17*16); 

            } 

             

            else 

            { 

                line(21.4*16,17*16,23.9*16,17*16); 

                setcolor(BLACK); 

                line(6*16,17*16,8.5*16,17*16); 

            } 

        } 

        if(bioskey(1)) 

        { 

            key=bioskey(0); 

            if(key==UP||key==DOWN) 

            { 

                setcolor(BLACK); 

                settextstyle(0,0,4); 

                outtextxy(6.5*16,15*16,le); 

                outtextxy(21.9*16,15*16,sp); 

            } 

            if(key==ENTER) 

                break; 

            else if(key==UP) 

            { 

                if(left==1) 

                { 

                    level--; 

                    if(level==-1) 

                        level=9; 

                } 

                else 

                { 

                    speed--; 

                    if(speed==-1) 

                        speed=9; 

                } 

            } 

            else if(key==DOWN) 

            { 

                if(left==1) 

                { 

                    level++; 

                    if(level==10) 

                        level=0; 

                     

                } 

                else 

                { 

                    speed++; 

                    if(speed==10) 

                        speed=0; 

                } 

            } 

             

            else if(key==LEFT||RIGHT) 

                left*=-1; 

            setcolor(YELLOW); 

            sp[0]=speed+'0';

            le[0]=level+'0';

            settextstyle(0,0,4); 

            outtextxy(6.5*16,15*16,le); 

            outtextxy(21.9*16,15*16,sp); 

        } 

    } 

    setcolor(YELLOW); 

    sp[0]=speed+'0';

    le[0]=level+'0';

    settextstyle(0,0,4); 

    outtextxy(6.5*16,15*16,le); 

    outtextxy(21.9*16,15*16,sp); 

    setcolor(BLACK); 

    line(21.4*16,17*16,23.9*16,17*16); 

    line(6*16,17*16,8.5*16,17*16); 

    settextstyle(0,0,2); 

    outtextxy(500,340,"PRESS"); 

    outtextxy(500,360,"ENTER"); 

    outtextxy(500,380," TO  "); 

    outtextxy(500,400,"START"); 

} 

void level_change(void)         /*根据高度随机确定方块是否存在*/ 

{ 

    int i,j; 

    setcolor(WHITE); 

    for(i=1;i<=10;i++) 

        for(j=1;j<=level;j++) 

            if(random(2)) 

            { 

                board[i-1][20-j]=1; 

                 

                draw_little_block(i+MAPXOFT,21-j+MAPYOFT); 

            } 

} 


void draw_block(int x,int y,int style,int way) 

/*此x,y为虚拟坐标，为19中形式中的一种：0~18中的一个 

x,y为10*20的方格的坐标，下标从1开始 

way为1表填充，为0表清除*/ 

{ 

    int x1=x+MAPXOFT,y1=y+MAPYOFT; 

    int i; 

    if(way==1) 

        setcolor(WHITE); 

    else 

        setcolor(BLACK); 

    for(i=0;i<=6;i+=2) 

        draw_little_block(x1+shapes[style].xy[i],y1+shapes[style].xy[i+1]); 

    if(sign==1) 

        sign=0; 

} 


void draw_little_block(int x,int y)/*此处仅在10*20的游戏版内画小方块，若在外，不画之。*/ 

{ 

    if((x>=10&&x<20&&y>=6&&y<26)||sign) 

    { 

        rectangle(x*16,y*16,x*16+16,y*16+16); 

        circle(x*16+8,y*16+8,4); 

    } 

} 

int check_block(int x,int y,int style)/*此处检查在(x,y)处放置一方块是否可以，若是，则返回1，否则返回0*/ 

{ 

    int x1=x+MAPXOFT,y1=y+MAPYOFT; 

    int x2,y2,i; 

    for(i=0;i<=6;i+=2) 

    { 

        x2=x1+shapes[style].xy[i]; 

        y2=y1+shapes[style].xy[i+1]; 

        if(x2>=10&&x2<20&&y2<26&&(y2<6||board[x2-10][y2-6]==0)) 

            continue; 

        else 

            break; 

    } 

    if(i==8) 

        return 1; 

    else 

        return 0; 

} 

void speed_change(void)        /*此为变速函数，当分数超过一级时，即加一。*/ 

{ 

    if(score>=(sp[0]-'0')*100&&(sp[0]-'0')<9)

    { 

        settextstyle(0,0,4); 

        setcolor(BLACK); 

        outtextxy(21.9*16,15*16,sp); 

        sp[0]++; 

        speed++; 

        settextstyle(0,0,4); 

        setcolor(YELLOW); 

        outtextxy(21.9*16,15*16,sp); 

    } 

} 

void score_change(int count)        /*count为消去的行数，据此来改变score的值*/ 

{ 

    int score_inc[4]={1,3,7,13}; 

    int i; 

     

    setcolor(BLACK); 

    settextstyle(0,0,3); 

    outtextxy((MAPXOFT+21)*16,(MAPYOFT+6)*16,sc);/*此处将原有的分数盖掉*/ 

     

    score+=score_inc[count-1]; 

    sc[4]+=score_inc[count-1]; 

    for(i=4;i>=1;i--) 

    { 

         

        sc[i-1]+=(sc[i]-'0')/10;

        sc[i]-=(sc[i]-'0')/10*10;

    } 

    setcolor(YELLOW); 

    settextstyle(0,0,3); 

    outtextxy((MAPXOFT+21)*16,(MAPYOFT+6)*16,sc);      /*输出新分数*/ 

} 

void kill_line(int y)/*消去的函数，消去若干行并改变分数和速度*/ 

{ 

    int count=0,i,t=1,j,k; 

    for(;y>=1&&t<=4;y--,t++) 

    { 

        for(i=1;i<=10;i++) 

            if(!board[i-1][y-1]) 

                break; 

            if(i==11) 

            { 

                count++; 

                for(k=1;k<=10;k++) 

                { 

                    setcolor(BLACK); 

                    draw_little_block(k+MAPXOFT,y+MAPYOFT); 

                } 

                for(j=y-1;j>=1;j--) 

                    for(k=1;k<=10;k++) 

                    { 

                        board[k-1][j]=board[k-1][j-1]; 

                        if(board[k-1][j]) 

                        { 

                            setcolor(BLACK); 

                            draw_little_block(k+MAPXOFT,j+MAPYOFT); 

                            setcolor(WHITE); 

                            draw_little_block(k+MAPXOFT,j+1+MAPYOFT);                    } 

                    } 

                    renovate(); 

                    delay(10000); 

                    y++; 

            } 

             

    } 

     

    if(count>0) 

    { 

         

        setcolor(CYAN); 

        settextstyle(0,0,3); 

        outtextxy(500,370,"Good!"); 

        TimerCounter=0; 

        while(1) 

            if(TimerCounter>3) 

            { 

                TimerCounter=0; 

                break; 

            } 

             

            setcolor(BLACK); 

            settextstyle(0,0,3); 

            outtextxy(500,370,"Good!"); 

             

             

            score_change(count); 

            speed_change(); 

    } 

} 

void fill_board(int x,int y, int style) /*当一方块停止时，将中相应的值改为1*/ 

{ 

     

    int x1,y1,i; 

    for(i=0;i<=6;i+=2) 

    { 

        x1=x+shapes[style].xy[i]; 

        y1=y+shapes[style].xy[i+1]; 

        board[x1-1][y1-1]=1; 

    } 

} 

int change(int *i,int *j,int key)/*控制方块移动、变形的函数*/ 

{ 

    int key1; 

    if(key==UP&&check_block(*i,*j,shapes[style].next)) 

    { 

        draw_block(*i,*j,style,0); 

        style=shapes[style].next; 

        draw_block(*i,*j,style,1); 

    } 

    else if(key==LEFT&&check_block(*i-1,*j,style)) 

    { 

        draw_block(*i,*j,style,0); 

        (*i)--; 

        draw_block(*i,*j,style,1); 

    } 

    else if(key==RIGHT&&check_block(*i+1,*j,style)) 

    { 

        draw_block(*i,*j,style,0); 

        (*i)++; 

        draw_block(*i,*j,style,1); 

    } 

    else if(key==DOWN&&check_block(*i,*j+1,style)) 

    { 

        draw_block(*i,*j,style,0); 

        (*j)++; 

        draw_block(*i,*j,style,1); 

        speed0=9; 

    } 

    else if(key==ENTER) 

    { 

        while(1) 

        { 

             

            if(bioskey(1)) 

                key1=bioskey(0); 

            if(key1==ENTER) 

                break; 

        } 

    } 

    else if(key==ESC) 

        return 1; 

    else if(key==21040) 

          { 

        sign=1; 

        draw_block(MAPXOFT1,MAPYOFT1,style1,0); 

        style1=16; 

        sign=1; 

        draw_block(MAPXOFT1,MAPYOFT1,style1,1); 

    } 

    return 0; 

} 

void renovate(void)    /*刷新屏幕*/ 

{ 

    int i,j; 

    setcolor(WHITE); 

    for(i=1;i<=10;i++) 

        for(j=1;j<=20;j++) 

            if(board[i-1][j-1]==1) 

                draw_little_block(i+MAPXOFT,j+MAPYOFT); 

             

            setcolor(YELLOW); 

            rectangle(10*16,6*16,20*16,26*16); 
} 
回答者： wangtk1982 | 十一级 | 2008-9-13 09:15 
TC下面的 

/************************************ 
* Desc: 俄罗斯方块游戏 
* By: hoodlum1980 
* Email: jinfd@126.com 
* Date: 2008.03.12 22：30 
************************************/ 
#include <stdio.h> 
#include <bios.h> 
#include <dos.h> 
#include <graphics.h> 
#include <string.h> 
#include <stdlib.h> 
#define true 1 
#define false 0 
#define BoardWidth 12 
#define BoardHeight 23 
#define _INNER_HELPER /*inner helper method */ 
/*Scan Codes Define*/ 
enum KEYCODES 
{ 
K_ESC =0x011b, 
K_UP =0x4800, /* upward arrow */ 
K_LEFT =0x4b00, 
K_DOWN =0x5000, 
K_RIGHT =0x4d00, 
K_SPACE =0x3920, 
K_P =0x1970 
}; 

/* the data structure of the block */ 
typedef struct tagBlock 
{ 
char c[4][4]; /* cell fill info array, 0-empty, 1-filled */ 
int x; /* block position cx [ 0,BoardWidht -1] */ 
int y; /* block position cy [-4,BoardHeight-1] */ 
char color; /* block color */ 
char size; /* block max size in width or height */ 
char name; /* block name (the block's shape) */ 
} Block; 

/* game's global info */ 
int FrameTime= 1300; 
int CellSize= 18; 
int BoardLeft= 30; 
int BoardTop= 30; 

/* next block grid */ 
int NBBoardLeft= 300; 
int NBBoardTop= 30; 
int NBCellSize= 10; 

/* score board position */ 
int ScoreBoardLeft= 300; 
int ScoreBoardTop=100; 
int ScoreBoardWidth=200; 
int ScoreBoardHeight=35; 
int ScoreColor=LIGHTCYAN; 

/* infor text postion */ 
int InfoLeft=300; 
int InfoTop=200; 
int InfoColor=YELLOW; 

int BorderColor=DARKGRAY; 
int BkGndColor=BLACK; 
int GameRunning=true; 
int TopLine=BoardHeight-1; /* top empty line */ 
int TotalScore=100; 
char info_score[20]; 
char info_help[255]; 
char info_common[255]; 

/* our board, Board[x][y][0]-isFilled, Board[x][y][1]-fillColor */ 
unsigned char Board[BoardWidth][BoardHeight][2]; 
char BufferCells[4][4]; /* used to judge if can rotate block */ 
Block curBlock; /* current moving block */ 
Block nextBlock; /* next Block to appear */ 

/* function list */ 
int GetKeyCode(); 
int CanMove(int dx,int dy); 
int CanRotate(); 
int RotateBlock(Block *block); 
int MoveBlock(Block *block,int dx,int dy); 
void DrawBlock(Block *block,int,int,int); 
void EraseBlock(Block *block,int,int,int); 
void DisplayScore(); 
void DisplayInfo(char* text); 
void GenerateBlock(Block *block); 
void NextBlock(); 
void InitGame(); 
int PauseGame(); 
void QuitGame(); 

/*Get Key Code */ 
int _INNER_HELPER GetKeyCode() 
{ 
int key=0; 
if(bioskey(1)) 
{ 
key=bioskey(0); 
} 
return key; 
} 

/* display text! */ 
void _INNER_HELPER DisplayInfo(char *text) 
{ 
setcolor(BkGndColor); 
outtextxy(InfoLeft,InfoTop,info_common); 
strcpy(info_common,text); 
setcolor(InfoColor); 
outtextxy(InfoLeft,InfoTop,info_common); 
} 

/* create a new block by key number, 
* the block anchor to the top-left corner of 4*4 cells 
*/ 
void _INNER_HELPER GenerateBlock(Block *block) 
{ 
int key=(random(13)*random(17)+random(1000)+random(3000))%7; 
block->size=3;/* because most blocks' size=3 */ 
memset(block->c,0,16); 
switch(key) 
{ 
case 0: 
block->name='T'; 
block->color=RED; 
block->c[1][0]=1; 
block->c[1][1]=1, block->c[2][1]=1; 
block->c[1][2]=1; 
break; 
case 1: 
block->name='L'; 
block->color=YELLOW; 
block->c[1][0]=1; 
block->c[1][1]=1; 
block->c[1][2]=1, block->c[2][2]=1; 
break; 
case 2: 
block->name='J'; 
block->color=LIGHTGRAY; 
block->c[1][0]=1; 
block->c[1][1]=1; 
block->c[1][2]=1, block->c[0][2]=1; 
break; 
case 3: 
block->name='z'; 
block->color=CYAN; 
block->c[0][0]=1, block->c[1][0]=1; 
block->c[1][1]=1, block->c[2][1]=1; 
break; 
case 4: 
block->name='5'; 
block->color=LIGHTBLUE; 
block->c[1][0]=1, block->c[2][0]=1; 
block->c[0][1]=1, block->c[1][1]=1; 
break; 
case 5: 
block->name='o'; 
block->color=BLUE; 
block->size=2; 
block->c[0][0]=1, block->c[1][0]=1; 
block->c[0][1]=1, block->c[1][1]=1; 
break; 
case 6: 
block->name='I'; 
block->color=GREEN; 
block->size=4; 
block->c[1][0]=1; 
block->c[1][1]=1; 
block->c[1][2]=1; 
block->c[1][3]=1; 
break; 
} 
} 

/* get next block! */ 
void NextBlock() 
{ 
/* copy the nextBlock to curBlock */ 
curBlock.size=nextBlock.size; 
curBlock.color=nextBlock.color; 
curBlock.x=(BoardWidth-4)/2; 
curBlock.y=-curBlock.size; 
memcpy(curBlock.c,nextBlock.c,16); 
/* generate nextBlock and show it */ 
EraseBlock(&nextBlock,NBBoardLeft,NBBoardTop,NBCellSize); 
GenerateBlock(&nextBlock); 
nextBlock.x=1,nextBlock.y=0; 
DrawBlock(&nextBlock,NBBoardLeft,NBBoardTop,NBCellSize); 
} 

/* rotate the block, update the block struct data */ 
int _INNER_HELPER RotateCells(char c[4][4],char blockSize) 
{ 
char temp,i,j; 
switch(blockSize) 
{ 
case 3: 
temp=c[0][0]; 
c[0][0]=c[2][0], c[2][0]=c[2][2], c[2][2]=c[0][2], c[0][2]=temp; 
temp=c[0][1]; 
c[0][1]=c[1][0], c[1][0]=c[2][1], c[2][1]=c[1][2], c[1][2]=temp; 
break; 
case 4: /* only 'I' block arived here! */ 
c[1][0]=1-c[1][0], c[1][2]=1-c[1][2], c[1][3]=1-c[1][3]; 
c[0][1]=1-c[0][1], c[2][1]=1-c[2][1], c[3][1]=1-c[3][1]; 
break; 
} 
} 

/* judge if the block can move toward the direction */ 
int CanMove(int dx,int dy) 
{ 
int i,j,tempX,tempY; 
for(i=0;i<curBlock.size;i++) 
{ 
for(j=0;j<curBlock.size;j++) 
{ 
if(curBlock.c[i][j]) 
{ 
/* cannot move leftward or rightward */ 
tempX = curBlock.x + i + dx; 
if(tempX<0 || tempX>(BoardWidth-1)) return false; /* make sure x is valid! */ 
/* cannot move downward */ 
tempY = curBlock.y + j + dy; 
if(tempY>(BoardHeight-1)) return false; /* y is only checked lower bound, maybe negative!!!! */ 
/* the cell already filled, we must check Y's upper bound before check cell ! */ 
if(tempY>=0 && Board[tempX][tempY][0]) return false; 
} 
} 
} 
return true; 
} 

/* judge if the block can rotate */ 
int CanRotate() 
{ 
int i,j,tempX,tempY; 
/* update buffer */ 
memcpy(BufferCells, curBlock.c, 16); 
RotateCells(BufferCells,curBlock.size); 
for(i=0;i<curBlock.size;i++) 
{ 
for(j=0;j<curBlock.size;j++) 
{ 
if(BufferCells[i][j]) 
{ 
tempX=curBlock.x+i; 
tempY=curBlock.y+j; 
if(tempX<0 || tempX>(BoardWidth-1)) 
return false; 
if(tempY>(BoardHeight-1)) 
return false; 
if(tempY>=0 && Board[tempX][tempY][0]) 
return false; 
} 
} 
} 
return true; 
} 

/* draw the block */ 
void _INNER_HELPER DrawBlock(Block *block,int bdLeft,int bdTop,int cellSize) 
{ 
int i,j; 
setfillstyle(SOLID_FILL,block->color); 
for(i=0;i<block->size;i++) 
{ 
for(j=0;j<block->size;j++) 
{ 
if(block->c[i][j] && (block->y+j)>=0) 
{ 
floodfill( 
bdLeft+cellSize*(i+block->x)+cellSize/2, 
bdTop+cellSize*(j+block->y)+cellSize/2, 
BorderColor); 
} 
} 
} 
} 

/* Rotate the block, if success, return true */ 
int RotateBlock(Block *block) 
{ 
char temp,i,j; 
int b_success; 
if(block->size==2) 
return true; 
if(( b_success=CanRotate())) 
{ 
EraseBlock(block,BoardLeft,BoardTop,CellSize); 
memcpy(curBlock.c,BufferCells,16); 
DrawBlock(block,BoardLeft,BoardTop,CellSize); 
} 
return b_success; 
} 


/* erase a block, only fill the filled cell with background color */ 
void _INNER_HELPER EraseBlock(Block *block,int bdLeft,int bdTop,int cellSize) 
{ 
int i,j; 
setfillstyle(SOLID_FILL,BkGndColor); 
for(i=0;i<block->size;i++) 
{ 
for(j=0;j<block->size;j++) 
{ 
if(block->c[i][j] && (block->y+j>=0)) 
{ 
floodfill( 
bdLeft+cellSize*(i+block->x)+cellSize/2, 
bdTop+cellSize*(j+block->y)+cellSize/2, 
BorderColor); 
} 
} 
} 
} 

/* move by the direction if can, donothing if cannot 
* return value: true - success, false - cannot move toward this direction 
*/ 
int MoveBlock(Block *block,int dx,int dy) 
{ 
int b_canmove=CanMove(dx,dy); 
if(b_canmove) 
{ 
EraseBlock(block,BoardLeft,BoardTop,CellSize); 
curBlock.x+=dx; 
curBlock.y+=dy; 
DrawBlock(block,BoardLeft,BoardTop,CellSize); 
} 
return b_canmove; 
} 

/* drop the block to the bottom! */ 
int DropBlock(Block *block) 
{ 
EraseBlock(block,BoardLeft,BoardTop,CellSize); 
while(CanMove(0,1)) 
{ 
curBlock.y++; 
} 
DrawBlock(block,BoardLeft,BoardTop,CellSize); 
return 0;/* return value is assign to the block's alive */ 
} 


/* init the graphics mode, draw the board grid */ 
void InitGame() 
{ 
int i,j,gdriver=DETECT,gmode; 
struct time sysTime; 
/* draw board cells */ 
memset(Board,0,BoardWidth*BoardHeight*2); 
memset(nextBlock.c,0,16); 
strcpy(info_help,"P: Pause Game. --by hoodlum1980"); 
initgraph(&gdriver,&gmode,""); 
setcolor(BorderColor); 
for(i=0;i<=BoardWidth;i++) 
{ 
line(BoardLeft+i*CellSize, BoardTop, BoardLeft+i*CellSize, BoardTop+ BoardHeight*CellSize); 
} 
for(i=0;i<=BoardHeight;i++) 
{ 
line(BoardLeft, BoardTop+i*CellSize, BoardLeft+BoardWidth*CellSize, BoardTop+ i*CellSize); 
} 
/* draw board outer border rect */ 
rectangle(BoardLeft-CellSize/4, BoardTop-CellSize/4, 
BoardLeft+BoardWidth*CellSize+CellSize/4, 
BoardTop+BoardHeight*CellSize+CellSize/4); 

/* draw next block grids */ 
for(i=0;i<=4;i++) 
{ 
line(NBBoardLeft+i*NBCellSize, NBBoardTop, NBBoardLeft+i*NBCellSize, NBBoardTop+4*NBCellSize); 
line(NBBoardLeft, NBBoardTop+i*NBCellSize, NBBoardLeft+4*NBCellSize, NBBoardTop+ i*NBCellSize); 
} 

/* draw score rect */ 
rectangle(ScoreBoardLeft,ScoreBoardTop,ScoreBoardLeft+ScoreBoardWidth,ScoreBoardTop+ScoreBoardHeight); 
DisplayScore(); 

/* set new seed! */ 
gettime(&sysTime); 
srand(sysTime.ti_hour*3600+sysTime.ti_min*60+sysTime.ti_sec); 

GenerateBlock(&nextBlock); 
NextBlock(); /* create first block */ 
setcolor(DARKGRAY); 
outtextxy(InfoLeft,InfoTop+20,"Up -rotate Space-drop"); 
outtextxy(InfoLeft,InfoTop+35,"Left-left Right-right"); 
outtextxy(InfoLeft,InfoTop+50,"Esc -exit"); 
DisplayInfo(info_help); 
} 

/* set the isFilled and fillcolor data to the board */ 
void _INNER_HELPER FillBoardData() 
{ 
int i,j; 
for(i=0;i<curBlock.size;i++) 
{ 
for(j=0;j<curBlock.size;j++) 
{ 
if(curBlock.c[i][j] && (curBlock.y+j)>=0) 
{ 
Board[curBlock.x+i][curBlock.y+j][0]=1; 
Board[curBlock.x+i][curBlock.y+j][1]=curBlock.color; 
} 
} 
} 
} 

/* draw one line of the board */ 
void _INNER_HELPER PaintBoard() 
{ 
int i,j,fillcolor; 
for(j=max((TopLine-4),0);j<BoardHeight;j++) 
{ 
for(i=0;i<BoardWidth;i++) 
{ 
fillcolor=Board[i][j][0]? Board[i][j][1]:BkGndColor; 
setfillstyle(SOLID_FILL,fillcolor); 
floodfill(BoardLeft+i*CellSize+CellSize/2,BoardTop+j*CellSize+CellSize/2,BorderColor); 
} 
} 
} 

/* check if one line if filled full and increase the totalScore! */ 
void _INNER_HELPER CheckBoard() 
{ 
int i,j,k,score=10,sum=0,topy,lines=0; 
/* we find the top empty line! */ 
j=topy=BoardHeight-1; 
do 
{ 
sum=0; 
for(i=0;i< BoardWidth; i++) 
{ 
sum+=Board[i][topy][0]; 
} 
topy--; 
} while(sum>0 && topy>0); 

/* remove the full filled line (max remove lines count = 4) */ 
do 
{ 
sum=0; 
for(i=0;i< BoardWidth; i++) 
sum+=Board[i][j][0]; 

if(sum==BoardWidth)/* we find this line is full filled, remove it! */ 
{ 
/* move the cells data down one line */ 
for(k=j; k > topy;k--) 
{ 
for(i=0;i<BoardWidth;i++) 
{ 
Board[i][k][0]=Board[i][k-1][0]; 
Board[i][k][1]=Board[i][k-1][1]; 
} 
} 
/*make the top line empty! */ 
for(i=0;i<BoardWidth;i++) 
{ 
Board[i][topy][0]=0; 
Board[i][topy][1]=0; 
} 
topy++; /* move the topline downward one line! */ 
lines++; /* lines <=4 */ 
TotalScore+=score; 
score*=2; /* adding: 10, 30, 70, 150 */ 
} 
else 
j--; 
} while(sum>0 && j>topy && lines<4); 
/* speed up the game when score is high, minimum is 400 */ 
FrameTime=max(1200-100*(TotalScore/200), 400); 
TopLine=topy;/* update the top line */ 
/* if no lines remove, only add 1: */ 
if(lines==0) 
TotalScore++; 
} 

/* display the score */ 
void _INNER_HELPER DisplayScore() 
{ 
setcolor(BkGndColor); 
outtextxy(ScoreBoardLeft+5,ScoreBoardTop+5,info_score); 
setcolor(ScoreColor); 
sprintf(info_score,"Score: %d",TotalScore); 
outtextxy(ScoreBoardLeft+5,ScoreBoardTop+5,info_score); 
} 

/* we call this function when a block is inactive. */ 
void UpdateBoard() 
{ 
FillBoardData(); 
CheckBoard(); 
PaintBoard(); 
DisplayScore(); 
} 

/* pause the game, and timer handler stop move down the block! */ 
int PauseGame() 
{ 
int key=0; 
DisplayInfo("Press P to Start or Resume!"); 
while(key!=K_P && key!=K_ESC) 
{ 
while(!(key=GetKeyCode())){} 
} 
DisplayInfo(info_help); 
return key; 
} 

/* quit the game and do cleaning work. */ 
void QuitGame() 
{ 
closegraph(); 
} 

/* the entry point function. */ 
void main() 
{ 
int i,flag=1,j,key=0,tick=0; 
InitGame(); 
if(PauseGame()==K_ESC) 
goto GameOver; 
/* wait until a key pressed */ 
while(key!=K_ESC) 
{ 
/* wait until a key pressed */ 
while(!(key=GetKeyCode())) 
{ 
tick++; 
if(tick>=FrameTime) 
{ 
/* our block has dead! (can't move down), we get next block */ 
if(!MoveBlock(&curBlock,0,1)) 
{ 
UpdateBoard(); 
NextBlock(); 
if(!CanMove(0,1)) 
goto GameOver; 
} 
tick=0; 
} 
delay(100); 
} 
switch(key) 
{ 
case K_LEFT: 
MoveBlock(&curBlock,-1,0); 
break; 
case K_RIGHT: 
MoveBlock(&curBlock,1,0); 
break; 
case K_DOWN: 
MoveBlock(&curBlock,0,1); 
break; 
case K_UP: 
RotateBlock(&curBlock); 
break; 
case K_SPACE: 
DropBlock(&curBlock); 
break; 
case K_P: 
PauseGame(); 
break; 
} 
} 
GameOver: 
DisplayInfo("GAME OVER! Press any key to exit!"); 
getch(); /* wait the user Press any key. */ 
QuitGame(); 
回答者： ωǒ是狼 | 二级 | 2008-9-13 09:16 
#include <Conio.h>
#include <stdlib.h>
#include <graphics.h>
#include <bios.h>
#include <time.h>
#include <stdio.h>

#define WAY "f:\\hightscort.txt"

#define closegr closegraph

#define DIFFTIME 5      /**** 下落间隔时间 ****/

#define FLOOR1 10       /**** 消层的得分 ****/
#define FLOOR2 20
#define FLOOR3 50
#define FLOOR4 100

#define LEFT  19200     /*****bioskey(0)函数中各键对应的值****/
#define RIGHT 19712
#define UP 18432
#define DOWN  20480
#define ESC   283
#define SPACE 14624

#define BLACK     0     /***** 定义颜色 ****/
#define YELLOW    222
#define COLOR     888
#define BACKCOLOR 33

#define LONG  12       /**** 定义游戏区的长和高*****/
#define HIGHT 21
#define KUAN  20       /**** 定义方块的宽 **********/

int  a[LONG+2][HIGHT+4];
int  scort;
main()
{
  FILE *fp;
  void initgr(void);
  void getready(void);
  void print(int x,int y,int color,int patter);
  int  canmove(int x,int y,int shade);
  int  printbar(int x,int y,int shade,int color);
  void deletefloor(int y);
  int i,shade,nextshade,hightscort;
  char c,str[7];
  initgr();   /***** BGI初始化 *-*****/

  if((fp=fopen(WAY,"rb+"))==NULL)/******最高分初始化********/
    {
      hightscort=0;
      fp=fopen(WAY,"wb");
    }
  else
    hightscort=getw(fp);

  while(1)
  {
    getready();
    itoa(hightscort,str,10);
    outtextxy(30,20,"the hight:");
    outtextxy(150,20,str);
    randomize();
    nextshade=rand()%28;
    printbar(LONG+2,0,nextshade,COLOR+nextshade/4);
    getch();
    randomize();
    while(1)
    {
      shade=nextshade;
      nextshade=rand()%28;
      printbar(LONG+2,0,shade,BACKCOLOR);
      printbar(LONG+2,0,nextshade,COLOR+nextshade/4);
      if(!canmove(LONG/2-1,0,shade))break;
      printbar(LONG/2-1,0,shade,COLOR+shade/4);
      i=down(shade);if(!i)break;
      deletefloor(i);
    }
    setfillstyle(SOLID_FILL,BACKCOLOR);
    bar(0,10,200,17);
    outtextxy(30,10,"Exit?input 'y' to exit");
    if(scort>hightscort)
    {
      hightscort=scort;
      outtextxy(30,30,"Congratulation!");
      outtextxy(30,40,"you are the best!");
    }
    c=getch();
    if(c=='y'||c=='Y')break;
  }
  rewind(fp);
  fwrite(&hightscort,2,1,fp);
  fclose(fp);
  closegr();     /***** 恢复TEXT屏幕模式 ******/
}


/******************** initgr函数BGI初始化************************************/
void initgr(void)
{int gd=DETECT,gm=0;
registerbgidriver(EGAVGA_driver);
initgraph(&gd,&gm,"");
}


/******************** pirnt函数 ***************************************/
void print(int x,int y,int color,int patter)
{
  x*=KUAN,y*=KUAN;
  setfillstyle(patter,color);
  bar(180+x,20+y,199+x,40+y);
}


/******************** getready函数 ***********************************/
void getready()
{
void print(int x,int y,int color,int pater);
int i,j;
scort=0;
setfillstyle(SOLID_FILL,BACKCOLOR);
bar(0,0,640,480);
outtextxy(30,10,"your scort is  0");
for(i=1;i<=LONG;i++)
    for(j=0;j<HIGHT;j++)
      a[i][j]=BLACK;
  for(j=0;j<=HIGHT;j++)a[0][j]=YELLOW;
  for(j=0;j<=HIGHT;j++)a[LONG+1][j]=YELLOW;
  for(i=1;i<=LONG;i++)a[i][HIGHT]=YELLOW;
  for(i=0;i<=LONG+1;i++)
    for(j=0;j<=HIGHT;j++)
      print(i,j,a[i][j],INTERLEAVE_FILL);
}
/*******canmoveprintbar函数认定当收到移动信号时是否能移动到下一位置*************/
canmove(int x,int y,int shade)
{
  switch(shade)
  {
    case(0):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+2][y+2]==BLACK)return(1);else return(0);
    case(1):if(a[x+2][y]==BLACK&&a[x+3][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(2):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+2][y+2]==BLACK)return(1);else return(0);
    case(3):if(a[x+2][y]==BLACK&&a[x+3][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);

    case(4):if(a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(5):if(a[x][y]==BLACK&&a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(6):if(a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(7):if(a[x][y]==BLACK&&a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);

    case(8):if(a[x+1][y]==BLACK&&a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(9):if(a[x+1][y]==BLACK&&a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(10):if(a[x+1][y]==BLACK&&a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(11):if(a[x+1][y]==BLACK&&a[x+2][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);

    case(12):if(a[x+1][y]==BLACK&&a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);
    case(13):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(14):if(a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(15):if(a[x+1][y]==BLACK&&a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);

    case(16):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+1][y+2]==BLACK&&a[x+2][y+2]==BLACK)return(1);else return(0);
    case(17):if(a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+3][y+1]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(18):if(a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+2][y+2]==BLACK&&a[x+2][y+3]==BLACK)return(1);else return(0);
    case(19):if(a[x+2][y+1]==BLACK&&a[x][y+2]==BLACK&&a[x+1][y+2]==BLACK&&a[x+2][y+2]==BLACK)return(1);else return(0);

    case(20):if(a[x+2][y]==BLACK&&a[x+2][y+1]==BLACK&&a[x+2][y+2]==BLACK&&a[x+1][y+2]==BLACK)return(1);else return(0);
    case(21):if(a[x+1][y+2]==BLACK&&a[x+2][y+2]==BLACK&&a[x+3][y+2]==BLACK&&a[x+1][y+1]==BLACK)return(1);else return(0);
    case(22):if(a[x+2][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+1][y+2]==BLACK&&a[x+1][y+3]==BLACK)return(1);else return(0);
    case(23):if(a[x+2][y+2]==BLACK&&a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK)return(1);else return(0);

    case(24):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+1][y+2]==BLACK&&a[x+1][y+3]==BLACK)return(1);else return(0);
    case(25):if(a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+3][y+1]==BLACK)return(1);else return(0);
    case(26):if(a[x+1][y]==BLACK&&a[x+1][y+1]==BLACK&&a[x+1][y+2]==BLACK&&a[x+1][y+3]==BLACK)return(1);else return(0);
    case(27):if(a[x][y+1]==BLACK&&a[x+1][y+1]==BLACK&&a[x+2][y+1]==BLACK&&a[x+3][y+1]==BLACK)return(1);else return(0);
  }
}


/******************  printbar函数打出给定的坐标与形状***********************/
printbar(int x,int y,int shade,int color)
{
  void print(int,int,int,int);
  switch(shade)
  {
    case(0):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);break;
    case(1):print(x+2,y,color,SOLID_FILL);print(x+3,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(2):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);break;
    case(3 ):print(x+2,y,color,SOLID_FILL);print(x+3,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;

    case(4):print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(5):print(x,y,color,SOLID_FILL);print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(6):print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(7):print(x,y,color,SOLID_FILL);print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;

    case(8):print(x+1,y,color,SOLID_FILL);print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(9):print(x+1,y,color,SOLID_FILL);print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(10L):print(x+1,y,color,SOLID_FILL);print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(11):print(x+1,y,color,SOLID_FILL);print(x+2,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;

    case(12):print(x+1,y,color,SOLID_FILL);print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;
    case(13):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(14):print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(15):print(x+1,y,color,SOLID_FILL);print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;

    case(16):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);break;
    case(17):print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+3,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(18):print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);print(x+2,y+3,color,SOLID_FILL);break;
    case(19):print(x+2,y+1,color,SOLID_FILL);print(x,y+2,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);break;

    case(20):print(x+2,y,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);break;
    case(21):print(x+1,y+2,color,SOLID_FILL);print(x+2,y+2,color,SOLID_FILL);print(x+3,y+2,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);break;
    case(22):print(x+2,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);print(x+1,y+3,color,SOLID_FILL);break;
    case(23):print(x+2,y+2,color,SOLID_FILL);print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);break;

    case(24):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);print(x+1,y+3,color,SOLID_FILL);break;
    case(25):print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+3,y+1,color,SOLID_FILL);break;
    case(26):print(x+1,y,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+1,y+2,color,SOLID_FILL);print(x+1,y+3,color,SOLID_FILL);break;
    case(27):print(x,y+1,color,SOLID_FILL);print(x+1,y+1,color,SOLID_FILL);print(x+2,y+1,color,SOLID_FILL);print(x+3,y+1,color,SOLID_FILL);break;
  }
}


/*****************   down函数方块下落*****************************/
int down(int shade)
{
  int x=LONG/2-1,y=0,sign;
  int color=COLOR+shade/4;
  void redefinea(int x,int y,int shade,int color);
  long starttime=biostime(0, 0L),move;
  while(1)
  {
    move=(biostime(0,0L)-starttime)%DIFFTIME;
    if(bioskey(1))
    {
      switch(bioskey(0))
      {
        case  LEFT: if(canmove(x-1,y,shade)) printbar(x--,y,shade,BLACK);break;
        case RIGHT: if(canmove(x+1,y,shade)) printbar(x++,y,shade,BLACK);break;
        case    UP: if(canmove(x,y,shade%4==3?shade-3:shade+1)){printbar(x,y,shade,BLACK);shade%4==3?shade=shade-3:++shade;}break;
        case  DOWN: if(canmove(x,y+1,shade)) printbar(x,y++,shade,BLACK);break;
        case SPACE: if(canmove(x,y+1,shade)) printbar(x,y++,shade,BLACK);{while(canmove(x,y+1,shade))y++;sign=0;move=0;}break;
        case   ESC: return(0);
      }
    }
    printbar(x,y,shade,color);
/******************   控制时间，过DIFFTIME下降一格  *****************/
    if(move!=0)sign=0;
    if(move==0&&sign==0)
    {
      sign=1;
      if(canmove(x,y+1,shade))
      {
        printbar(x,y,shade,BLACK);
        y++;
      }
      else
      {
        redefinebar(x,y,shade,color);
        return(y);
      }
    }
  }
}


/***************  redefinebar函数当有方块落下修改数组********************************/
redefinebar(int x,int y,int shade,int color)
{
  switch(shade)
  {
    case(0):a[x+1][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+2][y+2]=color;break;
    case(1):a[x+2][y]=color;a[x+3][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(2):a[x+1][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+2][y+2]=color;break;
    case(3):a[x+2][y]=color;a[x+3][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;

    case(4):a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+1][y+2]=color;break;
    case(5):a[x][y]=color;a[x+1][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(6):a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+1][y+2]=color;break;
    case(7):a[x][y]=color;a[x+1][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;

    case(8):a[x+1][y]=color;a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(9):a[x+1][y]=color;a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(10):a[x+1][y]=color;a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(11):a[x+1][y]=color;a[x+2][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;

    case(12):a[x+1][y]=color;a[x][y+1]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;
    case(13):a[x+1][y]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+1][y+2]=color;break;
    case(14):a[x][y+1]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+1][y+2]=color;break;
    case(15):a[x+1][y]=color;a[x][y+1]=color;a[x+1][y+1]=color;a[x+1][y+2]=color;break;

    case(16):a[x+1][y]=color;a[x+1][y+1]=color;a[x+1][y+2]=color;a[x+2][y+2]=color;break;
    case(17):a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+3][y+1]=color;a[x+1][y+2]=color;break;
    case(18):a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+2][y+2]=color;a[x+2][y+3]=color;break;
    case(19):a[x+2][y+1]=color;a[x][y+2]=color;a[x+1][y+2]=color;a[x+2][y+2]=color;break;

    case(20):a[x+2][y]=color;a[x+2][y+1]=color;a[x+2][y+2]=color;a[x+1][y+2]=color;break;
    case(21):a[x+1][y+2]=color;a[x+2][y+2]=color;a[x+3][y+2]=color;a[x+1][y+1]=color;break;
    case(22):a[x+2][y+1]=color;a[x+1][y+1]=color;a[x+1][y+2]=color;a[x+1][y+3]=color;break;
    case(23):a[x+2][y+2]=color;a[x][y+1]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;break;

    case(24):a[x+1][y]=color;a[x+1][y+1]=color;a[x+1][y+2]=color;a[x+1][y+3]=color;break;
    case(25):a[x][y+1]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+3][y+1]=color;break;
    case(26):a[x+1][y]=color;a[x+1][y+1]=color;a[x+1][y+2]=color;a[x+1][y+3]=color;break;
    case(27):a[x][y+1]=color;a[x+1][y+1]=color;a[x+2][y+1]=color;a[x+3][y+1]=color;break;
  }
}


/**************  deletefloor函数控制消层  ********************************/
void deletefloor(int y)
{
  char string[7];
  int x,i,j,s,floor=0,sign=0;
  for(;sign<=4;sign++,y++)
  {
    s=0;
    for(x=1;x<=LONG;x++)
      if(a[x][y]!=BLACK&&a[x][y]!=YELLOW)s++;
    if(s==LONG)
    {
      floor++;
      for(j=y;j>=1;j--)
        for(i=1;i<=LONG;i++)
          a[i][j]=a[i][j-1];
      for(i=1;i<=LONG;i++)a[i][0]=BLACK;
    }
  }
  for(i=1;i<=floor;i++)
  {
    sound(567);                        /****** 控制音乐，如果那算音乐******/
    delay(5000);
    nosound();
    sound(659);
    delay(10000);
    nosound();
  }

  switch(floor)                       /******* 控制得分 **************/
  {
    case(0):break;
    case(1):scort+=FLOOR1;break;
    case(2):scort+=FLOOR2;break;
    case(3):scort+=FLOOR3;break;
    case(4):scort+=FLOOR4;break;
  }
  itoa(scort,string,10);
  setfillstyle(SOLID_FILL,BACKCOLOR);
  bar(150,10,200,17);
  outtextxy(150,10,string);
  for(i=1;i<=LONG;i++)
    for(j=0;j<HIGHT;j++)
  {
    print(i,j,a[i][j],SOLID_FILL);
  }
}

有注释的，希望能帮到你 
回答者： 小鱼冬眠中 | 二级 | 2008-9-13 10:50 
你好,我是做编程的,你去中国IT实验室下载吧,那的东西足够你学了,别说是这个游戏,别的游戏的都有.
QQ:176535590 
回答者： 80443865 | 三级 | 2008-9-13 12:01 
这个网上有很多了吧
最好自己看看C的图形编程
不然很多图形方便的函数不太容易懂 
回答者： foxpeter | 一级 | 2008-9-13 15:40 
网上随便都有 
回答者： pz0513 | 五级 | 2008-9-15 14:00 
#include <stdio.h> 
#include <dos.h> 
#include <conio.h> 
#include <graphics.h> 
#include <stdlib.h> 

#define __CPPARGS
#define MINBOXSIZE 15 /* 最小方块的尺寸 */ 
#define BGCOLOR 7 /* 背景着色 */ 
#define GX 200 
#define GY 10 
#define SJNUM 10000 /* 每当玩家打到一万分等级加一级*/ 

/* 按键码*/ 
#define VK_LEFT 0x4b00 
#define VK_RIGHT 0x4d00 
#define VK_DOWN 0x5000 
#define VK_UP 0x4800 
#define VK_HOME 0x4700 
#define VK_END 0x4f00 
#define VK_SPACE 0x3920 
#define VK_ESC 0x011b 
#define VK_ENTER 0x1c0d 

/* 定义俄罗斯方块的方向（我定义他为4种）*/ 
#define F_DONG 0 
#define F_NAN 1 
#define F_XI 2 
#define F_BEI 3 

#define NEXTCOL 20 /* 要出的下一个方块的纵坐标*/ 
#define NEXTROW 12 /* 要出的下一个方块的横从标*/ 
#define MAXROW 14 /* 游戏屏幕大小*/ 
#define MAXCOL 20 
#define SCCOL 100 /*游戏屏幕大显示器上的相对位置*/ 
#define SCROW 60 


int gril[22][16]; /* 游戏屏幕坐标*/ 
int col=1,row=7; /* 当前方块的横纵坐标*/ 
int boxfx=0,boxgs=0; /* 当前寺块的形壮和方向*/ 
int nextboxfx=0,nextboxgs=0,maxcol=22;/*下一个方块的形壮和方向*/ 
int minboxcolor=6,nextminboxcolor=6; 
int num=0; /*游戏分*/ 
int dj=0,gamedj[10]={18,16,14,12,10,8,6,4,2,1};/* 游戏等级*/ 
/* 以下我用了一个3维数组来纪录方块的最初形状和方向*/ 
int boxstr[7][4][16]={{ 
{1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}, 
{0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0}, 
{1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}, 
{0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0}}, 
{ 
{0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0}, 
{1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0}, 
{0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0}, 
{1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0}}, 
{ 
{1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0}, 
{1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0}, 
{1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0}, 
{0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0}}, 
{ 
{1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0}, 
{1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0}, 
{0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0}, 
{1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0}}, 
{ 
{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}, 
{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0}, 
{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}, 
{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0}}, 
{ 
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0}, 
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0}, 
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0}, 
{1,1,0,0,1,1,0,0,0,0,0,0.0,0,0,0}}, 
{ 
{0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0}, 
{1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0}, 
{0,1,0,0,1,1,1,0,0,0,0,0.0,0,0,0}, 
{0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0}} 

}; 

/* 随机得到当前方块和下一个方块的形状和方向*/ 
void boxrad(){ 
minboxcolor=nextminboxcolor; 
boxgs=nextboxgs; 
boxfx=nextboxfx; 
nextminboxcolor=random(14)+1; 
if(nextminboxcolor==4||nextminboxcolor==7||nextminboxcolor==8) 
nextminboxcolor=9; 
nextboxfx=F_DONG; 
nextboxgs=random(7); 
} 
/*初始化图形模试*/ 
void init(int gdrive,int gmode){ 
int errorcode; 
initgraph(&gdrive,&gmode,"D:\\tc\\"); 
errorcode=graphresult(); 
if(errorcode!=grOk){ 
printf("error of: %s",grapherrormsg(errorcode)); 
exit(1); 
} 
} 

/* 在图形模式下的清屏 */ 
void cls() 
{ 
setfillstyle(SOLID_FILL,0); 
setcolor(0); 
bar(0,0,640,480); 
} 
/*在图形模式下的高级清屏*/ 
void clscr(int a,int b,int c,int d,int color){ 
setfillstyle(SOLID_FILL,color); 
setcolor(color); 
bar(a,b,c,d); 
} 
/*最小方块的绘制*/ 
void minbox(int asc,int bsc,int color,int bdcolor){ 
int a=0,b=0; 
a=SCCOL+asc; 
b=SCROW+bsc; 
clscr(a+1,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE,color); 
if(color!=BGCOLOR){ 
setcolor(bdcolor); 
line(a+1,b+1,a-1+MINBOXSIZE,b+1); 
line(a+1,b+1,a+1,b-1+MINBOXSIZE); 
line(a-1+MINBOXSIZE,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE); 
line(a+1,b-1+MINBOXSIZE,a-1+MINBOXSIZE,b-1+MINBOXSIZE); 
} 
} 

/*游戏中出现的文字*/ 
void txt(int a,int b,char *txt,int font,int color){ 
setcolor(color); 
settextstyle(0,0,font); 
outtextxy(a,b,txt); 
} 
/*windows 绘制*/ 
void win(int a,int b,int c,int d,int bgcolor,int bordercolor){ 
clscr(a,b,c,d,bgcolor); 
setcolor(bordercolor); 
line(a,b,c,b); 
line(a,b,a,d); 
line(a,d,c,d); 
line(c,b,c,d); 

} 

/* 当前方块的绘制*/ 
void funbox(int a,int b,int color,int bdcolor){ 
int i,j; 
int boxz[4][4]; 
for(i=0;i<16;i++) 
boxz[i/4][i%4]=boxstr[boxgs][boxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(boxz[i][j]==1) 
minbox((j+row+a)*MINBOXSIZE,(i+col+b)*MINBOXSIZE,color,bdcolor); 
} 
/*下一个方块的绘制*/ 
void nextfunbox(int a,int b,int color,int bdcolor){ 
int i,j; 
int boxz[4][4]; 
for(i=0;i<16;i++) 
boxz[i/4][i%4]=boxstr[nextboxgs][nextboxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(boxz[i][j]==1) 
minbox((j+a)*MINBOXSIZE,(i+b)*MINBOXSIZE,color,bdcolor); 
} 
/*时间中断定义*/ 
#define TIMER 0x1c 
int TimerCounter=0; 
void interrupt ( *oldhandler)(__CPPARGS); 
void interrupt newhandler(__CPPARGS){ 
TimerCounter++; 
oldhandler(); 
} 
void SetTimer(void interrupt (*IntProc)(__CPPARGS)){ 
oldhandler=getvect(TIMER); 
disable(); 
setvect(TIMER,IntProc); 
enable(); 
} 
/*由于游戏的规则，消掉都有最小方块的一行*/ 
void delcol(int a){ 
int i,j; 
for(i=a;i>1;i--) 
for(j=1;j<15;j++){ 
minbox(j*MINBOXSIZE,i*MINBOXSIZE,BGCOLOR,BGCOLOR); 
gril[i][j]=gril[i-1][j]; 
if(gril[i][j]==1) 
minbox(j*MINBOXSIZE,i*MINBOXSIZE,minboxcolor,0); 
} 
} 

/*消掉所有都有最小方块的行*/ 
void delete(){ 
int i,j,zero,delgx=0; 
char *nm="00000"; 
for(i=1;i<21;i++){ 
zero=0; 
for(j=1;j<15;j++) 
if(gril[i][j]==0) 
zero=1; 
if(zero==0){ 
delcol(i); 
delgx++; 
} 
} 
num=num+delgx*delgx*10; 
dj=num/10000; 

sprintf(nm,"%d",num); 
clscr(456,173,500,200,4); 
txt(456,173,"Number:",1,15); 
txt(456,193,nm,1,15); 
} 
/*时间中断结束*/ 
void KillTimer(){ 
disable(); 
setvect(TIMER,oldhandler); 
enable(); 
} 
/* 测试当前方块是否可以向下落*/ 
int downok(){ 
int i,j,k=1,a[4][4]; 
for(i=0;i<16;i++) 
a[i/4][i%4]=boxstr[boxgs][boxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(a[i][j] && gril[col+i+1][row+j]) 
k=0; 
return(k); 
} 
/* 测试当前方块是否可以向左行*/ 
int leftok(){ 
int i,j,k=1,a[4][4]; 
for(i=0;i<16;i++) 
a[i/4][i%4]=boxstr[boxgs][boxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(a[i][j] && gril[col+i][row+j-1]) 
k=0; 
return(k); 
} 
/* 测试当前方块是否可以向右行*/ 
int rightok(){ 
int i,j,k=1,a[4][4]; 
for(i=0;i<16;i++) 
a[i/4][i%4]=boxstr[boxgs][boxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(a[i][j] && gril[col+i][row+j+1]) 
k=0; 
return(k); 
} 
/* 测试当前方块是否可以变形*/ 
int upok(){ 
int i,j,k=1,a[4][4]; 
for(i=0;i<4;i++) 
for(i=0;i<16;i++) 
a[i/4][i%4]=boxstr[boxgs][boxfx+1][i]; 
for(i=3;i>=0;i--) 
for(j=3;j>=0;j--) 
if(a[i][j] && gril[col+i][row+j]) 
k=0; 
return(k); 
} 
/*当前方块落下之后，给屏幕坐标作标记*/ 
void setgril(){ 
int i,j,a[4][4]; 
funbox(0,0,minboxcolor,0); 
for(i=0;i<16;i++) 
a[i/4][i%4]=boxstr[boxgs][boxfx][i]; 
for(i=0;i<4;i++) 
for(j=0;j<4;j++) 
if(a[i][j]) 
gril[col+i][row+j]=1; 
col=1;row=7; 
} 
/*游戏结束*/ 
void gameover(){ 
int i,j; 

for(i=20;i>0;i--) 
for(j=1;j<15;j++) 
minbox(j*MINBOXSIZE,i*MINBOXSIZE,2,0); 
txt(103,203,"Game Over",3,10); 
} 
/*按键的设置*/ 
void call_key(int keyx){ 
switch(keyx){ 
case VK_DOWN: { /*下方向键，横坐标加一。*/ 
if(downok()){ 
col++; 
funbox(0,0,minboxcolor,0);} 
else{ 
funbox(0,0,minboxcolor,0); 
setgril(); 
nextfunbox(NEXTCOL,NEXTROW,4,4); 
boxrad(); 
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0); 
delete(); 
} 
break; 
} 
case VK_UP: { /*上方向键，方向形状旋转90度*/ 
if(upok()) 
boxfx++; 
if(boxfx>3) 
boxfx=0; 
funbox(0,0,minboxcolor,0); 
break; 
} 
case VK_LEFT:{ /*左方向键，纵坐标减一*/ 
if(leftok()) 
row--; 
funbox(0,0,minboxcolor,0); 
break; 
} 
case VK_RIGHT:{ /*右方向键，纵坐标加一*/ 
if(rightok()) 
row++; 
funbox(0,0,minboxcolor,0); 
break; 
} 
case VK_SPACE: /*空格键，直接落到最后可以落到的们置*/ 
while(downok()) 
col++; 
funbox(0,0,minboxcolor,0); 
setgril(); 
nextfunbox(NEXTCOL,NEXTROW,4,4); 
boxrad(); 
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0); 
delete(); 
break; 
default: 
{ 
txt(423,53,"worng key!",1,4); 
txt(428,80,"Plese Enter Anly Key AG!",1,4); 
getch(); 
clscr(420,50,622,97,BGCOLOR); 
} 
} 
} 

/*时间中断开始*/ 
void timezd(void){ 
int key; 
SetTimer(newhandler); 
boxrad(); 
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0); 
for(;;){ 
if(bioskey(1)){ 
key=bioskey(0); 
funbox(0,0,BGCOLOR,BGCOLOR); 
if(key==VK_ESC) 
break; 
call_key(key); 
} 
if(TimerCounter>gamedj[dj]){ 
TimerCounter=0; 
if(downok()){ 
funbox(0,0,BGCOLOR,BGCOLOR); 
col++; 
funbox(0,0,minboxcolor,0); 
} 
else { 
if(col==1){ 
gameover(); 
getch(); 
break; 
} 
setgril(); 
delete(); 
funbox(0,0,minboxcolor,0); 
col=1;row=7; 
funbox(0,0,BGCOLOR,BGCOLOR); 
nextfunbox(NEXTCOL,NEXTROW,4,4); 
boxrad(); 
nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor,0); 
} 
} 
} 
} 

/*主程序开始*/ 
void main(void){ 
int i,j; 
char *nm="00000"; 
init(VGA,VGAHI); 
cls(); 
/*屏幕坐标初始化*/ 
for(i=0;i<=MAXCOL+1;i++) 
for(j=0;j<=MAXROW+1;j++) 
gril[i][j]=0; 
for(i=0;i<=MAXCOL+1;i++) { 
gril[i][0]=1; 
gril[i][15]=1; 
} 
for(j=1;j<=MAXROW;j++){ 
gril[0][j]=1; 
gril[21][j]=1; 
} 
clscr(0,0,640,480,15); 
win(1,1,639,479,4,15); 
win(SCCOL+MINBOXSIZE-2,SCROW+MINBOXSIZE-2,SCCOL+15*MINBOXSIZE+2,SCROW+21*MINBOXSIZE+2,BGCOLOR,0); 
nextboxgs=random(8); 
nextboxfx=random(4); 
sprintf(nm,"%d",num); 
txt(456,173,"Number:",1,15); 
txt(456,193,nm,1,15); 
txt(456,243,"Next Box:",1,15); 
timezd(); 
KillTimer(); 
closegraph(); 
} 
