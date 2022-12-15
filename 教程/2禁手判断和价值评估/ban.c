/*采取GBK编码
1.如果用DEVc++打开则中文是正常的
2.如果用vscode打开则需要更换编码方式
方法是点击右下角的UTF-8，点击通过编码重新打开
在搜索栏搜索"GBK",选中GBK后可以看到乱码消失，中文正常
具体操作截图在教程pdf中
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*----------------------------宏定义部分--------------------------*/

/*define bool,true,false是为了适应
C语言没有bool定义的情况
我们使用宏——TRUE and FALSE是为了代码更容易阅读
*/
#define bool int
#define true 1
#define false 0

//我们在字符串输入时会用到以下这两个宏
#define SIZE 15
#define CHARSIZE 2

/*定义棋盘上每个点的情况
NONE————空位
BLACK————黑子
WHITE————白子
*/
#define NONE 0
#define BLACK 1
#define WHITE 2

//决定在打印棋盘时是否先清屏
#define CLEAR 0

//对应每种形状的价值
#define Value_Right_5 50000
#define Value_Living_4 4320
#define Value_Rush_4 720
#define Value_Living_3 720
#define Value_Rush_3 100
#define Value_Living_2 120
#define Value_Rush_2 20

/*------------------------------end------------------------------*/

/*--------------------------全局变量部分--------------------------*/

int aRecordBoard[SIZE][SIZE];       //此数组用于记录棋盘格局

//1:棋子的落点位置信息(Pos --> Position)
typedef struct Point_Pos
{
	int hang;
	int lie;
}Point_Pos, *Ptr_to_Point_Pos;

//2:棋子落点处的连子信息(info --> information)
typedef struct Point_Info
{
	/*
	*这里将长连与合法5连设置为布尔类型的原因是
	*我们并不关心其数量，而是只关心它的真假
	*从上至下一般来说价值是递减的
	*我们根据info可以计算出某点的价值
	*/
	bool Too_long;     //长连（对于黑棋而言）
	bool Right_5; 	   //合法的5连
	int Living_4;	   //合法活四
	int Rush_4;		   //合法冲4
	int Living_3;	   //合法活3
	int Rush_3;		   //合法冲3
	int Living_2;	   //合法活2
	int Rush_2;		   //合法冲2
}Point_Info, *Ptr_to_Point_Info;

//3:对于单颗棋子的位置信息+价值
typedef struct Point
{
	Point_Pos pos;
	int Score;		//价值
	int Min;		//Min是用在α-β剪枝算法的
	int Max;		//Max是用在α-β剪枝算法的
}Point, *Ptr_to_Point;

//4:棋盘上某点的信息
/*
注意上面3中我们已经定义了包含某颗棋子的
位置信息+价值信息的Point结构体，但从整块棋盘的
角度来看，评估某点的价值时，我们应该将其对黑的价值
和对白的价值加起来得到一个allscore，为此我们从棋盘
的角度定义结构体Board_info
*/
typedef struct Board_info
{
	Point_Info Direction[2];  //[0] for black ; [1] for white
	int Score[2];  // [0] for black ; [1] for white
	int All_score; // Score[0] + Score[1]
}Board_info, *Ptr_to_Board_info;

Board_info Evaluated_Board[SIZE][SIZE];

/*------------------------------end------------------------------*/


void check_ban_and_evaluate(int hang, int lie, int color);
//check_ban_and_evaluate的子函数如下：
void Remake_Evaluated_Board(int hang, int lie, int index);

void Calculate_array(int hang, int lie, int color,
					 int *Same, int *Same_Empty,
					 int *Same_Empty_Same, int *Same_Empty_Same_Empty,
					 int *Same_Empty_Same_Empty_Same);
	
//递归判断是否为禁手
bool judge_next(int hang,int lie,int distance,int Direction,int color);
bool next_just_check_ban(int hang, int lie, int color);

//判断局面价值
int evaluate_jumian_value(int color);

/*------------------------------end------------------------------*/

void check_ban_and_evaluate(int hang, int lie, int color)
{
	/*检查禁手(对黑棋)同时评估价值
	*param：
	*int hang: 当前落子横坐标
	*int lie: 当前落子纵坐标
	*int color: 当前落子颜色（黑子 / 白子）
	*hint：黑子需要判断禁手，而白子不需要
	*/

	//我们用1表示黑棋，2表示白棋
	//但在Board_info中的数组中，下标0表示黑，下标1对应白
	//因此将color-1即为颜色对应的下标

	int index = color - 1;

	//Evaluated_Board中现在存储的是上回落子前的信息
	//也就是说，最近一次落子后，这些信息已经过时了
	//因此我们首先需要将过时信息清零

	Remake_Evaluated_Board(hang, lie, index);

	int Same[8] = {0};
	int Same_Empty[8] = {0};
	int Same_Empty_Same[8] = {0};
	int Same_Empty_Same_Empty[8] = {0};
	int Same_Empty_Same_Empty_Same[8] = {0};

	Calculate_array(hang, lie, color,
					Same, Same_Empty, Same_Empty_Same,
					Same_Empty_Same_Empty, Same_Empty_Same_Empty_Same);

	int i;
	for(i = 0 ; i < 4 ; i++)
	{
        //判断是否构成不禁手的五连,如果正确的五连实现，那其他的都不用判断了，直接return
        if(Same[i]+Same[i+4] == 4 || (Same[i]==4 && Same[i+4]==4)){   //后者条件是禁手的特殊情况，即上下都是4个，此时下两个4中间不构成长连禁手
            Evaluated_Board[hang][lie].Direction[color-1].Right_5=true;
            return;
        }
        //如果是白棋，则长连也算赢 
        if(Same[i]+Same[i+4]>=5 && color==WHITE){    //只有黑棋有禁手
            Evaluated_Board[hang][lie].Direction[color-1].Right_5=true;
            return;
        }
        //判断长连禁手，如果禁手出现也是其他的都不用判断了
        if(Same[i]+Same[i+4]>=5 && color==BLACK){    //只有黑棋有禁手
            Evaluated_Board[hang][lie].Direction[color-1].Too_long=true;
            return;
        }
        //4连
        else if(Same[i]+Same[i+4]==3){
            int flag=0;
            if(Same_Empty[i]>0 && judge_next(hang,lie,Same[i],i,color)){
                flag++;
            }
            if(Same_Empty[i+4]>0 && judge_next(hang,lie,Same[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //活四
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){       //冲四
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                ;       //两边都被堵了或者下不了，没用
            }
        }
        //3连
        else if(Same[i]+Same[i+4]==2){
            //检验上下方有没有可能冲四,两边都是冲四就是活四    形如  “xxxox”
            int flag=0;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==1){     
                if(judge_next(hang,lie,Same[i],i,color)){      //检验“xxxox”的o处
                    flag++;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){     //检验“xoxxx”处的o处
                    flag++;
                }
            }
            
            if(flag==2){
                //形如  "xoxxxox"且两个0处均能落子形成活四
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                //形如 "xxxox"且0处能落子形成冲四
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                //检查上方是否有活三，即下一步落子在上方第一个空位上能否形成活四；如果没有活三，判断是否有冲3
                //例如（以右为上）"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" 下一步落子在右边第一个o处(也是judge_next应传入的参数都能形成活四，故为活三
                //而形如"xoxxxooo"  / "ooxxxoox" 下一步落子在右边第一个o处时是无法形成活四的（禁手限制）
        
                //flag1 is for shangmian ; flag2 is for xiamian
                bool flag1_1=(Same_Empty[i]>2 || (Same_Empty[i]==2 && Same_Empty_Same[i]==0))? true : false;
                bool flag1_2=(Same_Empty[i]==1 && Same_Empty_Same[i]==0)? true : false;
                bool flag1_3=(Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0))? true : false;

                bool flag2_1=(Same_Empty[i+4]>2 || (Same_Empty[i+4]==2 && Same_Empty_Same[i+4]==0))? true : false;
                bool flag2_2=(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)? true : false;
                bool flag2_3=(Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0))? true : false;

                if((flag1_1 && flag1_3 && judge_next(hang,lie,Same[i],i,color)) || (flag2_1 && flag2_3 && judge_next(hang,lie,Same[i+4],i+4,color))){
                    Evaluated_Board[hang][lie].Direction[color-1].Living_3+=1;
                }
                else if( (((flag1_2 && flag1_3) || (flag1_1 && !flag1_3)) && judge_next(hang,lie,Same[i],i,color))
                        || (((flag2_2 && flag2_3) || (flag2_1 && !flag2_3)) && judge_next(hang,lie,Same[i+4],i+4,color)) )
                {
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_3+=1;
                }
            }
        }
        //二连
        else if(Same[i]+Same[i+4]==1){
            int flag=0;   
            //活四冲四判断
            //形如上或下"oxxoxx" / "xxoxxo"为冲四
            //形如上加下"xxoxxoxx" 为活四
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==2){
                if(judge_next(hang,lie,Same[i],i,color)){
                    flag++;      //检验“oxxoxx”右边那个o
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    flag++;      //检验“xxoxxo”左边那个o
                }
            }

            if(flag==2){
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                //活三判断
                bool flag1_1=(Same_Empty[i]==1 && Same_Empty_Same[i]==1)? true : false;
                bool flag1_2=(Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same[i]==0))? true : false;
                bool flag1_3=(Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0))? true : false;

                bool flag2_1=(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1)? true : false;
                bool flag2_2=(Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same[i+4]==0))? true : false;
                bool flag2_3=(Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0))? true : false;

                if((flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,Same[i],i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,Same[i+4],i+4,color))){
                    Evaluated_Board[hang][lie].Direction[color-1].Living_3+=1;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,Same[i],i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,Same[i+4],i+4,color)) ){
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_3+=1;
                }
            }

            //判断活2（计算价值用）
            bool flag1_1=(Same_Empty[i]>2 || (Same_Empty[i]==2 && Same_Empty_Same[i]==0))? true : false;
            bool flag1_2=(Same_Empty[i]>3 || (Same_Empty[i]==3 && Same_Empty_Same[i]==0))? true : false;
            bool flag1_3=(Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0))? true : false;

            bool flag2_1=(Same_Empty[i+4]>2 || (Same_Empty[i+4]==2 && Same_Empty_Same[i+4]==0))? true : false;
            bool flag2_2=(Same_Empty[i+4]>3 || (Same_Empty[i+4]==3 && Same_Empty_Same[i+4]==0))? true : false;
            bool flag2_3=(Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0))? true : false;

            if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,Same[i],i,color) && judge_next(hang,lie,Same[i]+1,i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,Same[i+4],i+4,color) && judge_next(hang,lie,Same[i+4]+1,i+4,color)) ){
                Evaluated_Board[hang][lie].Direction[color-1].Living_2+=1;
            }
            else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,Same[i],i,color) && judge_next(hang,lie,Same[i]+1,i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,Same[i+4],i+4,color) && judge_next(hang,lie,Same[i+4]+1,i+4,color)) ){
                Evaluated_Board[hang][lie].Direction[color-1].Rush_2+=1;
            }
        }
        //单独一子
        else if(Same[i]+Same[i+4]==0){
            //活四冲四判断
            //形如“oxoxxx”且中间o可落子位活四
            //若有两个冲四（两边对称）则刚好为禁手
            bool flag=false;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==3){
                if(judge_next(hang,lie,Same[i],i,color)){    //需要判断的点是“oxoxxx”中间的o
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
                    flag=true;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==3){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
                    flag=true;
                }
            }
            if(!flag){       //有冲四了就不用判断三了
                //活三判断
                //形如“ooxoxxoo”或“ooxoxxo”或“oxoxxoo”的且中间o可落子为活三

                bool flag1_1=(Same_Empty[i]==1 && Same_Empty_Same[i]==2)? true : false;
                bool flag1_2=(Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same[i]==0))? true : false;
                bool flag1_3=(Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0))? true : false;

                bool flag2_1=(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2)? true : false;
                bool flag2_2=(Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same[i+4]==0))? true : false;
                bool flag2_3=(Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0))? true : false;

                if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,Same[i],i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,Same[i+4],i+4,color)) ){
                    Evaluated_Board[hang][lie].Direction[color-1].Living_3+=1;
                    flag=true;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,Same[i],i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,Same[i+4],i+4,color)) ){
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_3+=1;
                    flag=true;
                }
            }

            if(!flag){
                bool flag1_1=(Same_Empty[i]==1 && Same_Empty_Same[i]==1 && Same_Empty_Same_Empty[i]>=1)? true : false;
                bool flag1_2=(Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same[i]==0))? true : false;
                bool flag1_3=(Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0));

                bool flag2_1=(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1 && Same_Empty_Same_Empty[i+4]>=1)? true : false;
                bool flag2_2=(Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same[i+4]==0))? true : false;
                bool flag2_3=(Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0));

                if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,Same[i],i,color) && judge_next(hang,lie,Same[i]+2,i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,Same[i+4],i+4,color) && judge_next(hang,lie,Same[i+4]+2,i+4,color)) ){
                    Evaluated_Board[hang][lie].Direction[color-1].Living_2+=1;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,Same[i],i,color) && judge_next(hang,lie,Same[i]+2,i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,Same[i+4],i+4,color) && judge_next(hang,lie,Same[i+4]+2,i+4,color)) ){
                    Evaluated_Board[hang][lie].Direction[color-1].Rush_2+=1;
                }
            }
        }
	}
}

void Remake_Evaluated_Board(int hang, int lie, int index)
{
	Evaluated_Board[hang][lie].Score[index] = 0;
	Evaluated_Board[hang][lie].Direction[index].Too_long = \
	Evaluated_Board[hang][lie].Direction[index].Right_5 = \
	Evaluated_Board[hang][lie].Direction[index].Living_4 = \
	Evaluated_Board[hang][lie].Direction[index].Rush_4 = \
	Evaluated_Board[hang][lie].Direction[index].Living_3 = \
	Evaluated_Board[hang][lie].Direction[index].Rush_3 = \
	Evaluated_Board[hang][lie].Direction[index].Living_2 = \
	Evaluated_Board[hang][lie].Direction[index].Rush_2 = 0;
}

void Calculate_array(int hang, int lie, int color,
					 int *Same, int *Same_Empty,
					 int *Same_Empty_Same, int *Same_Empty_Same_Empty,
					 int *Same_Empty_Same_Empty_Same)
{
	/*
	这5个数组，每个数组8个元素(8个方向)
	包含了某点向各个方向的连子及空位信息，
	结合这些信息以及一些递归调用的方法，我们就能精准无bug得
	判断出禁手信息，并同时评估出其价值
	*/

    //计算各个方向的信息
    int x,y;
    //向上搜索即y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,Same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,Same_Empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,Same_Empty_Same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,Same_Empty_Same_Empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,Same_Empty_Same_Empty_Same[0]++);

    //向右上搜索即y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,Same_Empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same_Empty_Same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,Same_Empty_Same_Empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same_Empty_Same_Empty_Same[1]++);

    //向右搜索即x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,Same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,Same_Empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,Same_Empty_Same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,Same_Empty_Same_Empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,Same_Empty_Same_Empty_Same[2]++);

    //向右下搜索即y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,Same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,Same_Empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,Same_Empty_Same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,Same_Empty_Same_Empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,Same_Empty_Same_Empty_Same[3]++);

    //向下搜索即y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,Same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,Same_Empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,Same_Empty_Same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,Same_Empty_Same_Empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,Same_Empty_Same_Empty_Same[4]++);

    //向左下搜索即y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,Same_Empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same_Empty_Same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,Same_Empty_Same_Empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same_Empty_Same_Empty_Same[5]++);

    //向左搜索即x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,Same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,Same_Empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,Same_Empty_Same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,Same_Empty_Same_Empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,Same_Empty_Same_Empty_Same[6]++);

    //向左上搜索即y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,Same_Empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same_Empty_Same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,Same_Empty_Same_Empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same_Empty_Same_Empty_Same[7]++);
}

bool judge_next(int hang,int lie,int distance,int Direction,int color){    
    if(color==WHITE)
	{         //如果是白棋显然没有禁手问题
        return true;
    }      
    int next_hang,next_lie;
    distance++;    //因为相隔一个，坐标数要加二
    if(Direction>=4)
	{
        distance=-distance;       //方向相反，加减应随之相反
    }
    //计算关键点坐标
    switch(Direction%4)
    {
        case 0:
            next_hang=hang-distance;
            next_lie=lie;
            break;
        case 1:
            next_hang=hang-distance;
            next_lie=lie+distance;
            break;
        case 2:
            next_hang=hang;
            next_lie=lie+distance;
            break;
        case 3:
            next_hang=hang+distance;
            next_lie=lie+distance;
            break;
        default:
            printf("传入的方向有误");
            break;
    }
    bool if_just_evaluate=false;
    if(aRecordBoard[hang][lie]!=NONE)
	{
    	if_just_evaluate=true;
	}

    aRecordBoard[hang][lie]=color;        //将刚才那个子落了

    bool flag = next_just_check_ban(next_hang,next_lie,color);

    if(!if_just_evaluate)
	{         
		//适应判断局面分的需要，在判断局面分时，原本这里就有子，不能擦除 
    	aRecordBoard[hang][lie]=NONE;        //恢复 
	}
    if(flag){
    	return true;
	}
	else{
		return false;
	}
}

bool next_just_check_ban(int hang, int lie, int color){ //用于判断关键点是否是禁手
    int Same[8];      
    int Same_Empty[8];   
    int Same_Empty_Same[8]; 
    int Same_Empty_Same_Empty[8];
    int Same_Empty_Same_Empty_Same[8];

    int j;
    //初始化数组，赋值当然全为0
    for(j=0;j<8;j++){
        Same[j]=Same_Empty[j]=Same_Empty_Same[j]=Same_Empty_Same_Empty[j]=Same_Empty_Same_Empty_Same[j]=0;
    }

	Calculate_array(hang, lie, color,
					Same, Same_Empty, Same_Empty_Same,
					Same_Empty_Same_Empty, Same_Empty_Same_Empty_Same);

    //统计完毕，下面应当开始计算4个方向活3活4的数目及禁手
	//这里与check_ban_and_evaluate的区别即不用计算冲三、活二和冲二这几种与禁手无关而只与价值无关的形状。
    int Living_4, Rush_4, Living_3;
    Living_4 = Rush_4 = Living_3 = 0;

    //首先判断是否成连五，若成连五，显然不构成禁手
    int i;
    for(i=0;i<4;i++){
        //判断是否构成不禁手的五连,如果正确的五连实现，那其他的都不用判断了，直接return
        if(Same[i]+Same[i+4] == 4 || (Same[i]==4 && Same[i+4]==4)){   //后者条件是禁手的特殊情况，即上下都是4个，此时下两个4中间不构成长连禁手
            //正确五连，说明该关键点不是禁手，返回true就完了
            return true;
        }
        //判断长连禁手，如果禁手出现也是其他的都不用判断了
        if(Same[i]+Same[i+4]>=5){
            //长连禁手，直接返回false
            return false;
        }
        //4连
        else if(Same[i]+Same[i+4]==3){
            int flag=0;
            if(Same_Empty[i]>0 && judge_next(hang,lie,Same[i],i,color)){
                flag++;
            }
            if(Same_Empty[i+4]>0 && judge_next(hang,lie,Same[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //活四
                Living_4+=1;
            }
            else if(flag==1){       //冲四
                Rush_4+=1;
            }
            else{
                ;       //两边都被堵了或者下不了，没用
            }
        }
        //3连
        else if(Same[i]+Same[i+4]==2){
            //检验上下方有没有可能冲四,两边都是冲四就是活四    形如  “xxxox”
            int flag=0;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==1){     
                if(judge_next(hang,lie,Same[i],i,color)){      //检验“xxxox”的o处
                    flag++;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){     //检验“xoxxx”处的o处
                    flag++;
                }
            }
            
            if(flag==2){
                //形如  "xoxxxox"且两个0处均能落子形成活四
                Living_4+=1;
            }
            else if(flag==1){
                //形如 "xxxox"且0处能落子形成冲四
                Rush_4+=1;
            }
            else{
                //检查上方是否有活三，即下一步落子在上方第一个空位上能否形成活四
                //例如（以右为上）"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" 下一步落子在右边第一个o处(也是judge_next应传入的参数都能形成活四，故为活三
                //而形如"xoxxxooo"  / "ooxxxoox" 下一步落子在右边第一个o处时是无法形成活四的（禁手限制）
                bool flag_living_3=false;

                if((Same_Empty[i]>2 || (Same_Empty[i]==2 && Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){    //检验xxx上边的第一个o处
                        flag_living_3=true;
                    }
                }
                //与上方相同道理，现在检测下方
                if((Same_Empty[i+4]>2 || (Same_Empty[i+4]==2 && Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){    //检验xxx下边的第一个o处
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //二连
        else if(Same[i]+Same[i+4]==1){
            int flag=0;   
            //活四冲四判断
            //形如上或下"oxxoxx" / "xxoxxo"为冲四
            //形如上加下"xxoxxoxx" 为活四
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==2){
                if(judge_next(hang,lie,Same[i],i,color)){
                    flag++;      //检验“oxxoxx”右边那个o
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    flag++;      //检验“xxoxxo”左边那个o
                }
            }

            if(flag==2){
                Living_4+=1;
            }
            else if(flag==1){
                Rush_4+=1;
            }
            else{
                //活三判断
                bool flag_living_3=false;
                if(Same_Empty[i]==1 && Same_Empty_Same[i]==1
                    && (Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same_Empty_Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){    //检验“ooxxoxoo"中间的空位
                        flag_living_3=true;
                    }
                }
                if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1
                    && (Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same_Empty_Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){    //检验“ooxoxxo"中间的空位
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //单独一子
        else if(Same[i]+Same[i+4]==0){
            //活四冲四判断
            //形如“oxoxxx”且中间o可落子位活四
            bool flag=false;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==3){
                if(judge_next(hang,lie,Same[i],i,color)){    //需要判断的点是“oxoxxx”中间的o
                    Rush_4+=1;
                    flag=true;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==3){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    Rush_4+=1;
                    flag=true;
                }
            }
            //活三判断
            //形如“ooxoxxoo”或“ooxoxxo”或“oxoxxoo”的且中间o可落子为活三

            if(!flag){
                bool flag_living_3=false;

                if(Same_Empty[i]==1 && Same_Empty_Same[i]==2
                    && (Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same_Empty_Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){   //需要判断的点是中间的o
                        flag_living_3=true;
                    }
                }
                if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2
                    && (Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same_Empty_Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){   //需要判断的点是中间的o
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
    }

    //判断禁手
    if(Living_3==1 && (Living_4+Rush_4)==1)
	{
		//先考虑特殊情况，即四三不是禁手。
        return true;            
    }

    if(Living_3+Living_4+Rush_4>=2)
	{             
		//除了一个四一个三以外，其余情况下只要和大于等于2，一定是禁手。
        return false;
    }
    else{
        return true;
    }
} 

void evaluate_value(int hang, int lie){
    int i;
    int value[2]={0,0};
    for(i=0;i<2;i++){
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Right_5*Value_Right_5;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Living_4*Value_Living_4;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Rush_4*Value_Rush_4;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Living_3*Value_Living_3;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Rush_3*Value_Rush_3;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Living_2*Value_Living_2;
        value[i]+=Evaluated_Board[hang][lie].Direction[i].Rush_2*Value_Rush_2;
        Evaluated_Board[hang][lie].Score[i]=value[i];
    }
    Evaluated_Board[hang][lie].All_score = value[0] + value[1];
}

int evaluate_jumian_value(int color){
	//优先级是对方冲四>己方活四>对方活3 
	int Sef_Value_Living_4=100000;
	int Rival_Value_Living_4=10000000; 
	int Sef_Value_Rush_4=720;
	int Rival_Value_Rush_4=1000000;
	int Sef_Value_Living_3=720;
	int Rival_Value_Living_3=50000;
	int Sef_Value_Rush_3=480;
	int Rival_Value_Rush_3=720;
	int Sef_Value_Living_2=480;
	int Rival_Value_Living_2=480;
	int Sef_Value_Rush_2=20;
	int Rival_Value_Rush_2=100;
	//这里关于己方活三，活二，对面冲三，等的赋值还有待商榷 
	
    int i,j;
    int Right_5[2]={0,0};
    int Living_4[2]={0,0};
    int Rush_4[2]={0,0};
    int Living_3[2]={0,0};
    int Rush_3[2]={0,0};
    int Living_2[2]={0,0};
    int Rush_2[2]={0,0};

    for(i=0;i<SIZE;i++)
	{
        for(j=0;j<SIZE;j++)
		{
            if(aRecordBoard[i][j]==BLACK)
			{
                check_ban_and_value(i,j,BLACK);
                Living_4[0]+=Evaluated_Board[i][j].Direction[0].Living_4;
                Rush_4[0]+=Evaluated_Board[i][j].Direction[0].Rush_4;
                Living_3[0]+=Evaluated_Board[i][j].Direction[0].Living_3;
                Rush_3[0]+=Evaluated_Board[i][j].Direction[0].Rush_3;
                Living_2[0]+=Evaluated_Board[i][j].Direction[0].Living_2;
                Rush_2[0]+=Evaluated_Board[i][j].Direction[0].Rush_2;
            }
            if(aRecordBoard[i][j]==WHITE)
			{
                check_ban_and_value(i,j,WHITE);
                Living_4[1]+=Evaluated_Board[i][j].Direction[1].Living_4;
                Rush_4[1]+=Evaluated_Board[i][j].Direction[1].Rush_4;
                Living_3[1]+=Evaluated_Board[i][j].Direction[1].Living_3;
                Rush_3[1]+=Evaluated_Board[i][j].Direction[1].Rush_3;
                Living_2[1]+=Evaluated_Board[i][j].Direction[1].Living_2;
                Rush_2[1]+=Evaluated_Board[i][j].Direction[1].Rush_2;
            }
        }
    }

    int value=0;
    if(color==BLACK)
	{
    	value+=Living_4[0]*Sef_Value_Living_4/4;
        value+=Rush_4[0]*Sef_Value_Rush_4/4;
        value+=Living_3[0]*Sef_Value_Living_3/3;
        value+=Rush_3[0]*Sef_Value_Rush_3/3;
        value+=Living_2[0]*Sef_Value_Living_2/2;
        value+=Rush_2[0]*Sef_Value_Rush_2/2;
        
        value-=Living_4[1]*Rival_Value_Rush_4/4;
        value-=Rush_4[1]*Rival_Value_Rush_4/4;
        value-=Living_3[1]*Rival_Value_Living_3/3;
        value-=Rush_3[1]*Rival_Value_Rush_3/3;
        value-=Living_2[1]*Rival_Value_Living_2/2;
        value-=Rush_2[1]*Rival_Value_Rush_2/2;
    }
    else
	{
    	value+=Living_4[1]*Sef_Value_Living_4/4;
        value+=Rush_4[1]*Sef_Value_Rush_4/4;
        value+=Living_3[1]*Sef_Value_Living_3/3;
        value+=Rush_3[1]*Sef_Value_Rush_3/3;
        value+=Living_2[1]*Sef_Value_Living_2/2;
        value+=Rush_2[1]*Sef_Value_Rush_2/2;
        
        value-=Living_4[0]*Rival_Value_Rush_4/4;
        value-=Rush_4[0]*Rival_Value_Rush_4/4;
        value-=Living_3[0]*Rival_Value_Living_3/3;
        value-=Rush_3[0]*Rival_Value_Rush_3/3;
        value-=Living_2[0]*Rival_Value_Living_2/2;
        value-=Rush_2[0]*Rival_Value_Rush_2/2;
    }
    return value;
}