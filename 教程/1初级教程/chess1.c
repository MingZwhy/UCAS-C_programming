/*��ȡGBK����
1.�����DEVc++����������������
2.�����vscode������Ҫ�������뷽ʽ
�����ǵ�����½ǵ�UTF-8�����ͨ���������´�
������������"GBK",ѡ��GBK����Կ���������ʧ����������
���������ͼ�ڽ̳�pdf��
*/

//ͷ�ļ�
//�Ȳ���ΪɶҪ��Щͷ��include�Ͼ�������
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

/*------------------------------end------------------------------*/

/*--------------------------ȫ�ֱ�������--------------------------*/

/*����ʹ�õ���GBK���룬ÿһ�������ַ�ռ��2���ֽڡ�
���̻���ģ�� 
�����ʹ��vscode�򿪣�����ܷ������������Ǳ����
�б�������DEVc++���������gedit��ʱ������ʾ��
�������Ҳ�����
��Ҫ���ģ�����"����˵��.pdf"��ר�Ž�����
�������̱����˵���������vscode��������ʾ
���̵Ľ��������
*/
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

//aDisplayBoardArray����������ʾ����
char aDisplayBoardArray[SIZE][SIZE*CHARSIZE+1];

/*aRecordBoard�������ڼ�¼���̸�֣�����Ϊ��չʾ,����Ϊ�����Ǽ��㣩
���ǿ��Ե���recordtoDisplayArray������aRecordBoard�ļ�¼ת��Ϊ
aDisplayBoardArray�����ݣ�Ȼ���ٵ���displayBoard��ӡ�����̡�

aRecordBoard[i][j]
= NONE     :����[i][j]��Ϊ�գ�û������ 
= WHITE	   :����[i][j]���������
= BLACK    :����[i][j]���������
����һ�£������ú궨����NONE,WHITE,BLACK�ֱ�Ϊ0,1,2
*/
int aRecordBoard[SIZE][SIZE];

/*
"��"Ϊ����ĺ�����
"��"Ϊ����ĺ����ӣ�Ҳ�������Ϊ����ģ�
"��"Ϊ����İ�����
"��"Ϊ����İ����ӣ�Ҳ�������Ϊ����ģ�
*/
char play1Pic[]="��";
char play1CurrentPic[]="��"; 

char play2Pic[]="��";
char play2CurrentPic[]="��";

//leave[]��Ϊ�����롰quit���˳���Ϸ
char leave[]="quit";

/*------------------------------end------------------------------*/

/*----------------------------������������------------------------*/

//part1:�������̵Ĳ���

//��ʼ�����̡���ʵ���Ͼ��ǽ�ȫ�ֱ�����aRecordBoard��ΪNONE
void initRecordBorard(void);
//��aRecordBoard������ת��ΪaDisplayBoardArray������
void recordtoDisplayArray(int hang,int lie);
//��ӡ��aDisplayBoardArray������
void displayBoard(void);


//part2:���˶��Ĳ���
void Human_vs_Human(void);
void black_go(void);	  //�����������
void white_go(void);	  //�����������
bool judge_result(void);  //�жϽ��

/*------------------------------end------------------------------*/

/*----------------------------�������岿��------------------------*/

//part1:�������̵Ĳ���

//��ʼ������
void initRecordBorard(void)
{
	//ͨ��˫��ѭ������aRecordBoard��0
	int i,j;
	for(i=0 ; i<SIZE ; i++){
		for(j=0 ; j<SIZE ; j++){
			aRecordBoard[i][j] = NONE;
		}
	}
}

//��initRecordBorard�д洢����Ϣ������aDisplayBoardArray��
//����hang��lie Ϊ��ǰҪ����λ�õĺᡢ������
void recordtoDisplayArray(int hang, int lie){

	//��һ������aInitDisplayBoardArray�м�¼�Ŀ����̣����Ƶ�aDisplayBoardArray��
	int i,j;
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE*CHARSIZE+1;j++){
			aDisplayBoardArray[i][j] = aInitDisplayBoardArray[i][j];        
		}
	}

	//�ڶ�����ɨ��aRecordBoard����������0��Ԫ�أ�������ߡ��Ƶ�aDisplayBoardArray����Ӧλ����
	//ע��ÿ������ռ�����ַ�����˸���ʱҪ�������������ַ�
	for(i=0;i<SIZE;i++){
		for(j=0;j<SIZE;j++){
			if(aRecordBoard[i][j] != NONE){
				if(aRecordBoard[i][j] == BLACK){
                    if(i==hang && j==lie){
						//��Ҫ���ӡ���play1CurrentPic
                        aDisplayBoardArray[i][2*j] = play1CurrentPic[0];
					    aDisplayBoardArray[i][2*j+1] = play1CurrentPic[1];
                    }
                    else{
						//���������ӡ���play1Pic
                        aDisplayBoardArray[i][2*j] = play1Pic[0];
					    aDisplayBoardArray[i][2*j+1] = play1Pic[1];
                    }
				}
				else if(aRecordBoard[i][j] == WHITE){
                    if(i==hang && j==lie){
						//��Ҫ���ӡ���play2CurrentPic
                        aDisplayBoardArray[i][2*j] = play2CurrentPic[0];
				        aDisplayBoardArray[i][2*j+1] = play2CurrentPic[1];
                    }
                    else{
						//���������ӡ���play2Pic
					    aDisplayBoardArray[i][2*j] = play2Pic[0];
					    aDisplayBoardArray[i][2*j+1] = play2Pic[1];
                    }
				}
			}
		}
	}
} 

//��ӡչʾ����
void displayBoard(void){
	//��һ������������ϵͳ����system("clr")
	//����д�ͻ�֪������ʱ������ϣ�����������������
	//����������˺�CLEAR�����Ƿ�����
	//CLEAR=1������ �� CLEAR=0��������
#if CLEAR
	system("clr");
#endif

	//�ڶ�������aDisplayBoardArray�������Ļ��
	/*
	hint1��
	��ϸ�۲����̿��Է��֣�������������½ǳ�����
	����Ϊ0-SIZE-1 ; ����ΪA-O
	�����Ǵ�ӡ���Ǵ������´�ӡ�ģ�������������� SIZE - i
	
	hint2��
	��һ���棬ע�⵽�������10-14�Ƚϳ���������ǰ���ͬ��ʽ
	��ӡ����������̶Բ�����������˶�������10-14��
	������ǰ���ٿ�һ�񣬾��ܶ�����

	hint3��
	�����ڴ�ӡʱֻ�����У�ÿ�����ַ���ռλ��%s��ӡһ�У�
	����������ԭ���C����ָ���йء�������ʦ�ὲhh
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
	//������������������һ����ĸA B .... 
	printf("    A B C D E F G H I J K L M N O\n");

	//��Ȼ�㲻���Լ���һ��displayBoard����������һ�ԣ����Լ���
	//�����̵����
}


//part2�����˶���

void Human_vs_Human(void){

	//����ǰ��ȻҪ�ȳ�ʼ�����̡�������initRecordBorard����
	//��aRecordBoard��ֵ��ΪNONE
    initRecordBorard();

	//��������ʦ��Ҫ�󡪡���ӡһ��˭д��
    printf("Work of xxx\n");

	/*�������Ѿ����ˣ�����������ӡ���̵�aDisplayBoardArray
	��ʱ���ǿյģ���ʵ��Ҳ���ˣ��������¼��д��������˰����ڸ���hhh
	���ڸ�� ��ʼ����һ��aDisplayBoardArray���飬������һ��
	��׼��aInitDisplayBoardArray���̵�aDisplayBoardArray��
	*/
	int i,j;
	for(i=0 ; i<SIZE ; i++){
		for(j=0 ; j<SIZE*CHARSIZE+1 ; j++){
			aDisplayBoardArray[i][j]=aInitDisplayBoardArray[i][j];        
		}
	}

	//����displayBoard����չʾһ�³�ʼ������
	displayBoard();

	//������׼�����ˣ���ʼ���壡
    printf("\n���˶�ս��ʼ���ɺ��������ӣ�\n");

    int count=1;   //������ǰ����

    while(1){
		//˫����������ֱ����һ�������ʤ��
		//���������һ��whileѭ��ģ��Ծ�
		//ֱ������ʤ�����˳�ѭ��

        printf("�ֵ������µ�%d���壺\n",count);
		//����ڷ�����
        black_go();
        count++;  //���Ӽ���+1
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }

        printf("�ֵ������µ�%d���壺\n",count);
		//����׷�����
        white_go();
		count++;  //���Ӽ���+1
        if(judge_result()){
            printf("����ʤ������Ϸ����\n");
            break;
        }
    }
}

void black_go(void){

    char inf[20];
    printf("�������������������(��ĸ,����):");
    scanf("%s",inf);

	//char leave[] = "quit"
	//������Ϊquitʱ���˳���Ϸ
    if(strcmp(inf,leave)==0){              
        exit(0);
    }

	/*
	char eat = getchar();
	���д����������Ϊ����ӦC����scanf������ȱ��
	C���Ե�scanf�������û�����'\n'���س�ʱ��������
	����û�н�'\n'����inf��Ҳû�н������������ǽ���
	���ڻ�����������������д��뽫����д��'\n'����
	���»ص���scanfʱ�ͻ��Զ��������'\n'��Ҳ���ֱ�ӽ������롣

	hint: ��c������ʹ��scanf���һ��char eat=getchar();��һ��
	��ϰ�ߣ��ܹ�����ܶ����Է��ֵ�bug
	��Ȼ��char����Ϊeat�Ǳ��ߵĸ���ϰ�ߣ�����C�����Ƽ��Ĺ���
	*/
    char eat=getchar();

	/*
	��������������
	inf[0] = A/a / B/b / ... / Z/z   (����Ӧ�ð����û���Сд)
	inf[1] = ,
	inf[2:3] = 0 / 1 / 2 / ... / 14
	*/

	//���¿�����һЩ���������������ȻҲû������ȫ����������������һ��
    if(inf[0]<'A' || (inf[0]>'Y' && inf[0]<'a') || inf[0]>'y' || inf[1]!=',' || strlen(inf)>4 ){
        printf("������ʽ��������(��ĸ,����)��ʽ��������\n");
		//���������󣬲�Ҫֱ���˳����򣬳�����ҪһЩ���ݵĽ�����
		//ֻҪ���µ���black_go()�����������������
        black_go();
        return;
    }

	/*���ܱ���д��Щ�����ʱ�򻹲�����������row��col��
	�������� hang �� lie ��ʾ �� �� �� 
	*/
	

	/*			      ����hang        

	ע�⵱������ʱ����������ַ�������inf��2��3λ
	������1λ����2λ����������λʱ������Ҫ��һЩ���´���
	���ַ�ת��Ϊ����
	
	ͬʱ�ڴ�ӡ���̺������ᵽ���������Ͽ����ĺ�������ʵ����
	�Ķ�ά�����һ���±����෴��
	�� fact_hang = 15 - look_hang
	�����겻����������⣬�����ĺ�ʵ�ʵ�˳����ͬ
	*/

    int hang;

	//isdigit��C����Ϊ����������ṩ���е��õĺ���
	//���þ���������˼�����Ƿ�Ϊ�ַ�('0' �� '9')
	if(isdigit(inf[2]) && isdigit(inf[3])){     //����λ����ʱ 
		//hint: when inf[2] = '5' 
		//int num = inf[2] - '0';
		//now num is 5 but not '5'
		hang = 15 - ((inf[2]-'0')*10 + (inf[3]-'0'));
	}
	else{										//ֻ��һλ����ʱ
		hang = 15-(inf[2]-'0');    
	}


	/*					����lie
	���У������ڿ����ĺ�ʵ�ʵ�˳���෴�����⣬ΨһҪע�⵽��������Ӧ��
	�����û������д��ĸ����Сд��ĸ
	hint: 'a' - 'A' = 'b' - 'B' = 32
	*/

	int lie;

	//���ΪСд����ת��Ϊ��д
    if(inf[0]>='a'){
        inf[0]=inf[0]-32;
    }
    lie = inf[0]-'A';   

	//������Ҫ����ĳһ���������������ӵ�λ�õ����
    if(aRecordBoard[hang][lie]!=NONE){
        printf("�㿪���ǰɣ�");
        printf("\n����������\n");
		//���������������ͬ����Ҫ��������
        black_go();
        return;
    }

	/*
	���ں��壬ʵ�������ǳ���Ҫ�ж����Ƿ���������
	�����ӵ�λ�ã���Ҫ�жϺ����µ��Ƿ�Ϊ����
	���ڳ����̳��У������ݲ����ǽ���
	�ں��������У����ǽ���������Ľ����ж�
	if(Judge_if_banhand(hang,lie))
	{
		printf("���֣�");
		black_go();
		return;
	}
	*/

	//��������
    aRecordBoard[hang][lie]=BLACK;
	//��aRecordBoard����Ϣת��arecordtoDisplayArray
    recordtoDisplayArray(hang,lie);
	//��ӡ����   
    displayBoard();

	//���˶�ս�����û����壬��Ȼ����ǰҪѯ���û�
    //�ж��Ƿ����

	/*
	����char laizi ���ǰ���ס�ˣ���ʵ����
	*/
    printf("�Ƿ�ϣ�����壿(Y/�����)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
		printf("�������ϣ�\n");
		printf("���������ӣ�\n");
        black_go();
		return;
    }
}

void white_go(void){
	//white_go��black_go�Ǿ����
	//Ψһ�����ǰ��岻���жϽ���

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

    //���岻���жϽ���

    aRecordBoard[hang][lie]=WHITE;      //���Ӷ�Ӧֵ2
    recordtoDisplayArray(hang,lie); //�������hang��lie��Ϊ�˱��������
    displayBoard();
	
    //�ж��Ƿ����
    printf("�Ƿ�ϣ�����壿(Y/�����)");
    char laizi=getchar();
    if(laizi=='Y' || laizi=='y'){
        aRecordBoard[hang][lie]=NONE;
		printf("�������ϣ�\n");
		printf("���������ӣ�\n");
        white_go();
    }
}

//�ж���ֽ��
//�����̳��е��жϽ�������ô����߼������ܺÿ����������Ǽ�����������Ч�ʡ��ں�������׷���ٶ�ʱ��Ӧ�������Ż�judge_result����
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
	system("color xx")��Ϊ�����ÿ���̨�ı�����ɫ
	ϸ���Ҽǲ�����ˣ��������csdn�����ô�ı����̨�ı���ɫ
	Ŀ���������̺ÿ�Щ
	ŪЩ��ɫ���������Ǳ��Ϊ���������Ȥ��
	*/
	//system("color f8"); 

    char choice;
    printf("ģʽѡ��-����/�˻�/AI��ս(A/B/C):");
    scanf("%c",&choice);
    char eat=getchar();

    if(choice=='A' || choice=='a'){
        Human_vs_Human();
    }
    else if(choice=='B' || choice=='b'){
		//������δʵ����ֻ���ս��������ע�͵�
        //Human_vs_AI();
    }
    else if(choice=='C' || choice=='c'){
		//������δʵ��ֻ��ֻ���ս��������ע�͵�
        //AI_vs_AI();
    }
    else{
    	printf("wrong order!");
	}
    return 0;
}