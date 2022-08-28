#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define MAXLEN 255
#define NO_FRB 0
#define THREE_FRB 1
#define FOUR_FRB 2
#define LONG_FRB 3

#define NONE 2
#define BLACK 1
#define WHITE 0

#define width 15
#define depth 7
#define INF -10000000
clock_t start_time;
clock_t finish_time;
float current_time;
int isWin = 0;
int x, y;
int step = 0;
int swit = 1;
int qipan[16][16];
int frb_qipan[16][16];
int real_qipan[16][16];
void LastTime();
void Printd();
int Handling(char s[]);
int Initialization();
char s[450];
char _s[3];
int Boundary(int x_b, int y_b, char c, int level);
int Judgement(int _x, int _y);
int ForbiddenCheck(int _x, int _y, int _swit);
void ClearMyArray();
int Score(int _x, int _y, int _swit);
int RealScore(int _x, int _y, int _swit, int object);
void ScoreCheck(int _x, int _y, int switScore);
int ScoreAllCheck(int _x, int _y, int _swit);
int PointForbiddenCheck(int _x, int _y, int direction, int same);
struct forbidden
{
    bool frbdone;
    int five;
    int four[11];
    /*
    0. 活四：_ o o o o _
    1. 冲四：x o o o o _
    2. 死四：x o o o o x
    3. （甲）跳四：_ o _ o o o _
    4. 乙跳四：_ o _ o o o x
    5. 异乙跳四：x o _ o o o _
    6. 丙跳四：x o _ o o o x
    7. （甲）隔四：_ o o _ o o _
    8. 乙隔四：x o o _ o o _
    9. 丙隔四：x o o _ o o x
    */
    int three[11];
    /*
    0. 活三：_ _ o o o _ _ 或 x _ o o o _ _
    1. 冲三：x o o o _ _
    2. 死三：x o o o _ x 或 x o o o x
    3. 假死三：x _ o o o _x
    4. （甲）跳三：_ o _ o o _
    5. 乙跳三：_ o _ o o x
    6. 异乙跳三：x o _ o o _
    7. 丙跳三：x o _ o o x
    8. （甲）隔三：_ o _ o _ o _
    9. 乙隔三：x o _ o _ o _
    10. 丙隔三：x o _ o _ o x
    */
    int two[11];
    /*
    0. 活二：_ _ o o _ _  或 x _ o o _ _ 
    1. 冲二：x o o _ _
    2. 死二：x _ o o _ x 或 x o o x 或 x o o _ _ x  或 x _ o _ o x  或 x _ o o x 或 x o _ o x
    3. （甲）跳二：_ o _ o _
    4. 乙跳二：_ o _ o x
    */
    // int frb;
    //bool nofrb;
    bool frbing;
    //int score;
};
struct forbidden Fqipan[15][15];
struct decision
{
    int score;
    int _x;
    int _y;
};
struct decision Dtree[width];
//资源库
char source[MAXLEN][2 * MAXLEN];
struct LocationNode
{
    int _x;
    int _y;
};
typedef struct TreeNode
{
    struct TreeNode *childern[MAXLEN];
    struct LocationNode location;
} ChessTree, *Ptr_To_ChessTree;

ChessTree *talloc(void)
{
    return (ChessTree *)malloc(sizeof(ChessTree));
}
void ReadSource(int first)
{
    FILE *_fp;
    if (first == 1)
        _fp = fopen("mysource.txt", "r");
    else
        _fp = fopen("mysource0.txt", "r");
    int i = 0;
    while (fgets(source[i], 450, _fp))
    {
        i++;
    }
    fclose(_fp);
    return;
}
ChessTree *TreeBirth()
{
    Ptr_To_ChessTree plist, p;
    plist = talloc();
    plist->childern[0] = NULL;
    plist->location._x = -1;
    plist->location._y = -1;

    for (int i = 0; i < MAXLEN; i++)
    {
        p = plist;
        for (int j = 0; j < MAXLEN; j = j + 2)
        {
            if (source[i][j] == '\0' || source[i][j] == '\n')
            {
                break;
            }
            int jumpSwit = 0;
            Ptr_To_ChessTree pre = p;
            int k = 0;
            while (pre->childern[k] != NULL)
            {
                if (pre->childern[k]->location._x == (source[i][j] - 'a') && pre->childern[k]->location._y == (source[i][j + 1] - 'a'))
                {
                    p = p->childern[k];
                    jumpSwit = 1;
                    break;
                }
                k++;
            }
            if (jumpSwit != 0)
            {
                continue;
            }
            Ptr_To_ChessTree pnode = talloc();
            pnode->childern[0] = NULL;
            pnode->location._x = source[i][j] - 'a';
            pnode->location._y = source[i][j + 1] - 'a';
            pre->childern[k] = pnode;
            pre->childern[k + 1] = NULL;
            p = p->childern[k];
        }
    }
    return plist;
    /*int i = 0;
    p = plist;
   // p = p->childern[1];
   /* while (1)
    {
        if (p->childern[0] != NULL)
        {
            printf("%d %d\n", p->location._x, p->location._y);
            p = p->childern[0];
        }
        else
        {
            printf("%d %d\n", p->location._x, p->location._y);
            break;
        }
    }*/
}
int RandomChess(int level, int passValue, int _swit);
int isInSource = 1;
int main()
{
    int c;
    char input[100];
    int mode = Initialization();
    if (mode == 1)
    {
        Printd();
        getchar();
        while (1)
        {
            ClearMyArray();

            for (int i = 0; i < 15; i++)
            { //赋值到禁手棋盘上
                for (int j = 0; j < 15; j++)
                {
                    frb_qipan[i][j] = qipan[i][j];
                }
            }
            int inputNum = 0;
            while ((c = getchar()) != EOF && c != '\n')
            {
                input[inputNum] = c;
                inputNum++;
            };
            input[inputNum] = '\0';
            while (!Handling(input))
            {
                printf("请输入坐标（例如：3，h），输入quit可以直接结束进程：\n");
                inputNum = 0;
                while ((c = getchar()) != EOF && c != '\n')
                {
                    input[inputNum] = c;
                    inputNum++;
                };
                input[inputNum] = '\0';
            };
            if ((qipan[x][y] != 2))
            {
                printf("此处已经有棋子了！\n");
                continue;
            }
            else
            {
                if (swit == 1 && ForbiddenCheck(x, y, 1) != NO_FRB)
                {
                    printf("禁手\n");
                    exit(0);
                }
                qipan[x][y] = swit;
                frb_qipan[x][y] = qipan[x][y];
                for (int i = 0; i < 15; i++)
                { //赋值到禁手棋盘上
                    for (int j = 0; j < 15; j++)
                    {
                        if (qipan[i][j] == 2 && (1 - swit) == 1 && ForbiddenCheck(i, j, 1) != NO_FRB)
                            real_qipan[i][j] = -1;
                        else
                        {
                            real_qipan[i][j] = qipan[i][j];
                        }
                    }
                }
            }
            //system("clear");
            Printd();
            ClearMyArray();
            if (Judgement(x, y))
            {
                if (swit == 1)
                {
                    printf("黑子获胜，游戏结束！\n");
                }
                if (swit == 0)
                {
                    printf("白子获胜，游戏结束！\n");
                }
                exit(1);
            }
            // swit = 1 - swit;
        }
    }
    else if (mode == 2)
    {
        int first;
        printf("选择先手：\n1.机器先手\n2.玩家先手\n");
        scanf("%d", &first);
        printf("\n");
        Printd();
        getchar();
        ReadSource(first);
        Ptr_To_ChessTree p = TreeBirth();
        FILE *pf;
        if (first == 1)
        {
            pf = fopen("mysource0.txt", "a+");
        }
        else
            pf = fopen("mysource.txt", "a+");
        while (1)
        {
            ClearMyArray();
            for (int i = 0; i < 15; i++)
            { //赋值到禁手棋盘上
                for (int j = 0; j < 15; j++)
                {
                    frb_qipan[i][j] = qipan[i][j];
                }
            }
            if ((swit == 1 && first == 1) || (swit == 0 && first == 2))
            {
                if (step == 0 && first == 1)
                {
                    x = 7;
                    y = 7;
                    int k = 0;
                    while (1)
                    {
                        if (x == p->childern[k]->location._x && y == p->childern[k]->location._y)
                        {
                            p = p->childern[k];
                            break;
                        }
                        k++;
                        if (p->childern[k] == NULL)
                        {
                            isInSource = 0;
                            break;
                        }
                    }
                }
                else if (isInSource == 1 && swit == 1 && first == 1)
                {
                    int k = 0;
                    if (p->childern[0] != NULL)
                    {
                        while (1)
                        {
                            if (x == p->childern[k]->location._x && y == p->childern[k]->location._y)
                            {
                                p = p->childern[k];
                                if (p->childern[0] != NULL)
                                {
                                    p = p->childern[0];
                                    x = p->location._x;
                                    y = p->location._y;
                                    break;
                                }
                            }
                            k++;
                            if (p->childern[k] == NULL)
                            {
                                isInSource = 0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        isInSource = 0;
                    }
                }
                else if (isInSource == 1 && swit == 0)
                {
                    int k = 0;
                    if (p->childern[0] != NULL)
                    {
                        while (1)
                        {
                            if (x == p->childern[k]->location._x && y == p->childern[k]->location._y)
                            {
                                p = p->childern[k];
                                if (p->childern[0] != NULL)
                                {
                                    p = p->childern[0];
                                    x = p->location._x;
                                    y = p->location._y;
                                    break;
                                }
                            }
                            k++;
                            if (p->childern[k] == NULL)
                            {
                                isInSource = 0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        isInSource = 0;
                    }
                }
                if (isInSource == 0)
                    RandomChess(1, -INF, swit);
            }
            else
            {
                int inputNum = 0;
                while ((c = getchar()) != EOF && c != '\n')
                {
                    input[inputNum] = c;
                    inputNum++;
                };

                input[inputNum] = '\0';
                while (!Handling(input))
                {
                    printf("请输入坐标（例如：3，h），输入quit可以直接结束进程：\n");
                    inputNum = 0;
                    while ((c = getchar()) != EOF && c != '\n')
                    {
                        input[inputNum] = c;
                        inputNum++;
                    };
                    input[inputNum] = '\0';
                };
                if ((qipan[x][y] != 2))
                {
                    printf("此处已经有棋子了！\n");
                    continue;
                }
            }
            for (int i = 0; i < 15; i++)
            {
                for (int j = 0; j < 15; j++)
                {
                    frb_qipan[i][j] = qipan[i][j];
                }
            }
            if (swit == 1 && ForbiddenCheck(x, y, 1) != NO_FRB)
            {
                printf("%d %d ", x, y);
                printf("%d", real_qipan[x][y]);
                printf("禁手\n");
                exit(0);
            }

            qipan[x][y] = swit;
            _s[0] = 'a' + x;
            _s[1] = 'a' + y;
            _s[2] = '\0';
            strcat(s, _s);
            step++;
            frb_qipan[x][y] = qipan[x][y];
            swit = 1 - swit;
            for (int i = 0; i < 15; i++)
            { //赋值到禁手棋盘上
                for (int j = 0; j < 15; j++)
                {
                    if (swit == 1 && qipan[i][j] == 2 && ForbiddenCheck(i, j, 1) != NO_FRB)
                    {
                        real_qipan[i][j] = -1;
                    }
                    else
                    {
                        real_qipan[i][j] = qipan[i][j];
                    }
                }
            }
            //system("clear");
            Printd();
            ClearMyArray();
            //ScoreCheck(x, y);
            //printf("生成的活3：%d，生成的冲3：%d，生成的活4：%d，生成的冲4：%d\n", Fqipan[x][y].three[0], Fqipan[x][y].three[1], Fqipan[x][y].four[0], Fqipan[x][y].four[1]);
            // printf("Score:%d\n", Score(x, y));

            if (Judgement(x, y))
            {
                if (swit == 1)
                {
                    printf("白子获胜，游戏结束！\n");
                    if (isInSource != 1)
                    {
                        fprintf(pf, "%s\n", s);
                        fclose(pf);
                    }
                }
                if (swit == 0)
                {
                    printf("黑子获胜，游戏结束！\n");
                    if (isInSource != 1)
                    {
                        fprintf(pf, "%s\n", s);
                        fclose(pf);
                    }
                }
                exit(0);
            }
        }
    }
    else
    {
        printf("错误的输入，程序停止！\n");
        exit(0);
    }
}
void LastTime(int i, int j) //标记最近一次的棋子
{
    if (qipan[i][j] == 1)
    {
        if ((i) == x && (j) == y)
            printf("▲");
        else
            printf("●");
    }
    if (qipan[i][j] == 0)
    {
        if ((i) == x && (j) == y)
            printf("△");
        else
            printf("○");
    }
}
void Printd() //打印棋盘
{
    int i, j;
    for (i = 0; i <= 14; i++)
    {
        //第1行
        if (i == 0)
        {
            //第1列
            printf("15");
            j = 0;
            LastTime(i, j);
            if (qipan[i][j] == 2)
                printf("┌");

            //第2-14列
            for (j = 1; j <= 13; j++)
            {
                LastTime(i, j);
                if (qipan[i][j] == 2)
                    printf("┬");
            }

            //第15列
            LastTime(i, j);
            if (qipan[i][j] == 2)
                printf("┐");

            printf("\n");
        }

        //第2-14行
        if (i <= 13 && i >= 1)
        {
            //第1列
            if (15 - i >= 10)
                printf("%d", 15 - i);
            else
                printf("%d ", 15 - i);
            j = 0;
            LastTime(i, j);
            if (qipan[i][0] == 2)
                printf("├");

            //第2-14列
            for (j = 1; j <= 13; j++)
            {
                LastTime(i, j);
                if (qipan[i][j] == 2)
                    printf("┼");
            }

            //第15列
            LastTime(i, j);
            if (qipan[i][j] == 2)
                printf("┤");
            printf("\n");
        }

        //第15行

        if (i == 14)
        {
            printf("%d ", 15 - i);
            j = 0;
            LastTime(i, j);
            if (qipan[i][0] == 2)
                printf("└");

            for (j = 1; j <= 13; j++)
            {
                LastTime(i, j);
                if (qipan[i][j] == 2)
                    printf("┴");
            }
            LastTime(i, j);
            if (qipan[i][j] == 2)
                printf("┘");
            printf("\n");
        }
    }
    printf("  a b c d e f g h i j k l m n o          作者：谷朝阳\n");
    printf("请输入坐标（例如：3，h），输入quit可以直接结束进程：\n");
}
int Handling(char s[]) //处理输入的数组
{
    if ((s[0] == 'q' || s[0] == 'Q') && (s[1] == 'u' || s[1] == 'U') && (s[2] == 'i' || s[2] == 'I') && (s[3] == 't' || s[3] == 'T'))
        exit(0);
    if (s[0] >= 'a' && s[0] <= 'o')
    {
        y = s[0] - 'a';
        if (s[1] == ',' && s[2] >= '0' && s[2] <= '9')
        {
            if (s[3] >= '0' && s[3] <= '9')
            {
                x = (s[2] - '0') * 10 + (s[3] - '0');
                if (x <= 15 && x >= 1)
                {
                    x--;
                    x = 14 - x;
                    return 1;
                }
            }
            else
            {
                x = s[2] - '0';
                if (x >= 1)
                {
                    x--;
                    x = 14 - x;
                    return 1;
                }
            }
        }
    }
    if (s[0] >= 'A' && s[0] <= 'O')
    {
        y = s[0] - 'A';
        if (s[1] == ',' && s[2] >= '0' && s[2] <= '9')
        {
            if (s[3] >= '0' && s[3] <= '9')
            {
                x = (s[2] - '0') * 10 + (s[3] - '0');
                if (x <= 15 && x >= 1)
                {
                    x--;
                    x = 14 - x;
                    return 1;
                }
            }
            else
            {
                x = s[2] - '0';
                if (x >= 1)
                {
                    x--;
                    x = 14 - x;
                    return 1;
                }
            }
        }
    }
    if (s[0] >= '0' && s[0] <= '9')
    {
        if (s[1] == ',')
        {
            x = s[0] - '0';
            x--;
            x = 14 - x;
            if (s[2] >= 'a' && s[2] <= 'o')
            {
                y = s[2] - 'a';
                return 1;
            }
        }
        else if (s[1] >= '0' && s[1] <= '9')
        {
            x = (s[0] - '0') * 10 + (s[1] - '0');
            if (x <= 15 && x >= 1)
            {
                x--;
                x = 14 - x;
                if (s[2] == ',' && s[3] >= 'a' && s[3] <= 'o')
                {
                    y = s[3] - 'a';
                    return 1;
                }
            }
        }
    }
    if (s[0] >= '0' && s[0] <= '9')
    {
        if (s[1] == ',')
        {
            x = s[0] - '0';
            x--;
            x = 14 - x;
            if (s[2] >= 'A' && s[2] <= 'O')
            {
                y = s[2] - 'A';
                return 1;
            }
        }
        else if (s[1] >= '0' && s[1] <= '9')
        {
            x = (s[0] - '0') * 10 + (s[1] - '0');
            if (x <= 15 && x >= 1)
            {
                x--;
                x = 14 - x;
                if (s[2] == ',' && s[3] >= 'A' && s[3] <= 'O')
                {
                    y = s[3] - 'A';
                    return 1;
                }
            }
        }
    }
    printf("格式输入有误，请重新输入\n");
    return 0;
}
int Initialization() //棋盘初始化
{
    x = -2;
    y = -2;
    int mode;
    for (int m = 0; m < 15; m++)
    {
        for (int n = 0; n < 15; n++)
        {
            qipan[m][n] = 2;
            frb_qipan[m][n] = 2;
            Fqipan[m][n].frbing = 0;
            real_qipan[m][n] = 2;
        }
    }
    // qipan[2][7]=qipan[5][3]=qipan[6][4]=qipan[6][7]=qipan[7][2]=qipan[7][4]=qipan[7][5]=qipan[7][6]=qipan[7][7]=qipan[7][9]=qipan[8][6]=qipan[8][7]=qipan[8][8]=qipan[9][4]=qipan[9][5]=qipan[9][8]=qipan[10][4]=1;
    // qipan[3][6]=qipan[4][2]=qipan[4][5]=qipan[4][6]=qipan[5][4]=qipan[5][7]=qipan[6][3]=qipan[6][6]=qipan[6][8]=qipan[7][3]=qipan[7][8]=qipan[8][4]=qipan[8][5]=qipan[8][10]=qipan[9][7]=qipan[10][9]=qipan[11][3]=0;
    //qipan[3][6]=qipan[4][2]=qipan[4][4]=qipan[4][5]=qipan[5][1]=qipan[5][6]=qipan[5][7]=qipan[6][2]=qipan[6][6]=qipan[6][7]=qipan[7][3]=qipan[7][6]=qipan[7][7]=qipan[7][8]=qipan[7][9]=qipan[8][1]=qipan[8][3]=qipan[8][4]=qipan[8][5]=qipan[8][6]=qipan[8][10]=qipan[9][4]=qipan[9][8]=qipan[10][4]=qipan[11][3]=1;
    //qipan[3][4]=qipan[4][0]=qipan[4][3]=qipan[4][6]=qipan[4][7]=qipan[5][2]=qipan[5][3]=qipan[5][4]=qipan[5][5]=qipan[5][8]=qipan[6][4]=qipan[6][8]=qipan[7][5]=qipan[7][10]=qipan[8][2]=qipan[8][7]=qipan[8][8]=qipan[8][9]=qipan[9][2]=qipan[9][5]=qipan[9][6]=qipan[9][7]=qipan[10][3]=qipan[10][7]=qipan[11][4]=0;
    //qipan[7][7] = qipan[7][6] = qipan[7][8] = qipan[8][7] = qipan[6][7] = qipan[8][5] = qipan[8][4] = qipan[6][4] = qipan[5][9] = qipan[4][6] = qipan[9][11] = qipan[10][9] = qipan[10][7] = qipan[11][9] = 1;
    // qipan[8][8] = qipan[8][6] = qipan[8][9] = qipan[8][10] = qipan[9][7] = qipan[9][8] = qipan[10][8] = qipan[6][8] = qipan[6][9] = qipan[5][4] = qipan[5][7] = qipan[5][8] = qipan[7][5] = qipan[7][9] = 0;
    //qipan[7][7]=qipan[7][6]=qipan[7][5]=qipan[7][11]=qipan[8][6]=qipan[8][9]=qipan[9][6]=qipan[9][7]=qipan[9][8]=qipan[6][6]=qipan[6][8]=qipan[5][7]=qipan[3][9]=1;
    //qipan[7][8]=qipan[7][9]=qipan[8][8]=qipan[10][6]=qipan[6][4]=qipan[6][7]=qipan[6][9]=qipan[6][10]=qipan[5][6]=qipan[5][9]=qipan[4][8]=qipan[4][9]=qipan[4][10]=0;
    // qipan[7][7]=qipan[8][5]=qipan[6][6]=qipan[5][6]=qipan[5][4]=qipan[5][3]=qipan[4][3]=qipan[4][5]=qipan[4][6]=qipan[4][7]=qipan[4][9]=qipan[3][4]=qipan[2][3]=qipan[1][1]=1;
    // qipan[7][6]=qipan[8][7]=qipan[6][7]=qipan[6][5]=qipan[5][5]=qipan[4][4]=qipan[3][3]=qipan[2][2]=qipan[2][5]=qipan[1][2]=qipan[3][2]=qipan[4][2]=0;
    // qipan[7][7] = qipan[7][6] = qipan[7][5] = qipan[8][6] = qipan[9][8] = qipan[6][7] = qipan[5][8] = qipan[4][6] = qipan[4][7] = qipan[4][9] = qipan[4][7] = 1;
    // qipan[7][8] = qipan[6][8] = qipan[8][8] = qipan[8][7] = qipan[8][5] = qipan[6][4] = qipan[3][10] = qipan[5][6] = qipan[5][7] = qipan[4][5] = 0;
    //  qipan[7][7]=qipan[6][7]=qipan[8][7]=qipan[9][6]=qipan[10][8]=1;
    // qipan[7][8]=qipan[8][8]=qipan[9][8]=qipan[9][7]=qipan[10][6]=0;
    //   qipan[7][7]=qipan[7][8]=qipan[6][9]=qipan[5][10]=qipan[5][7]=qipan[6][5]=qipan[8][5]=qipan[4][7]=1;
    //qipan[3][10]=qipan[4][9]=qipan[4][11]=qipan[6][7]=qipan[7][6]=qipan[8][7]=qipan[9][7]=qipan[10][7]=0;
    //qipan[3][4] = qipan[3][5] = qipan[3][6] = 0;
    // qipan[3][2] = 1;
    //qipan[3][6] = qipan[4][7] = qipan[4][10] = qipan[5][5] = qipan[5][7] = qipan[5][8] = qipan[6][4] = qipan[6][8] = qipan[7][7] = qipan[7][8] = qipan[8][4] = qipan[8][6] = qipan[8][7] = qipan[9][4] = qipan[9][6] = qipan[9][7] = qipan[9][8] = qipan[10][5] = qipan[10][7] = 0;
    // qipan[3][9] = qipan[4][6] = qipan[4][8] = qipan[5][6] = qipan[5][9] = qipan[6][5] = qipan[6][6] = qipan[6][7] = qipan[6][9] = qipan[7][4] = qipan[7][5] = qipan[7][6] = qipan[8][5] = qipan[8][8] = qipan[9][5] = qipan[10][6] = qipan[11][4] = qipan[11][7] = 1;
    // qipan[5][7] = qipan[6][6] = qipan[6][10] = qipan[7][6] = qipan[7][7] = qipan[7][8] = qipan[9][9] = qipan[10][12] = 1;
    //  qipan[6][7] = qipan[6][8] = qipan[6][9] = qipan[7][9] = qipan[8][8] = qipan[8][9] = qipan[8][10] = qipan[9][11] = 0;
    /*  qipan[7][5] = qipan[7][6] = qipan[7][7] = 0;
    qipan[8][6] = qipan[9][6] = 1;
    qipan[7][9] = 1;
    //qipan[4][6] = qipan[6][6] = qipan[7][7] = qipan[7][5] = qipan[7][6] = qipan[8][6] = qipan[8][7] = qipan[8][9] = qipan[6][11] = 1;
    // qipan[5][6] = qipan[5][7] = qipan[6][7] = qipan[6][8] = qipan[6][9] = qipan[6][10] = qipan[7][8] = qipan[8][8] = qipan[9][6] = 0;
    /*  qipan[1][7] = 0;
    qipan[1][8] =0;
    qipan[1][9] = 0;
    /*
     qipan[7][7] = 1;
    qipan[7][8] =1;
    qipan[7][9] = 2;
    /*qipan[6][9]=0;
    qipan[8][9]=0;*/
    //qipan[7][6] = 1;
    /*qipan[8][6] = 1;
     
    qipan[9][6] = 1;
    qipan[10][7] = 1;
    qipan[10][8] = 1;
    qipan[9][9] = 1;
    qipan[8][9] = 1;
    //
    qipan[0][0] = 0;
    qipan[1][0] = 0;
    qipan[2][0] = 0;
    qipan[14][0] = 0;
    qipan[13][0] = 0;
    qipan[12][0] = 0;
    qipan[14][1] = 0;
    qipan[0][14] = 0;*/
    printf("请选择模式：\n1.人人对战\n2.人机对战\n");
    scanf("%d", &mode);
    return mode;
}
void Qsort(struct decision Dtree[], int left, int right)
{
    int i, last;
    void swap(struct decision Dtree[], int i, int j);
    if (left >= right)
    {
        return;
    }
    swap(Dtree, left, (left + right) / 2);
    last = left;
    for (i = left + 1; i <= right; i++)
    {
        if (Dtree[i].score > Dtree[left].score)
        {
            swap(Dtree, ++last, i);
        }
    }
    swap(Dtree, left, last);
    Qsort(Dtree, left, last - 1);
    Qsort(Dtree, last + 1, right);
}
void swap(struct decision Dtree[], int i, int j)
{
    int temp_x = Dtree[i]._x;
    int temp_y = Dtree[i]._y;
    int tempscore = Dtree[i].score;
    Dtree[i].score = Dtree[j].score;
    Dtree[i]._x = Dtree[j]._x;
    Dtree[i]._y = Dtree[j]._y;
    Dtree[j].score = tempscore;
    Dtree[j]._x = temp_x;
    Dtree[j]._y = temp_y;
    return;
}
int RandomChess(int level, int passValue, int _swit)
{
    int score = 0;
    ClearMyArray(); //清理数组
    int _x = 0;
    int _y = 0;
    struct decision Dtree[250];
    for (int i = 0; i < width; i++)
    {
        Dtree[i].score = INF;
        Dtree[i]._x = -1;
        Dtree[i]._y = -1;
    }

    int isscore[16][16];
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            isscore[i][j] = 0;
            frb_qipan[i][j] = qipan[i][j];
        }
    }
    if (level >= depth) //底层用局面分
    {
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                if (qipan[i][j] != 2)
                {
                    for (int m = -1; m < 2; m++)
                    {
                        for (int n = -1; n < 2; n++)
                        {
                            if (i + m >= 0 && i + m <= 14 && j + n >= 0 && j + n <= 14)
                            {
                                if (qipan[i + m][j + n] == 2 && isscore[i + m][j + n] != 1 && (_swit == 0 || (_swit == 1 && ForbiddenCheck(i + m, j + n, 1) == NO_FRB)))
                                {
                                    isscore[i + m][j + n] = 1;
                                    if ((score = ScoreAllCheck(i + m, j + n, _swit)) >= Dtree[0].score)
                                    {
                                        Dtree[0].score = score;
                                        Dtree[0]._x = i + m;
                                        Dtree[0]._y = j + n;
                                        if (Dtree[0].score >= (-INF))
                                        {
                                            x = i + m;
                                            y = j + n;
                                            return Dtree[0].score;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        x = Dtree[0]._x;
        y = Dtree[0]._y;
        return Dtree[0].score;
    }
    else
    {
        //这里我们利用_swit代替全局变量，使得函数更加独立。
        //首先要判断的是反向的swit，白子先判断黑子得分，然后判断白子得分
        //对于白子，存在反向禁手
        if (_swit == 0)
        {
            for (int i = 0; i < 15; i++) //real棋盘赋值，对于白子而言，黑子的禁手点对它没有影响；对于黑子而言，白子的分布对他没有影响
            {
                for (int j = 0; j < 15; j++)
                {
                    if (qipan[i][j] == 2 && ForbiddenCheck(i, j, 1) != NO_FRB)
                        real_qipan[i][j] = -1;
                    else
                    {
                        real_qipan[i][j] = qipan[i][j];
                    }
                }
            }
            for (int i = 0; i < 15; i++)
            {
                for (int j = 0; j < 15; j++)
                {
                    if (qipan[i][j] != 2)
                    {
                        for (int m = -1; m < 2; m++)
                        {
                            for (int n = -1; n < 2; n++)
                            {
                                if (i + m >= 0 && i + m <= 14 && j + n >= 0 && j + n <= 14)
                                {
                                    if (qipan[i + m][j + n] == 2 && isscore[i + m][j + n] != 1)
                                    {
                                        isscore[i + m][j + n] = 1;
                                        if (real_qipan[i + m][j + n] != -1)
                                            ScoreCheck(i + m, j + n, 1);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (int i = 0; i < 15; i++)
            { //真实棋盘变换
                for (int j = 0; j < 15; j++)
                {
                    if (real_qipan[i][j] == -1)
                        real_qipan[i][j] = 2;
                }
            }
        }
        else if (_swit == 1)
        {
            for (int i = 0; i < 15; i++) //real棋盘赋值，对于白子而言，黑子的禁手点对它没有影响；对于黑子而言，白子的分布对他没有影响
            {
                for (int j = 0; j < 15; j++)
                {
                    real_qipan[i][j] = qipan[i][j];
                }
            }
            for (int i = 0; i < 15; i++)
            {
                for (int j = 0; j < 15; j++)
                {
                    if (qipan[i][j] != 2)
                    {
                        for (int m = -1; m < 2; m++)
                        {
                            for (int n = -1; n < 2; n++)
                            {
                                if (i + m >= 0 && i + m <= 14 && j + n >= 0 && j + n <= 14)
                                {
                                    if (qipan[i + m][j + n] == 2 && isscore[i + m][j + n] != 1)
                                    {
                                        isscore[i + m][j + n] = 1;
                                        ScoreCheck(i + m, j + n, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (int i = 0; i < 15; i++)
            { //真实棋盘变换
                for (int j = 0; j < 15; j++)
                {
                    if (qipan[i][j] == 2 && ForbiddenCheck(i, j, 1) != NO_FRB)
                        real_qipan[i][j] = -1;
                }
            }
        }
        //最后的评分
        int m = 0;
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                if (isscore[i][j] == 1 && real_qipan[i][j] != -1)
                {
                    int fiveRecord = Fqipan[i][j].five;
                    ScoreCheck(i, j, _swit);
                    if (Fqipan[i][j].five > fiveRecord)
                    {
                        qipan[i][j] = _swit;
                        if (Judgement(i, j))
                        {
                            qipan[i][j] = 2;
                            score = -INF;
                            x = i;
                            y = j;
                            return score;
                        }
                        qipan[i][j] = 2;
                    }
                    Dtree[m].score = Score(i, j, _swit);
                    Dtree[m]._x = i;
                    Dtree[m]._y = j;
                    m++;
                    /*if ((score = Score(i, j, _swit)) > Dtree[width - 1].score)
                    {
                        if (Dtree[width - 1]._x < 0)
                        {
                            int m = 0;
                            while (m < width)
                            {
                                if (Dtree[m]._x < 0)
                                {
                                    Dtree[m].score = score;
                                    Dtree[m]._x = i;
                                    Dtree[m]._y = j;
                                    if (m == width - 1)
                                    {
                                        Qsort(Dtree, 0, width - 1);
                                    }
                                    break;
                                }
                                m++;
                            }
                        }
                        else
                        {
                            Dtree[width - 1].score = score;
                            Dtree[width - 1]._x = i;
                            Dtree[width - 1]._y = j;
                            Qsort(Dtree, 0, width - 1);
                        }
                    }*/
                }
            }
        }
        Qsort(Dtree, 0, m - 1);
        //开一层的计算
        if (Dtree[0].score >= 50000 && level == 1)
        {
            x = Dtree[0]._x;
            y = Dtree[0]._y;
            printf("本步落子位置为%d,%c,得分为：%d\n", 15 - x, y + 'a', Dtree[0].score);
            return Dtree[0].score;
        }
        /*if (level >= depth)
        {
            //开一层的计算
            if (level == 1)
            {
                x = Dtree[0]._x;
                y = Dtree[0]._y;
            }
            return Dtree[0];
        }*/
        int maxVaule = INF; //剪枝的准备
        for (int i = 0; i < width; i++)
        {
            /*if (level >= 6 && i >= 3)
            {
                Dtree[i].score = INF;
                continue;
            }*/
            if (level >= 5 && i >= 5)
            {
                Dtree[i].score = INF;
                continue;
            }
            if (level >= 4 && i >= 7)
            {
                Dtree[i].score = INF;
                continue;
            }
            if (level >= 3 && i >= 12)
            {
                Dtree[i].score = INF;
                continue;
            }
            if (Dtree[i]._x >= 0)
            {
                qipan[Dtree[i]._x][Dtree[i]._y] = _swit;
                if (Dtree[i].score >= 50000 && Judgement(Dtree[i]._x, Dtree[i]._y))
                {
                    qipan[Dtree[i]._x][Dtree[i]._y] = 2;
                    return Dtree[i].score;
                }
                Dtree[i].score = -RandomChess(++level, -maxVaule, 1 - _swit);
                level--;
                qipan[Dtree[i]._x][Dtree[i]._y] = 2;
                if (Dtree[i].score > passValue)
                { //剪枝
                    return Dtree[i].score;
                }
                if (Dtree[i].score > maxVaule)
                { //标记
                    maxVaule = Dtree[i].score;
                }
                qipan[Dtree[i]._x][Dtree[i]._y] = _swit;
                if (Dtree[i].score >= 50000 && Judgement(Dtree[i]._x, Dtree[i]._y))
                {
                    qipan[Dtree[i]._x][Dtree[i]._y] = 2;
                    return Dtree[i].score;
                }
                qipan[Dtree[i]._x][Dtree[i]._y] = 2;
            }
        }
        Qsort(Dtree, 0, width - 1);
        if (level == 1)
        {
            Printd();
            x = Dtree[0]._x;
            y = Dtree[0]._y;
            printf("本步落子位置为%d,%c,得分为：%d\n", 15 - x, y + 'a', Dtree[0].score);
        }
        return Dtree[0].score;
    }
}

int Boundary(int x_b, int y_b, char c, int level)
{
    switch (c)
    {
    case 0:
        if (y_b + level > 14)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b][y_b + level] == 2)
            {
                if (real_qipan[x_b][y_b + level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b][y_b + level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b][y_b + level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 1:
        if (x_b - level < 0 || y_b + level > 14)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b - level][y_b + level] == 2)
            {
                if (real_qipan[x_b - level][y_b + level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b - level][y_b + level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b - level][y_b + level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 2:
        if (x_b - level < 0)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b - level][y_b] == 2)
            {
                if (real_qipan[x_b - level][y_b] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b - level][y_b] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b - level][y_b] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 3:
        if (x_b - level < 0 || y_b - level < 0)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b - level][y_b - level] == 2)
            {
                if (real_qipan[x_b - level][y_b - level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b - level][y_b - level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b - level][y_b - level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 4:
        if (y_b - level < 0)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b][y_b - level] == 2)
            {
                if (real_qipan[x_b][y_b - level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b][y_b - level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b][y_b - level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 5:
        if (x_b + level > 14 || y_b - level < 0)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b + level][y_b - level] == 2)
            {
                if (real_qipan[x_b + level][y_b - level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b + level][y_b - level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b + level][y_b - level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 6:
        if (x_b + level > 14)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b + level][y_b] == 2)
            {
                if (real_qipan[x_b + level][y_b] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b + level][y_b] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b + level][y_b] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    case 7:
        if (x_b + level > 14 || y_b + level > 14)
        {
            return -1;
        }
        else
        {
            if (qipan[x_b + level][y_b + level] == 2)
            {
                if (real_qipan[x_b + level][y_b + level] == -1)
                {
                    return -1;
                }
                return 2;
            }
            else if (qipan[x_b + level][y_b + level] == qipan[x_b][y_b])
            {
                return 1;
            }
            else if (qipan[x_b + level][y_b + level] == (1 - qipan[x_b][y_b]))
            {
                return 0;
            }
        }
        break;
    }
}
int ForbiddenCheck(int _x, int _y, int _swit)
{
    /*for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            frb_qipan[i][j] = qipan[i][j];
        }
    }*/
    int same_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    frb_qipan[_x][_y] = _swit;
    int f_x, f_y;
    //向右搜索
    for (f_y = _y + 1; f_y <= 14 && frb_qipan[_x][f_y] == _swit; f_y++, same_0[0]++)
        ;
    for (; f_y <= 14 && frb_qipan[_x][f_y] == 2; f_y++, empty_0[0]++)
        ;
    for (; f_y <= 14 && frb_qipan[_x][f_y] == _swit; f_y++, same_1[0]++)
        ;
    for (; f_y <= 14 && frb_qipan[_x][f_y] == 2; f_y++, empty_1[0]++)
        ;
    for (; f_y <= 14 && frb_qipan[_x][f_y] == _swit; f_y++, same_2[0]++)
        ;
    //向右上搜索
    for (f_x = _x - 1, f_y = _y + 1; f_x >= 0 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y++, same_0[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && frb_qipan[f_x][f_y] == 2; f_x--, f_y++, empty_0[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y++, same_1[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && frb_qipan[f_x][f_y] == 2; f_x--, f_y++, empty_1[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y++, same_2[1]++)
        ;
    //向上搜索
    for (f_x = _x - 1; f_x >= 0 && frb_qipan[f_x][_y] == _swit; f_x--, same_0[2]++)
        ;
    for (; f_x >= 0 && frb_qipan[f_x][_y] == 2; f_x--, empty_0[2]++)
        ;
    for (; f_x >= 0 && frb_qipan[f_x][_y] == _swit; f_x--, same_1[2]++)
        ;
    for (; f_x >= 0 && frb_qipan[f_x][_y] == 2; f_x--, empty_1[2]++)
        ;
    for (; f_x >= 0 && frb_qipan[f_x][_y] == _swit; f_x--, same_2[2]++)
        ;
    //向左上搜索
    for (f_x = _x - 1, f_y = _y - 1; f_x >= 0 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y--, same_0[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && frb_qipan[f_x][f_y] == 2; f_x--, f_y--, empty_0[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y--, same_1[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && frb_qipan[f_x][f_y] == 2; f_x--, f_y--, empty_1[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x--, f_y--, same_2[3]++)
        ;
    //向左搜索
    for (f_y = _y - 1; f_y >= 0 && frb_qipan[_x][f_y] == _swit; f_y--, same_0[4]++)
        ;
    for (; f_y >= 0 && frb_qipan[_x][f_y] == 2; f_y--, empty_0[4]++)
        ;
    for (; f_y >= 0 && frb_qipan[_x][f_y] == _swit; f_y--, same_1[4]++)
        ;
    for (; f_y >= 0 && frb_qipan[_x][f_y] == 2; f_y--, empty_1[4]++)
        ;
    for (; f_y >= 0 && frb_qipan[_x][f_y] == _swit; f_y--, same_2[4]++)
        ;
    //向左下搜索
    for (f_x = _x + 1, f_y = _y - 1; f_x <= 14 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y--, same_0[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && frb_qipan[f_x][f_y] == 2; f_x++, f_y--, empty_0[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y--, same_1[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && frb_qipan[f_x][f_y] == 2; f_x++, f_y--, empty_1[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y--, same_2[5]++)
        ;
    //向下搜索
    for (f_x = _x + 1; f_x <= 14 && frb_qipan[f_x][_y] == _swit; f_x++, same_0[6]++)
        ;
    for (; f_x <= 14 && frb_qipan[f_x][_y] == 2; f_x++, empty_0[6]++)
        ;
    for (; f_x <= 14 && frb_qipan[f_x][_y] == _swit; f_x++, same_1[6]++)
        ;
    for (; f_x <= 14 && frb_qipan[f_x][_y] == 2; f_x++, empty_1[6]++)
        ;
    for (; f_x <= 14 && frb_qipan[f_x][_y] == _swit; f_x++, same_2[6]++)
        ;
    //向右下搜索
    for (f_x = _x + 1, f_y = _y + 1; f_x <= 14 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y++, same_0[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && frb_qipan[f_x][f_y] == 2; f_x++, f_y++, empty_0[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y++, same_1[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && frb_qipan[f_x][f_y] == 2; f_x++, f_y++, empty_1[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && frb_qipan[f_x][f_y] == _swit; f_x++, f_y++, same_2[7]++)
        ;
    //搜索结束
    frb_qipan[_x][_y] = NONE;
    //五连判断
    for (int i = 0; i < 4; i++)
    {
        if (same_0[i] + same_0[i + 4] == 4)
        {
            if (Fqipan[_x][_y].frbing == 1)
                Fqipan[_x][_y].five++;
            return NO_FRB;
        }
    }
    //return 0;
    //棋型统计
    int threecount = 0;
    int fourcount = 0;
    //定义四连和三连的数量
    for (int i = 0; i < 4; i++)
    {
        if (same_0[i] + same_0[i + 4] >= 5) //长连禁手
        {
            return LONG_FRB;
        }
        //test

        else if (same_0[i] + same_0[i + 4] == 3) //四子相连 ？0000？
        {
            //活四冲四判断
            bool isFour = false;
            //活四和冲四的分别判断
            bool isFour_live = false;
            //开始判断
            if (empty_0[i] > 0) //?0000+?
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    isFour = true;
                    if (Boundary(_x, _y, i + 4, same_0[i + 4] + 1) == 2) //判断两边是否都可走
                    {
                        isFour_live = true;
                    }
                }
            }
            if (empty_0[i + 4] > 0) //?0000+?
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    isFour = true;
                    if (Boundary(_x, _y, i, same_0[i] + 1) == 2) //判断两边是否都可走
                    {
                        isFour_live = true;
                    }
                }
            }
            if (isFour)
            {
                fourcount++;
                if (Fqipan[_x][_y].frbing == 1)
                {
                    if (isFour_live)
                    {
                        Fqipan[_x][_y].four[0]++;
                    }
                    else
                    {
                        Fqipan[_x][_y].four[1]++;
                    }
                }
            }
        }

        //test

        else if (same_0[i] + same_0[i + 4] == 2) //三子相连 ？000？
        {
            //下述非禁手部分
            //判断冲3的情况
            if (Fqipan[_x][_y].frbing == 1)
            {
                int a;
                if ((a = Boundary(_x, _y, i, same_0[i] + 1)) == -1 || a == 1 - _swit)
                {
                    if (Boundary(_x, _y, i + 4, same_0[i + 4] + 1) == 2 && Boundary(_x, _y, i + 4, same_0[i + 4] + 2) == 2)
                    {
                        Fqipan[_x][_y].three[1]++;
                    }
                }
                if ((a = Boundary(_x, _y, i + 4, same_0[i + 4] + 1)) == -1 || a == 1 - _swit)
                {
                    if (Boundary(_x, _y, i, same_0[i] + 1) == 2 && Boundary(_x, _y, i, same_0[i] + 2) == 2)
                    {
                        Fqipan[_x][_y].three[1]++;
                    }
                }
            }

            //活四冲四检查
            if (empty_0[i] == 1 && same_1[i] == 1) //?0+000?
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    fourcount++;
                    //判断冲四还是活四
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        /*int isFour = 0;
                        if (Boundary(_x, _y, i, same_0[i] + empty_0[i] + same_1[i] + 1) == 2)
                        {
                            isFour++;
                        }
                        if (Boundary(_x, _y, i + 4, same_0[i + 4] + 1) == 2)
                        {
                            isFour++;
                        }
                        //有错误
                        if (isFour == 2)
                        {
                            Fqipan[_x][_y].four[0]++;
                        }
                        else
                        {*/
                        Fqipan[_x][_y].four[1]++;
                        //}
                        //判断结束*/
                    }
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1) //?0+000?
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    fourcount++;
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        //判断冲四还是活四
                        int isFour = 0;
                        if (Boundary(_x, _y, i + 4, same_0[i + 4] + empty_0[i + 4] + same_1[i + 4] + 1) == 2)
                        {
                            isFour++;
                        }
                        if (Boundary(_x, _y, i, same_0[i] + 1) == 2)
                        {
                            isFour++;
                        }
                        if (isFour == 2)
                        {
                            Fqipan[_x][_y].four[0]++;
                        }
                        else
                        {
                            Fqipan[_x][_y].four[1]++;
                        }
                        //判断结束
                    }
                }
            }
            //活三检查
            int isThree = 0;
            //?++000+?
            if ((empty_0[i] > 2 || (empty_0[i] == 2 && same_1[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                    isThree = 1;
            }
            //?+000++?
            if ((empty_0[i + 4] > 2 || (empty_0[i + 4] == 2 && same_1[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                    isThree = 1;
            }
            if (isThree)
            {
                threecount++;
                //判断活三
                if (Fqipan[_x][_y].frbing == 1)
                {
                    //printf("test");
                    Fqipan[_x][_y].three[0]++;
                }
            }
        }
        else if (same_0[i] + same_0[i + 4] == 1) //两子相连 ？00？
        {
            //活四冲四判断
            if (empty_0[i] == 1 && same_1[i] == 2) //?00+00?
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    fourcount++;
                    if (Fqipan[_x][_y].frbing == 1)
                        Fqipan[_x][_y].four[1]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2) //?00+00?
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    fourcount++;
                    if (Fqipan[_x][_y].frbing == 1)
                        Fqipan[_x][_y].four[1]++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 1 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    threecount++;
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        if (Boundary(_x, _y, i + 4, same_0[i + 4] + 1) == 2)
                        {
                            Fqipan[_x][_y].three[0]++;
                        }
                        else
                        {
                            Fqipan[_x][_y].three[1]++;
                        }
                    }
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    threecount++;
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        if (Boundary(_x, _y, i, same_0[i] + 1) == 2)
                        {
                            Fqipan[_x][_y].three[0]++;
                        }
                        else
                        {
                            Fqipan[_x][_y].three[1]++;
                        }
                    }
                }
            }
        }
        else if (same_0[i] + same_0[i + 4] == 0) //单独一子
        {
            if (empty_0[i] == 1 && same_1[i] == 3)
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    fourcount++;
                    if (Fqipan[_x][_y].frbing == 1)
                        Fqipan[_x][_y].four[1]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 3)
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    fourcount++;
                    if (Fqipan[_x][_y].frbing == 1)
                        Fqipan[_x][_y].four[1]++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 2 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i, same_0[i]) == NO_FRB)
                {
                    threecount++;
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        if (Boundary(_x, _y, i + 4, same_0[i + 4] + 1) == 2)
                        {
                            Fqipan[_x][_y].three[0]++;
                        }
                        else
                        {
                            Fqipan[_x][_y].three[1]++;
                        }
                    }
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(_x, _y, i + 4, same_0[i + 4]) == NO_FRB)
                {
                    threecount++;
                    if (Fqipan[_x][_y].frbing == 1)
                    {
                        if (Boundary(_x, _y, i, same_0[i] + 1) == 2)
                        {
                            Fqipan[_x][_y].three[0]++;
                        }
                        else
                        {
                            Fqipan[_x][_y].three[1]++;
                        }
                    }
                }
            }
        }
    }
    //禁手分析结束
    if (fourcount > 1)
    {
        return FOUR_FRB;
    }
    if (threecount > 1)
    {
        return THREE_FRB;
    }
    return NO_FRB;
}
int PointForbiddenCheck(int _x, int _y, int direction, int same)
{
    int i, j;
    same++;
    if (direction >= 4)
        same = -same;
    //关键点的坐标
    switch (direction % 4)
    {
    case 0:
        i = _x;
        j = _y + same;
        break;
    case 1:
        i = _x - same;
        j = _y + same;
        break;
    case 2:
        i = _x - same;
        j = _y;
        break;
    default:
        i = _x - same;
        j = _y - same;
        break;
    }
    if (i < 0 || i > 14 || j < 0 || j > 14)
    {
        printf("error");
    }
    //放入棋子
    frb_qipan[_x][_y] = 1;
    int result = ForbiddenCheck(i, j, 1);
    frb_qipan[_x][_y] = NONE;
    return result;
}
int Judgement(int _x, int _y)
{
    int x_judge = _x;
    int y_judge = _y;
    int judgeNumber = 1;
    while (Boundary(x_judge, y_judge, 0, 1) == 1)
    {
        y_judge++;
        judgeNumber++;
    }
    x_judge = _x;
    y_judge = _y;
    while (Boundary(x_judge, y_judge, 4, 1) == 1)
    {
        y_judge--;
        judgeNumber++;
    }
    if (judgeNumber >= 5)
        return 1;
    x_judge = _x;
    y_judge = _y;
    judgeNumber = 1;
    while (Boundary(x_judge, y_judge, 1, 1) == 1)
    {
        x_judge--;
        y_judge++;
        judgeNumber++;
    }
    x_judge = _x;
    y_judge = _y;
    while (Boundary(x_judge, y_judge, 5, 1) == 1)
    {
        x_judge++;
        y_judge--;
        judgeNumber++;
    }
    if (judgeNumber >= 5)
        return 1;
    x_judge = _x;
    y_judge = _y;
    judgeNumber = 1;
    while (Boundary(x_judge, y_judge, 2, 1) == 1)
    {
        x_judge--;
        judgeNumber++;
    }
    x_judge = _x;
    y_judge = _y;
    while (Boundary(x_judge, y_judge, 6, 1) == 1)
    {
        x_judge++;
        judgeNumber++;
    }
    if (judgeNumber >= 5)
        return 1;
    x_judge = _x;
    y_judge = _y;
    judgeNumber = 1;
    while (Boundary(x_judge, y_judge, 3, 1) == 1)
    {
        x_judge--;
        y_judge--;
        judgeNumber++;
    }
    x_judge = _x;
    y_judge = _y;
    while (Boundary(x_judge, y_judge, 7, 1) == 1)
    {
        x_judge++;
        y_judge++;
        judgeNumber++;
    }
    if (judgeNumber >= 5)
        return 1;
    return 0;
}
void ClearMyArray()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            Fqipan[i][j].five = 0;
            //Fqipan[i][j].score = 0;

            for (int k = 0; k < 11; k++)
            {
                Fqipan[i][j].four[k] = 0;
                Fqipan[i][j].three[k] = 0;
                Fqipan[i][j].two[k] = 0;
            }
        }
    }
}
int Score(int _x, int _y, int _swit)
{
    int score = 0;
    //对成五评分
    if (Fqipan[_x][_y].five > 0)
    {
        score += Fqipan[_x][_y].five * 50000;
        return score;
    }
    score += (Fqipan[_x][_y].four[0] * 4320 + Fqipan[_x][_y].four[1] * 720 + Fqipan[_x][_y].four[3] * 720 + Fqipan[_x][_y].four[4] * 720 + Fqipan[_x][_y].four[5] * 720 + Fqipan[_x][_y].four[6] * 720 + Fqipan[_x][_y].four[7] * 720 + Fqipan[_x][_y].four[8] * 720 + Fqipan[_x][_y].four[9] * 720 + Fqipan[_x][_y].three[0] * 720 + Fqipan[_x][_y].three[1] * 480 + Fqipan[_x][_y].three[3] * 480 + Fqipan[_x][_y].three[4] * 720 + Fqipan[_x][_y].three[5] * 480 + Fqipan[_x][_y].three[6] * 480 + Fqipan[_x][_y].three[8] * 480 + Fqipan[_x][_y].three[9] * 480 + Fqipan[_x][_y].three[10] * 480 + Fqipan[_x][_y].two[0] * 20 + Fqipan[_x][_y].two[1] * 10);
    //对四评分
    /* score += Fqipan[_x][_y].four[0] * 4320;
    score += Fqipan[_x][_y].four[1] * 720;
    score += Fqipan[_x][_y].four[2] * 0;
    score += Fqipan[_x][_y].four[3] * 720;
    score += Fqipan[_x][_y].four[4] * 720;
    score += Fqipan[_x][_y].four[5] * 720;
    score += Fqipan[_x][_y].four[6] * 720;
    score += Fqipan[_x][_y].four[7] * 720;
    score += Fqipan[_x][_y].four[8] * 720;
    score += Fqipan[_x][_y].four[9] * 720;
    //对三评分
    score += Fqipan[_x][_y].three[0] * 720;
    score += Fqipan[_x][_y].three[1] * 720;
    score += Fqipan[_x][_y].three[2] * 0;
    score += Fqipan[_x][_y].three[3] * 720;
    score += Fqipan[_x][_y].three[4] * 720;
    score += Fqipan[_x][_y].three[5] * 100;
    score += Fqipan[_x][_y].three[6] * 200;
    score += Fqipan[_x][_y].three[7] * 0;
    score += Fqipan[_x][_y].three[8] * 210;
    score += Fqipan[_x][_y].three[9] * 210;
    score += Fqipan[_x][_y].three[10] * 210;
    //对二评分
    score += Fqipan[_x][_y].two[0] * 20;
    score += Fqipan[_x][_y].two[1] * 10;*/
    //三四不禁手赋分
    /*if (Fqipan[_x][_y].four[0] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[1] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[3] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[4] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[5] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[6] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[7] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[8] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    else if (Fqipan[_x][_y].four[9] != 0 && Fqipan[_x][_y].three[0] != 0)
    {
        score += 4300;
    }
    if (_swit == 0)
    {
        if (Fqipan[_x][_y].three[0] > 1)
        {
            score += 4300;
        }
        if (Fqipan[_x][_y].four[0] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[1] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[3] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[4] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[5] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[6] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[7] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[8] > 1)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[9] > 1)
        {
            score += 4300;
        }
    }
*/
    /*if (_x == 7 && _y == 7)
    {
        score += 5;
    }
    else if (_x >= 6 && _x <= 8 && _y >= 6 && _y <= 8)
    {
        score += 4;
    }
    else if (_x >= 5 && _x <= 9 && _y >= 5 && _y <= 9)
    {
        score += 3;
    }
    else if (_x >= 4 && _x <= 10 && _y >= 4 && _y <= 10)
    {
        score += 2;
    }
    else if (_x >= 3 && _x <= 11 && _y >= 3 && _y <= 11)
    {
        score += 1;
    }*/
    return score;
}
int RealScore(int _x, int _y, int _swit, int object)
{
    int score = 0;
    if (object == 1) //目标是对方的时候
    {
        //不可能出现成五的情况
        //现在考虑必输的情况，包含所有冲四和活三
        if (Fqipan[_x][_y].three[0] > 0 ||Fqipan[_x][_y].three[4]>0|| Fqipan[_x][_y].four[0] > 0 || Fqipan[_x][_y].four[1] > 0 || Fqipan[_x][_y].four[3] > 0 || Fqipan[_x][_y].four[4] > 0 || Fqipan[_x][_y].four[5] > 0 || Fqipan[_x][_y].four[6] > 0 || Fqipan[_x][_y].four[7] > 0 || Fqipan[_x][_y].four[8] > 0 || Fqipan[_x][_y].four[9] > 0)
        {
            score += 50000;
        }
        //现在考虑其余情况
        score += Fqipan[_x][_y].three[1] * 720;
       // score += Fqipan[_x][_y].three[2] * 0;
        score += Fqipan[_x][_y].three[3] * 720;
        score += Fqipan[_x][_y].three[5] * 480;
        score += Fqipan[_x][_y].three[6] * 480;
       // score += Fqipan[_x][_y].three[7] * 0;
        score += Fqipan[_x][_y].three[8] * 480;
        score += Fqipan[_x][_y].three[9] * 480;
        score += Fqipan[_x][_y].three[10] * 480;
        //对二评分
        score += Fqipan[_x][_y].two[0] * 20;
        score += Fqipan[_x][_y].two[1] * 10;
      /*  if (Fqipan[_x][_y].four[0] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[1] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[3] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[4] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[5] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[6] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[7] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[8] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[9] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        if (_swit == 0)
        {
            if (Fqipan[_x][_y].three[0] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[0] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[1] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[3] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[4] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[5] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[6] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[7] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[8] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[9] > 1)
            {
                score += 4300;
            }
        }
*/
       /* if (_x == 7 && _y == 7)
        {
            score += 5;
        }
        else if (_x >= 6 && _x <= 8 && _y >= 6 && _y <= 8)
        {
            score += 4;
        }
        else if (_x >= 5 && _x <= 9 && _y >= 5 && _y <= 9)
        {
            score += 3;
        }
        else if (_x >= 4 && _x <= 10 && _y >= 4 && _y <= 10)
        {
            score += 2;
        }
        else if (_x >= 3 && _x <= 11 && _y >= 3 && _y <= 11)
        {
            score += 1;
        }*/
        return score;
    }
    else
    {
        if (Fqipan[_x][_y].five > 0) //五必胜，赋分为最大值
        {
            score += (-INF);
            return score;
        }
        score += Fqipan[_x][_y].four[0] * 4320;
        score += Fqipan[_x][_y].four[1] * 720;
       // score += Fqipan[_x][_y].four[2] * 0;
        score += Fqipan[_x][_y].four[3] * 720;
        score += Fqipan[_x][_y].four[4] * 720;
        score += Fqipan[_x][_y].four[5] * 720;
        score += Fqipan[_x][_y].four[6] * 720;
        score += Fqipan[_x][_y].four[7] * 720;
        score += Fqipan[_x][_y].four[8] * 720;
        score += Fqipan[_x][_y].four[9] * 720;
        //对三评分
        score += Fqipan[_x][_y].three[0] * 720;
        score += Fqipan[_x][_y].three[1] * 480;
        //score += Fqipan[_x][_y].three[2] * 0;
        score += Fqipan[_x][_y].three[3] * 480;
        score += Fqipan[_x][_y].three[4] * 720;
        score += Fqipan[_x][_y].three[5] * 480;
        score += Fqipan[_x][_y].three[6] * 480;
        //score += Fqipan[_x][_y].three[7] * 0;
        score += Fqipan[_x][_y].three[8] * 480;
        score += Fqipan[_x][_y].three[9] * 480;
        score += Fqipan[_x][_y].three[10] * 480;
        //对二评分
        score += Fqipan[_x][_y].two[0] * 20;
        score += Fqipan[_x][_y].two[1] * 10;
        //三四不禁手赋分
        /*if (Fqipan[_x][_y].four[0] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[1] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[3] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[4] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[5] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[6] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[7] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[8] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        else if (Fqipan[_x][_y].four[9] != 0 && Fqipan[_x][_y].three[0] != 0)
        {
            score += 4300;
        }
        if (_swit == 0)
        {
            if (Fqipan[_x][_y].three[0] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[0] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[1] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[3] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[4] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[5] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[6] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[7] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[8] > 1)
            {
                score += 4300;
            }
            if (Fqipan[_x][_y].four[9] > 1)
            {
                score += 4300;
            }
        }

        if (_x == 7 && _y == 7)
        {
            score += 5;
        }
        else if (_x >= 6 && _x <= 8 && _y >= 6 && _y <= 8)
        {
            score += 4;
        }
        else if (_x >= 5 && _x <= 9 && _y >= 5 && _y <= 9)
        {
            score += 3;
        }
        else if (_x >= 4 && _x <= 10 && _y >= 4 && _y <= 10)
        {
            score += 2;
        }
        else if (_x >= 3 && _x <= 11 && _y >= 3 && _y <= 11)
        {
            score += 1;
        }*/
        return score;
    }
}
int ScoreAllCheck(int _x, int _y, int _swit)
{
    int score1 = 0;
    int score0 = 0;
    int isWin = 0;
    qipan[_x][_y] = _swit;
    frb_qipan[_x][_y] = _swit;
    ClearMyArray();
    //这里我们利用_swit代替全局变量，使得函数更加独立。
    struct forbidden scoreMeasure;
    scoreMeasure.five = 0;
    for (int i = 0; i < 11; i++)
    {
        scoreMeasure.four[i] = 0;
        scoreMeasure.three[i] = 0;
        scoreMeasure.two[i] = 0;
    }
    //首先要判断对手得分
    for (int i = 0; i < 15; i++)
    { //真实棋盘变换
        for (int j = 0; j < 15; j++)
        {
            if (qipan[i][j] == 2 && (1 - _swit) == 1 && ForbiddenCheck(i, j, 1) != NO_FRB)
                real_qipan[i][j] = -1;
            else
            {
                real_qipan[i][j] = qipan[i][j];
            }
        }
    }
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (qipan[i][j] == 1 - _swit)
            {
                ScoreCheck(i, j, 1 - _swit);
                scoreMeasure.five += Fqipan[i][j].five;
                for (int k = 0; k < 11; k++)
                {
                    scoreMeasure.four[k] += Fqipan[i][j].four[k];
                    scoreMeasure.three[k] += Fqipan[i][j].three[k];
                    scoreMeasure.two[k] += Fqipan[i][j].two[k];
                }
            }
        }
    }
    //计算完对手的后，我们进行赋分计算
    Fqipan[_x][_y].five = scoreMeasure.five;
    for (int k = 0; k < 11; k++)
    {
        Fqipan[_x][_y].four[k] = scoreMeasure.four[k];
        Fqipan[_x][_y].three[k] = scoreMeasure.three[k] / 3;
        Fqipan[_x][_y].two[k] = scoreMeasure.two[k] / 2;
    }
    score1 = RealScore(_x, _y, 1 - swit, 1);
    if (score1 >= 50000)
    {
        isWin = 1;
    }
    score1 = -score1;
    //对自己进行评分
    ClearMyArray(); //清楚页面
    scoreMeasure.five = 0;
    for (int i = 0; i < 11; i++)
    {
        scoreMeasure.four[i] = 0;
        scoreMeasure.three[i] = 0;
        scoreMeasure.two[i] = 0;
    }
    for (int i = 0; i < 15; i++)
    { //真实棋盘变换
        for (int j = 0; j < 15; j++)
        {
            if (qipan[i][j] == 2 && _swit == 1 && ForbiddenCheck(i, j, 1) != NO_FRB)
                real_qipan[i][j] = -1;
            else
            {
                real_qipan[i][j] = qipan[i][j];
            }
        }
    }
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (qipan[i][j] == _swit)
            {
                ScoreCheck(i, j, _swit);
                scoreMeasure.five += Fqipan[i][j].five;
                for (int k = 0; k < 11; k++)
                {
                    scoreMeasure.four[k] += Fqipan[i][j].four[k];
                    scoreMeasure.three[k] += Fqipan[i][j].three[k];
                    scoreMeasure.two[k] += Fqipan[i][j].two[k];
                }
            }
        }
    }
    qipan[_x][_y] = 2;
    frb_qipan[_x][_y] = 2;
    for (int k = 0; k < 11; k++)
    {
        scoreMeasure.four[k] /= 4;
        scoreMeasure.three[k] /= 3;
        scoreMeasure.two[k] /= 2;
    }
    ClearMyArray();
    Fqipan[_x][_y].five = scoreMeasure.five;
    for (int k = 0; k < 11; k++)
    {
        Fqipan[_x][_y].four[k] = scoreMeasure.four[k];
        Fqipan[_x][_y].three[k] = scoreMeasure.three[k];
        Fqipan[_x][_y].two[k] = scoreMeasure.two[k];
    }
    score0 = RealScore(_x, _y, _swit, 0);
    if (score0 >= -INF)
    {
        return -INF;
    }
    if (isWin > 0)
    {
        return score1;
    }
    else
    {
        return (score0 + score1);
    }
}
void ScoreCheck(int _x, int _y, int switScore)
{
    start_time = clock();
    current_time = (float)start_time / CLOCKS_PER_SEC;
    //printf("start time:%f\n",current_time);
    int same_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int f_x, f_y;
    //向右搜索
    for (f_y = _y + 1; f_y <= 14 && real_qipan[_x][f_y] == switScore; f_y++, same_0[0]++)
        ;
    for (; f_y <= 14 && real_qipan[_x][f_y] == 2; f_y++, empty_0[0]++)
        ;
    for (; f_y <= 14 && real_qipan[_x][f_y] == switScore; f_y++, same_1[0]++)
        ;
    for (; f_y <= 14 && real_qipan[_x][f_y] == 2; f_y++, empty_1[0]++)
        ;
    for (; f_y <= 14 && real_qipan[_x][f_y] == switScore; f_y++, same_2[0]++)
        ;
    //向右上搜索
    for (f_x = _x - 1, f_y = _y + 1; f_x >= 0 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x--, f_y++, same_0[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && real_qipan[f_x][f_y] == 2; f_x--, f_y++, empty_0[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x--, f_y++, same_1[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && real_qipan[f_x][f_y] == 2; f_x--, f_y++, empty_1[1]++)
        ;
    for (; f_x >= 0 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x--, f_y++, same_2[1]++)
        ;
    //向上搜索
    for (f_x = _x - 1; f_x >= 0 && real_qipan[f_x][_y] == switScore; f_x--, same_0[2]++)
        ;
    for (; f_x >= 0 && real_qipan[f_x][_y] == 2; f_x--, empty_0[2]++)
        ;
    for (; f_x >= 0 && real_qipan[f_x][_y] == switScore; f_x--, same_1[2]++)
        ;
    for (; f_x >= 0 && real_qipan[f_x][_y] == 2; f_x--, empty_1[2]++)
        ;
    for (; f_x >= 0 && real_qipan[f_x][_y] == switScore; f_x--, same_2[2]++)
        ;
    //向左上搜索
    for (f_x = _x - 1, f_y = _y - 1; f_x >= 0 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x--, f_y--, same_0[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && real_qipan[f_x][f_y] == 2; f_x--, f_y--, empty_0[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x--, f_y--, same_1[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && real_qipan[f_x][f_y] == 2; f_x--, f_y--, empty_1[3]++)
        ;
    for (; f_x >= 0 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x--, f_y--, same_2[3]++)
        ;
    //向左搜索
    for (f_y = _y - 1; f_y >= 0 && real_qipan[_x][f_y] == switScore; f_y--, same_0[4]++)
        ;
    for (; f_y >= 0 && real_qipan[_x][f_y] == 2; f_y--, empty_0[4]++)
        ;
    for (; f_y >= 0 && real_qipan[_x][f_y] == switScore; f_y--, same_1[4]++)
        ;
    for (; f_y >= 0 && real_qipan[_x][f_y] == 2; f_y--, empty_1[4]++)
        ;
    for (; f_y >= 0 && real_qipan[_x][f_y] == switScore; f_y--, same_2[4]++)
        ;
    //向左下搜索
    for (f_x = _x + 1, f_y = _y - 1; f_x <= 14 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x++, f_y--, same_0[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && real_qipan[f_x][f_y] == 2; f_x++, f_y--, empty_0[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x++, f_y--, same_1[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && real_qipan[f_x][f_y] == 2; f_x++, f_y--, empty_1[5]++)
        ;
    for (; f_x <= 14 && f_y >= 0 && real_qipan[f_x][f_y] == switScore; f_x++, f_y--, same_2[5]++)
        ;
    //向下搜索
    for (f_x = _x + 1; f_x <= 14 && real_qipan[f_x][_y] == switScore; f_x++, same_0[6]++)
        ;
    for (; f_x <= 14 && real_qipan[f_x][_y] == 2; f_x++, empty_0[6]++)
        ;
    for (; f_x <= 14 && real_qipan[f_x][_y] == switScore; f_x++, same_1[6]++)
        ;
    for (; f_x <= 14 && real_qipan[f_x][_y] == 2; f_x++, empty_1[6]++)
        ;
    for (; f_x <= 14 && real_qipan[f_x][_y] == switScore; f_x++, same_2[6]++)
        ;
    //向右下搜索
    for (f_x = _x + 1, f_y = _y + 1; f_x <= 14 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x++, f_y++, same_0[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && real_qipan[f_x][f_y] == 2; f_x++, f_y++, empty_0[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x++, f_y++, same_1[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && real_qipan[f_x][f_y] == 2; f_x++, f_y++, empty_1[7]++)
        ;
    for (; f_x <= 14 && f_y <= 14 && real_qipan[f_x][f_y] == switScore; f_x++, f_y++, same_2[7]++)
        ;
    //搜索结束
    for (int i = 0; i < 4; i++)
    {
        //五子最优先
        if (same_0[i] + same_0[i + 4] == 4)
        {
            Fqipan[_x][_y].five++; //成五
            return;
        }
        else if (switScore == 0 && same_0[i] + same_0[i + 4] > 4)
        {
            Fqipan[_x][_y].five++; //成五
            return;
        }
        //四子相连 判断活四，冲四，死四
        else if (same_0[i] + same_0[i + 4] == 3)
        {
            if (empty_0[i] >= 1 && empty_0[i + 4] >= 1)
            {
                Fqipan[_x][_y].four[0]++; //活四
            }
            else if ((empty_0[i] >= 1 && empty_0[i + 4] == 0) || (empty_0[i] == 0 && empty_0[i + 4] >= 1))
            {
                Fqipan[_x][_y].four[1]++; //冲四
            }
            else if (empty_0[i] == 0 && empty_0[i + 4] == 0)
            {
                Fqipan[_x][_y].four[2]++; //死四
            }
        }
        //三子相连 判断跳四 活三 冲三 死三
        else if (same_0[i] + same_0[i + 4] == 2)
        {
            //判断跳四
            if (empty_0[i] == 1 && same_1[i] == 1) //?0+000?
            {
                if (empty_1[i] >= 1 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[3]++; //跳四
                }
                if (empty_1[i] >= 1 && empty_0[i + 4] == 0)
                {
                    Fqipan[_x][_y].four[4]++; //乙跳四
                }
                if (empty_1[i] == 0 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[5]++; //异乙跳四
                }
                if (empty_1[i] == 0 && empty_0[i + 4] == 0)
                {
                    Fqipan[_x][_y].four[6]++; //丙跳四
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1) //?0+000?
            {
                if (empty_1[i + 4] >= 1 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[3]++; //跳四
                }
                if (empty_1[i + 4] >= 1 && empty_0[i] == 0)
                {
                    Fqipan[_x][_y].four[4]++; //乙跳四
                }
                if (empty_1[i + 4] == 0 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[5]++; //异乙跳四
                }
                if (empty_1[i + 4] == 0 && empty_0[i] == 0)
                {
                    Fqipan[_x][_y].four[6]++; //丙跳四
                }
            }
            //判断活三
            if ((empty_0[i] >= 2 && empty_0[i + 4] >= 1) || (empty_0[i + 4] >= 2 && empty_0[i] >= 1))
            {
                Fqipan[_x][_y].three[0]++;
            }
            //判断冲三
            else if ((empty_0[i] >= 2 && empty_0[i + 4] == 0) || (empty_0[i] == 0 && empty_0[i + 4] >= 2))
            {
                Fqipan[_x][_y].three[1]++;
            }
            //判断死三
            else if ((empty_0[i] == 0 && empty_0[i + 4] == 0) || (empty_0[i] == 1 && same_1[i] == 0 && empty_0[i + 4] == 0) || (empty_0[i + 4] == 1 && same_1[i + 4] == 0 && empty_0[i] == 0))
            {
                Fqipan[_x][_y].three[2]++;
            }
            //判断假死三
            else if (empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 0 && same_1[i + 4] == 0)
            {
                Fqipan[_x][_y].three[3]++;
            }
        }
        //判断隔四 跳三 活二 冲二 死二
        else if (same_0[i] + same_0[i + 4] == 1)
        {
            //判断隔四
            if (empty_0[i] == 1 && same_1[i] == 2) //?00+00?
            {
                if (empty_1[i] >= 1 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[7]++;
                } //判断丙隔四
                else if (empty_1[i] == 0 && empty_0[i + 4] == 0)
                {
                    Fqipan[_x][_y].four[9]++;
                } //判断乙隔四
                else
                {
                    Fqipan[_x][_y].four[8]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2) //?00+00?
            {
                if (empty_1[i + 4] >= 1 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[7]++;
                } //判断丙隔四
                else if (empty_1[i + 4] == 0 && empty_0[i] == 0)
                {
                    Fqipan[_x][_y].four[9]++;
                } //判断乙隔四
                else
                {
                    Fqipan[_x][_y].four[8]++;
                }
            }
            //判断跳三
            if (empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] >= 1 && empty_0[i + 4] >= 1)
            {
                Fqipan[_x][_y].three[4]++;
            }
            //判断乙跳三
            else if (empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] >= 1 && empty_0[i + 4] == 0)
            {
                Fqipan[_x][_y].three[5]++;
            }
            //判断异乙跳三
            else if (empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 0 && empty_0[i + 4] >= 1)
            {
                Fqipan[_x][_y].three[6]++;
            }
            //判断丙跳三
            else if (empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 0 && empty_0[i + 4] == 0)
            {
                Fqipan[_x][_y].three[7]++;
            }
            //对称方向

            //判断跳三
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] >= 1 && empty_0[i] >= 1)
            {
                Fqipan[_x][_y].three[4]++;
            }
            //判断乙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] >= 1 && empty_0[i] == 0)
            {
                Fqipan[_x][_y].three[5]++;
            }
            //判断异乙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 0 && empty_0[i] >= 1)
            {
                Fqipan[_x][_y].three[6]++;
            }
            //判断丙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 0 && empty_0[i] == 0)
            {
                Fqipan[_x][_y].three[7]++;
            }

            //判断活二
            if ((empty_0[i] >= 2 && empty_0[i + 4] >= 2) || (empty_0[i] == 1 && same_1[i] == 0 && empty_0[i + 4] >= 2) || (empty_0[i + 4] == 1 && same_1[i + 4] == 0 && empty_0[i] >= 2))
            {
                Fqipan[_x][_y].two[0]++;
            }
            //判断冲二
            else if ((empty_0[i] == 0 && empty_0[i + 4] >= 2) || (empty_0[i + 4] == 0 && empty_0[i] >= 2))
            {
                Fqipan[_x][_y].two[1]++;
            }
            //判断死二
            /*else if ((empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 0 && same_1[i + 4] == 0) || (empty_0[i] == 0 && empty_0[i + 4] == 0) || (empty_0[i] == 0 && empty_0[i + 4] == 2 && same_1[i + 4] == 0) || (empty_0[i + 4] == 0 && empty_0[i] == 2 && same_1[i] == 0))
            {
            }*/
        }
        //判断跳四 跳三 隔三
        else if (same_0[i] + same_0[i + 4] == 0) //单独一子
        {
            if (empty_0[i] == 1 && same_1[i] == 3)
            {
                if (empty_1[i] >= 1 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[3]++;
                }
                //乙跳四
                else if (empty_1[i] == 0 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[4]++;
                } //异乙跳四
                else if (empty_1[i + 4] == 0 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[5]++;
                }
                //丙跳四
                else if (empty_1[i + 4] == 0 && empty_0[i] == 0)
                {
                    Fqipan[_x][_y].four[6]++;
                }
            }
            //对称
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 3)
            {
                if (empty_1[i + 4] >= 1 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[3]++;
                }
                //乙跳四
                else if (empty_1[i + 4] == 0 && empty_0[i] >= 1)
                {
                    Fqipan[_x][_y].four[4]++;
                } //异乙跳四
                else if (empty_1[i] == 0 && empty_0[i + 4] >= 1)
                {
                    Fqipan[_x][_y].four[5]++;
                }
                //丙跳四
                else if (empty_1[i] == 0 && empty_0[i + 4] == 0)
                {
                    Fqipan[_x][_y].four[6]++;
                }
            }
            //判断跳三
            if (empty_0[i] == 1 && same_1[i] == 2 && empty_1[i] >= 1 && empty_0[i + 4] >= 1)
            {
                Fqipan[_x][_y].three[4]++;
            } //判断乙跳三
            else if (empty_0[i] == 1 && same_1[i] == 2 && empty_1[i] == 0 && empty_0[i + 4] >= 1)
            {
                Fqipan[_x][_y].three[5]++;
            } //判断异乙跳三
            else if (empty_0[i] == 1 && same_1[i] == 2 && empty_1[i] >= 1 && empty_0[i + 4] == 0)
            {
                Fqipan[_x][_y].three[6]++;
            } //判断丙跳三
            else if (empty_0[i] == 1 && same_1[i] == 2 && empty_1[i] == 0 && empty_0[i + 4] == 0)
            {
                Fqipan[_x][_y].three[7]++;
            }
            //对称处理
            //判断跳三
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && empty_1[i + 4] >= 1 && empty_0[i] >= 1)
            {
                Fqipan[_x][_y].three[4]++;
            } //判断乙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && empty_1[i + 4] == 0 && empty_0[i] >= 1)
            {
                Fqipan[_x][_y].three[5]++;
            } //判断异乙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && empty_1[i + 4] >= 1 && empty_0[i] == 0)
            {
                Fqipan[_x][_y].three[6]++;
            } //判断丙跳三
            else if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && empty_1[i + 4] == 0 && empty_0[i] == 0)
            {
                Fqipan[_x][_y].three[7]++;
            }
            //判断隔三
            if (empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 1 && same_1[i + 4] == 1 && empty_1[i] >= 1 && empty_1[i + 4] >= 1)
            {
                Fqipan[_x][_y].three[8]++;
            } //判断乙隔三
            else if ((empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 1 && same_1[i + 4] == 1 && empty_1[i] == 0 && empty_1[i + 4] >= 1) || (empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 1 && same_1[i + 4] == 1 && empty_1[i] >= 1 && empty_1[i + 4] == 0))
            {
                Fqipan[_x][_y].three[9]++;
            } //判断丙隔三
            else if (empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 1 && same_1[i + 4] == 1 && empty_1[i] == 0 && empty_1[i + 4] == 0)
            {
                Fqipan[_x][_y].three[10]++;
            }
            //中心处理
            if (empty_0[i] >= 1 && empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 1 && same_2[i + 4] == 1 && Boundary(_x, _y, i + 4, 5) == 2)
            {
                Fqipan[_x][_y].three[8]++;
            } //判断乙隔三
            else if ((empty_0[i] >= 1 && empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 1 && same_2[i + 4] == 1 && Boundary(_x, _y, i + 4, 5) != 2) || (empty_0[i] == 0 && empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 1 && same_2[i + 4] == 1 && Boundary(_x, _y, i + 4, 5) == 2))
            {
                Fqipan[_x][_y].three[9]++;
            } //判断丙隔三
            else if (empty_0[i] == 0 && empty_0[i + 4] == 1 && same_1[i + 4] == 1 && empty_1[i + 4] == 1 && same_2[i + 4] == 1 && Boundary(_x, _y, i + 4, 5) != 2)
            {
                Fqipan[_x][_y].three[10]++;
            }
            //对称处理
            if (empty_0[i + 4] >= 1 && empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 1 && same_2[i] == 1 && Boundary(_x, _y, i, 5) == 2)
            {
                Fqipan[_x][_y].three[8]++;
            } //判断乙隔三
            else if ((empty_0[i + 4] >= 1 && empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 1 && same_2[i + 4] == 1 && Boundary(_x, _y, i, 5) != 2) || (empty_0[i + 4] == 0 && empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 1 && same_2[i] == 1 && Boundary(_x, _y, i, 5) == 2))
            {
                Fqipan[_x][_y].three[9]++;
            } //判断丙隔三
            else if (empty_0[i + 4] == 0 && empty_0[i] == 1 && same_1[i] == 1 && empty_1[i] == 1 && same_2[i] == 1 && Boundary(_x, _y, i, 5) != 2)
            {
                Fqipan[_x][_y].three[10]++;
            }
        }
    }
    finish_time = clock();
    current_time = (float)finish_time / CLOCKS_PER_SEC;
    // printf("finish time:%f\n",current_time);
    return;
}