// 코드트리 2020 상반기 오전 2번 - 술래잡기 체스
//
// 문제 요약
// 4x4 격자판 위에 1번 ~ 16번 도둑말이 하나씩 놓여 있다.
// 각 도둑말은 방향을 가지고 있으며,
// 술래는 (0,0)에서 시작해서 해당 칸의 도둑말을 먼저 잡아먹는다.
//
// 진행 규칙
// 1. 술래가 현재 칸의 도둑말을 잡아먹고, 그 말의 방향을 물려받는다.
// 2. 살아 있는 도둑말들은 번호가 작은 순서부터 이동한다.
// 3. 도둑말은 현재 방향으로 이동하려고 하며,
//    범위를 벗어나거나 술래 칸이면 반시계 방향으로 45도씩 회전하며
//    이동 가능한 칸을 찾는다.
// 4. 도둑말은 빈칸으로 가는 것이 아니라,
//    이동 대상 칸의 말과 자리를 서로 바꾼다.
// 5. 도둑말 이동이 끝나면 술래는 현재 방향으로 1~3칸 이동할 수 있다.
// 6. 술래가 이동한 칸에 있는 도둑말을 다시 잡아먹으며 같은 과정을 반복한다.
//
// 목표
// 술래가 잡아먹을 수 있는 도둑말 번호 합의 최댓값을 구한다.
//
// 핵심 아이디어
// - MAP[r][c] : 해당 칸에 있는 도둑말 번호
// - chess[i]  : i번 도둑말의 위치, 방향, 생존 여부
// - DFS()     : 현재 상태에서 술래가 갈 수 있는 모든 경우를 탐색
//
// 즉,
// "현재 상태를 복사 -> 도둑말 전부 이동 -> 술래의 다음 이동 후보를 재귀 탐색"
// 하는 완전탐색(백트래킹) 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (4 + 2)
#define MAX_H (16 + 5)   // 도둑말 최대 개수(1~16)

// 도둑말 상태 구조체
struct CHESS
{
    int r;       // 현재 행
    int c;       // 현재 열
    int dir;     // 현재 방향
    bool dead;   // 죽었는지 여부
};

typedef struct CHESS CHESS;

int T;

// MAP[r][c] = (r,c)에 있는 도둑말 번호
int MAP[MAX][MAX];

// chess[i] = i번 도둑말의 상태
CHESS chess[MAX_H];

// 술래 상태
CHESS tagger;

// 정답(최대 점수)
int maxAnswer;

// 방향 번호 기준
// -, ↑, ↖, ←, ↙, ↓, ↘, →, ↗
int dr[] = { 0,-1,-1,0,1,1,1,0,-1 };
int dc[] = { 0, 0,-1,-1,-1,0,1,1,1 };

/*
    입력 함수

    4x4 칸에 대해
    (도둑말 번호, 방향) 쌍을 입력받는다.

    MAP[r][c] 에는 도둑말 번호를 저장하고,
    chess[index] 에는 해당 도둑말의 위치/방향/생존 여부를 저장한다.
*/
void input()
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            int index, dir;

            scanf("%d %d", &index, &dir);

            MAP[r][c] = index;

            chess[index].r = r;
            chess[index].c = c;
            chess[index].dir = dir;
            chess[index].dead = false;
        }
    }
}

/*
    디버그용 출력 함수

    현재 점수, 술래 상태, 살아 있는 도둑말,
    그리고 보드 상태를 출력한다.
*/
void printStatus(CHESS tagger, int map[MAX][MAX], CHESS chess[MAX_H], int score)
{
    printf("score : %d\n", score);
    printf("tagger : r %d, c %d, dir %d\n", tagger.r, tagger.c, tagger.dir);

    printf("live chess : ");
    for (int i = 1; i <= 16; i++)
        if (chess[i].dead == false) printf("%d ", i);
    putchar('\n');

    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (r == tagger.r && c == tagger.c)
            {
                printf("(%d, %d) ", -1, tagger.dir);
            }
            else
            {
                int idx = map[r][c];
                printf("(%d, %d) ", idx, chess[idx].dir);
            }
        }
        putchar('\n');
    }
    putchar('\n');
}

/*
    DFS 함수

    prevTagger : 이전 상태의 술래 정보
    prevMap    : 이전 상태의 보드
    prevChess  : 이전 상태의 도둑말 정보
    score      : 현재까지 누적 점수

    동작 순서
    1. 현재 상태를 복사
    2. 술래가 현재 칸의 도둑말을 잡아먹음
    3. 살아 있는 도둑말을 1번부터 16번까지 이동
    4. 술래가 현재 방향으로 1~3칸 이동 가능한 경우를 DFS로 탐색
*/
void DFS(CHESS prevTagger, int prevMap[MAX][MAX], CHESS prevChess[MAX_H], int score)
{
    int tmpMAP[MAX][MAX] = { 0 };
    CHESS tmpChess[MAX_H] = { 0 };
    CHESS pt = prevTagger;

    // -------------------------
    // 현재 상태 깊은 복사
    // -------------------------
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            tmpMAP[r][c] = prevMap[r][c];

    for (int i = 1; i <= 16; i++)
        tmpChess[i] = prevChess[i];

    // 현재 술래가 있는 칸의 도둑말 번호
    int deadIndex = tmpMAP[pt.r][pt.c];

    // 이미 죽은 말이라면 더 진행할 수 없음
    // (정상적인 탐색에서는 거의 안 나오지만 안전장치)
    if (tmpChess[deadIndex].dead == true) return;

    // -------------------------
    // 1. 술래가 현재 칸의 도둑말을 잡아먹음
    // -------------------------

    // 술래는 잡아먹은 도둑말의 방향을 물려받음
    pt.dir = tmpChess[deadIndex].dir;

    // 도둑말 죽음 처리
    tmpChess[deadIndex].dead = true;

    // 점수 누적
    score += deadIndex;

    // -------------------------
    // 2. 도둑말 이동
    // -------------------------
    // 번호가 작은 도둑말부터 차례대로 이동
    for (int i = 1; i <= 16; i++)
    {
        CHESS horse = tmpChess[i];

        // 이미 죽은 말은 이동하지 않음
        if (horse.dead == true) continue;

        // 이동 가능한 방향이 나올 때까지 회전
        while (1)
        {
            int dir = horse.dir;
            int nr = horse.r + dr[dir];
            int nc = horse.c + dc[dir];

            // 이동 불가능한 경우
            // 1) 술래가 있는 칸
            // 2) 보드 밖
            if ((nr == pt.r && nc == pt.c) || (nr < 0 || nc < 0 || nr > 3 || nc > 3))
            {
                // 반시계 방향으로 45도 회전
                //    -, ↑, ↖, ←, ↙, ↓, ↘, →, ↗
                // => -, ↖, ←, ↙, ↓, ↘, →, ↗, ↑
                int changeDir[9] = { 0, 2, 3, 4, 5, 6, 7, 8, 1 };
                int nextDir = changeDir[dir];

                horse.dir = nextDir;
                tmpChess[i].dir = nextDir;

                continue;
            }
            else
            {
                // 이동 가능한 경우:
                // 대상 칸의 도둑말과 서로 위치를 교환
                int changeIndex = tmpMAP[nr][nc];
                int tmp;

                // 도둑말 배열에서 좌표 교환
                tmp = tmpChess[i].r;
                tmpChess[i].r = tmpChess[changeIndex].r;
                tmpChess[changeIndex].r = tmp;

                tmp = tmpChess[i].c;
                tmpChess[i].c = tmpChess[changeIndex].c;
                tmpChess[changeIndex].c = tmp;

                // MAP에서도 말 번호 위치 교환
                tmp = tmpMAP[nr][nc];
                tmpMAP[nr][nc] = tmpMAP[horse.r][horse.c];
                tmpMAP[horse.r][horse.c] = tmp;

                break;
            }
        }
    }

    // -------------------------
    // 3. 술래 이동
    // -------------------------
    // 술래는 현재 방향으로 1~3칸 이동 가능
    int sr = pt.r;
    int sc = pt.c;

    bool canMove = false;

    for (int i = 1; i <= 3; i++)
    {
        int nr = sr + dr[pt.dir] * i;
        int nc = sc + dc[pt.dir] * i;

        // 범위를 벗어나면 더 멀리도 당연히 못 가므로 종료
        if (nr < 0 || nc < 0 || nr > 3 || nc > 3)
            break;

        // 이 문제의 보드는 모든 칸에 말이 처음 하나씩 있고,
        // 이후에는 잡아먹힌 말의 칸을 지나갈 수는 있지만,
        // 실제로 DFS를 이어갈 수 있는 것은 "잡아먹을 말이 있는 칸"일 때뿐이다.
        if (tmpChess[tmpMAP[nr][nc]].dead == true)
            continue;

        canMove = true;

        CHESS nextTagger = pt;
        nextTagger.r = nr;
        nextTagger.c = nc;

        DFS(nextTagger, tmpMAP, tmpChess, score);
    }

    // 더 이상 술래가 이동할 수 없으면 현재 점수로 정답 갱신
    if (canMove == false)
    {
        if (maxAnswer < score) maxAnswer = score;
    }
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        maxAnswer = 0;

        // 술래는 (0,0)에서 시작
        tagger.r = 0;
        tagger.c = 0;
        tagger.dir = 0;
        tagger.dead = false;

        DFS(tagger, MAP, chess, 0);

        printf("%d\n", maxAnswer);
    }

    return 0;
}