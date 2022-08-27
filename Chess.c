
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//为了适应c语言无bool.h
#define bool int
#define true 1
#define false 0
//为了字符串输入
#define SIZE 15
#define CHARSIZE 2
//对应棋子花色
#define NONE 0
#define BLACK 1
#define WHITE 2
//对应每种形状的价值
#define Value_Right_5 50000
#define Value_Living_4 4320
#define Value_Rush_4 720
#define Value_Living_3 720
#define Value_Rush_3 100
#define Value_Living_2 120
#define Value_Rush_2 20
//决定在判断禁手时需不需要打印信息
#define NEED_INF 1
#define NO_NEED_INF 0
//决定在找点的数组时需不需要点值
#define NEED_VALUE 1
#define NO_NEED_VALUE 0

//定义搜索深度
int DEPTH=8;
//定义选点宽度
#define Base_Width 15
int WIDTH=14;
//选择是算7层还是9层 
char able;
#define Fuwuqiong -10000000     //α的初值
#define Zhengwuqiong 10000000   //β的初值

typedef struct{       //记录点的位置，注意并非每处都用此struct，有些地方为了方便直接用了hang和lie两个量
    int hang;
    int lie;
}Point, *PTR_To_Point;

typedef struct{
    Point p;
    int score;
    int min;
    int max;
}Points, *PTR_To_Points;

struct Point_Imf{          //记录某点某一方向上的连子信息
    bool Too_long;
    bool Right_5;
    int Living_4;
    int Rush_4;
    int Living_3;
    int Rush_3;
    int Living_2;
    int Rush_2;
};

struct Point_Score{
    struct Point_Imf direction[2];     //direction[0] is for black   direction[1] is for white    
    int score[2];       //score[0] is for black     score[1] is for white
    int allscore;
};

//棋盘使用的是GBK编码，每一个中文字符占用2个字节。
//棋盘基本模板 
char aInitDisplayBoardArray[SIZE][SIZE*CHARSIZE+1] = 
{
		"┏┯┯┯┯┯┯┯┯┯┯┯┯┯┓",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
		"┗┷┷┷┷┷┷┷┷┷┷┷┷┷┛"
};
//此数组用于显示棋盘 
char aDisplayBoardArray[SIZE][SIZE*CHARSIZE+1];
 
char play1Pic[]="●";//黑棋子;
char play1CurrentPic[]="▲"; 

char play2Pic[]="◎";//白棋子;
char play2CurrentPic[]="△";

char leave[]="quit";
 
int aRecordBoard[SIZE][SIZE];       //此数组用于记录棋盘格局
struct Point_Score Value_Board[SIZE][SIZE];          //此数组用来记录每个点的各种信息

void initRecordBorard(void);
void recordtoDisplayArray(int hang,int lie);
void displayBoard(void);

void check_ban_and_value(int hang, int lie, int color);   //color是为了选黑白
bool judge_next(int hang,int lie,int distance,int direction,int color);
bool next_just_check_ban(int hang, int lie, int color);
bool judge_if_banhand(int hang,int lie,int color,bool print);
void evaluate_value(int hang, int lie);
int evaluate_jumian_value(int color);

int Best_choice(int color, PTR_To_Point final, int depth, int a, int b, char able);
int Find_Points(PTR_To_Points All_Points, int color, bool flag, bool *flag_if_win, PTR_To_Point Bisha, bool *flag_will_lose, PTR_To_Point Bidang); //返回的是找到的点数
void quicksort(PTR_To_Points s, int left, int right);
int Find_base_point(int color);

bool judge_result(void);      //判断输赢

void Human_vs_Human(void);    //人人对战模式
void Human_vs_AI(void);       //人机对战模式
void AI_vs_AI(void);          //AI对战模式 

void black_go();              //人类下黑棋
void white_go();              //人类下白棋
void AI_black_go(int count, char able);           //AI下黑棋
void AI_white_go(int count, char able);           //AI下白棋
bool judge_result(void);
void Buju_Black(int hang, int lie);        //用于布局自动落子 
void Buju_White(int hang, int lie);

//----------------------------------------------------------------------------//

void check_ban_and_value(int hang, int lie, int color){
    //先将原信息清零
    Value_Board[hang][lie].score[color-1]=0;
    Value_Board[hang][lie].direction[color-1].Too_long=false;
    Value_Board[hang][lie].direction[color-1].Right_5=false;
    Value_Board[hang][lie].direction[color-1].Living_4=0;
    Value_Board[hang][lie].direction[color-1].Rush_4=0;
    Value_Board[hang][lie].direction[color-1].Living_3=0;
    Value_Board[hang][lie].direction[color-1].Rush_3=0;
    Value_Board[hang][lie].direction[color-1].Living_2=0;
    Value_Board[hang][lie].direction[color-1].Rush_2=0;
    //清零完毕
    int ajcsame[8];       //ajc==adjacent附近的,same意为相同颜色的，0-7依次代表上，右上，右，右下，下，左下，左，左上
    int after_same_is_empty[8];    //记录连续相同颜色棋子后的空位
    int after_same_empty_is_same[8]; //记录空位后的相同颜色数
    int after_same_empty_same_is_empty[8];
    int after_same_empty_same_empty_is_same[8];

    int j;
    //初始化数组，赋值当然全为0
    for(j=0;j<8;j++){
        ajcsame[j]=after_same_is_empty[j]=after_same_empty_is_same[j]=after_same_empty_same_is_empty[j]=after_same_empty_same_empty_is_same[j]=0;
    }

    //计算各个方向的信息
    int x,y;
    //向上搜索即y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,ajcsame[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_is_same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_empty_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_same_empty_is_same[0]++);

    //向右上搜索即y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,ajcsame[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_is_same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_empty_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_same_empty_is_same[1]++);

    //向右搜索即x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,ajcsame[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_is_same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_empty_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_same_empty_is_same[2]++);

    //向右下搜索即y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,ajcsame[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_is_same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_empty_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_same_empty_is_same[3]++);

    //向下搜索即y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,ajcsame[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_is_same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_empty_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_same_empty_is_same[4]++);

    //向左下搜索即y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,ajcsame[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_is_same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_empty_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_same_empty_is_same[5]++);

    //向左搜索即x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,ajcsame[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_is_same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_empty_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_same_empty_is_same[6]++);

    //向左上搜索即y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,ajcsame[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_is_same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_empty_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_same_empty_is_same[7]++);
    //统计完毕，下面应当开始计算4个方向活3活4的数目及禁手

    //首先判断是否成连五，若成连五，显然不构成禁手
    int i;
    for(i=0;i<4;i++){
        //判断是否构成不禁手的五连,如果正确的五连实现，那其他的都不用判断了，直接return
        if(ajcsame[i]+ajcsame[i+4] == 4 || (ajcsame[i]==4 && ajcsame[i+4]==4)){   //后者条件是禁手的特殊情况，即上下都是4个，此时下两个4中间不构成长连禁手
            Value_Board[hang][lie].direction[color-1].Right_5=true;
            return;
        }
        //如果是白棋，则长连也算赢 
        if(ajcsame[i]+ajcsame[i+4]>=5 && color==WHITE){    //只有黑棋有禁手
            Value_Board[hang][lie].direction[color-1].Right_5=true;
            return;
        }
        //判断长连禁手，如果禁手出现也是其他的都不用判断了
        if(ajcsame[i]+ajcsame[i+4]>=5 && color==BLACK){    //只有黑棋有禁手
            Value_Board[hang][lie].direction[color-1].Too_long=true;
            return;
        }
        //4连
        else if(ajcsame[i]+ajcsame[i+4]==3){
            int flag=0;
            if(after_same_is_empty[i]>0 && judge_next(hang,lie,ajcsame[i],i,color)){
                flag++;
            }
            if(after_same_is_empty[i+4]>0 && judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //活四
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){       //冲四
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                ;       //两边都被堵了或者下不了，没用
            }
        }
        //3连
        else if(ajcsame[i]+ajcsame[i+4]==2){
            //检验上下方有没有可能冲四,两边都是冲四就是活四    形如  “xxxox”
            int flag=0;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1){     
                if(judge_next(hang,lie,ajcsame[i],i,color)){      //检验“xxxox”的o处
                    flag++;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){     //检验“xoxxx”处的o处
                    flag++;
                }
            }
            
            if(flag==2){
                //形如  "xoxxxox"且两个0处均能落子形成活四
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                //形如 "xxxox"且0处能落子形成冲四
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                //检查上方是否有活三，即下一步落子在上方第一个空位上能否形成活四；如果没有活三，判断是否有冲3
                //例如（以右为上）"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" 下一步落子在右边第一个o处(也是judge_next应传入的参数都能形成活四，故为活三
                //而形如"xoxxxooo"  / "ooxxxoox" 下一步落子在右边第一个o处时是无法形成活四的（禁手限制）
        
                //flag1 is for shangmian ; flag2 is for xiamian
                bool flag1_1=(after_same_is_empty[i]>2 || (after_same_is_empty[i]==2 && after_same_empty_is_same[i]==0))? true : false;
                bool flag1_2=(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)? true : false;
                bool flag1_3=(after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0))? true : false;

                bool flag2_1=(after_same_is_empty[i+4]>2 || (after_same_is_empty[i+4]==2 && after_same_empty_is_same[i+4]==0))? true : false;
                bool flag2_2=(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)? true : false;
                bool flag2_3=(after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0))? true : false;

                if((flag1_1 && flag1_3 && judge_next(hang,lie,ajcsame[i],i,color)) || (flag2_1 && flag2_3 && judge_next(hang,lie,ajcsame[i+4],i+4,color))){
                    Value_Board[hang][lie].direction[color-1].Living_3+=1;
                }
                else if( (((flag1_2 && flag1_3) || (flag1_1 && !flag1_3)) && judge_next(hang,lie,ajcsame[i],i,color))
                        || (((flag2_2 && flag2_3) || (flag2_1 && !flag2_3)) && judge_next(hang,lie,ajcsame[i+4],i+4,color)) )
                {
                    Value_Board[hang][lie].direction[color-1].Rush_3+=1;
                }
            }
        }
        //二连
        else if(ajcsame[i]+ajcsame[i+4]==1){
            int flag=0;   
            //活四冲四判断
            //形如上或下"oxxoxx" / "xxoxxo"为冲四
            //形如上加下"xxoxxoxx" 为活四
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2){
                if(judge_next(hang,lie,ajcsame[i],i,color)){
                    flag++;      //检验“oxxoxx”右边那个o
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                    flag++;      //检验“xxoxxo”左边那个o
                }
            }

            if(flag==2){
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                //活三判断
                bool flag1_1=(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1)? true : false;
                bool flag1_2=(after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))? true : false;
                bool flag1_3=(after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0))? true : false;

                bool flag2_1=(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1)? true : false;
                bool flag2_2=(after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))? true : false;
                bool flag2_3=(after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0))? true : false;

                if((flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,ajcsame[i],i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,ajcsame[i+4],i+4,color))){
                    Value_Board[hang][lie].direction[color-1].Living_3+=1;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,ajcsame[i],i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,ajcsame[i+4],i+4,color)) ){
                    Value_Board[hang][lie].direction[color-1].Rush_3+=1;
                }
            }

            //判断活2（计算价值用）
            bool flag1_1=(after_same_is_empty[i]>2 || (after_same_is_empty[i]==2 && after_same_empty_is_same[i]==0))? true : false;
            bool flag1_2=(after_same_is_empty[i]>3 || (after_same_is_empty[i]==3 && after_same_empty_is_same[i]==0))? true : false;
            bool flag1_3=(after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0))? true : false;

            bool flag2_1=(after_same_is_empty[i+4]>2 || (after_same_is_empty[i+4]==2 && after_same_empty_is_same[i+4]==0))? true : false;
            bool flag2_2=(after_same_is_empty[i+4]>3 || (after_same_is_empty[i+4]==3 && after_same_empty_is_same[i+4]==0))? true : false;
            bool flag2_3=(after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0))? true : false;

            if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,ajcsame[i],i,color) && judge_next(hang,lie,ajcsame[i]+1,i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,ajcsame[i+4],i+4,color) && judge_next(hang,lie,ajcsame[i+4]+1,i+4,color)) ){
                Value_Board[hang][lie].direction[color-1].Living_2+=1;
            }
            else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,ajcsame[i],i,color) && judge_next(hang,lie,ajcsame[i]+1,i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,ajcsame[i+4],i+4,color) && judge_next(hang,lie,ajcsame[i+4]+1,i+4,color)) ){
                Value_Board[hang][lie].direction[color-1].Rush_2+=1;
            }
        }
        //单独一子
        else if(ajcsame[i]+ajcsame[i+4]==0){
            //活四冲四判断
            //形如“oxoxxx”且中间o可落子位活四
            //若有两个冲四（两边对称）则刚好为禁手
            bool flag=false;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==3){
                if(judge_next(hang,lie,ajcsame[i],i,color)){    //需要判断的点是“oxoxxx”中间的o
                    Value_Board[hang][lie].direction[color-1].Rush_4+=1;
                    flag=true;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==3){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                    Value_Board[hang][lie].direction[color-1].Rush_4+=1;
                    flag=true;
                }
            }
            if(!flag){       //有冲四了就不用判断三了
                //活三判断
                //形如“ooxoxxoo”或“ooxoxxo”或“oxoxxoo”的且中间o可落子为活三

                bool flag1_1=(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2)? true : false;
                bool flag1_2=(after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))? true : false;
                bool flag1_3=(after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0))? true : false;

                bool flag2_1=(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2)? true : false;
                bool flag2_2=(after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))? true : false;
                bool flag2_3=(after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0))? true : false;

                if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,ajcsame[i],i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,ajcsame[i+4],i+4,color)) ){
                    Value_Board[hang][lie].direction[color-1].Living_3+=1;
                    flag=true;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,ajcsame[i],i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,ajcsame[i+4],i+4,color)) ){
                    Value_Board[hang][lie].direction[color-1].Rush_3+=1;
                    flag=true;
                }
            }

            if(!flag){
                bool flag1_1=(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1 && after_same_empty_same_is_empty[i]>=1)? true : false;
                bool flag1_2=(after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))? true : false;
                bool flag1_3=(after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0));

                bool flag2_1=(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1 && after_same_empty_same_is_empty[i+4]>=1)? true : false;
                bool flag2_2=(after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))? true : false;
                bool flag2_3=(after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0));

                if( (flag1_1 && flag1_2 && flag1_3 && judge_next(hang,lie,ajcsame[i],i,color) && judge_next(hang,lie,ajcsame[i]+2,i,color)) || (flag2_1 && flag2_2 && flag2_3 && judge_next(hang,lie,ajcsame[i+4],i+4,color) && judge_next(hang,lie,ajcsame[i+4]+2,i+4,color)) ){
                    Value_Board[hang][lie].direction[color-1].Living_2+=1;
                }
                else if( (flag1_1 && (flag1_2 || flag1_3) && judge_next(hang,lie,ajcsame[i],i,color) && judge_next(hang,lie,ajcsame[i]+2,i,color)) || (flag2_1 && (flag2_2 || flag2_3) && judge_next(hang,lie,ajcsame[i+4],i+4,color) && judge_next(hang,lie,ajcsame[i+4]+2,i+4,color)) ){
                    Value_Board[hang][lie].direction[color-1].Rush_2+=1;
                }
            }
        }
    }
    //禁手判断完毕
}

bool judge_next(int hang,int lie,int distance,int direction,int color){    
    if(color==WHITE){         //如果是白棋显然没有禁手问题
        return true;
    }      
    int next_hang,next_lie;
    distance++;    //因为相隔一个，坐标数要加二
    if(direction>=4){
        distance=-distance;       //方向相反，加减应随之相反
    }
    //计算关键点坐标
    switch(direction%4)
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
    bool flag_if_youzi=true;
    if(aRecordBoard[hang][lie]!=NONE){
    	flag_if_youzi=false;
	}
    aRecordBoard[hang][lie]=color;        //将刚才那个子落了
    bool flag = next_just_check_ban(next_hang,next_lie,color);
    if(flag_if_youzi){         //适应判断局面分的需要，在判断局面分时，原本这里就有子，不能擦除 
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
    int ajcsame[8];       //ajc==adjacent附近的,same意为相同颜色的，0-7依次代表上，右上，右，右下，下，左下，左，左上
    int after_same_is_empty[8];    //记录连续相同颜色棋子后的空位
    int after_same_empty_is_same[8]; //记录空位后的相同颜色数
    int after_same_empty_same_is_empty[8];
    int after_same_empty_same_empty_is_same[8];

    int j;
    //初始化数组，赋值当然全为0
    for(j=0;j<8;j++){
        ajcsame[j]=after_same_is_empty[j]=after_same_empty_is_same[j]=after_same_empty_same_is_empty[j]=after_same_empty_same_empty_is_same[j]=0;
    }

    //计算各个方向的信息
    int x,y;
    //向上搜索即y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,ajcsame[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_is_same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_empty_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_same_empty_is_same[0]++);

    //向右上搜索即y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,ajcsame[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_is_same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_empty_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_same_empty_is_same[1]++);

    //向右搜索即x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,ajcsame[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_is_same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_empty_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_same_empty_is_same[2]++);

    //向右下搜索即y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,ajcsame[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_is_same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_empty_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_same_empty_is_same[3]++);

    //向下搜索即y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,ajcsame[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_is_same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_empty_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_same_empty_is_same[4]++);

    //向左下搜索即y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,ajcsame[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_is_same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_empty_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_same_empty_is_same[5]++);

    //向左搜索即x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,ajcsame[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_is_same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_empty_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_same_empty_is_same[6]++);

    //向左上搜索即y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,ajcsame[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_is_same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_empty_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_same_empty_is_same[7]++);
    //统计完毕，下面应当开始计算4个方向活3活4的数目及禁手
    int Living_4,Rush_4,Living_3;
    Living_4=Rush_4=Living_3=0;

    //首先判断是否成连五，若成连五，显然不构成禁手
    int i;
    for(i=0;i<4;i++){
        //判断是否构成不禁手的五连,如果正确的五连实现，那其他的都不用判断了，直接return
        if(ajcsame[i]+ajcsame[i+4] == 4 || (ajcsame[i]==4 && ajcsame[i+4]==4)){   //后者条件是禁手的特殊情况，即上下都是4个，此时下两个4中间不构成长连禁手
            //正确五连，说明该关键点不是禁手，返回true就完了
            return true;
        }
        //判断长连禁手，如果禁手出现也是其他的都不用判断了
        if(ajcsame[i]+ajcsame[i+4]>=5){
            //长连禁手，直接返回false
            return false;
        }
        //4连
        else if(ajcsame[i]+ajcsame[i+4]==3){
            int flag=0;
            if(after_same_is_empty[i]>0 && judge_next(hang,lie,ajcsame[i],i,color)){
                flag++;
            }
            if(after_same_is_empty[i+4]>0 && judge_next(hang,lie,ajcsame[i+4],i+4,color)){
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
        else if(ajcsame[i]+ajcsame[i+4]==2){
            //检验上下方有没有可能冲四,两边都是冲四就是活四    形如  “xxxox”
            int flag=0;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1){     
                if(judge_next(hang,lie,ajcsame[i],i,color)){      //检验“xxxox”的o处
                    flag++;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){     //检验“xoxxx”处的o处
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

                if((after_same_is_empty[i]>2 || (after_same_is_empty[i]==2 && after_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){    //检验xxx上边的第一个o处
                        flag_living_3=true;
                    }
                }
                //与上方相同道理，现在检测下方
                if((after_same_is_empty[i+4]>2 || (after_same_is_empty[i+4]==2 && after_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){    //检验xxx下边的第一个o处
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //二连
        else if(ajcsame[i]+ajcsame[i+4]==1){
            int flag=0;   
            //活四冲四判断
            //形如上或下"oxxoxx" / "xxoxxo"为冲四
            //形如上加下"xxoxxoxx" 为活四
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2){
                if(judge_next(hang,lie,ajcsame[i],i,color)){
                    flag++;      //检验“oxxoxx”右边那个o
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
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
                if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1
                    && (after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){    //检验“ooxxoxoo"中间的空位
                        flag_living_3=true;
                    }
                }
                if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1
                    && (after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){    //检验“ooxoxxo"中间的空位
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //单独一子
        else if(ajcsame[i]+ajcsame[i+4]==0){
            //活四冲四判断
            //形如“oxoxxx”且中间o可落子位活四
            bool flag=false;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==3){
                if(judge_next(hang,lie,ajcsame[i],i,color)){    //需要判断的点是“oxoxxx”中间的o
                    Rush_4+=1;
                    flag=true;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==3){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                    Rush_4+=1;
                    flag=true;
                }
            }
            //活三判断
            //形如“ooxoxxoo”或“ooxoxxo”或“oxoxxoo”的且中间o可落子为活三

            if(!flag){
                bool flag_living_3=false;

                if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2
                    && (after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){   //需要判断的点是中间的o
                        flag_living_3=true;
                    }
                }
                if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2
                    && (after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){   //需要判断的点是中间的o
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
    }
    //禁手判断完毕
    if(Living_3==1 && (Living_4+Rush_4)==1){
        return true;            //先考虑特殊情况，即四三不是禁手。
    }

    if(Living_3+Living_4+Rush_4>=2){             //除了一个四一个三以外，其余情况下只要和大于等于2，一定是禁手。
        return false;
    }
    else{
        return true;
    }
} 

bool judge_if_banhand(int hang,int lie,int color,bool print){
    if(Value_Board[hang][lie].direction[color-1].Too_long==true){
        if(print)
            printf("长连禁手！\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_3>=2 && Value_Board[hang][lie].direction[color-1].Living_4==0 && Value_Board[hang][lie].direction[color-1].Rush_4==0){
        if(print)
            printf("双活三禁手！\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4>=2 && Value_Board[hang][lie].direction[color-1].Living_3==0){
        if(print)
            printf("双活四/冲四禁手！\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==1 && Value_Board[hang][lie].direction[color-1].Living_3>=2){
        if(print)
            printf("四三三(三)禁手！\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==2 && Value_Board[hang][lie].direction[color-1].Living_3>=1){
        if(print)
            printf("四四三(三)禁手！\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==3 && Value_Board[hang][lie].direction[color-1].Living_3==1){
        if(print)
            printf("四四四三禁手！\n");
        return false;
    }
    //注意43不是禁手
    return true;
}

void evaluate_value(int hang, int lie){
    int i;
    int value[2]={0,0};
    for(i=0;i<2;i++){
        value[i]+=Value_Board[hang][lie].direction[i].Right_5*Value_Right_5;
        value[i]+=Value_Board[hang][lie].direction[i].Living_4*Value_Living_4;
        value[i]+=Value_Board[hang][lie].direction[i].Rush_4*Value_Rush_4;
        value[i]+=Value_Board[hang][lie].direction[i].Living_3*Value_Living_3;
        value[i]+=Value_Board[hang][lie].direction[i].Rush_3*Value_Rush_3;
        value[i]+=Value_Board[hang][lie].direction[i].Living_2*Value_Living_2;
        value[i]+=Value_Board[hang][lie].direction[i].Rush_2*Value_Rush_2;
        Value_Board[hang][lie].score[i]=value[i];
    }
    Value_Board[hang][lie].allscore=value[0]+value[1];
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
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            if(aRecordBoard[i][j]==BLACK){
                check_ban_and_value(i,j,BLACK);
                Living_4[0]+=Value_Board[i][j].direction[0].Living_4;
                Rush_4[0]+=Value_Board[i][j].direction[0].Rush_4;
                Living_3[0]+=Value_Board[i][j].direction[0].Living_3;
                Rush_3[0]+=Value_Board[i][j].direction[0].Rush_3;
                Living_2[0]+=Value_Board[i][j].direction[0].Living_2;
                Rush_2[0]+=Value_Board[i][j].direction[0].Rush_2;
            }
            if(aRecordBoard[i][j]==WHITE){
                check_ban_and_value(i,j,WHITE);
                Living_4[1]+=Value_Board[i][j].direction[1].Living_4;
                Rush_4[1]+=Value_Board[i][j].direction[1].Rush_4;
                Living_3[1]+=Value_Board[i][j].direction[1].Living_3;
                Rush_3[1]+=Value_Board[i][j].direction[1].Rush_3;
                Living_2[1]+=Value_Board[i][j].direction[1].Living_2;
                Rush_2[1]+=Value_Board[i][j].direction[1].Rush_2;
            }
        }
    }
    int value=0;
    if(color==BLACK){
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
    else{
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

int Find_Points(PTR_To_Points All_Points, int color, bool flag, bool *flag_if_win, PTR_To_Point Bisha, bool *flag_will_lose, PTR_To_Point Bidang){
    int hang,lie;
    int index=0;
    PTR_To_Point cunchu;  //存储已搜索过的点
    cunchu=(PTR_To_Point)malloc(sizeof(Point)*225);
    int cunchu_index=0;
    for(hang=0;hang<SIZE;hang++){
        for(lie=0;lie<SIZE;lie++){
            if(aRecordBoard[hang][lie]!=NONE){
                int x1,y1,x2,y2;
                if(hang>0){
                    x1=hang-1;
                }
                else{
                	x1=hang;
				}
                if(hang<SIZE-1){
                    x2=hang+1;
                }
                else{
                	x2=hang;
				}
                if(lie>0){
                    y1=lie-1;
                }
                else{
                	y1=lie;
				}
                if(lie<SIZE-1){
                    y2=lie+1;
                }
                else{
                	y2=lie;
				}
                int i,j;
                for(i=x1;i<=x2;i++){
                    for(j=y1;j<=y2;j++){
                        if(aRecordBoard[i][j]==NONE){
                            bool flag_cf=true;
                            int t;
                            for(t=0;t<cunchu_index;t++){
                                if(i==cunchu[t].hang && j==cunchu[t].lie){
                                    flag_cf=false;
                                    break;
                                }
                            }
                            if(!flag_cf){  //说明该点已经判断过了，直接到下一个点去
                                continue;
                            }
                            else{
                                cunchu[cunchu_index].hang=i;
                                cunchu[cunchu_index].lie=j;
                                cunchu_index++;
                            }
                            check_ban_and_value(i,j,BLACK);
                            check_ban_and_value(i,j,WHITE);

                            //如果是轮白棋则不用判断，如果轮黑棋但这步是禁手，则也不放入All_Points中
                            if(color==WHITE || judge_if_banhand(i,j,color,NO_NEED_INF)){
                                if(Value_Board[i][j].direction[color-1].Right_5){   //如果己方有终结手需要标记出来
                            		Bisha->hang=i;
                            		Bisha->lie=j;
                                	*flag_if_win=true;
                                	return ++index;
                            	}
                                int ag_color = (color==WHITE? BLACK : WHITE);
                            	if( (ag_color==WHITE || judge_if_banhand(i,j,ag_color,NO_NEED_INF)) && Value_Board[i][j].direction[ag_color-1].Right_5){
                            		Bidang->hang=i;
                            		Bidang->lie=j;
                            		*flag_will_lose=true;
								}
                                All_Points[index].p.hang=i;
                                All_Points[index].p.lie=j;
                                if(flag){
                                    evaluate_value(i,j);
                                    All_Points[index].score=Value_Board[i][j].allscore;
                                }
                                index++;
                            }
                        }
                    }
                }
            }
        }
    }
    return index;
}

void quicksort(PTR_To_Points s, int left, int right){
    int i,j;
    if(left<right){
        i=left;
        j=right+1;
        while(1){
            do{
                i++;
            }while(i<right && s[i].score<=s[left].score);
            do{
                j--;
            }while(j>left && s[j].score>=s[left].score);
            if(i<j){
                int temp;
                temp=s[i].score;
                s[i].score=s[j].score;
                s[j].score=temp;
                temp=s[i].p.hang;
                s[i].p.hang=s[j].p.hang;
                s[j].p.hang=temp;
                temp=s[i].p.lie;
                s[i].p.lie=s[j].p.lie;
                s[j].p.lie=temp;
            }
            else{
                break;
            }
        }
        int temp;
        temp=s[j].score;
        s[j].score=s[left].score;
        s[left].score=temp;
        temp=s[j].p.hang;
        s[j].p.hang=s[left].p.hang;
        s[left].p.hang=temp;
        temp=s[j].p.lie;
        s[j].p.lie=s[left].p.lie;
        s[left].p.lie=temp;
        quicksort(s,j+1,right);
        quicksort(s,left,j-1);
    }
}

int Find_base_point(int color){
    PTR_To_Points All_Points;     //所有选点
    All_Points=(PTR_To_Points)malloc(sizeof(Points)*225);
    bool flag_if_win=false;
    bool flag_will_lose=false;
    Point Bisha,Bidang;
    int n=Find_Points(All_Points,color,NO_NEED_VALUE, &flag_if_win, &Bisha, &flag_will_lose, &Bidang);
    if(flag_if_win){
        //说明底层轮己方下时有终结手，返回一个极大值
        return 200000;
    }
    if(flag_will_lose){
    	//说明底层轮己方下时只能去堵对面的，只有一种选择 
    	aRecordBoard[Bidang.hang][Bidang.lie]=color;
    	int only=evaluate_jumian_value(color);
    	aRecordBoard[Bidang.hang][Bidang.lie]=NONE; 
    	return only;
	}
    int i;
    int value,max;
    max=-100000;
    for(i=0;i<n;i++){
        aRecordBoard[All_Points[i].p.hang][All_Points[i].p.lie]=color;
        value=evaluate_jumian_value(color);
        aRecordBoard[All_Points[i].p.hang][All_Points[i].p.lie]=NONE;
        if(value>max){
            max=value;
        }
    }
    return max;
}

int Best_choice(int color, PTR_To_Point final, int depth, int a, int b, char able){
	if(able=='S' || able=='s'){
		WIDTH=Base_Width;
	}
	else if(able=='E' || able=='e'){
		switch(depth){
        	case 8:
            	WIDTH=12;
            	break;
        	case 7:
            	WIDTH=10;
            	break;
        	case 6:
            	WIDTH=10;
            	break;
        	case 5:
            	WIDTH=7;
            	break;
        	case 4:
            	WIDTH=7;
            	break;
        	case 3:
            	WIDTH=7;
            	break;
        	case 2:
            	WIDTH=7;
            	break;
        	case 1:
            	WIDTH=7;
            	break;
        	case 0:
            	WIDTH=14;
            	break;
        	default:
            	printf("error");
            	break;
    	} 
	}
    //递归终止条件为到达底层，此时要根据局面分选点
    if(depth==0){
        int value=Find_base_point(color);
        return value;
    }
    //若非底层，则应该根据点分选点
    int next_color=(color==BLACK? WHITE : BLACK);   //判断一下一步的棋子颜色
    PTR_To_Points All_Points;     //所有选点
    All_Points=(PTR_To_Points)malloc(sizeof(Points)*225);
    if(All_Points==NULL){
    	printf("malloc error");
    	exit(1);
	}
	bool flag_if_win=false;
    bool flag_will_lose=false;
    Point Bisha,Bidang;
    int n=Find_Points(All_Points,color,NEED_VALUE, &flag_if_win, &Bisha, &flag_will_lose, &Bidang);
	//如果轮到color方，此步能终结比赛 
    if(flag_if_win){
        if(depth%2==1){
            //即己方落子后，发现对方下一步能终结比赛，说明己方该落子不合适，应返回一个极小值
            return -200000;
        }
        else{
            //即对方落子(或初始局面)时，我方下一步能终结比赛，说明对方该落子不合适，应返回一个极大值
            //同时如果此时是根节点即初始局面，则应当返回己方终结手的坐标
            if(depth==DEPTH){
                final->hang=Bisha.hang;
                final->lie=Bisha.lie;
            }
            return 200000;
        }
    }
	if(n<WIDTH){   //即可找的点少于14个 
		WIDTH=n; 
	}
	int temp_width=WIDTH;
	
    quicksort(All_Points,0,n-1);
    int i,j;
    PTR_To_Points choice;       //最优的WIDTH个选点
    choice=(PTR_To_Points)malloc(sizeof(Points)*WIDTH);
    //将ALL_Points中的最优的WIDTH个点复制到choice[]中去
    for(i=0,j=n-1;i<WIDTH;i++,j--){
        choice[i].p=All_Points[j].p;
        choice[i].score=All_Points[j].score;
        choice[i].min=a;    //继承来自父节点的α
        choice[i].max=b;    //继承来自父节点的β
    }
    int *value;  //记录每个子节点的回推值
    value=(int *)malloc(sizeof(int)*WIDTH);
    for(i=0;i<temp_width;i++){
        //step1:先把字落上去
        aRecordBoard[choice[i].p.hang][choice[i].p.lie]=color;
        //step2:根据下面的值回推回来
        value[i]=Best_choice(next_color,final,depth-1,a,b,able);
        //step3:根据回推值改变α和β
        //如果是偶数层(己方落子,取子节点回推值中的最大值，因此改变下限min)
        if(depth%2==0){
            if(value[i]>a){
                a=value[i];        //若比下限大，则改变下限
                //顶层根节点时偶数层，需要知道落点位置
                if(depth==DEPTH){     
                    final->hang=choice[i].p.hang;
                    final->lie=choice[i].p.lie;
                }
            }
            if(a>=b){    //即α>=β，不用再往下搜索了，本层给上层返回α
            	aRecordBoard[choice[i].p.hang][choice[i].p.lie]=NONE;
                return a;
            }
        }
        //如果是奇数层(对方落子，取子节点回推值中的最小值，因此改变上限max)
        else{
            if(value[i]<b){
                b=value[i];
            }
            if(a>=b){    //即α>=β，不用往下搜索栏，本层给上层返回β 
            	aRecordBoard[choice[i].p.hang][choice[i].p.lie]=NONE;
                return b;
            }
        }
        //step4:把落的子擦除
        aRecordBoard[choice[i].p.hang][choice[i].p.lie]=NONE;
    }
    if(depth%2==0){
        return a;
    }
    else{
        return b;
    }
}

void Human_vs_Human(void){
    initRecordBorard();               //初始化棋盘
    printf("Work of 陈远腾\n");
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
	displayBoard();
    printf("\n人人对战开始：由黑棋先落子：\n");
    int count=1;
    while(1){
        printf("轮到黑棋下第%d手棋：\n",count);
        black_go();
        count++;
        if(judge_result()){
            printf("黑棋胜利，游戏结束\n");
            break;
        }
        printf("轮到白棋下第%d手棋：\n",count);
        count++;
        white_go();
        if(judge_result()){
            printf("白棋胜利，游戏结束\n");
            break;
        }
    }
}

void Human_vs_AI(void){
	printf("选择算力:(E的算力略高)(S/E):");
	scanf("%c",&able);
	if(able!='S' && able!='E' && able!='s' && able!='e'){
		printf("输入错误，请重新输入(S/E)");
		scanf("%c",&able);
	}
	if(able=='S' || able=='s'){
		DEPTH=6;
	}
	else if(able=='E' || able=='e'){
		DEPTH=8;
	}
    initRecordBorard();               //初始化棋盘
    printf("Work of 陈远腾\n");
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
	displayBoard();
    int choice;
    printf("\n人机对战开始：请人选择先后手:（输入1选择先手，输入2选择后手)\n");
    scanf("%d",&choice);
    char eat=getchar();
    int count=1;
    clock_t start,end;
    if(choice==1){
        while(1){
            printf("轮到黑棋下第%d手棋：\n",count);
            black_go();
            count++;
            if(judge_result()){
                printf("黑棋胜利，游戏结束\n");
                break;
            }
            printf("轮到白棋下第%d手棋：\n",count);
            start=clock();
            AI_white_go(count,able);
            end=clock();
#if 0
            printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
#endif
            count++;
            if(judge_result()){
                printf("白棋胜利，游戏结束\n");
                break;
            }
        }
    }
    else if(choice==2){
        while(1){
            printf("轮到黑棋下第%d手棋：\n",count);
            start=clock();
            AI_black_go(count,able);
            end=clock();
#if 0
            printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
#endif
            count++;
            if(judge_result()){
                printf("黑棋胜利，游戏结束\n");
                break;
            }
            printf("轮到白棋下第%d手棋：\n",count);
            white_go();
            count++;
            if(judge_result()){
                printf("白棋胜利，游戏结束\n");
                break;
            }
        }
    }
    else{
        printf("wrong order");
    }
}

void AI_vs_AI(void){
	able='E';
	int count=1;
	clock_t start,end;
    while(1){
        printf("轮到黑棋下第%d手棋：\n",count);
        start=clock();
        AI_black_go(count,able);
        end=clock();
        printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
        count++;
        if(judge_result()){
            printf("黑棋胜利，游戏结束\n");
            break;
        }
        printf("轮到白棋下第%d手棋：\n",count);
        start=clock();
        AI_white_go(count,able);
        end=clock();
        printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
        count++;
        if(judge_result()){
            printf("白棋胜利，游戏结束\n");
            break;
        }
    }
}

void black_go(){
    char inf[20];
    printf("请输入欲落黑子子坐标(字母,数字):");
    scanf("%s",inf);
    if(strcmp(inf,leave)==0){              //为了适应退出游戏的功能 leave="quit"
        exit(0);
    }
    char eat=getchar();
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("输入形式错误，请以(字母,数字)形式重新输入\n");
        black_go();
        return;
    }
    int hang;
	if(isdigit(inf[2]) && isdigit(inf[3])){           //有两位数字时 
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{
		hang = 15-(inf[2]-'0');    //y轴对应行
	}

    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    int lie = inf[0]-'A';    //x轴对应列
    if(aRecordBoard[hang][lie]!=NONE){
        printf("你开挂是吧？");
        printf("\n请重新落子\n");
        black_go();
        return;
    }

    check_ban_and_value(hang, lie, BLACK);
    if(!judge_if_banhand(hang,lie,BLACK,NEED_INF)){
        printf("黑棋走了禁手，判定白棋胜利");
        system("pause");
    }

    aRecordBoard[hang][lie]=BLACK;    //黑子对应值1
    recordtoDisplayArray(hang,lie);   //引入参数hang，lie是为了标记新落子
    displayBoard();
    //判断是否悔棋
    printf("是否希望悔棋？(Y/任意键)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
        printf("请慎重落子！");
        black_go();
    }
}

void white_go(){
    char inf[20];
    printf("请输入欲落白子子坐标(字母,数字):");
    scanf("%s",inf);
    if(strcmp(inf,leave)==0){
        exit(0);
    }
    char eat=getchar();
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("输入形式错误，请以(字母,数字)形式重新输入\n");
        white_go();
        return;
    }
    int hang;
	if(isdigit(inf[2]) && isdigit(inf[3])){           //有两位数字时 
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{
		hang = 15-(inf[2]-'0');    //y轴对应行
	}

    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    int lie = inf[0]-'A';          //x轴对应列
    if(aRecordBoard[hang][lie]!=NONE){
        printf("你开挂是吧？");
        printf("\n请重新落子\n");
        white_go();
        return;
    }

    //白棋不用判断禁手，且这里是人人对战，不用价值判断，所以不用date_imformation

    aRecordBoard[hang][lie]=WHITE;      //白子对应值2
    recordtoDisplayArray(hang,lie); //引入参数hang，lie是为了标记新落子
    displayBoard();
    //判断是否悔棋
    printf("是否希望悔棋？(Y/任意键)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
        printf("请慎重落子！");
        white_go();
    }
}

void AI_black_go(int count, char able){
    if(count==1){
		Buju_Black(7,7);
        return;
    }
#if 1 
    if(count==3){
    	//白棋第二步下右下或左下 
    	if(aRecordBoard[8][8]==WHITE || aRecordBoard[8][6]==WHITE){
			Buju_Black(6,7);
        	return;
		}
		//白棋第二步下右上或左上 
    	if(aRecordBoard[6][6]==WHITE || aRecordBoard[6][8]==WHITE){
			Buju_Black(8,7);
        	return;
		}
		//白棋第二步下下面或左边
    	if(aRecordBoard[8][7]==WHITE || aRecordBoard[7][6]==WHITE){
			Buju_Black(8,6);
        	return;
		}
		//白棋第二步下上面或右边
    	if(aRecordBoard[6][7]==WHITE || aRecordBoard[7][8]==WHITE){
			Buju_Black(6,8);
        	return;
		}
	}
	if(count==5){
		//第二步白棋下右下 
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[8][7]==WHITE){
			Buju_Black(8,6);
        	return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[5][7]==WHITE){
			Buju_Black(6,8);
        	return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[7][9]==WHITE){
			Buju_Black(7,6);
        	return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[8][6]==WHITE){
			Buju_Black(8,7);
        	return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[9][7]==WHITE){
			Buju_Black(7,9);
        	return;
		}
		//第二步白棋下左下 
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[8][7]==WHITE){
			Buju_Black(8,8);
        	return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[5][7]==WHITE){
			Buju_Black(6,6);	
        	return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[7][5]==WHITE){
			Buju_Black(7,8);
        	return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[8][8]==WHITE){
			Buju_Black(8,7);
        	return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[9][7]==WHITE){
			Buju_Black(7,5);
        	return;
		}
		//第二步白棋下右上 
		if(aRecordBoard[6][8]==WHITE&&aRecordBoard[9][7]==WHITE){
			Buju_Black(8,8);
        	return;
		}
		if(aRecordBoard[6][8]==WHITE&&aRecordBoard[6][7]==WHITE){
			Buju_Black(6,6);
        	return;
		}
		if(aRecordBoard[6][8]==WHITE&&aRecordBoard[7][9]==WHITE){
			Buju_Black(7,6);
        	return;
		}
		if(aRecordBoard[6][8]==WHITE&&aRecordBoard[6][6]==WHITE){
			Buju_Black(6,7);
        	return;
		}
		if(aRecordBoard[6][8]==WHITE&&aRecordBoard[5][7]==WHITE){
			Buju_Black(7,9);
        	return;
		}
		//第二步白棋下左上 
		if(aRecordBoard[6][6]==WHITE&&aRecordBoard[6][7]==WHITE){
			Buju_Black(6,8);
        	return;
		}
		if(aRecordBoard[6][6]==WHITE&&aRecordBoard[9][7]==WHITE){
			Buju_Black(8,6);	
        	return;
		}
		if(aRecordBoard[6][6]==WHITE&&aRecordBoard[7][5]==WHITE){
			Buju_Black(7,8);	
        	return;
		}
		if(aRecordBoard[6][6]==WHITE&&aRecordBoard[6][8]==WHITE){
			Buju_Black(6,7);	
        	return;
		}
		if(aRecordBoard[6][6]==WHITE&&aRecordBoard[5][7]==WHITE){
			Buju_Black(7,5);	
        	return;
		}
		
		//第二步白棋下下面
		if(aRecordBoard[8][7]==WHITE&& (aRecordBoard[9][5]==WHITE || aRecordBoard[9][6]==WHITE)){
			Buju_Black(6,8);
        	return;
		}
		if(aRecordBoard[8][7]==WHITE&&aRecordBoard[6][8]==WHITE){
			Buju_Black(9,6);	
        	return;
		}
		if(aRecordBoard[8][7]==WHITE&&aRecordBoard[7][8]==WHITE){
			Buju_Black(9,5);	
        	return;
		}
		if(aRecordBoard[8][7]==WHITE&&aRecordBoard[7][6]==WHITE){
			Buju_Black(6,5);	
        	return;
		}
		//第二步白棋下左面
		if(aRecordBoard[7][6]==WHITE&& (aRecordBoard[9][5]==WHITE || aRecordBoard[8][5]==WHITE)){
			Buju_Black(6,8);
        	return;
		}
		if(aRecordBoard[7][6]==WHITE&&aRecordBoard[6][8]==WHITE){
			Buju_Black(8,5);	
        	return;
		}
		if(aRecordBoard[7][6]==WHITE&&aRecordBoard[6][7]==WHITE){
			Buju_Black(9,5);	
        	return;
		}
		if(aRecordBoard[7][6]==WHITE&&aRecordBoard[8][7]==WHITE){
			Buju_Black(6,5);	
        	return;
		}
		//第二步白棋下上面
		if(aRecordBoard[6][7]==WHITE&& (aRecordBoard[5][8]==WHITE || aRecordBoard[5][9]==WHITE)){
			Buju_Black(8,6);
        	return;
		}
		if(aRecordBoard[6][7]==WHITE&&aRecordBoard[8][6]==WHITE){
			Buju_Black(5,8);	
        	return;
		}
		if(aRecordBoard[6][7]==WHITE&&aRecordBoard[7][6]==WHITE){
			Buju_Black(5,9);	
        	return;
		}
		if(aRecordBoard[6][7]==WHITE&&aRecordBoard[7][8]==WHITE){
			Buju_Black(5,6);	
        	return;
		}
		//第二步白棋下右面
		if(aRecordBoard[7][8]==WHITE&& (aRecordBoard[5][9]==WHITE || aRecordBoard[6][9]==WHITE)){
			Buju_Black(8,6);
        	return;
		}
		if(aRecordBoard[7][8]==WHITE&&aRecordBoard[8][7]==WHITE){
			Buju_Black(5,9);	
        	return;
		}
		if(aRecordBoard[7][8]==WHITE&&aRecordBoard[8][6]==WHITE){
			Buju_Black(6,9);	
        	return;
		}
		if(aRecordBoard[7][8]==WHITE&&aRecordBoard[6][7]==WHITE){
			Buju_Black(5,6);	
        	return;
		}
	}
	if(count==7){
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[8][6]==WHITE&&aRecordBoard[6][8]==BLACK){
			Buju_Black(8,9);
			return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[8][6]==WHITE&&aRecordBoard[6][6]==BLACK){
			Buju_Black(8,5);
			return;
		}
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[5][9]==WHITE){
			Buju_Black(5,8);
			return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[5][5]==WHITE){
			Buju_Black(5,6);
			return;
		}
	}
	if(count==9){
		if(aRecordBoard[8][8]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[5][9]==WHITE&&aRecordBoard[7][6]==WHITE){
			Buju_Black(6,9);
			return;
		}
		if(aRecordBoard[8][6]==WHITE&&aRecordBoard[5][7]==WHITE&&aRecordBoard[5][5]==WHITE&&aRecordBoard[7][8]==WHITE){
			Buju_Black(6,5);
			return;
		}
	}
#endif
    PTR_To_Point final;
    final=(PTR_To_Point)malloc(sizeof(Point)*1);
    int value=Best_choice(BLACK, final, DEPTH, Fuwuqiong, Zhengwuqiong,able);
    printf("\n黑子落子位置为(%d,%c)\n",15-final->hang,final->lie+'A');
    aRecordBoard[final->hang][final->lie]=BLACK;
    
    recordtoDisplayArray(final->hang,final->lie); 
    displayBoard();
}

void AI_white_go(int count, char able){
    PTR_To_Point final;
    final=(PTR_To_Point)malloc(sizeof(Point)*1);
    if(count==2){
		if(aRecordBoard[7][7]==BLACK){
			aRecordBoard[8][8]=WHITE;
			recordtoDisplayArray(8,8);
			displayBoard();
			return;
		}
    }
#if 1
    if(count==4){
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][7]==BLACK){
    		Buju_White(5,7);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[7][6]==BLACK){
    		Buju_White(7,5);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[7][8]==BLACK){
    		Buju_White(7,9);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[8][7]==BLACK){
    		Buju_White(9,7);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][8]==BLACK){
    		Buju_White(8,6);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[8][6]==BLACK){
    		Buju_White(6,8);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][6]==BLACK){
    		Buju_White(8,7);
    		return;
		}
	}
	if(count==6){
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][7]==BLACK && aRecordBoard[6][8]==BLACK){
    		Buju_White(8,6);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][7]==BLACK && aRecordBoard[7][6]==BLACK){
    		Buju_White(7,5);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[6][6]==BLACK && aRecordBoard[8][6]==BLACK){
    		Buju_White(6,8);
    		return;
		}
    	if(aRecordBoard[7][7]==BLACK && aRecordBoard[7][6]==BLACK && aRecordBoard[8][6]==BLACK){
    		Buju_White(6,8);
    		return;
		}
	}
#endif
	int value=Best_choice(WHITE, final, DEPTH, Fuwuqiong, Zhengwuqiong,able);
	printf("\n白子落子位置为(%d,%c)\n",15-final->hang,final->lie+'A');
    aRecordBoard[final->hang][final->lie]=WHITE;
    
    recordtoDisplayArray(final->hang,final->lie); 
    displayBoard();
}

void Buju_Black(int hang, int lie){
    aRecordBoard[hang][lie]=BLACK;
    recordtoDisplayArray(hang,lie); 
    displayBoard();
    return;	
}

void Buju_White(int hang, int lie){
    aRecordBoard[hang][lie]=WHITE;
    recordtoDisplayArray(hang,lie); 
    displayBoard();
    return;	
}

bool judge_result(void){
    int i,j;
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            if(aRecordBoard[i][j]==1){
                if(j+4<SIZE && aRecordBoard[i][j+1] == 1 && aRecordBoard[i][j+2] == 1 && aRecordBoard[i][j+3] == 1 && aRecordBoard[i][j+4] == 1){
                    return true;
                }
                if(i+4<SIZE && aRecordBoard[i+1][j] == 1 && aRecordBoard[i+2][j] == 1 && aRecordBoard[i+3][j] == 1 && aRecordBoard[i+4][j] == 1){
                    return true;
                }
                if(i+4<SIZE && j+4<SIZE && aRecordBoard[i+1][j+1] == 1 && aRecordBoard[i+2][j+2] == 1 && aRecordBoard[i+3][j+3] == 1 && aRecordBoard[i+4][j+4] == 1){
                    return true;
                }
                if(i+4<SIZE && j-4>=0 && aRecordBoard[i+1][j-1] == 1 && aRecordBoard[i+2][j-2] == 1 && aRecordBoard[i+3][j-3] == 1 && aRecordBoard[i+4][j-4] == 1){
                	return true;
				}
            }
            if(aRecordBoard[i][j]==2){
                if(j+4<SIZE && aRecordBoard[i][j+1] == 2 && aRecordBoard[i][j+2] == 2 && aRecordBoard[i][j+3] == 2 && aRecordBoard[i][j+4] == 2){
                    return true;
                }
                if(i+4<SIZE && aRecordBoard[i+1][j] == 2 && aRecordBoard[i+2][j] == 2 && aRecordBoard[i+3][j] == 2 && aRecordBoard[i+4][j] == 2){
                    return true;
                }
                if(i+4<SIZE && j+4<SIZE && aRecordBoard[i+1][j+1] == 2 && aRecordBoard[i+2][j+2] == 2 && aRecordBoard[i+3][j+3] == 2 && aRecordBoard[i+4][j+4] == 2){
                    return true;
                }
                if(i+4<SIZE && j-4>=0 && aRecordBoard[i+1][j-1] == 2 && aRecordBoard[i+2][j-2] == 2 && aRecordBoard[i+3][j-3] == 2 && aRecordBoard[i+4][j-4] == 2){
                	return true;
				}
            }
        }
    }
    return false;
}

void initRecordBorard(void){
//通过双重循环，将aRecordBoard清0
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE;j++){
			aRecordBoard[i][j]=0;
		}
	}
}
void recordtoDisplayArray(int hang, int lie){
//第一步：将aInitDisplayBoardArray中记录的空棋盘，复制到aDisplayBoardArray中
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
//第二步：扫描aRecordBoard，当遇到非0的元素，将●或者◎复制到aDisplayBoardArray的相应位置上
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE;j++){
			if(aRecordBoard[i][j]!=0){
				if(aRecordBoard[i][j]==1){
                    if(i==hang && j==lie){
                        aDisplayBoardArray[i][2*j]=play1CurrentPic[0];
					    aDisplayBoardArray[i][2*j+1]=play1CurrentPic[1];
                    }
                    else{
                        aDisplayBoardArray[i][2*j]=play1Pic[0];
					    aDisplayBoardArray[i][2*j+1]=play1Pic[1];
                    }
				}
				else if(aRecordBoard[i][j]==2){
                    if(i==hang && j==lie){
                        aDisplayBoardArray[i][2*j]=play2CurrentPic[0];
				        aDisplayBoardArray[i][2*j+1]=play2CurrentPic[1];
                    }
                    else{
					    aDisplayBoardArray[i][2*j]=play2Pic[0];
					    aDisplayBoardArray[i][2*j+1]=play2Pic[1];
                    }
				}
			}
		}
	}
//注意：aDisplayBoardArray所记录的字符是中文字符，每个字符占2个字节。●和◎也是中文字符，每个也占2个字节。
} 
void displayBoard(void){
	int i;
	//第一步：清屏
	//system("clear");   //清屏 
#if 0 
	system("clr");
#endif
	//第二步：将aDisplayBoardArray输出到屏幕上
	for(i=0;i<SIZE;i++){
		if(i<6){
			printf("%d %s\n",SIZE-i,aDisplayBoardArray[i]);
		}
		else{
			printf("%d  %s\n",SIZE-i,aDisplayBoardArray[i]);
		}
	}
	//第三步：输出最下面的一行字母A B .... 
	printf("    A B C D E F G H I J K L M N O\n");
} 

int main()
{
	system("color f8"); 
    char choice;
    printf("模式选择-人人/人机/AI对战(A/B/C):");
    scanf("%c",&choice);
    char eat=getchar();
    if(choice=='A' || choice=='a'){
        Human_vs_Human();
    }
    else if(choice=='B' || choice=='b'){
        Human_vs_AI();
    }
    else if(choice=='C' || choice=='c'){
        AI_vs_AI();
    }
    else{
    	printf("wrong order!");
	}
    return 0;
}
