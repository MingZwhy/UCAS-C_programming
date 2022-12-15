/*��ȡGBK����
1.�����DEVc++����������������
2.�����vscode������Ҫ�������뷽ʽ
�����ǵ�����½ǵ�UTF-8�����ͨ���������´�
������������"GBK",ѡ��GBK����Կ���������ʧ����������
���������ͼ�ڽ̳�pdf��
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*----------------------------�궨�岿��--------------------------*/

/*define bool,true,false��Ϊ����Ӧ
C����û��bool��������
����ʹ�úꡪ��TRUE and FALSE��Ϊ�˴���������Ķ�
*/
#define bool int
#define true 1
#define false 0

//�������ַ�������ʱ���õ�������������
#define SIZE 15
#define CHARSIZE 2

/*����������ÿ��������
NONE����������λ
BLACK������������
WHITE������������
*/
#define NONE 0
#define BLACK 1
#define WHITE 2

//�����ڴ�ӡ����ʱ�Ƿ�������
#define CLEAR 0

//��Ӧÿ����״�ļ�ֵ
#define Value_Right_5 50000
#define Value_Living_4 4320
#define Value_Rush_4 720
#define Value_Living_3 720
#define Value_Rush_3 100
#define Value_Living_2 120
#define Value_Rush_2 20

/*------------------------------end------------------------------*/

/*--------------------------ȫ�ֱ�������--------------------------*/

int aRecordBoard[SIZE][SIZE];       //���������ڼ�¼���̸��

//1:���ӵ����λ����Ϣ(Pos --> Position)
typedef struct Point_Pos
{
	int hang;
	int lie;
}Point_Pos, *Ptr_to_Point_Pos;

//2:������㴦��������Ϣ(info --> information)
typedef struct Point_Info
{
	/*
	*���ｫ������Ϸ�5������Ϊ�������͵�ԭ����
	*���ǲ�������������������ֻ�����������
	*��������һ����˵��ֵ�ǵݼ���
	*���Ǹ���info���Լ����ĳ��ļ�ֵ
	*/
	bool Too_long;     //���������ں�����ԣ�
	bool Right_5; 	   //�Ϸ���5��
	int Living_4;	   //�Ϸ�����
	int Rush_4;		   //�Ϸ���4
	int Living_3;	   //�Ϸ���3
	int Rush_3;		   //�Ϸ���3
	int Living_2;	   //�Ϸ���2
	int Rush_2;		   //�Ϸ���2
}Point_Info, *Ptr_to_Point_Info;

//3:���ڵ������ӵ�λ����Ϣ+��ֵ
typedef struct Point
{
	Point_Pos pos;
	int Score;		//��ֵ
	int Min;		//Min�����ڦ�-�¼�֦�㷨��
	int Max;		//Max�����ڦ�-�¼�֦�㷨��
}Point, *Ptr_to_Point;

//4:������ĳ�����Ϣ
/*
ע������3�������Ѿ������˰���ĳ�����ӵ�
λ����Ϣ+��ֵ��Ϣ��Point�ṹ�壬�����������̵�
�Ƕ�����������ĳ��ļ�ֵʱ������Ӧ�ý���Ժڵļ�ֵ
�Ͷ԰׵ļ�ֵ�������õ�һ��allscore��Ϊ�����Ǵ�����
�ĽǶȶ���ṹ��Board_info
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
//check_ban_and_evaluate���Ӻ������£�
void Remake_Evaluated_Board(int hang, int lie, int index);

void Calculate_array(int hang, int lie, int color,
					 int *Same, int *Same_Empty,
					 int *Same_Empty_Same, int *Same_Empty_Same_Empty,
					 int *Same_Empty_Same_Empty_Same);
	
//�ݹ��ж��Ƿ�Ϊ����
bool judge_next(int hang,int lie,int distance,int Direction,int color);
bool next_just_check_ban(int hang, int lie, int color);

//�жϾ����ֵ
int evaluate_jumian_value(int color);

/*------------------------------end------------------------------*/

void check_ban_and_evaluate(int hang, int lie, int color)
{
	/*������(�Ժ���)ͬʱ������ֵ
	*param��
	*int hang: ��ǰ���Ӻ�����
	*int lie: ��ǰ����������
	*int color: ��ǰ������ɫ������ / ���ӣ�
	*hint��������Ҫ�жϽ��֣������Ӳ���Ҫ
	*/

	//������1��ʾ���壬2��ʾ����
	//����Board_info�е������У��±�0��ʾ�ڣ��±�1��Ӧ��
	//��˽�color-1��Ϊ��ɫ��Ӧ���±�

	int index = color - 1;

	//Evaluated_Board�����ڴ洢�����ϻ�����ǰ����Ϣ
	//Ҳ����˵�����һ�����Ӻ���Щ��Ϣ�Ѿ���ʱ��
	//�������������Ҫ����ʱ��Ϣ����

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
        //�ж��Ƿ񹹳ɲ����ֵ�����,�����ȷ������ʵ�֣��������Ķ������ж��ˣ�ֱ��return
        if(Same[i]+Same[i+4] == 4 || (Same[i]==4 && Same[i+4]==4)){   //���������ǽ��ֵ���������������¶���4������ʱ������4�м䲻���ɳ�������
            Evaluated_Board[hang][lie].Direction[color-1].Right_5=true;
            return;
        }
        //����ǰ��壬����Ҳ��Ӯ 
        if(Same[i]+Same[i+4]>=5 && color==WHITE){    //ֻ�к����н���
            Evaluated_Board[hang][lie].Direction[color-1].Right_5=true;
            return;
        }
        //�жϳ������֣�������ֳ���Ҳ�������Ķ������ж���
        if(Same[i]+Same[i+4]>=5 && color==BLACK){    //ֻ�к����н���
            Evaluated_Board[hang][lie].Direction[color-1].Too_long=true;
            return;
        }
        //4��
        else if(Same[i]+Same[i+4]==3){
            int flag=0;
            if(Same_Empty[i]>0 && judge_next(hang,lie,Same[i],i,color)){
                flag++;
            }
            if(Same_Empty[i+4]>0 && judge_next(hang,lie,Same[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //����
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){       //����
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                ;       //���߶������˻����²��ˣ�û��
            }
        }
        //3��
        else if(Same[i]+Same[i+4]==2){
            //�������·���û�п��ܳ���,���߶��ǳ��ľ��ǻ���    ����  ��xxxox��
            int flag=0;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==1){     
                if(judge_next(hang,lie,Same[i],i,color)){      //���顰xxxox����o��
                    flag++;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){     //���顰xoxxx������o��
                    flag++;
                }
            }
            
            if(flag==2){
                //����  "xoxxxox"������0�����������γɻ���
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                //���� "xxxox"��0���������γɳ���
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                //����Ϸ��Ƿ��л���������һ���������Ϸ���һ����λ���ܷ��γɻ��ģ����û�л������ж��Ƿ��г�3
                //���磨����Ϊ�ϣ�"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" ��һ���������ұߵ�һ��o��(Ҳ��judge_nextӦ����Ĳ��������γɻ��ģ���Ϊ����
                //������"xoxxxooo"  / "ooxxxoox" ��һ���������ұߵ�һ��o��ʱ���޷��γɻ��ĵģ��������ƣ�
        
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
        //����
        else if(Same[i]+Same[i+4]==1){
            int flag=0;   
            //���ĳ����ж�
            //�����ϻ���"oxxoxx" / "xxoxxo"Ϊ����
            //�����ϼ���"xxoxxoxx" Ϊ����
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==2){
                if(judge_next(hang,lie,Same[i],i,color)){
                    flag++;      //���顰oxxoxx���ұ��Ǹ�o
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    flag++;      //���顰xxoxxo������Ǹ�o
                }
            }

            if(flag==2){
                Evaluated_Board[hang][lie].Direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                Evaluated_Board[hang][lie].Direction[color-1].Rush_4+=1;
            }
            else{
                //�����ж�
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

            //�жϻ�2�������ֵ�ã�
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
        //����һ��
        else if(Same[i]+Same[i+4]==0){
            //���ĳ����ж�
            //���硰oxoxxx�����м�o������λ����
            //�����������ģ����߶Գƣ���պ�Ϊ����
            bool flag=false;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==3){
                if(judge_next(hang,lie,Same[i],i,color)){    //��Ҫ�жϵĵ��ǡ�oxoxxx���м��o
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
            if(!flag){       //�г����˾Ͳ����ж�����
                //�����ж�
                //���硰ooxoxxoo����ooxoxxo����oxoxxoo�������м�o������Ϊ����

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
	��5�����飬ÿ������8��Ԫ��(8������)
	������ĳ���������������Ӽ���λ��Ϣ��
	�����Щ��Ϣ�Լ�һЩ�ݹ���õķ��������Ǿ��ܾ�׼��bug��
	�жϳ�������Ϣ����ͬʱ���������ֵ
	*/

    //��������������Ϣ
    int x,y;
    //����������y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,Same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,Same_Empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,Same_Empty_Same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,Same_Empty_Same_Empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,Same_Empty_Same_Empty_Same[0]++);

    //������������y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,Same_Empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same_Empty_Same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,Same_Empty_Same_Empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,Same_Empty_Same_Empty_Same[1]++);

    //����������x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,Same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,Same_Empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,Same_Empty_Same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,Same_Empty_Same_Empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,Same_Empty_Same_Empty_Same[2]++);

    //������������y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,Same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,Same_Empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,Same_Empty_Same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,Same_Empty_Same_Empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,Same_Empty_Same_Empty_Same[3]++);

    //����������y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,Same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,Same_Empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,Same_Empty_Same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,Same_Empty_Same_Empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,Same_Empty_Same_Empty_Same[4]++);

    //������������y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,Same_Empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same_Empty_Same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,Same_Empty_Same_Empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,Same_Empty_Same_Empty_Same[5]++);

    //����������x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,Same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,Same_Empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,Same_Empty_Same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,Same_Empty_Same_Empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,Same_Empty_Same_Empty_Same[6]++);

    //������������y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,Same_Empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same_Empty_Same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,Same_Empty_Same_Empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,Same_Empty_Same_Empty_Same[7]++);
}

bool judge_next(int hang,int lie,int distance,int Direction,int color){    
    if(color==WHITE)
	{         //����ǰ�����Ȼû�н�������
        return true;
    }      
    int next_hang,next_lie;
    distance++;    //��Ϊ���һ����������Ҫ�Ӷ�
    if(Direction>=4)
	{
        distance=-distance;       //�����෴���Ӽ�Ӧ��֮�෴
    }
    //����ؼ�������
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
            printf("����ķ�������");
            break;
    }
    bool if_just_evaluate=false;
    if(aRecordBoard[hang][lie]!=NONE)
	{
    	if_just_evaluate=true;
	}

    aRecordBoard[hang][lie]=color;        //���ղ��Ǹ�������

    bool flag = next_just_check_ban(next_hang,next_lie,color);

    if(!if_just_evaluate)
	{         
		//��Ӧ�жϾ���ֵ���Ҫ�����жϾ����ʱ��ԭ����������ӣ����ܲ��� 
    	aRecordBoard[hang][lie]=NONE;        //�ָ� 
	}
    if(flag){
    	return true;
	}
	else{
		return false;
	}
}

bool next_just_check_ban(int hang, int lie, int color){ //�����жϹؼ����Ƿ��ǽ���
    int Same[8];      
    int Same_Empty[8];   
    int Same_Empty_Same[8]; 
    int Same_Empty_Same_Empty[8];
    int Same_Empty_Same_Empty_Same[8];

    int j;
    //��ʼ�����飬��ֵ��ȻȫΪ0
    for(j=0;j<8;j++){
        Same[j]=Same_Empty[j]=Same_Empty_Same[j]=Same_Empty_Same_Empty[j]=Same_Empty_Same_Empty_Same[j]=0;
    }

	Calculate_array(hang, lie, color,
					Same, Same_Empty, Same_Empty_Same,
					Same_Empty_Same_Empty, Same_Empty_Same_Empty_Same);

    //ͳ����ϣ�����Ӧ����ʼ����4�������3��4����Ŀ������
	//������check_ban_and_evaluate�����𼴲��ü������������ͳ���⼸��������޹ض�ֻ���ֵ�޹ص���״��
    int Living_4, Rush_4, Living_3;
    Living_4 = Rush_4 = Living_3 = 0;

    //�����ж��Ƿ�����壬�������壬��Ȼ�����ɽ���
    int i;
    for(i=0;i<4;i++){
        //�ж��Ƿ񹹳ɲ����ֵ�����,�����ȷ������ʵ�֣��������Ķ������ж��ˣ�ֱ��return
        if(Same[i]+Same[i+4] == 4 || (Same[i]==4 && Same[i+4]==4)){   //���������ǽ��ֵ���������������¶���4������ʱ������4�м䲻���ɳ�������
            //��ȷ������˵���ùؼ��㲻�ǽ��֣�����true������
            return true;
        }
        //�жϳ������֣�������ֳ���Ҳ�������Ķ������ж���
        if(Same[i]+Same[i+4]>=5){
            //�������֣�ֱ�ӷ���false
            return false;
        }
        //4��
        else if(Same[i]+Same[i+4]==3){
            int flag=0;
            if(Same_Empty[i]>0 && judge_next(hang,lie,Same[i],i,color)){
                flag++;
            }
            if(Same_Empty[i+4]>0 && judge_next(hang,lie,Same[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //����
                Living_4+=1;
            }
            else if(flag==1){       //����
                Rush_4+=1;
            }
            else{
                ;       //���߶������˻����²��ˣ�û��
            }
        }
        //3��
        else if(Same[i]+Same[i+4]==2){
            //�������·���û�п��ܳ���,���߶��ǳ��ľ��ǻ���    ����  ��xxxox��
            int flag=0;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==1){     
                if(judge_next(hang,lie,Same[i],i,color)){      //���顰xxxox����o��
                    flag++;
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){     //���顰xoxxx������o��
                    flag++;
                }
            }
            
            if(flag==2){
                //����  "xoxxxox"������0�����������γɻ���
                Living_4+=1;
            }
            else if(flag==1){
                //���� "xxxox"��0���������γɳ���
                Rush_4+=1;
            }
            else{
                //����Ϸ��Ƿ��л���������һ���������Ϸ���һ����λ���ܷ��γɻ���
                //���磨����Ϊ�ϣ�"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" ��һ���������ұߵ�һ��o��(Ҳ��judge_nextӦ����Ĳ��������γɻ��ģ���Ϊ����
                //������"xoxxxooo"  / "ooxxxoox" ��һ���������ұߵ�һ��o��ʱ���޷��γɻ��ĵģ��������ƣ�
                bool flag_living_3=false;

                if((Same_Empty[i]>2 || (Same_Empty[i]==2 && Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){    //����xxx�ϱߵĵ�һ��o��
                        flag_living_3=true;
                    }
                }
                //���Ϸ���ͬ�������ڼ���·�
                if((Same_Empty[i+4]>2 || (Same_Empty[i+4]==2 && Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){    //����xxx�±ߵĵ�һ��o��
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //����
        else if(Same[i]+Same[i+4]==1){
            int flag=0;   
            //���ĳ����ж�
            //�����ϻ���"oxxoxx" / "xxoxxo"Ϊ����
            //�����ϼ���"xxoxxoxx" Ϊ����
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==2){
                if(judge_next(hang,lie,Same[i],i,color)){
                    flag++;      //���顰oxxoxx���ұ��Ǹ�o
                }
            }
            if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2){
                if(judge_next(hang,lie,Same[i+4],i+4,color)){
                    flag++;      //���顰xxoxxo������Ǹ�o
                }
            }

            if(flag==2){
                Living_4+=1;
            }
            else if(flag==1){
                Rush_4+=1;
            }
            else{
                //�����ж�
                bool flag_living_3=false;
                if(Same_Empty[i]==1 && Same_Empty_Same[i]==1
                    && (Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same_Empty_Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){    //���顰ooxxoxoo"�м�Ŀ�λ
                        flag_living_3=true;
                    }
                }
                if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==1
                    && (Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same_Empty_Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){    //���顰ooxoxxo"�м�Ŀ�λ
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //����һ��
        else if(Same[i]+Same[i+4]==0){
            //���ĳ����ж�
            //���硰oxoxxx�����м�o������λ����
            bool flag=false;
            if(Same_Empty[i]==1 && Same_Empty_Same[i]==3){
                if(judge_next(hang,lie,Same[i],i,color)){    //��Ҫ�жϵĵ��ǡ�oxoxxx���м��o
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
            //�����ж�
            //���硰ooxoxxoo����ooxoxxo����oxoxxoo�������м�o������Ϊ����

            if(!flag){
                bool flag_living_3=false;

                if(Same_Empty[i]==1 && Same_Empty_Same[i]==2
                    && (Same_Empty_Same_Empty[i]>1 || (Same_Empty_Same_Empty[i]==1 && Same_Empty_Same_Empty_Same[i]==0))
                    && (Same_Empty[i+4]>1 || (Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==0)))
                {
                    if(judge_next(hang,lie,Same[i],i,color)){   //��Ҫ�жϵĵ����м��o
                        flag_living_3=true;
                    }
                }
                if(Same_Empty[i+4]==1 && Same_Empty_Same[i+4]==2
                    && (Same_Empty_Same_Empty[i+4]>1 || (Same_Empty_Same_Empty[i+4]==1 && Same_Empty_Same_Empty_Same[i+4]==0))
                    && (Same_Empty[i]>1 || (Same_Empty[i]==1 && Same_Empty_Same[i]==0)))
                {
                    if(judge_next(hang,lie,Same[i+4],i+4,color)){   //��Ҫ�жϵĵ����м��o
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
    }

    //�жϽ���
    if(Living_3==1 && (Living_4+Rush_4)==1)
	{
		//�ȿ���������������������ǽ��֡�
        return true;            
    }

    if(Living_3+Living_4+Rush_4>=2)
	{             
		//����һ����һ�������⣬���������ֻҪ�ʹ��ڵ���2��һ���ǽ��֡�
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
	//���ȼ��ǶԷ�����>��������>�Է���3 
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
	//������ڼ������������������������ȵĸ�ֵ���д���ȶ 
	
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