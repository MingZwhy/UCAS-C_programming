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

#define WIDTH 15
#define DEPTH 9
#define INF -10000000

//全局变量定义
int chess[15][15];

int x = 0;
int y = 0;
int step = 0;
//AI函数内容
void ScoreCheck(int x_frb, int y_frb, int color_frb);
int ForbiddenCheck(int x_frb, int y_frb, int color_frb);
int PointForbiddenCheck(int x_pointfrb, int y_pointfrb, int direction, int same, int color_pointfrb);
int Score(int x_score, int y_score, int color_score);
void ClearMyArray();
int ScoreAllCheck(int x_sac, int y_sac, int color_sac);
int RealScore(int x_rs, int y_rs, int color_rs, int object);
int RandomChess(int level, int color_rdm, int a_rdm, int b_rdm);
//基础函数内容
void Printd();
void LastChess(int i, int j);
int Judged(int x_judge, int y_judge, int switJudge);
void AiVsPlayer_Black();
void AiVsPlayer_White();
int Handling(char s[]);
void End();
void Initialized();

//DLC内容：新增资源库
#pragma region //DLC资源库内容
char source[MAXLEN][2 * MAXLEN];
struct LocationNode
{
    int x_loc;
    int y_loc;
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
void ReadSource(int color)
{
    FILE *_fp;
    if (color == 1)
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
    plist->location.x_loc = -1;
    plist->location.x_loc = -1;

    for (int i = 0; i < MAXLEN; i++)
    {
        p = plist;
        for (int j = 0; j < MAXLEN; j = j + 2)
        {
            if (source[i][j] == '\0' || source[i][j] == '\n')
            {
                break;
            }
            int color_jump = 0;
            Ptr_To_ChessTree pre = p;
            int k = 0;
            while (pre->childern[k] != NULL)
            {
                if (pre->childern[k]->location.x_loc == (source[i][j] - 'a') && pre->childern[k]->location.y_loc == (source[i][j + 1] - 'a'))
                {
                    p = p->childern[k];
                    color_jump = 1;
                    break;
                }
                k++;
            }
            if (color_jump != 0)
            {
                continue;
            }
            Ptr_To_ChessTree pnode = talloc();
            pnode->childern[0] = NULL;
            pnode->location.x_loc = source[i][j] - 'a';
            pnode->location.y_loc = source[i][j + 1] - 'a';
            pre->childern[k] = pnode;
            pre->childern[k + 1] = NULL;
            p = p->childern[k];
        }
    }
    return plist;
}
int isInSource = 1;
FILE *pf;
char sre[450];
char sre_piece[3];
Ptr_To_ChessTree p;
int DLC_Source()
{
    int k = 0;
    if (p->childern[0] != NULL)
    {
        while (1)
        {
            if (x == p->childern[k]->location.x_loc && y == p->childern[k]->location.y_loc)
            {
                p = p->childern[k];
                if (p->childern[0] != NULL)
                {
                    p = p->childern[0];
                    x = p->location.x_loc;
                    y = p->location.y_loc;
                    return 1;
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
    return 0;
}
#pragma endregion

int main()
{
    Initialized();
    Printd();
    while (1)
    {
        printf("请选择AI的棋色：\n0 白色\n1 黑色\n");
        int color;
        scanf("%d", &color);
        if (color != 0 && color != 1)
        {
            printf("读取过程发生错误，请重新输入！");
        }
        else
        {
            ReadSource(color);
            p = TreeBirth();
            if (color == 1)
            {
                pf = fopen("mysource0.txt", "a+");
                getchar();
                AiVsPlayer_Black();
            }
            else
            {
                pf = fopen("mysource.txt", "a+");
                getchar();
                AiVsPlayer_White();
            }
        }
    };
    Printd();
    return 0;
}
//五子棋AI基础函数
struct forbidden
{
    int five[2];
    int long_win[2];
    int four_live[2];
    int four_rush[2];
    int three_rush[2];
    int three_live[2];
    int two_live[2];
    int two_rush[2];
    int frb;
} Fchess[15][15];
struct decision
{
    int score;
    int x_dcs;
    int y_dcs;
};
void Qsort(struct decision Dtree[], int left, int right) //快速排序
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
    return;
}
void swap(struct decision Dtree[], int i, int j) //快速排序副函数
{
    int temp_x = Dtree[i].x_dcs;
    int temp_y = Dtree[i].y_dcs;
    int tempscore = Dtree[i].score;
    Dtree[i].score = Dtree[j].score;
    Dtree[i].x_dcs = Dtree[j].x_dcs;
    Dtree[i].y_dcs = Dtree[j].y_dcs;
    Dtree[j].score = tempscore;
    Dtree[j].x_dcs = temp_x;
    Dtree[j].y_dcs = temp_y;
    return;
}
void ScoreCheck(int x_frb, int y_frb, int color_frb) //单点分计算
{
    Fchess[x_frb][y_frb].long_win[color_frb] = 0;
    Fchess[x_frb][y_frb].five[color_frb] = 0;
    Fchess[x_frb][y_frb].four_live[color_frb] = 0;
    Fchess[x_frb][y_frb].four_rush[color_frb] = 0;
    Fchess[x_frb][y_frb].three_live[color_frb] = 0;
    Fchess[x_frb][y_frb].three_rush[color_frb] = 0;
    Fchess[x_frb][y_frb].two_live[color_frb] = 0;
    Fchess[x_frb][y_frb].two_rush[color_frb] = 0;

    int isHave = 1;
    if (color_frb == 1)
    {
        Fchess[x_frb][y_frb].frb = 0;
    }
    int same_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    if (chess[x_frb][y_frb] == 2) //判断原位置是否有点
    {
        chess[x_frb][y_frb] = color_frb;
        isHave = 0;
    }
    int fx_frb, fy_frb;
    //向右搜索
    for (fy_frb = y_frb + 1; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_0[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == 2; fy_frb++, empty_0[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_1[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == 2; fy_frb++, empty_1[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_2[0]++)
        ;
    //向右上搜索
    for (fx_frb = x_frb - 1, fy_frb = y_frb + 1; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_0[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb++, empty_0[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_1[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb++, empty_1[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_2[1]++)
        ;
    //向上搜索
    for (fx_frb = x_frb - 1; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_0[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == 2; fx_frb--, empty_0[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_1[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == 2; fx_frb--, empty_1[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_2[2]++)
        ;
    //向左上搜索
    for (fx_frb = x_frb - 1, fy_frb = y_frb - 1; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_0[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb--, empty_0[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_1[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb--, empty_1[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_2[3]++)
        ;
    //向左搜索
    for (fy_frb = y_frb - 1; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_0[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == 2; fy_frb--, empty_0[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_1[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == 2; fy_frb--, empty_1[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_2[4]++)
        ;
    //向左下搜索
    for (fx_frb = x_frb + 1, fy_frb = y_frb - 1; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_0[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb--, empty_0[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_1[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb--, empty_1[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_2[5]++)
        ;
    //向下搜索
    for (fx_frb = x_frb + 1; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_0[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == 2; fx_frb++, empty_0[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_1[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == 2; fx_frb++, empty_1[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_2[6]++)
        ;
    //向右下搜索
    for (fx_frb = x_frb + 1, fy_frb = y_frb + 1; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_0[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb++, empty_0[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_1[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb++, empty_1[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_2[7]++)
        ;
    //搜索结束
    //chess[x_frb][y_frb] = 2;
    //五连判断
    for (int i = 0; i < 4; i++)
    {
        if (same_0[i] + same_0[i + 4] == 4)
        {
            Fchess[x_frb][y_frb].five[color_frb]++;
        }
    }
    //棋型统计
    int threecount = 0;
    int fourcount = 0;
    //定义四连和三连的数量
    for (int i = 0; i < 4; i++)
    {
        if (same_0[i] + same_0[i + 4] >= 5) //长连禁手
        {
            Fchess[x_frb][y_frb].long_win[color_frb]++;
            if (color_frb == 1)
            {
                Fchess[x_frb][y_frb].frb = 1;
            }
            if (isHave == 0)
            {
                chess[x_frb][y_frb] = 2;
            }
            return;
        }
        else if (same_0[i] + same_0[i + 4] == 3) //四子相连 ？0000？
        {
            //活四冲四判断,当isfour是1为冲四，2为活四
            int isFour = 0;
            //开始判断
            if (empty_0[i] > 0) //?0000+?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    isFour++;
                }
            }
            if (empty_0[i + 4] > 0) //?+0000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    isFour++;
                }
            }
            if (isFour == 1)
            {
                fourcount++;
                Fchess[x_frb][y_frb].four_rush[color_frb]++;
            }
            else if (isFour == 2)
            {
                fourcount++;
                Fchess[x_frb][y_frb].four_live[color_frb]++;
            }
        }
        else if (same_0[i] + same_0[i + 4] == 2) //三子相连 ？000？
        {
            //判断冲3的情况?++000?和 ?000++?，死三 ?+000x不予赋值
            //活四冲四检查
            if (empty_0[i] == 1 && same_1[i] == 1) //?0+000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                    //必定是冲四
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1) //?0+000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                    //必定是冲四
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            //活三检查
            int isThree = 0;
            //?++000+?
            if ((empty_0[i] > 2 || (empty_0[i] == 2 && same_1[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                    isThree = 1;
            }
            if ((empty_0[i] > 2 || (empty_0[i] == 2 && same_1[i] == 0)) && empty_0[i + 4] == 0)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
            }
            //?+000++?
            if ((empty_0[i + 4] > 2 || (empty_0[i + 4] == 2 && same_1[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                    isThree = 1;
            }
            if ((empty_0[i + 4] > 2 || (empty_0[i + 4] == 2 && same_1[i + 4] == 0)) && empty_0[i] == 0)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
            }
            if (empty_0[i] == 1 && empty_0[i + 4] == 1 && same_1[i] == 0 && same_1[i + 4] == 0)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                    if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                        Fchess[x_frb][y_frb].three_rush[color_frb]++;
            }
            if (isThree)
            {
                threecount++;
                //判断活三
                Fchess[x_frb][y_frb].three_live[color_frb]++;
            }
        }
        else if (same_0[i] + same_0[i + 4] == 1) //两子相连 ？00？
        {
            //活四冲四判断
            if (empty_0[i] == 1 && same_1[i] == 2) //?00+00?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2) //?00+00?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 1 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    threecount++;
                    Fchess[x_frb][y_frb].three_live[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    threecount++;
                    Fchess[x_frb][y_frb].three_live[color_frb]++;
                }
            }
            //冲三判断
            if (empty_0[i] == 1 && same_1[i] == 1 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] == 0))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] == 0))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
                }
            }
            //活二和冲二的判断，可能出错 _ _ o o _ _  或 x _ o o _ _
            if (empty_0[i] >= 2 && empty_0[i + 4] >= 2)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                    if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                        Fchess[x_frb][y_frb].two_live[color_frb]++;
            }
            if ((empty_0[i] >= 2 && empty_0[i + 4] == 1 && same_1[i + 4] == 0) || (empty_0[i + 4] >= 2 && empty_0[i] == 1 && same_1[i] == 0))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                    if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                        Fchess[x_frb][y_frb].two_live[color_frb]++;
            }
            //冲二 x o o _ _
            if ((empty_0[i] == 0 && empty_0[i + 4] >= 2) || (empty_0[i + 4] == 0 && empty_0[i] >= 2))
            {
                Fchess[x_frb][y_frb].two_rush[color_frb]++;
            }
        }
        else if (same_0[i] + same_0[i + 4] == 0) //单独一子
        {
            if (empty_0[i] == 1 && same_1[i] == 3)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 3)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                    Fchess[x_frb][y_frb].four_rush[color_frb]++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 2 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    threecount++;
                    Fchess[x_frb][y_frb].three_live[color_frb]++;
                }
            } //冲三判断
            else if ((empty_0[i] == 1 && same_1[i] == 2 && empty_1[i] == 0 && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0))) || (empty_0[i] == 1 && same_1[i] == 2 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && empty_0[i + 4] == 0))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    threecount++;
                    Fchess[x_frb][y_frb].three_live[color_frb]++;
                }
            } //冲三判断
            else if ((empty_0[i + 4] == 1 && same_1[i + 4] == 2 && empty_1[i + 4] == 0 && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0))) || (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && empty_0[i] == 0))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    Fchess[x_frb][y_frb].three_rush[color_frb]++;
                }
            }
        }
    }
    //禁手分析结束
    if (isHave == 0)
    {
        chess[x_frb][y_frb] = 2;
    }
    if (color_frb == 0)
    {
        Fchess[x_frb][y_frb].frb = 0;
        return;
    }
    if (fourcount > 1)
    {
        Fchess[x_frb][y_frb].frb = 1;
        return;
    }
    if (threecount > 1)
    {
        Fchess[x_frb][y_frb].frb = 1;
        return;
    }
    Fchess[x_frb][y_frb].frb = 0;
    return;
}
int PointForbiddenCheck(int x_pointfrb, int y_pointfrb, int direction, int same, int color_pointfrb) //禁手判断选点
{
    if (color_pointfrb == 0)
    {
        return NO_FRB;
    }
    int i, j;
    same++;
    if (direction >= 4)
        same = -same;
    //关键点的坐标
    switch (direction % 4)
    {
    case 0:
        i = x_pointfrb;
        j = y_pointfrb + same;
        break;
    case 1:
        i = x_pointfrb - same;
        j = y_pointfrb + same;
        break;
    case 2:
        i = x_pointfrb - same;
        j = y_pointfrb;
        break;
    default:
        i = x_pointfrb - same;
        j = y_pointfrb - same;
        break;
    }
    if (i < 0 || i > 14 || j < 0 || j > 14)
    {
        printf("error");
    }
    if (chess[x_pointfrb][y_pointfrb] == 1 - color_pointfrb)
    {
        printf("ERROR");
        exit(1);
    }
    //放入棋子
    int isHave = 1;
    if (chess[x_pointfrb][y_pointfrb] == 2)
    {
        isHave = 0;
    }
    if (chess[x_pointfrb][y_pointfrb] == 1 - color_pointfrb)
    {
        printf("bug!!!");
        exit(1);
    }
    if (chess[7][1] == 1 && chess[7][2] == 1 && chess[7][3] == 1)
    {
        //  printf("bug!!!");
        // Printd();
    }
    chess[x_pointfrb][y_pointfrb] = color_pointfrb;
    int result = ForbiddenCheck(i, j, 1);
    if (isHave == 0)
        chess[x_pointfrb][y_pointfrb] = 2;
    return result;
}
int ForbiddenCheck(int x_frb, int y_frb, int color_frb) //禁手递归
{
    int same_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int empty_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int same_2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int fx_frb, fy_frb;
    //向右搜索
    for (fy_frb = y_frb + 1; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_0[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == 2; fy_frb++, empty_0[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_1[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == 2; fy_frb++, empty_1[0]++)
        ;
    for (; fy_frb <= 14 && chess[x_frb][fy_frb] == color_frb; fy_frb++, same_2[0]++)
        ;
    //向右上搜索
    for (fx_frb = x_frb - 1, fy_frb = y_frb + 1; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_0[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb++, empty_0[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_1[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb++, empty_1[1]++)
        ;
    for (; fx_frb >= 0 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb++, same_2[1]++)
        ;
    //向上搜索
    for (fx_frb = x_frb - 1; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_0[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == 2; fx_frb--, empty_0[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_1[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == 2; fx_frb--, empty_1[2]++)
        ;
    for (; fx_frb >= 0 && chess[fx_frb][y_frb] == color_frb; fx_frb--, same_2[2]++)
        ;
    //向左上搜索
    for (fx_frb = x_frb - 1, fy_frb = y_frb - 1; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_0[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb--, empty_0[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_1[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb--, fy_frb--, empty_1[3]++)
        ;
    for (; fx_frb >= 0 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb--, fy_frb--, same_2[3]++)
        ;
    //向左搜索
    for (fy_frb = y_frb - 1; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_0[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == 2; fy_frb--, empty_0[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_1[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == 2; fy_frb--, empty_1[4]++)
        ;
    for (; fy_frb >= 0 && chess[x_frb][fy_frb] == color_frb; fy_frb--, same_2[4]++)
        ;
    //向左下搜索
    for (fx_frb = x_frb + 1, fy_frb = y_frb - 1; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_0[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb--, empty_0[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_1[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb--, empty_1[5]++)
        ;
    for (; fx_frb <= 14 && fy_frb >= 0 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb--, same_2[5]++)
        ;
    //向下搜索
    for (fx_frb = x_frb + 1; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_0[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == 2; fx_frb++, empty_0[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_1[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == 2; fx_frb++, empty_1[6]++)
        ;
    for (; fx_frb <= 14 && chess[fx_frb][y_frb] == color_frb; fx_frb++, same_2[6]++)
        ;
    //向右下搜索
    for (fx_frb = x_frb + 1, fy_frb = y_frb + 1; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_0[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb++, empty_0[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_1[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == 2; fx_frb++, fy_frb++, empty_1[7]++)
        ;
    for (; fx_frb <= 14 && fy_frb <= 14 && chess[fx_frb][fy_frb] == color_frb; fx_frb++, fy_frb++, same_2[7]++)
        ;
    //搜索结束
    chess[x_frb][y_frb] = 2;
    //五连判断
    for (int i = 0; i < 4; i++)
    {
        if (same_0[i] + same_0[i + 4] == 4)
        {
            return NO_FRB;
        }
    }
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
        else if (same_0[i] + same_0[i + 4] == 3) //四子相连 ？0000？
        {
            //活四冲四判断,当isfour是1为冲四，2为活四
            int isFour = 0;
            //开始判断
            if (empty_0[i] > 0) //?0000+?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    isFour++;
                }
            }
            if (empty_0[i + 4] > 0) //?+0000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    isFour++;
                }
            }
            if (isFour > 0)
            {
                fourcount++;
            }
        }
        else if (same_0[i] + same_0[i + 4] == 2) //三子相连 ？000？
        {
            //活四冲四检查
            if (empty_0[i] == 1 && same_1[i] == 1) //?0+000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1) //?0+000?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            //活三检查
            int isThree = 0;
            //?++000+?
            if ((empty_0[i] > 2 || (empty_0[i] == 2 && same_1[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                    isThree = 1;
            }
            //?+000++?
            if ((empty_0[i + 4] > 2 || (empty_0[i + 4] == 2 && same_1[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                    isThree = 1;
            }
            if (isThree)
            {
                threecount++;
            }
        }
        else if (same_0[i] + same_0[i + 4] == 1) //两子相连 ？00？
        {
            //活四冲四判断
            if (empty_0[i] == 1 && same_1[i] == 2) //?00+00?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2) //?00+00?
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 1 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    threecount++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 1 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    threecount++;
                }
            }
        }
        else if (same_0[i] + same_0[i + 4] == 0) //单独一子
        {
            if (empty_0[i] == 1 && same_1[i] == 3)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 3)
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    fourcount++;
                }
            }
            //活三判断
            if (empty_0[i] == 1 && same_1[i] == 2 && (empty_1[i] > 1 || (empty_1[i] == 1 && same_2[i] == 0)) && (empty_0[i + 4] > 1 || (empty_0[i + 4] == 1 && same_1[i + 4] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i, same_0[i], color_frb) == NO_FRB)
                {
                    threecount++;
                }
            }
            if (empty_0[i + 4] == 1 && same_1[i + 4] == 2 && (empty_1[i + 4] > 1 || (empty_1[i + 4] == 1 && same_2[i + 4] == 0)) && (empty_0[i] > 1 || (empty_0[i] == 1 && same_1[i] == 0)))
            {
                if (PointForbiddenCheck(x_frb, y_frb, i + 4, same_0[i + 4], color_frb) == NO_FRB)
                {
                    threecount++;
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
int Score(int x_score, int y_score, int color_score) //得分
{
    int score = 0;
    if (Fchess[x_score][y_score].four_live[color_score] > 0 || Fchess[x_score][y_score].five[color_score] > 0)
    {
        score += 4320;
    }
    //对成五评分
    if (Fchess[x_score][y_score].five[0] > 0 || Fchess[x_score][y_score].five[1] > 0 || Fchess[x_score][y_score].long_win[0] > 0)
    {
        score += (Fchess[x_score][y_score].five[0] + Fchess[x_score][y_score].five[1] + Fchess[x_score][y_score].long_win[0] > 0) * 50000;
        return score;
    }
    score += ((Fchess[x_score][y_score].four_live[0] + Fchess[x_score][y_score].four_live[1]) * 4320 + (Fchess[x_score][y_score].four_rush[0] + Fchess[x_score][y_score].four_rush[1]) * 720 + (Fchess[x_score][y_score].three_live[0] + Fchess[x_score][y_score].three_live[1]) * 720 + (Fchess[x_score][y_score].three_rush[0] + Fchess[x_score][y_score].three_rush[1]) * 480 + (Fchess[x_score][y_score].two_rush[0] + Fchess[x_score][y_score].two_rush[1]) * 30 + (Fchess[x_score][y_score].two_live[0] + Fchess[x_score][y_score].two_live[1]) * 60);
    if (Fchess[x_score][y_score].four_live[color_score] > 0)
    {
        score += 4320;
    }
    return score;
}
void ClearMyArray() //清除数组
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                Fchess[i][j].five[k] = 0;
                Fchess[i][j].four_live[k] = 0;
                Fchess[i][j].four_rush[k] = 0;
                Fchess[i][j].three_live[k] = 0;
                Fchess[i][j].three_rush[k] = 0;
                Fchess[i][j].two_live[k] = 0;
                Fchess[i][j].two_rush[k] = 0;
            }
        }
    }
}
int ScoreAllCheck(int x_sac, int y_sac, int color_sac) //全局分
{
    int score1 = 0;
    int score0 = 0;
    int isWin = 0;
    chess[x_sac][y_sac] = color_sac;
    ClearMyArray();
    struct forbidden scoreMeasure;
    for (int i = 0; i < 2; i++)
    {
        scoreMeasure.five[i] = 0;
        scoreMeasure.four_rush[i] = 0;
        scoreMeasure.four_live[i] = 0;
        scoreMeasure.three_live[i] = 0;
        scoreMeasure.three_rush[i] = 0;
        scoreMeasure.two_live[i] = 0;
        scoreMeasure.two_rush[i] = 0;
        scoreMeasure.long_win[i] = 0;
    }
    //首先要判断对手得分
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (chess[i][j] == 1 - color_sac)
            {
                ScoreCheck(i, j, 1 - color_sac);
                scoreMeasure.five[1 - color_sac] += Fchess[i][j].five[1 - color_sac];
                scoreMeasure.four_rush[1 - color_sac] += Fchess[i][j].four_rush[1 - color_sac];
                scoreMeasure.four_live[1 - color_sac] += Fchess[i][j].four_live[1 - color_sac];
                scoreMeasure.three_rush[1 - color_sac] += Fchess[i][j].three_rush[1 - color_sac];
                scoreMeasure.three_live[1 - color_sac] += Fchess[i][j].three_live[1 - color_sac];
                scoreMeasure.two_live[1 - color_sac] += Fchess[i][j].two_live[1 - color_sac];
                scoreMeasure.two_rush[1 - color_sac] += Fchess[i][j].two_rush[1 - color_sac];
                scoreMeasure.long_win[1 - color_sac] += Fchess[i][j].long_win[1 - color_sac];
            }
            else if (chess[i][j] == color_sac)
            {
                ScoreCheck(i, j, color_sac);
                scoreMeasure.five[color_sac] += Fchess[i][j].five[color_sac];
                scoreMeasure.four_rush[color_sac] += Fchess[i][j].four_rush[color_sac];
                scoreMeasure.four_live[color_sac] += Fchess[i][j].four_live[color_sac];
                scoreMeasure.three_rush[color_sac] += Fchess[i][j].three_rush[color_sac];
                scoreMeasure.three_live[color_sac] += Fchess[i][j].three_live[color_sac];
                scoreMeasure.two_live[color_sac] += Fchess[i][j].two_live[color_sac];
                scoreMeasure.two_rush[color_sac] += Fchess[i][j].two_rush[color_sac];
                scoreMeasure.long_win[color_sac] += Fchess[i][j].long_win[color_sac];
            }
        }
    }
    chess[x_sac][y_sac] = 2;
    for (int i = 0; i < 2; i++)
    {
        Fchess[x_sac][y_sac].five[i] = scoreMeasure.five[i];
        Fchess[x_sac][y_sac].four_rush[i] = scoreMeasure.four_rush[i];
        Fchess[x_sac][y_sac].four_live[i] = scoreMeasure.four_live[i];
        Fchess[x_sac][y_sac].three_live[i] = scoreMeasure.three_live[i];
        Fchess[x_sac][y_sac].three_rush[i] = scoreMeasure.three_rush[i];
        Fchess[x_sac][y_sac].two_live[i] = scoreMeasure.two_live[i];
        Fchess[x_sac][y_sac].two_rush[i] = scoreMeasure.two_rush[i];
        Fchess[x_sac][y_sac].long_win[i] = scoreMeasure.long_win[i];
    }
    score1 = RealScore(x_sac, y_sac, 1 - color_sac, 1);
    if (score1 >= 50000)
    {
        isWin = 1;
    }
    score1 = -score1;
    score0 = RealScore(x_sac, y_sac, color_sac, 0);
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
int RealScore(int x_rs, int y_rs, int color_rs, int object) //真实得分
{
    int score = 0;
    if (object == 1) //目标是对方的时候
    {
        //不可能出现成五的情况
        //现在考虑必输的情况，包含所有冲四和活三
        if (Fchess[x_rs][y_rs].four_live[color_rs] > 0 || Fchess[x_rs][y_rs].four_rush[color_rs] > 0 || Fchess[x_rs][y_rs].five[color_rs] > 0 || Fchess[x_rs][y_rs].three_live[color_rs] > 0)
        {
            score += 50000;
        }
        //现在考虑其余情况
        score += Fchess[x_rs][y_rs].three_rush[color_rs] * 480 / 3;
        //对二评分
        score += Fchess[x_rs][y_rs].two_rush[color_rs] * 60 / 2;
        score += Fchess[x_rs][y_rs].two_live[color_rs] * 120 / 2;
        return score;
    }
    else
    {
        if (Fchess[x_rs][y_rs].five[color_rs] > 0 || (color_rs == 0 && Fchess[x_rs][y_rs].long_win[color_rs] > 0)) //五必胜，赋分为最大值
        {
            score += (-INF);
            return score;
        }
        score += Fchess[x_rs][y_rs].four_live[color_rs] * 4320 / 4;
        score += Fchess[x_rs][y_rs].four_rush[color_rs] * 720 / 4;
        score += Fchess[x_rs][y_rs].three_live[color_rs] * 720 / 3;
        score += Fchess[x_rs][y_rs].three_rush[color_rs] * 480 / 3;
        //对二评分
        score += Fchess[x_rs][y_rs].two_rush[color_rs] * 60 / 2;
        score += Fchess[x_rs][y_rs].two_live[color_rs] * 120 / 2;
        //三四不禁手赋分
        return score;
    }
}
//五子棋AI综合函数
void AiVsPlayer_Black() //AI执黑
{
    while (1)
    {
        //黑子的回合
        if (step == 0)
        {
            x = 7;
            y = 7;
            step++;

            int k = 0;
            while (1)
            {
                if (x == p->childern[k]->location.x_loc && y == p->childern[k]->location.y_loc)
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
        else
        {
            if (isInSource == 1 && DLC_Source() == 1)
            {
            }
            else
                RandomChess(1, 1, INF, -INF);
            if (chess[x][y] == 0)
            {
                End();
            }
        }
        chess[x][y] = 1;
        //资源库
        sre_piece[0] = 'a' + x;
        sre_piece[1] = 'a' + y;
        sre_piece[2] = '\0';
        strcat(sre, sre_piece);
        Printd();
        if (Judged(x, y, 1))
        {
            printf("黑子获胜，游戏结束！\n");
            scanf("%d", &x);
            exit(1);
        }
        //白子的回合
        while (1)
        {
            char input[100];
            int inputNum = 0;
            int c;
            while ((c = getchar()) != EOF && c != '\n')
            {
                input[inputNum] = c;
                inputNum++;
            }
            input[inputNum] = '\0';
            while (!Handling(input))
            {
                printf("请输入坐标（例如：3，h），输入quit可以直接结束进程：\n");
                inputNum = 0;
                while ((c = getchar()) != EOF && c != '\n')
                {
                    input[inputNum] = c;
                    inputNum++;
                }
                input[inputNum] = '\0';
            }
            if ((chess[x][y] != 2))
            {
                printf("此处已经有棋子了！\n");
                continue;
            }
            getchar();
            chess[x][y] = 0;
            //资源库
            sre_piece[0] = 'a' + x;
            sre_piece[1] = 'a' + y;
            sre_piece[2] = '\0';
            strcat(sre, sre_piece);
            Printd();
            break;
        }
        if (Judged(x, y, 0))
        {
            printf("白子获胜，游戏结束！\n");
            if (isInSource != 1)
            {
                fprintf(pf, "%s\n", sre);
                fclose(pf);
            }
            scanf("%d", &x);
            exit(1);
        }
    }
}
void AiVsPlayer_White() //AI执白
{
    Ptr_To_ChessTree p = TreeBirth();
    while (1)
    {
        while (1)
        {
            char input[100];
            int inputNum = 0;
            int c;
            while ((c = getchar()) != EOF && c != '\n')
            {
                input[inputNum] = c;
                inputNum++;
            }
            input[inputNum] = '\0';
            while (!Handling(input))
            {
                printf("请输入坐标（例如：3，h），输入quit可以直接结束进程：\n");
                inputNum = 0;
                while ((c = getchar()) != EOF && c != '\n')
                {
                    input[inputNum] = c;
                    inputNum++;
                }
                input[inputNum] = '\0';
            }
            if ((chess[x][y] != 2))
            {
                printf("此处已经有棋子了！\n");
                continue;
            }
            getchar();
            ScoreCheck(x, y, 1);
            if (Fchess[x][y].frb == 1)
            {
                printf("禁手！白子获胜，游戏结束！\n");
                scanf("%d", &x);
                exit(1);
            }
            chess[x][y] = 1;
            //资源库
            sre_piece[0] = 'a' + x;
            sre_piece[1] = 'a' + y;
            sre_piece[2] = '\0';
            strcat(sre, sre_piece);
            Printd();
            break;
        }

        if (Judged(x, y, 1))
        {
            printf("黑子获胜，游戏结束！\n");
            if (isInSource != 1)
            {
                fprintf(pf, "%s\n", sre);
                fclose(pf);
            }
            scanf("%d", &x);
            exit(1);
        }
        if (isInSource == 1 && DLC_Source() == 1)
        {
        }
        else
            RandomChess(1, 0, INF, -INF);
        if (chess[x][y] == 1)
        {
            End();
        }
        chess[x][y] = 0;
        //资源库
        sre_piece[0] = 'a' + x;
        sre_piece[1] = 'a' + y;
        sre_piece[2] = '\0';
        strcat(sre, sre_piece);
        Printd();
        if (Judged(x, y, 0))
        {
            printf("白子获胜，游戏结束！\n");
            scanf("%d", &x);
            exit(1);
        }
    }
}
int RandomChess(int level, int color_rdm, int a_rdm, int b_rdm) //递归算法
{
    int isScore[16][16] = {0};
    ClearMyArray();
    struct decision Dtree[225];
    for (int i = 0; i < 225; i++)
    {
        Dtree[i].score = INF;
        Dtree[i].x_dcs = -1;
        Dtree[i].y_dcs = -1;
    }
    if (level >= DEPTH) //底层用局面分
    {
        int score = 0;
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                if (chess[i][j] != 2)
                {
                    for (int m = -1; m < 2; m++)
                    {
                        for (int n = -1; n < 2; n++)
                        {
                            if (i + m >= 0 && i + m <= 14 && j + n >= 0 && j + n <= 14)
                            {
                                if (chess[i + m][j + n] == 2 && isScore[i + m][j + n] == 0)
                                {
                                    isScore[i + m][j + n] = 1;
                                    if (color_rdm == 1)
                                    {
                                        ScoreCheck(i + m, j + n, 1);
                                        if (Fchess[i + m][j + n].frb == 1)
                                        {
                                            isScore[i + m][j + n] = -1;
                                            break;
                                        }
                                    }
                                    if ((score = ScoreAllCheck(i + m, j + n, color_rdm)) >= Dtree[0].score)
                                    {
                                        Dtree[0].score = score;
                                        Dtree[0].x_dcs = i + m;
                                        Dtree[0].y_dcs = j + n;
                                        if (Dtree[0].score >= (-INF))
                                        {
                                            // x = i + m;
                                            // y = j + n;
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
        return Dtree[0].score;
    }
    int k = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (chess[i][j] != 2)
            {
                for (int m = -1; m < 2; m++)
                {
                    for (int n = -1; n < 2; n++)
                    {
                        if (i + m >= 0 && i + m <= 14 && j + n >= 0 && j + n <= 14)
                        {
                            if (chess[i + m][j + n] == 2 && isScore[i + m][j + n] != 1)
                            {
                                ScoreCheck(i + m, j + n, 0);
                                ScoreCheck(i + m, j + n, 1);
                                if (color_rdm == 1 && Fchess[i + m][j + n].frb == 1)
                                {
                                    isScore[i + m][j + n] = -1;
                                    //禁手点不做处理
                                }
                                else
                                {
                                    isScore[i + m][j + n] = 1;
                                    Dtree[k].score = Score(i + m, j + n, color_rdm);
                                    Dtree[k].x_dcs = i + m;
                                    Dtree[k].y_dcs = j + n;
                                    k++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Qsort(Dtree, 0, k - 1); //快速排序
    if (level == DEPTH)
    {
        if (level == 1)
        {
            x = Dtree[0].x_dcs;
            y = Dtree[0].y_dcs;
        }
        return Dtree[0].score;
    }
    if (Dtree[0].score >= 50000)
    {
        chess[Dtree[0].x_dcs][Dtree[0].y_dcs] = color_rdm;
        if (Judged(Dtree[0].x_dcs, Dtree[0].y_dcs, color_rdm))
        {
            chess[Dtree[0].x_dcs][Dtree[0].y_dcs] = 2;
            if (level % 2 == 0)
            {
                if (level == 1)
                {
                    x = Dtree[0].x_dcs;
                    y = Dtree[0].y_dcs;
                }
                return INF;
            }
            else
            {
                if (level == 1)
                {
                    x = Dtree[0].x_dcs;
                    y = Dtree[0].y_dcs;
                }
                return -INF;
            }
        }
    }
    for (int i = 0; i < WIDTH; i++)
    {
        if (DEPTH == 9)
        {
            if (level >= 8 && i >= 5)
            {
                // Dtree[i].score = INF;
                continue;
            }
            if (level >= 7 && i >= 8)
            {
                // Dtree[i].score = INF;
                continue;
            }
            if (level >= 4 && i >= 12)
            {
                // Dtree[i].score = INF;
                continue;
            }
            if (Dtree[i].x_dcs < 0)
            {
                continue;
            }
        }
        chess[Dtree[i].x_dcs][Dtree[i].y_dcs] = color_rdm;
        if (level == 1)
        {
            printf("|");
        }
        Dtree[i].score = RandomChess(++level, 1 - color_rdm, a_rdm, b_rdm);
        level--;
        chess[Dtree[i].x_dcs][Dtree[i].y_dcs] = 2;
        if (level % 2 == 0)
        {
            if (Dtree[i].score <= b_rdm)
            {
                b_rdm = Dtree[i].score;
            }
            if (a_rdm >= b_rdm)
            {
                return Dtree[i].score;
            }
        }
        else
        {
            if (Dtree[i].score > a_rdm)
            {
                a_rdm = Dtree[i].score;
                if (level == 1)
                {
                    x = Dtree[i].x_dcs;
                    y = Dtree[i].y_dcs;
                }
            }
            if (a_rdm >= b_rdm)
            {
                return Dtree[i].score;
            }
        }
    }
    if (level == 1)
    {

        printf("\n本步落子位置为%d,%c,得分为：%d\n", 15 - x, y + 'a', a_rdm);
    }
    if (level % 2 == 0)
    {
        return b_rdm;
    }
    else
    {
        return a_rdm;
    }
}
//五子棋基础函数
//包含打印棋盘函数Printd(),标记最新落子函数LastChess(),判断胜负函数Judged(),处理输入函数Handling()，初始化函数Initialized()
void Printd() //函数：打印棋盘
{
    int i, j;
    printf("----------R E B O R N----------\n          作者:谷朝阳         \n");
    for (i = 0; i <= 14; i++)
    {
        //第1行
        if (i == 0)
        {
            //第1列
            printf("15");
            j = 0;
            LastChess(i, j);
            if (chess[i][j] == 2)
                printf("┌");

            //第2-14列
            for (j = 1; j <= 13; j++)
            {
                LastChess(i, j);
                if (chess[i][j] == 2)
                    printf("┬");
            }

            //第15列
            LastChess(i, j);
            if (chess[i][j] == 2)
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
            LastChess(i, j);
            if (chess[i][0] == 2)
                printf("├");

            //第2-14列
            for (j = 1; j <= 13; j++)
            {
                LastChess(i, j);
                if (chess[i][j] == 2)
                    printf("┼");
            }

            //第15列
            LastChess(i, j);
            if (chess[i][j] == 2)
                printf("┤");
            printf("\n");
        }

        //第15行

        if (i == 14)
        {
            printf("%d ", 15 - i);
            j = 0;
            LastChess(i, j);
            if (chess[i][0] == 2)
                printf("└");

            for (j = 1; j <= 13; j++)
            {
                LastChess(i, j);
                if (chess[i][j] == 2)
                    printf("┴");
            }
            LastChess(i, j);
            if (chess[i][j] == 2)
                printf("┘");
            printf("\n");
        }
    }
    printf("  a b c d e f g h i j k l m n o          作者：谷朝阳\n");
    printf("请输入坐标（例如：8，h），输入quit可以直接结束进程：\n");
}
void LastChess(int i, int j) //标记最近一次的棋子
{
    if (chess[i][j] == 1)
    {
        if ((i) == x && (j) == y)
            printf("▲");
        else
            printf("●");
    }
    if (chess[i][j] == 0)
    {
        if ((i) == x && (j) == y)
            printf("△");
        else
            printf("○");
    }
}
int Judged(int x_judge, int y_judge, int switJudge) //判断胜负是否产生，胜负产生返回1，未产生返回0
{
    int x_judge_store = x_judge;
    int y_judge_store = y_judge;
    int judgeNumber = 1;
    //横向计算连子个数
    while (--x_judge >= 0 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    x_judge = x_judge_store;
    while (++x_judge <= 14 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    if (judgeNumber >= 5)
    {
        return 1;
    }
    judgeNumber = 1;
    x_judge = x_judge_store;
    //竖向计算连子个数
    while (--y_judge >= 0 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    y_judge = y_judge_store;
    while (++y_judge <= 14 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    if (judgeNumber >= 5)
    {
        return 1;
    }
    judgeNumber = 1;
    y_judge = y_judge_store;
    //左上向右下计算连子个数
    while (--y_judge >= 0 && --x_judge >= 0 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    x_judge = x_judge_store;
    y_judge = y_judge_store;
    while (++y_judge <= 14 && ++x_judge <= 14 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    if (judgeNumber >= 5)
    {
        return 1;
    }
    judgeNumber = 1;
    x_judge = x_judge_store;
    y_judge = y_judge_store;
    //右上向左下计算连子个数
    while (++y_judge <= 14 && --x_judge >= 0 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    x_judge = x_judge_store;
    y_judge = y_judge_store;
    while (--y_judge >= 0 && ++x_judge <= 14 && chess[x_judge][y_judge] == switJudge)
    {
        judgeNumber++;
    }
    if (judgeNumber >= 5)
    {
        return 1;
    }
    return 0;
}
int Handling(char s[]) //处理输入的字符数组
{                      //这里我考虑了先输入字母后数字以及相反的情况，而且大小写都要考虑
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
void End() //我不想摆烂
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (chess[i][j] == 2)
            {
                x = i;
                y = j;
                return;
            }
        }
    }
}
void Initialized() //棋盘初始化
{
    x = -2;
    y = -2;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            chess[i][j] = 2;
        }
    }
}
