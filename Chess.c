
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//Ϊ����Ӧc������bool.h
#define bool int
#define true 1
#define false 0
//Ϊ���ַ�������
#define SIZE 15
#define CHARSIZE 2
//��Ӧ���ӻ�ɫ
#define NONE 0
#define BLACK 1
#define WHITE 2
//��Ӧÿ����״�ļ�ֵ
#define Value_Right_5 50000
#define Value_Living_4 4320
#define Value_Rush_4 720
#define Value_Living_3 720
#define Value_Rush_3 100
#define Value_Living_2 120
#define Value_Rush_2 20
//�������жϽ���ʱ�費��Ҫ��ӡ��Ϣ
#define NEED_INF 1
#define NO_NEED_INF 0
//�������ҵ������ʱ�費��Ҫ��ֵ
#define NEED_VALUE 1
#define NO_NEED_VALUE 0

//�����������
int DEPTH=8;
//����ѡ����
#define Base_Width 15
int WIDTH=14;
//ѡ������7�㻹��9�� 
char able;
#define Fuwuqiong -10000000     //���ĳ�ֵ
#define Zhengwuqiong 10000000   //�µĳ�ֵ

typedef struct{       //��¼���λ�ã�ע�Ⲣ��ÿ�����ô�struct����Щ�ط�Ϊ�˷���ֱ������hang��lie������
    int hang;
    int lie;
}Point, *PTR_To_Point;

typedef struct{
    Point p;
    int score;
    int min;
    int max;
}Points, *PTR_To_Points;

struct Point_Imf{          //��¼ĳ��ĳһ�����ϵ�������Ϣ
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

//����ʹ�õ���GBK���룬ÿһ�������ַ�ռ��2���ֽڡ�
//���̻���ģ�� 
char aInitDisplayBoardArray[SIZE][SIZE*CHARSIZE+1] = 
{
		"���өөөөөөөөөөөөө�",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"�ĩ��������������",
		"���۩۩۩۩۩۩۩۩۩۩۩۩۩�"
};
//������������ʾ���� 
char aDisplayBoardArray[SIZE][SIZE*CHARSIZE+1];
 
char play1Pic[]="��";//������;
char play1CurrentPic[]="��"; 

char play2Pic[]="��";//������;
char play2CurrentPic[]="��";

char leave[]="quit";
 
int aRecordBoard[SIZE][SIZE];       //���������ڼ�¼���̸��
struct Point_Score Value_Board[SIZE][SIZE];          //������������¼ÿ����ĸ�����Ϣ

void initRecordBorard(void);
void recordtoDisplayArray(int hang,int lie);
void displayBoard(void);

void check_ban_and_value(int hang, int lie, int color);   //color��Ϊ��ѡ�ڰ�
bool judge_next(int hang,int lie,int distance,int direction,int color);
bool next_just_check_ban(int hang, int lie, int color);
bool judge_if_banhand(int hang,int lie,int color,bool print);
void evaluate_value(int hang, int lie);
int evaluate_jumian_value(int color);

int Best_choice(int color, PTR_To_Point final, int depth, int a, int b, char able);
int Find_Points(PTR_To_Points All_Points, int color, bool flag, bool *flag_if_win, PTR_To_Point Bisha, bool *flag_will_lose, PTR_To_Point Bidang); //���ص����ҵ��ĵ���
void quicksort(PTR_To_Points s, int left, int right);
int Find_base_point(int color);

bool judge_result(void);      //�ж���Ӯ

void Human_vs_Human(void);    //���˶�սģʽ
void Human_vs_AI(void);       //�˻���սģʽ
void AI_vs_AI(void);          //AI��սģʽ 

void black_go();              //�����º���
void white_go();              //�����°���
void AI_black_go(int count, char able);           //AI�º���
void AI_white_go(int count, char able);           //AI�°���
bool judge_result(void);
void Buju_Black(int hang, int lie);        //���ڲ����Զ����� 
void Buju_White(int hang, int lie);

//----------------------------------------------------------------------------//

void check_ban_and_value(int hang, int lie, int color){
    //�Ƚ�ԭ��Ϣ����
    Value_Board[hang][lie].score[color-1]=0;
    Value_Board[hang][lie].direction[color-1].Too_long=false;
    Value_Board[hang][lie].direction[color-1].Right_5=false;
    Value_Board[hang][lie].direction[color-1].Living_4=0;
    Value_Board[hang][lie].direction[color-1].Rush_4=0;
    Value_Board[hang][lie].direction[color-1].Living_3=0;
    Value_Board[hang][lie].direction[color-1].Rush_3=0;
    Value_Board[hang][lie].direction[color-1].Living_2=0;
    Value_Board[hang][lie].direction[color-1].Rush_2=0;
    //�������
    int ajcsame[8];       //ajc==adjacent������,same��Ϊ��ͬ��ɫ�ģ�0-7���δ����ϣ����ϣ��ң����£��£����£�������
    int after_same_is_empty[8];    //��¼������ͬ��ɫ���Ӻ�Ŀ�λ
    int after_same_empty_is_same[8]; //��¼��λ�����ͬ��ɫ��
    int after_same_empty_same_is_empty[8];
    int after_same_empty_same_empty_is_same[8];

    int j;
    //��ʼ�����飬��ֵ��ȻȫΪ0
    for(j=0;j<8;j++){
        ajcsame[j]=after_same_is_empty[j]=after_same_empty_is_same[j]=after_same_empty_same_is_empty[j]=after_same_empty_same_empty_is_same[j]=0;
    }

    //��������������Ϣ
    int x,y;
    //����������y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,ajcsame[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_is_same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_empty_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_same_empty_is_same[0]++);

    //������������y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,ajcsame[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_is_same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_empty_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_same_empty_is_same[1]++);

    //����������x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,ajcsame[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_is_same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_empty_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_same_empty_is_same[2]++);

    //������������y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,ajcsame[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_is_same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_empty_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_same_empty_is_same[3]++);

    //����������y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,ajcsame[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_is_same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_empty_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_same_empty_is_same[4]++);

    //������������y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,ajcsame[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_is_same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_empty_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_same_empty_is_same[5]++);

    //����������x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,ajcsame[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_is_same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_empty_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_same_empty_is_same[6]++);

    //������������y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,ajcsame[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_is_same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_empty_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_same_empty_is_same[7]++);
    //ͳ����ϣ�����Ӧ����ʼ����4�������3��4����Ŀ������

    //�����ж��Ƿ�����壬�������壬��Ȼ�����ɽ���
    int i;
    for(i=0;i<4;i++){
        //�ж��Ƿ񹹳ɲ����ֵ�����,�����ȷ������ʵ�֣��������Ķ������ж��ˣ�ֱ��return
        if(ajcsame[i]+ajcsame[i+4] == 4 || (ajcsame[i]==4 && ajcsame[i+4]==4)){   //���������ǽ��ֵ���������������¶���4������ʱ������4�м䲻���ɳ�������
            Value_Board[hang][lie].direction[color-1].Right_5=true;
            return;
        }
        //����ǰ��壬����Ҳ��Ӯ 
        if(ajcsame[i]+ajcsame[i+4]>=5 && color==WHITE){    //ֻ�к����н���
            Value_Board[hang][lie].direction[color-1].Right_5=true;
            return;
        }
        //�жϳ������֣�������ֳ���Ҳ�������Ķ������ж���
        if(ajcsame[i]+ajcsame[i+4]>=5 && color==BLACK){    //ֻ�к����н���
            Value_Board[hang][lie].direction[color-1].Too_long=true;
            return;
        }
        //4��
        else if(ajcsame[i]+ajcsame[i+4]==3){
            int flag=0;
            if(after_same_is_empty[i]>0 && judge_next(hang,lie,ajcsame[i],i,color)){
                flag++;
            }
            if(after_same_is_empty[i+4]>0 && judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                flag++;
            }

            if(flag==2){    //����
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){       //����
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                ;       //���߶������˻����²��ˣ�û��
            }
        }
        //3��
        else if(ajcsame[i]+ajcsame[i+4]==2){
            //�������·���û�п��ܳ���,���߶��ǳ��ľ��ǻ���    ����  ��xxxox��
            int flag=0;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1){     
                if(judge_next(hang,lie,ajcsame[i],i,color)){      //���顰xxxox����o��
                    flag++;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){     //���顰xoxxx������o��
                    flag++;
                }
            }
            
            if(flag==2){
                //����  "xoxxxox"������0�����������γɻ���
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                //���� "xxxox"��0���������γɳ���
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                //����Ϸ��Ƿ��л���������һ���������Ϸ���һ����λ���ܷ��γɻ��ģ����û�л������ж��Ƿ��г�3
                //���磨����Ϊ�ϣ�"ooxxxooo" / "ooxxxoo|" / "|oxxxooo" ��һ���������ұߵ�һ��o��(Ҳ��judge_nextӦ����Ĳ��������γɻ��ģ���Ϊ����
                //������"xoxxxooo"  / "ooxxxoox" ��һ���������ұߵ�һ��o��ʱ���޷��γɻ��ĵģ��������ƣ�
        
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
        //����
        else if(ajcsame[i]+ajcsame[i+4]==1){
            int flag=0;   
            //���ĳ����ж�
            //�����ϻ���"oxxoxx" / "xxoxxo"Ϊ����
            //�����ϼ���"xxoxxoxx" Ϊ����
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2){
                if(judge_next(hang,lie,ajcsame[i],i,color)){
                    flag++;      //���顰oxxoxx���ұ��Ǹ�o
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
                    flag++;      //���顰xxoxxo������Ǹ�o
                }
            }

            if(flag==2){
                Value_Board[hang][lie].direction[color-1].Living_4+=1;
            }
            else if(flag==1){
                Value_Board[hang][lie].direction[color-1].Rush_4+=1;
            }
            else{
                //�����ж�
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

            //�жϻ�2�������ֵ�ã�
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
        //����һ��
        else if(ajcsame[i]+ajcsame[i+4]==0){
            //���ĳ����ж�
            //���硰oxoxxx�����м�o������λ����
            //�����������ģ����߶Գƣ���պ�Ϊ����
            bool flag=false;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==3){
                if(judge_next(hang,lie,ajcsame[i],i,color)){    //��Ҫ�жϵĵ��ǡ�oxoxxx���м��o
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
            if(!flag){       //�г����˾Ͳ����ж�����
                //�����ж�
                //���硰ooxoxxoo����ooxoxxo����oxoxxoo�������м�o������Ϊ����

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
    //�����ж����
}

bool judge_next(int hang,int lie,int distance,int direction,int color){    
    if(color==WHITE){         //����ǰ�����Ȼû�н�������
        return true;
    }      
    int next_hang,next_lie;
    distance++;    //��Ϊ���һ����������Ҫ�Ӷ�
    if(direction>=4){
        distance=-distance;       //�����෴���Ӽ�Ӧ��֮�෴
    }
    //����ؼ�������
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
            printf("����ķ�������");
            break;
    }
    bool flag_if_youzi=true;
    if(aRecordBoard[hang][lie]!=NONE){
    	flag_if_youzi=false;
	}
    aRecordBoard[hang][lie]=color;        //���ղ��Ǹ�������
    bool flag = next_just_check_ban(next_hang,next_lie,color);
    if(flag_if_youzi){         //��Ӧ�жϾ���ֵ���Ҫ�����жϾ����ʱ��ԭ����������ӣ����ܲ��� 
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
    int ajcsame[8];       //ajc==adjacent������,same��Ϊ��ͬ��ɫ�ģ�0-7���δ����ϣ����ϣ��ң����£��£����£�������
    int after_same_is_empty[8];    //��¼������ͬ��ɫ���Ӻ�Ŀ�λ
    int after_same_empty_is_same[8]; //��¼��λ�����ͬ��ɫ��
    int after_same_empty_same_is_empty[8];
    int after_same_empty_same_empty_is_same[8];

    int j;
    //��ʼ�����飬��ֵ��ȻȫΪ0
    for(j=0;j<8;j++){
        ajcsame[j]=after_same_is_empty[j]=after_same_empty_is_same[j]=after_same_empty_same_is_empty[j]=after_same_empty_same_empty_is_same[j]=0;
    }

    //��������������Ϣ
    int x,y;
    //����������y--;
    for(x=lie,y=hang-1;y>=0 && aRecordBoard[y][x]==color;y--,ajcsame[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_is_same[0]++);

    for(;y>=0 && aRecordBoard[y][x]==NONE;y--,after_same_empty_same_is_empty[0]++);

    for(;y>=0 && aRecordBoard[y][x]==color;y--,after_same_empty_same_empty_is_same[0]++);

    //������������y--,x++;
    for(x=lie+1,y=hang-1;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,ajcsame[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_is_same[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==NONE;x++,y--,after_same_empty_same_is_empty[1]++);

    for(;x<SIZE && y>=0 && aRecordBoard[y][x]==color;x++,y--,after_same_empty_same_empty_is_same[1]++);

    //����������x++;
    for(x=lie+1,y=hang;x<SIZE && aRecordBoard[y][x]==color;x++,ajcsame[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_is_same[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==NONE;x++,after_same_empty_same_is_empty[2]++);

    for(;x<SIZE && aRecordBoard[y][x]==color;x++,after_same_empty_same_empty_is_same[2]++);

    //������������y++,x++;
    for(x=lie+1,y=hang+1;x<SIZE && y<=SIZE && aRecordBoard[y][x]==color;x++,y++,ajcsame[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_is_same[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==NONE;x++,y++,after_same_empty_same_is_empty[3]++);

    for(;x<SIZE && y<SIZE && aRecordBoard[y][x]==color;x++,y++,after_same_empty_same_empty_is_same[3]++);

    //����������y++;
    for(x=lie,y=hang+1;y<SIZE && aRecordBoard[y][x]==color;y++,ajcsame[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_is_same[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==NONE;y++,after_same_empty_same_is_empty[4]++);

    for(;y<SIZE && aRecordBoard[y][x]==color;y++,after_same_empty_same_empty_is_same[4]++);

    //������������y++,x--;
    for(x=lie-1,y=hang+1;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,ajcsame[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_is_same[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==NONE;x--,y++,after_same_empty_same_is_empty[5]++);

    for(;x>=0,y<SIZE && aRecordBoard[y][x]==color;x--,y++,after_same_empty_same_empty_is_same[5]++);

    //����������x--;
    for(x=lie-1,y=hang;x>=0 && aRecordBoard[y][x]==color;x--,ajcsame[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_is_same[6]++);

    for(;x>=0 && aRecordBoard[y][x]==NONE;x--,after_same_empty_same_is_empty[6]++);

    for(;x>=0 && aRecordBoard[y][x]==color;x--,after_same_empty_same_empty_is_same[6]++);

    //������������y--,x--;
    for(x=lie-1,y=hang-1;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,ajcsame[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_is_same[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==NONE;x--,y--,after_same_empty_same_is_empty[7]++);

    for(;x>=0,y>=0 && aRecordBoard[y][x]==color;x--,y--,after_same_empty_same_empty_is_same[7]++);
    //ͳ����ϣ�����Ӧ����ʼ����4�������3��4����Ŀ������
    int Living_4,Rush_4,Living_3;
    Living_4=Rush_4=Living_3=0;

    //�����ж��Ƿ�����壬�������壬��Ȼ�����ɽ���
    int i;
    for(i=0;i<4;i++){
        //�ж��Ƿ񹹳ɲ����ֵ�����,�����ȷ������ʵ�֣��������Ķ������ж��ˣ�ֱ��return
        if(ajcsame[i]+ajcsame[i+4] == 4 || (ajcsame[i]==4 && ajcsame[i+4]==4)){   //���������ǽ��ֵ���������������¶���4������ʱ������4�м䲻���ɳ�������
            //��ȷ������˵���ùؼ��㲻�ǽ��֣�����true������
            return true;
        }
        //�жϳ������֣�������ֳ���Ҳ�������Ķ������ж���
        if(ajcsame[i]+ajcsame[i+4]>=5){
            //�������֣�ֱ�ӷ���false
            return false;
        }
        //4��
        else if(ajcsame[i]+ajcsame[i+4]==3){
            int flag=0;
            if(after_same_is_empty[i]>0 && judge_next(hang,lie,ajcsame[i],i,color)){
                flag++;
            }
            if(after_same_is_empty[i+4]>0 && judge_next(hang,lie,ajcsame[i+4],i+4,color)){
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
        else if(ajcsame[i]+ajcsame[i+4]==2){
            //�������·���û�п��ܳ���,���߶��ǳ��ľ��ǻ���    ����  ��xxxox��
            int flag=0;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1){     
                if(judge_next(hang,lie,ajcsame[i],i,color)){      //���顰xxxox����o��
                    flag++;
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){     //���顰xoxxx������o��
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

                if((after_same_is_empty[i]>2 || (after_same_is_empty[i]==2 && after_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){    //����xxx�ϱߵĵ�һ��o��
                        flag_living_3=true;
                    }
                }
                //���Ϸ���ͬ�������ڼ���·�
                if((after_same_is_empty[i+4]>2 || (after_same_is_empty[i+4]==2 && after_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){    //����xxx�±ߵĵ�һ��o��
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //����
        else if(ajcsame[i]+ajcsame[i+4]==1){
            int flag=0;   
            //���ĳ����ж�
            //�����ϻ���"oxxoxx" / "xxoxxo"Ϊ����
            //�����ϼ���"xxoxxoxx" Ϊ����
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2){
                if(judge_next(hang,lie,ajcsame[i],i,color)){
                    flag++;      //���顰oxxoxx���ұ��Ǹ�o
                }
            }
            if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2){
                if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){
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
                if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==1
                    && (after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){    //���顰ooxxoxoo"�м�Ŀ�λ
                        flag_living_3=true;
                    }
                }
                if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==1
                    && (after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){    //���顰ooxoxxo"�м�Ŀ�λ
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
        //����һ��
        else if(ajcsame[i]+ajcsame[i+4]==0){
            //���ĳ����ж�
            //���硰oxoxxx�����м�o������λ����
            bool flag=false;
            if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==3){
                if(judge_next(hang,lie,ajcsame[i],i,color)){    //��Ҫ�жϵĵ��ǡ�oxoxxx���м��o
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
            //�����ж�
            //���硰ooxoxxoo����ooxoxxo����oxoxxoo�������м�o������Ϊ����

            if(!flag){
                bool flag_living_3=false;

                if(after_same_is_empty[i]==1 && after_same_empty_is_same[i]==2
                    && (after_same_empty_same_is_empty[i]>1 || (after_same_empty_same_is_empty[i]==1 && after_same_empty_same_empty_is_same[i]==0))
                    && (after_same_is_empty[i+4]>1 || (after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i],i,color)){   //��Ҫ�жϵĵ����м��o
                        flag_living_3=true;
                    }
                }
                if(after_same_is_empty[i+4]==1 && after_same_empty_is_same[i+4]==2
                    && (after_same_empty_same_is_empty[i+4]>1 || (after_same_empty_same_is_empty[i+4]==1 && after_same_empty_same_empty_is_same[i+4]==0))
                    && (after_same_is_empty[i]>1 || (after_same_is_empty[i]==1 && after_same_empty_is_same[i]==0)))
                {
                    if(judge_next(hang,lie,ajcsame[i+4],i+4,color)){   //��Ҫ�жϵĵ����м��o
                        flag_living_3=true;
                    }
                }
                if(flag_living_3){
                    Living_3+=1;
                }
            }
        }
    }
    //�����ж����
    if(Living_3==1 && (Living_4+Rush_4)==1){
        return true;            //�ȿ���������������������ǽ��֡�
    }

    if(Living_3+Living_4+Rush_4>=2){             //����һ����һ�������⣬���������ֻҪ�ʹ��ڵ���2��һ���ǽ��֡�
        return false;
    }
    else{
        return true;
    }
} 

bool judge_if_banhand(int hang,int lie,int color,bool print){
    if(Value_Board[hang][lie].direction[color-1].Too_long==true){
        if(print)
            printf("�������֣�\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_3>=2 && Value_Board[hang][lie].direction[color-1].Living_4==0 && Value_Board[hang][lie].direction[color-1].Rush_4==0){
        if(print)
            printf("˫�������֣�\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4>=2 && Value_Board[hang][lie].direction[color-1].Living_3==0){
        if(print)
            printf("˫����/���Ľ��֣�\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==1 && Value_Board[hang][lie].direction[color-1].Living_3>=2){
        if(print)
            printf("������(��)���֣�\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==2 && Value_Board[hang][lie].direction[color-1].Living_3>=1){
        if(print)
            printf("������(��)���֣�\n");
        return false;
    }
    if(Value_Board[hang][lie].direction[color-1].Living_4+Value_Board[hang][lie].direction[color-1].Rush_4==3 && Value_Board[hang][lie].direction[color-1].Living_3==1){
        if(print)
            printf("�����������֣�\n");
        return false;
    }
    //ע��43���ǽ���
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
    PTR_To_Point cunchu;  //�洢���������ĵ�
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
                            if(!flag_cf){  //˵���õ��Ѿ��жϹ��ˣ�ֱ�ӵ���һ����ȥ
                                continue;
                            }
                            else{
                                cunchu[cunchu_index].hang=i;
                                cunchu[cunchu_index].lie=j;
                                cunchu_index++;
                            }
                            check_ban_and_value(i,j,BLACK);
                            check_ban_and_value(i,j,WHITE);

                            //������ְ��������жϣ�����ֺ��嵫�ⲽ�ǽ��֣���Ҳ������All_Points��
                            if(color==WHITE || judge_if_banhand(i,j,color,NO_NEED_INF)){
                                if(Value_Board[i][j].direction[color-1].Right_5){   //����������ս�����Ҫ��ǳ���
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
    PTR_To_Points All_Points;     //����ѡ��
    All_Points=(PTR_To_Points)malloc(sizeof(Points)*225);
    bool flag_if_win=false;
    bool flag_will_lose=false;
    Point Bisha,Bidang;
    int n=Find_Points(All_Points,color,NO_NEED_VALUE, &flag_if_win, &Bisha, &flag_will_lose, &Bidang);
    if(flag_if_win){
        //˵���ײ��ּ�����ʱ���ս��֣�����һ������ֵ
        return 200000;
    }
    if(flag_will_lose){
    	//˵���ײ��ּ�����ʱֻ��ȥ�¶���ģ�ֻ��һ��ѡ�� 
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
    //�ݹ���ֹ����Ϊ����ײ㣬��ʱҪ���ݾ����ѡ��
    if(depth==0){
        int value=Find_base_point(color);
        return value;
    }
    //���ǵײ㣬��Ӧ�ø��ݵ��ѡ��
    int next_color=(color==BLACK? WHITE : BLACK);   //�ж�һ��һ����������ɫ
    PTR_To_Points All_Points;     //����ѡ��
    All_Points=(PTR_To_Points)malloc(sizeof(Points)*225);
    if(All_Points==NULL){
    	printf("malloc error");
    	exit(1);
	}
	bool flag_if_win=false;
    bool flag_will_lose=false;
    Point Bisha,Bidang;
    int n=Find_Points(All_Points,color,NEED_VALUE, &flag_if_win, &Bisha, &flag_will_lose, &Bidang);
	//����ֵ�color�����˲����ս���� 
    if(flag_if_win){
        if(depth%2==1){
            //���������Ӻ󣬷��ֶԷ���һ�����ս������˵�����������Ӳ����ʣ�Ӧ����һ����Сֵ
            return -200000;
        }
        else{
            //���Է�����(���ʼ����)ʱ���ҷ���һ�����ս������˵���Է������Ӳ����ʣ�Ӧ����һ������ֵ
            //ͬʱ�����ʱ�Ǹ��ڵ㼴��ʼ���棬��Ӧ�����ؼ����ս��ֵ�����
            if(depth==DEPTH){
                final->hang=Bisha.hang;
                final->lie=Bisha.lie;
            }
            return 200000;
        }
    }
	if(n<WIDTH){   //�����ҵĵ�����14�� 
		WIDTH=n; 
	}
	int temp_width=WIDTH;
	
    quicksort(All_Points,0,n-1);
    int i,j;
    PTR_To_Points choice;       //���ŵ�WIDTH��ѡ��
    choice=(PTR_To_Points)malloc(sizeof(Points)*WIDTH);
    //��ALL_Points�е����ŵ�WIDTH���㸴�Ƶ�choice[]��ȥ
    for(i=0,j=n-1;i<WIDTH;i++,j--){
        choice[i].p=All_Points[j].p;
        choice[i].score=All_Points[j].score;
        choice[i].min=a;    //�̳����Ը��ڵ�Ħ�
        choice[i].max=b;    //�̳����Ը��ڵ�Ħ�
    }
    int *value;  //��¼ÿ���ӽڵ�Ļ���ֵ
    value=(int *)malloc(sizeof(int)*WIDTH);
    for(i=0;i<temp_width;i++){
        //step1:�Ȱ�������ȥ
        aRecordBoard[choice[i].p.hang][choice[i].p.lie]=color;
        //step2:���������ֵ���ƻ���
        value[i]=Best_choice(next_color,final,depth-1,a,b,able);
        //step3:���ݻ���ֵ�ı���ͦ�
        //�����ż����(��������,ȡ�ӽڵ����ֵ�е����ֵ����˸ı�����min)
        if(depth%2==0){
            if(value[i]>a){
                a=value[i];        //�������޴���ı�����
                //������ڵ�ʱż���㣬��Ҫ֪�����λ��
                if(depth==DEPTH){     
                    final->hang=choice[i].p.hang;
                    final->lie=choice[i].p.lie;
                }
            }
            if(a>=b){    //����>=�£����������������ˣ�������ϲ㷵�ئ�
            	aRecordBoard[choice[i].p.hang][choice[i].p.lie]=NONE;
                return a;
            }
        }
        //�����������(�Է����ӣ�ȡ�ӽڵ����ֵ�е���Сֵ����˸ı�����max)
        else{
            if(value[i]<b){
                b=value[i];
            }
            if(a>=b){    //����>=�£�����������������������ϲ㷵�ئ� 
            	aRecordBoard[choice[i].p.hang][choice[i].p.lie]=NONE;
                return b;
            }
        }
        //step4:������Ӳ���
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
    initRecordBorard();               //��ʼ������
    printf("Work of ��Զ��\n");
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
	displayBoard();
    printf("\n���˶�ս��ʼ���ɺ��������ӣ�\n");
    int count=1;
    while(1){
        printf("�ֵ������µ�%d���壺\n",count);
        black_go();
        count++;
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }
        printf("�ֵ������µ�%d���壺\n",count);
        count++;
        white_go();
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }
    }
}

void Human_vs_AI(void){
	printf("ѡ������:(E�������Ը�)(S/E):");
	scanf("%c",&able);
	if(able!='S' && able!='E' && able!='s' && able!='e'){
		printf("�����������������(S/E)");
		scanf("%c",&able);
	}
	if(able=='S' || able=='s'){
		DEPTH=6;
	}
	else if(able=='E' || able=='e'){
		DEPTH=8;
	}
    initRecordBorard();               //��ʼ������
    printf("Work of ��Զ��\n");
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
	displayBoard();
    int choice;
    printf("\n�˻���ս��ʼ������ѡ���Ⱥ���:������1ѡ�����֣�����2ѡ�����)\n");
    scanf("%d",&choice);
    char eat=getchar();
    int count=1;
    clock_t start,end;
    if(choice==1){
        while(1){
            printf("�ֵ������µ�%d���壺\n",count);
            black_go();
            count++;
            if(judge_result()){
                printf("����ʤ������Ϸ����\n");
                break;
            }
            printf("�ֵ������µ�%d���壺\n",count);
            start=clock();
            AI_white_go(count,able);
            end=clock();
#if 0
            printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
#endif
            count++;
            if(judge_result()){
                printf("����ʤ������Ϸ����\n");
                break;
            }
        }
    }
    else if(choice==2){
        while(1){
            printf("�ֵ������µ�%d���壺\n",count);
            start=clock();
            AI_black_go(count,able);
            end=clock();
#if 0
            printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
#endif
            count++;
            if(judge_result()){
                printf("����ʤ������Ϸ����\n");
                break;
            }
            printf("�ֵ������µ�%d���壺\n",count);
            white_go();
            count++;
            if(judge_result()){
                printf("����ʤ������Ϸ����\n");
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
        printf("�ֵ������µ�%d���壺\n",count);
        start=clock();
        AI_black_go(count,able);
        end=clock();
        printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
        count++;
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }
        printf("�ֵ������µ�%d���壺\n",count);
        start=clock();
        AI_white_go(count,able);
        end=clock();
        printf("use time %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
        count++;
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }
    }
}

void black_go(){
    char inf[20];
    printf("�������������������(��ĸ,����):");
    scanf("%s",inf);
    if(strcmp(inf,leave)==0){              //Ϊ����Ӧ�˳���Ϸ�Ĺ��� leave="quit"
        exit(0);
    }
    char eat=getchar();
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("������ʽ��������(��ĸ,����)��ʽ��������\n");
        black_go();
        return;
    }
    int hang;
	if(isdigit(inf[2]) && isdigit(inf[3])){           //����λ����ʱ 
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{
		hang = 15-(inf[2]-'0');    //y���Ӧ��
	}

    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    int lie = inf[0]-'A';    //x���Ӧ��
    if(aRecordBoard[hang][lie]!=NONE){
        printf("�㿪���ǰɣ�");
        printf("\n����������\n");
        black_go();
        return;
    }

    check_ban_and_value(hang, lie, BLACK);
    if(!judge_if_banhand(hang,lie,BLACK,NEED_INF)){
        printf("�������˽��֣��ж�����ʤ��");
        system("pause");
    }

    aRecordBoard[hang][lie]=BLACK;    //���Ӷ�Ӧֵ1
    recordtoDisplayArray(hang,lie);   //�������hang��lie��Ϊ�˱��������
    displayBoard();
    //�ж��Ƿ����
    printf("�Ƿ�ϣ�����壿(Y/�����)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
        printf("���������ӣ�");
        black_go();
    }
}

void white_go(){
    char inf[20];
    printf("�������������������(��ĸ,����):");
    scanf("%s",inf);
    if(strcmp(inf,leave)==0){
        exit(0);
    }
    char eat=getchar();
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("������ʽ��������(��ĸ,����)��ʽ��������\n");
        white_go();
        return;
    }
    int hang;
	if(isdigit(inf[2]) && isdigit(inf[3])){           //����λ����ʱ 
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{
		hang = 15-(inf[2]-'0');    //y���Ӧ��
	}

    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    int lie = inf[0]-'A';          //x���Ӧ��
    if(aRecordBoard[hang][lie]!=NONE){
        printf("�㿪���ǰɣ�");
        printf("\n����������\n");
        white_go();
        return;
    }

    //���岻���жϽ��֣������������˶�ս�����ü�ֵ�жϣ����Բ���date_imformation

    aRecordBoard[hang][lie]=WHITE;      //���Ӷ�Ӧֵ2
    recordtoDisplayArray(hang,lie); //�������hang��lie��Ϊ�˱��������
    displayBoard();
    //�ж��Ƿ����
    printf("�Ƿ�ϣ�����壿(Y/�����)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
        printf("���������ӣ�");
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
    	//����ڶ��������»����� 
    	if(aRecordBoard[8][8]==WHITE || aRecordBoard[8][6]==WHITE){
			Buju_Black(6,7);
        	return;
		}
		//����ڶ��������ϻ����� 
    	if(aRecordBoard[6][6]==WHITE || aRecordBoard[6][8]==WHITE){
			Buju_Black(8,7);
        	return;
		}
		//����ڶ�������������
    	if(aRecordBoard[8][7]==WHITE || aRecordBoard[7][6]==WHITE){
			Buju_Black(8,6);
        	return;
		}
		//����ڶ�����������ұ�
    	if(aRecordBoard[6][7]==WHITE || aRecordBoard[7][8]==WHITE){
			Buju_Black(6,8);
        	return;
		}
	}
	if(count==5){
		//�ڶ������������� 
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
		//�ڶ������������� 
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
		//�ڶ������������� 
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
		//�ڶ������������� 
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
		
		//�ڶ�������������
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
		//�ڶ�������������
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
		//�ڶ�������������
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
		//�ڶ�������������
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
    printf("\n��������λ��Ϊ(%d,%c)\n",15-final->hang,final->lie+'A');
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
	printf("\n��������λ��Ϊ(%d,%c)\n",15-final->hang,final->lie+'A');
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
//ͨ��˫��ѭ������aRecordBoard��0
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE;j++){
			aRecordBoard[i][j]=0;
		}
	}
}
void recordtoDisplayArray(int hang, int lie){
//��һ������aInitDisplayBoardArray�м�¼�Ŀ����̣����Ƶ�aDisplayBoardArray��
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}
//�ڶ�����ɨ��aRecordBoard����������0��Ԫ�أ�������ߡ��Ƶ�aDisplayBoardArray����Ӧλ����
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
//ע�⣺aDisplayBoardArray����¼���ַ��������ַ���ÿ���ַ�ռ2���ֽڡ���͡�Ҳ�������ַ���ÿ��Ҳռ2���ֽڡ�
} 
void displayBoard(void){
	int i;
	//��һ��������
	//system("clear");   //���� 
#if 0 
	system("clr");
#endif
	//�ڶ�������aDisplayBoardArray�������Ļ��
	for(i=0;i<SIZE;i++){
		if(i<6){
			printf("%d %s\n",SIZE-i,aDisplayBoardArray[i]);
		}
		else{
			printf("%d  %s\n",SIZE-i,aDisplayBoardArray[i]);
		}
	}
	//������������������һ����ĸA B .... 
	printf("    A B C D E F G H I J K L M N O\n");
} 

int main()
{
	system("color f8"); 
    char choice;
    printf("ģʽѡ��-����/�˻�/AI��ս(A/B/C):");
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
