/*采取GBK编码
1.如果用DEVc++打开则中文是正常的
2.如果用vscode打开则需要更换编码方式
方法是点击右下角的UTF-8，点击通过编码重新打开
在搜索栏搜索"GBK",选中GBK后可以看到乱码消失，中文正常
具体操作截图在教程pdf中
*/

//头文件
//先不管为啥要这些头，include上就完事了
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

/*------------------------------end------------------------------*/

/*--------------------------全局变量部分--------------------------*/

/*棋盘使用的是GBK编码，每一个中文字符占用2个字节。
棋盘基本模板 
如果你使用vscode打开，你可能发现棋盘纵向是扁的且
有报错，而用DEVc++或虚拟机的gedit打开时棋盘显示是
正常的且不报错。
不要担心，我在"棋盘说明.pdf"中专门蒋捷了
关于棋盘编码的说明及如何在vscode中正常显示
棋盘的解决方案。
*/
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

//aDisplayBoardArray数组用于显示棋盘
char aDisplayBoardArray[SIZE][SIZE*CHARSIZE+1];

/*aRecordBoard数组用于记录棋盘格局（并非为了展示,而是为了我们计算）
我们可以调用recordtoDisplayArray函数将aRecordBoard的记录转换为
aDisplayBoardArray的内容，然后再调用displayBoard打印出棋盘。

aRecordBoard[i][j]
= NONE     :代表[i][j]点为空，没人落子 
= WHITE	   :代表[i][j]点已落白子
= BLACK    :代表[i][j]点已落黑子
回忆一下！我们用宏定义了NONE,WHITE,BLACK分别为0,1,2
*/
int aRecordBoard[SIZE][SIZE];

/*
"●"为已落的黑棋子
"▲"为待落的黑棋子（也可以理解为刚落的）
"◎"为已落的白棋子
"△"为待落的白棋子（也可以理解为刚落的）
*/
char play1Pic[]="●";
char play1CurrentPic[]="▲"; 

char play2Pic[]="◎";
char play2CurrentPic[]="△";

//leave[]是为了输入“quit”退出游戏
char leave[]="quit";

/*------------------------------end------------------------------*/

/*----------------------------函数声明部分------------------------*/

//part1:关于棋盘的部分

//初始化棋盘——实际上就是将全局变量的aRecordBoard置为NONE
void initRecordBorard(void);
//将aRecordBoard的内容转换为aDisplayBoardArray的内容
void recordtoDisplayArray(int hang,int lie);
//打印出aDisplayBoardArray的内容
void displayBoard(void);


//part2:人人对弈部分
void Human_vs_Human(void);
void black_go(void);	  //人类黑棋落子
void white_go(void);	  //人类白棋落子
bool judge_result(void);  //判断结果

/*------------------------------end------------------------------*/

/*----------------------------函数定义部分------------------------*/

//part1:关于棋盘的部分

//初始化棋盘
void initRecordBorard(void)
{
	//通过双重循环，将aRecordBoard清0
	int i,j;
	for(i=0 ; i<SIZE ; i++){
		for(j=0 ; j<SIZE ; j++){
			aRecordBoard[i][j] = NONE;
		}
	}
}

//将initRecordBorard中存储的信息呈现在aDisplayBoardArray上
//参数hang，lie 为当前要落子位置的横、纵坐标
void recordtoDisplayArray(int hang, int lie){

	//第一步：将aInitDisplayBoardArray中记录的空棋盘，复制到aDisplayBoardArray中
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j] = aInitDisplayBoardArray[i][j];        
		}
	}

	//第二步：扫描aRecordBoard，当遇到非0的元素，将●或者◎复制到aDisplayBoardArray的相应位置上
	//注意每个棋子占两个字符，因此复制时要连续复制两个字符
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE;j++){
			if(aRecordBoard[i][j] != NONE){
				if(aRecordBoard[i][j] == BLACK){
                    if(i==hang && j==lie){
						//正要落子——play1CurrentPic
                        aDisplayBoardArray[i][2*j] = play1CurrentPic[0];
					    aDisplayBoardArray[i][2*j+1] = play1CurrentPic[1];
                    }
                    else{
						//过往的落子——play1Pic
                        aDisplayBoardArray[i][2*j] = play1Pic[0];
					    aDisplayBoardArray[i][2*j+1] = play1Pic[1];
                    }
				}
				else if(aRecordBoard[i][j] == WHITE){
                    if(i==hang && j==lie){
						//正要落子——play2CurrentPic
                        aDisplayBoardArray[i][2*j] = play2CurrentPic[0];
				        aDisplayBoardArray[i][2*j+1] = play2CurrentPic[1];
                    }
                    else{
						//过往的落子——play2Pic
					    aDisplayBoardArray[i][2*j] = play2Pic[0];
					    aDisplayBoardArray[i][2*j+1] = play2Pic[1];
                    }
				}
			}
		}
	}
} 

//打印展示棋盘
void displayBoard(void){
	//第一步：清屏——系统调用system("clr")
	//往后写就会知道，有时候我们希望看过往的棋盘情况
	//因此我设置了宏CLEAR决定是否清屏
	//CLEAR=1：清屏 ； CLEAR=0：不清屏
#if CLEAR
	system("clr");
#endif

	//第二步：将aDisplayBoardArray输出到屏幕上
	/*
	hint1：
	仔细观察棋盘可以发现：棋盘坐标从左下角出发，
	向上为0-SIZE-1 ; 向右为A-O
	但我们打印是是从上向下打印的，因此纵坐标总是 SIZE - i
	
	hint2：
	另一方面，注意到纵坐标的10-14比较长，如果我们按相同方式
	打印，会出现棋盘对不齐的情况，因此对纵坐标10-14，
	我们在前面少空一格，就能对齐了

	hint3：
	我们在打印时只遍历行，每次用字符串占位符%s打印一行，
	能这样做的原因跟C语言指针有关——武老师会讲hh
	*/
	int i;
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

	//当然你不妨自己改一改displayBoard函数运行试一试，可以加深
	//对棋盘的理解
}


//part2：人人对弈

void Human_vs_Human(void){

	//下棋前当然要先初始化棋盘——调用initRecordBorard函数
	//将aRecordBoard的值置为NONE
    initRecordBorard();

	//按你武老师的要求——打印一下谁写的
    printf("Work of xxx\n");

	/*或许你已经忘了，我们用来打印棋盘的aDisplayBoardArray
	此时还是空的，其实我也忘了，看到以下几行代码我想了半天在干嘛hhh
	我在干嘛？ 初始化了一下aDisplayBoardArray数组，复制了一份
	标准的aInitDisplayBoardArray棋盘到aDisplayBoardArray上
	*/
	int i,j;
	for(i=0 ; i<SIZE ; i++){
		for(j=0 ; j<SIZE*CHARSIZE+1 ; j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}

	//调用displayBoard函数展示一下初始棋盘先
	displayBoard();

	//空棋盘准备好了，开始下棋！
    printf("\n人人对战开始：由黑棋先落子：\n");

    int count=1;   //计数当前步数

    while(1){
		//双方持续落子直到有一方获得了胜利
		//因此我们用一个while循环模拟对局
		//直到有人胜利再退出循环

        printf("轮到黑棋下第%d手棋：\n",count);
		//人类黑方落子
        black_go();
        count++;  //落子计数+1
        if(judge_result()){
            printf("黑棋胜利，游戏结束\n");
            break;
        }

        printf("轮到白棋下第%d手棋：\n",count);
		//人类白方落子
        white_go();
		count++;  //落子计数+1
        if(judge_result()){
            printf("白棋胜利，游戏结束\n");
            break;
        }
    }
}

void black_go(void){

    char inf[20];
    printf("请输入欲落黑子子坐标(字母,数字):");
    scanf("%s",inf);

	//char leave[] = "quit"
	//当输入为quit时，退出游戏
    if(strcmp(inf,leave)==0){              
        exit(0);
    }

	/*
	char eat = getchar();
	这行代码的作用是为了适应C语言scanf函数的缺陷
	C语言的scanf函数在用户输入'\n'即回车时结束，但
	程序并没有将'\n'读入inf，也没有将其舍弃，而是将其
	留在缓冲区，如果不用这行代码将这个残存的'\n'吃了
	在下回调用scanf时就会自动读入这个'\n'，也因此直接结束读入。

	hint: 在c语言中使用scanf后加一个char eat=getchar();是一个
	好习惯，能够避免很多难以发现的bug
	当然将char命名为eat是笔者的个人习惯，不是C语言推荐的惯例
	*/
    char eat=getchar();

	/*
	正常输入的情况下
	inf[0] = A/a / B/b / ... / Z/z   (程序应该包容用户大小写)
	inf[1] = ,
	inf[2:3] = 0 / 1 / 2 / ... / 14
	*/

	//以下考虑了一些输入错误的情况，当然也没有囊括全部情况，你可以完善一下
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("输入形式错误，请以(字母,数字)形式重新输入\n");
		//如果输入错误，不要直接退出程序，程序需要一些包容的交互性
		//只要重新调用black_go()就能请黑棋重新输入
        black_go();
        return;
    }

	/*可能笔者写这些程序的时候还不能熟练区分row和col吧
	所以用了 hang 和 lie 表示 行 和 列 
	*/
	

	/*			      对行hang        

	注意当输入行时，输入的是字符串，即inf的2到3位
	可能是1位或是2位，当输入两位时我们需要做一些以下处理
	将字符转换为数字
	
	同时在打印棋盘函数中提到过，棋盘上看到的横坐标与实际上
	的二维数组第一个下标是相反的
	即 fact_hang = 15 - look_hang
	纵坐标不存在这个问题，看到的和实际的顺序相同
	*/

    int hang;

	//isdigit是C语言为数不多的自提供的有点用的函数
	//作用就是字面意思——是否为字符('0' 至 '9')
	if(isdigit(inf[2]) && isdigit(inf[3])){     //有两位数字时 
		//hint: when inf[2] = '5' 
		//int num = inf[2] - '0';
		//now num is 5 but not '5'
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{										//只有一位数字时
		hang = 15-(inf[2]-'0');    
	}


	/*					对列lie
	对列，不存在看到的和实际的顺序相反的问题，唯一要注意到的是我们应当
	包容用户输入大写字母或者小写字母
	hint: 'a' - 'A' = 'b' - 'B' = 32
	*/

	int lie;

	//如果为小写，则转换为大写
    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    lie = inf[0]-'A';   

	//我们需要避免某一方将子落在已有子的位置的情况
    if(aRecordBoard[hang][lie]!=NONE){
        printf("你开挂是吧？");
        printf("\n请重新落子\n");
		//跟输入错误的情况相同，需要重新输入
        black_go();
        return;
    }

	/*
	对于黑棋，实际上我们除了要判断其是否将子落在了
	已有子的位置，还要判断黑棋下的是否为禁手
	但在初级教程中，我们暂不考虑禁手
	在后续进程中，我们将完善这里的禁手判断
	if(Judge_if_banhand(hang,lie))
	{
		printf("禁手！");
		black_go();
		return;
	}
	*/

	//更新棋盘
    aRecordBoard[hang][lie]=BLACK;
	//将aRecordBoard的信息转入arecordtoDisplayArray
    recordtoDisplayArray(hang,lie);
	//打印棋盘   
    displayBoard();

	//人人对战包容用户悔棋，当然悔棋前要询问用户
    //判断是否悔棋

	/*
	看到char laizi 我是蚌埠住了，属实逆天
	*/
    printf("是否希望悔棋？(Y/任意键)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
		printf("你干嘛，哎呦！\n");
		printf("请慎重落子！\n");
        black_go();
		return;
    }
}

void white_go(void){
	//white_go跟black_go是镜像的
	//唯一区别是白棋不用判断禁手

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

    //白棋不用判断禁手

    aRecordBoard[hang][lie]=WHITE;      //白子对应值2
    recordtoDisplayArray(hang,lie); //引入参数hang，lie是为了标记新落子
    displayBoard();
	
    //判断是否悔棋
    printf("是否希望悔棋？(Y/任意键)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
		printf("你干嘛，哎呦！\n");
		printf("请慎重落子！\n");
        white_go();
    }
}

//判断棋局结果
//初级教程中的判断结果函数好处是逻辑清晰很好看懂；坏处是计算冗余有损效率。在后续我们追求速度时，应当回来优化judge_result函数
bool judge_result(void){
    int i,j;
    for(i=0 ; i<SIZE ; i++){
        for(j=0 ; j<SIZE ; j++){
            if(aRecordBoard[i][j] == BLACK){
                if(j+4<SIZE && aRecordBoard[i][j+1] == BLACK && aRecordBoard[i][j+2] == BLACK && aRecordBoard[i][j+3] == BLACK && aRecordBoard[i][j+4] == BLACK){
                    return true;
                }
                if(i+4<SIZE && aRecordBoard[i+1][j] == BLACK && aRecordBoard[i+2][j] == BLACK && aRecordBoard[i+3][j] == BLACK && aRecordBoard[i+4][j] == BLACK){
                    return true;
                }
                if(i+4<SIZE && j+4<SIZE && aRecordBoard[i+1][j+1] == BLACK && aRecordBoard[i+2][j+2] == BLACK && aRecordBoard[i+3][j+3] == BLACK && aRecordBoard[i+4][j+4] == BLACK){
                    return true;
                }
                if(i+4<SIZE && j-4>=0 && aRecordBoard[i+1][j-1] == BLACK && aRecordBoard[i+2][j-2] == BLACK && aRecordBoard[i+3][j-3] == BLACK && aRecordBoard[i+4][j-4] == BLACK){
                	return true;
				}
            }
            if(aRecordBoard[i][j] == WHITE){
                if(j+4<SIZE && aRecordBoard[i][j+1] == WHITE && aRecordBoard[i][j+2] == WHITE && aRecordBoard[i][j+3] == WHITE && aRecordBoard[i][j+4] == WHITE){
                    return true;
                }
                if(i+4<SIZE && aRecordBoard[i+1][j] == WHITE && aRecordBoard[i+2][j] == WHITE && aRecordBoard[i+3][j] == WHITE && aRecordBoard[i+4][j] == WHITE){
                    return true;
                }
                if(i+4<SIZE && j+4<SIZE && aRecordBoard[i+1][j+1] == WHITE && aRecordBoard[i+2][j+2] == WHITE && aRecordBoard[i+3][j+3] == WHITE && aRecordBoard[i+4][j+4] == WHITE){
                    return true;
                }
                if(i+4<SIZE && j-4>=0 && aRecordBoard[i+1][j-1] == WHITE && aRecordBoard[i+2][j-2] == WHITE && aRecordBoard[i+3][j-3] == WHITE && aRecordBoard[i+4][j-4] == WHITE){
                	return true;
				}
            }
        }
    }
    return false;
}

/*------------------------------end------------------------------*/

int main()
{
	/*
	system("color xx")是为了设置控制台的背景颜色
	细节我记不清楚了，你可以上csdn查查怎么改变控制台的背景色
	目的是让棋盘好看些
	弄些彩色背景来看是编程为数不多的乐趣了
	*/
	//system("color f8"); 

    char choice;
    printf("模式选择-人人/人机/AI对战(A/B/C):");
    scanf("%c",&choice);
    char eat=getchar();

    if(choice=='A' || choice=='a'){
        Human_vs_Human();
    }
    else if(choice=='B' || choice=='b'){
		//我们暂未实现人只因对战，所以先注释掉
        //Human_vs_AI();
    }
    else if(choice=='C' || choice=='c'){
		//我们暂未实现只因只因对战，所以先注释掉
        //AI_vs_AI();
    }
    else{
    	printf("wrong order!");
	}
    return 0;
}