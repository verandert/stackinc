#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define true 1
#define fasle 0
#define Row 10
#define Column 10
#define E 0
#define W 2
#define South 1
#define N 3

int map[Row][Column];//迷宫地图
int pass[Row][Column];//二维数组记录已经走过的通道。
typedef struct//地图某个点
{
    int row, column;
}PosType;
typedef struct 
{
    int ord; //位置编号
    PosType seat; //点
    int dir; //进入下个点的方向0/1/2/3表示东南西北
}Elem;

typedef Elem elemtype;
typedef int bool;
typedef struct StackNode
{
    elemtype data;
    struct StackNode *next;
}StackNode, *SNode;

typedef struct
{
    StackNode *base, *top;
    int length;
}stack;

void InitStack(stack *S);
void DestroyStack(stack *S);
void ClearStack(stack *S);
bool IsEmptyStack(stack *S);
void Push(stack *S, elemtype data);
elemtype Pop(stack *S);
StackNode* GetTop(stack *S);
bool IsInMap(PosType Pos);
bool Pass(PosType Pos);
PosType NextPos(PosType Pos, int dir);
void InitMap(int (*)[Row]);
bool MazePath(stack *S, PosType start, PosType end);

int main()
{
    stack S, S1;
    PosType start, end, pos;
    Elem e;
    int result;
    start.row = 0; start.column = 0;
    end.row = 2; end.column = 3;
    InitMap(map);
    for (int i = 0; i < Row; i++)
    {
        for (int j = 0; j < Column; j++)
        {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
    InitStack(&S);
    InitStack(&S1);
    result = MazePath(&S, start, end);
    printf("是否有路径找到出口:%d\n", result);
    while(!IsEmptyStack(&S))
    {
        Push(&S1, Pop(&S));
    }
    while(!IsEmptyStack(&S1))
    {
        e = Pop(&S1);
        printf("-> (%d, %d) ", e.seat.row, e.seat.column);
    }
    printf("\n");
    return 0;
}

//初始化迷宫地图 10X10
void InitMap(int (*p)[Row])
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < Row; i++)
    {
        for (int j = 0; j < Column; j++)
        {
            *(*(p + i) + j) = rand() % 2;
        }
    }
    
}
//该点是否在迷宫中
bool IsInMap(PosType Pos)
{
    if (Pos.row >= 0 && Pos.column >= 0 && Pos.row <= Row - 1 && Pos.column <= Column - 1) return true;
    return fasle;
}

//该点是否可通, 并且是未曾走过的道路。
bool Pass(PosType Pos)
{
    if (!IsInMap(Pos)) return fasle;
    if (!pass[Pos.row][Pos.column] && map[Pos.row][Pos.column])
    {
        pass[Pos.row][Pos.column] = 1;
        return true;
    }
    return fasle;
}

//取该点的下一个方向的点，0/1/2/3 分别表示东南西北方向；
PosType NextPos(PosType Pos, int dir)
{
    PosType NextPos;
    if (!IsInMap(Pos)) {NextPos.row = -1; NextPos.column = -1; return NextPos;}
    NextPos = Pos;
    switch (dir)
    {
    case E: NextPos.column++;  break;
    case South: NextPos.row++; break;
    case W: NextPos.column--; break;
    case N: NextPos.row--; break;
    default: break;
    }
    if (!IsInMap(NextPos)) {NextPos.row = -1; NextPos.column = -1;}
    return NextPos;
}

//栈应用--迷宫求解
bool MazePath(stack *S, PosType start, PosType end)
{
    PosType curpos;
    int step = 1;
    Elem e;
    curpos = start;
    do
    {
       if(Pass(curpos))
       {
           e.dir = 0; e.ord = step; e.seat = curpos;
           Push(S, e);
           if(curpos.row == end.row && curpos.column == end.column) return true;
           curpos = NextPos(curpos, 0);
           step++;
       }
       else
       {
            if(!IsEmptyStack(S))
            {
                e = Pop(S);
                pass[e.seat.row][e.seat.column] = 0;
                while (e.dir == 3 && !IsEmptyStack(S))
                {
                    e = Pop(S);
                    pass[e.seat.row][e.seat.column] = 0;
                }
                if (e.dir < 3 && !pass[e.seat.row][e.seat.column]) 
                {
                    
                    e.dir++; Push(S, e); pass[e.seat.row][e.seat.column] = 1;
                    curpos = NextPos(e.seat, e.dir);
                }
            }
       }
    } while (!IsEmptyStack(S));
    return fasle;
}

//栈初始化
void InitStack(stack *S)
{
    StackNode *p;
    if((p = (StackNode *)malloc(sizeof(StackNode)))){
        p->next = NULL;
        S->base = S->top = p;
        S->length = 0;
    } else {
        printf("内存分配失败！");
        exit(-1);
    }
}

//摧毁栈
void DestroyStack(stack *S)
{
    StackNode *p, *q;
    q = S->top;
    while (q != S->base)
    {
        p = q; q = q->next;
        free(p);
    }
    free(q);//与清空栈的区别
    S->top = S->base = NULL;
}

//清空栈
void ClearStack(stack *S)
{
    StackNode *p, *q;
    q = S->top;
    while (q != S->base)
    {
        p = q; q = q->next;
        free(p);
    }
    S->top = S->base;   
    S->length = 0;
}

//栈是否为空
bool IsEmptyStack(stack *S)
{
    if (S->base == S->top) {return true;}
    return fasle;
}

//进栈
void Push(stack *S, elemtype data)
{
    StackNode *p;
    if((p = (StackNode *)malloc(sizeof(StackNode)))){
        S->top->data = data;
        p->next = S->top;
        S->top = p;
        S->length++;
    } else
    {
        printf("堆分配失败");
        exit(-1);
    }
    
}

//出栈
elemtype Pop(stack *S)
{
    if (IsEmptyStack(S)) {printf("空栈"); exit(-1);}
    elemtype data;
    StackNode *p;
    p = S->top;
    data = p->next->data; S->top = S->top->next;
    free(p);
    S->length--;
    return data;
}

//取栈顶元素，但不出栈；
StackNode* GetTop(stack *S)
{
    return S->top->next;
}

