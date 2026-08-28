/*
    [코드트리] 2020 상반기 오전 2번 - 술래잡기 체스
    (백준 19236 "청소년 상어"와 같은 문제다  https://www.acmicpc.net/problem/19236)

    ■ 문제 요약
      4 x 4 판의 각 칸에 1번부터 16번까지의 말이 하나씩 있고, 말마다 여덟 방향 중 하나를 본다.
      술래는 (0, 0)에서 시작해 그 칸의 말을 먼저 잡아먹는다.

      한 턴은 아래 순서로 진행된다.

        1) 술래가 지금 칸의 말을 먹고, 그 말이 보던 방향을 물려받는다.
        2) 살아 있는 말들이 번호가 작은 것부터 차례로 한 칸 움직인다.
           - 자기 방향으로 가려 하는데 판 밖이거나 술래가 있는 칸이면
             갈 수 있는 방향이 나올 때까지 반시계로 45도씩 돌린다.
           - 이동은 "빈칸으로 가는 것"이 아니라 그 칸에 있는 말과 자리를 맞바꾸는 것이다.
        3) 술래가 자기 방향으로 1칸, 2칸, 3칸 중 하나만큼 이동한다.
           단 말이 있는 칸으로만 갈 수 있다.
        4) 술래가 더 이상 갈 곳이 없으면 게임이 끝난다.

      먹은 말 번호의 합의 최댓값을 출력한다.

    ■ 풀이 방침 : 상태를 통째로 들고 다니는 백트래킹
      3단계에서 술래의 선택지가 최대 3개뿐이라 모든 경우를 재귀로 펼칠 수 있다.
      다만 2단계에서 판 전체가 뒤섞이므로, 되돌리기(undo)를 만들기가 매우 번거롭다.

      그래서 이 코드는 상태를 되돌리는 대신 매 호출마다 통째로 복사한다.

          DFS(술래 상태, 판, 말 배열, 점수)

      판은 4x4, 말은 16개뿐이라 복사 비용이 거의 없고,
      덕분에 "이 가지에서 무슨 일이 일어나든 부모 상태는 안전하다"가 보장된다.
      순열/조합 문제의 visit 되돌리기보다 이쪽이 훨씬 단순하고 실수도 적다.

    ■ 빈칸을 따로 표시하지 않는다
      먹힌 말은 chess[i].dead = true 로만 표시하고 MAP에서는 지우지 않는다.
      그래서 "빈칸"은 곧 "죽은 말의 번호가 적힌 칸"이다.

        - 말끼리 자리를 바꿀 때 상대가 죽은 말이어도 같은 코드로 처리된다.
          죽은 말의 좌표도 같이 갱신되므로 빈칸 표시가 정확히 따라 움직인다.
        - 술래가 이동할 수 있는지 판정할 때는 그 칸의 말이 죽었는지만 보면 된다.

    ■ 방향 번호
        1 = ↑, 2 = ↖, 3 = ←, 4 = ↙, 5 = ↓, 6 = ↘, 7 = →, 8 = ↗   (0은 더미)

      번호가 반시계 순서로 붙어 있어서 "반시계 45도 회전"이 +1 (8 다음은 1)이 된다.
      changeDir 표가 바로 그 회전이다.

    ■ 무한 회전 걱정
      말이 여덟 방향을 다 돌아도 못 움직이면 while(1)에 갇힌다.
      하지만 이동을 막는 것은 판 밖과 술래 칸뿐이고(다른 말은 자리를 바꿔 주므로 막지 않는다),
      4x4에서 가장 갇힌 구석 칸조차 판 안 이웃이 3칸이라 술래가 하나를 막아도 두 칸이 남는다.
      그래서 실제로는 갇히지 않는다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (4 + 2)
#define MAX_H (16 + 5)   // 말 번호 1 ~ 16

// 말 하나의 상태 (술래도 같은 구조체를 쓴다)
struct CHESS
{
    int r;       // 행
    int c;       // 열
    int dir;     // 보고 있는 방향
    bool dead;   // 먹혔는지
};

typedef struct CHESS CHESS;

int T;

// MAP[r][c] : 그 칸에 있는 말의 번호 (죽은 말의 번호면 그 칸은 빈칸이라는 뜻)
int MAP[MAX][MAX];

// chess[i] : i번 말의 상태
CHESS chess[MAX_H];

// 술래
CHESS tagger;

int maxAnswer;

// 방향 배열 (인덱스 0은 더미)
//   -, ↑, ↖, ←, ↙, ↓, ↘, →, ↗
int dr[] = { 0,-1,-1,0,1,1,1,0,-1 };
int dc[] = { 0, 0,-1,-1,-1,0,1,1,1 };

/*
    입력

    칸마다 (말 번호, 방향)이 주어진다.
    판에는 번호를, 말 배열에는 위치와 방향을 저장한다.
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
    디버그용: 점수, 술래, 살아 있는 말, 판 상태를 한꺼번에 출력
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
    DFS : 술래가 지금 칸의 말을 먹은 뒤 한 턴을 진행하고, 갈 수 있는 곳마다 재귀한다

      prevTagger / prevMap / prevChess : 부모 상태 (읽기만 한다)
      score                            : 여기까지 먹은 번호의 합
*/
void DFS(CHESS prevTagger, int prevMap[MAX][MAX], CHESS prevChess[MAX_H], int score)
{
    int tmpMAP[MAX][MAX] = { 0 };
    CHESS tmpChess[MAX_H] = { 0 };
    CHESS pt = prevTagger;

    // 부모 상태를 통째로 복사한다.
    // 이 가지에서 판을 아무리 뒤섞어도 부모에게는 영향이 없다 (되돌리기 불필요).
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            tmpMAP[r][c] = prevMap[r][c];

    for (int i = 1; i <= 16; i++)
        tmpChess[i] = prevChess[i];

    int deadIndex = tmpMAP[pt.r][pt.c];

    // 술래는 말이 있는 칸으로만 오므로 정상 흐름에서는 걸리지 않는 안전장치
    if (tmpChess[deadIndex].dead == true) return;

    // -------------------------
    // 1) 술래가 지금 칸의 말을 먹는다
    // -------------------------
    pt.dir = tmpChess[deadIndex].dir;   // 먹은 말의 방향을 물려받는다
    tmpChess[deadIndex].dead = true;    // 그 칸은 이제 빈칸 취급이 된다
    score += deadIndex;

    // -------------------------
    // 2) 살아 있는 말들이 번호 순서대로 이동
    // -------------------------
    for (int i = 1; i <= 16; i++)
    {
        CHESS horse = tmpChess[i];

        if (horse.dead == true) continue;

        // 갈 수 있는 방향이 나올 때까지 반시계로 45도씩 돌린다
        while (1)
        {
            int dir = horse.dir;
            int nr = horse.r + dr[dir];
            int nc = horse.c + dc[dir];

            // 막히는 경우는 두 가지뿐이다 : 술래가 있는 칸, 판 밖
            // (다른 말이 있는 칸은 자리를 바꿔 주므로 막히지 않는다)
            if ((nr == pt.r && nc == pt.c) || (nr < 0 || nc < 0 || nr > 3 || nc > 3))
            {
                // 방향 번호가 반시계 순서라 +1 이 곧 45도 반시계 회전이다 (8 다음은 1)
                int changeDir[9] = { 0, 2, 3, 4, 5, 6, 7, 8, 1 };
                int nextDir = changeDir[dir];

                horse.dir = nextDir;
                tmpChess[i].dir = nextDir;   // 회전은 실제 상태에도 남는다

                continue;
            }
            else
            {
                // 대상 칸의 말과 자리를 맞바꾼다.
                // 상대가 죽은 말(= 빈칸)이어도 같은 코드로 처리되고,
                // 죽은 말의 좌표도 함께 갱신되어 빈칸 위치가 정확히 따라 움직인다.
                int changeIndex = tmpMAP[nr][nc];
                int tmp;

                // 말 배열에서 좌표 교환
                tmp = tmpChess[i].r;
                tmpChess[i].r = tmpChess[changeIndex].r;
                tmpChess[changeIndex].r = tmp;

                tmp = tmpChess[i].c;
                tmpChess[i].c = tmpChess[changeIndex].c;
                tmpChess[changeIndex].c = tmp;

                // 판에서도 번호 교환 (horse는 이동 전 좌표를 들고 있다)
                tmp = tmpMAP[nr][nc];
                tmpMAP[nr][nc] = tmpMAP[horse.r][horse.c];
                tmpMAP[horse.r][horse.c] = tmp;

                break;
            }
        }
    }

    // -------------------------
    // 3) 술래 이동 : 현재 방향으로 1 ~ 3칸
    // -------------------------
    int sr = pt.r;
    int sc = pt.c;

    bool canMove = false;

    for (int i = 1; i <= 3; i++)
    {
        int nr = sr + dr[pt.dir] * i;
        int nc = sc + dc[pt.dir] * i;

        // 판 밖으로 나가면 더 먼 거리도 볼 필요가 없다
        if (nr < 0 || nc < 0 || nr > 3 || nc > 3)
            break;

        // 빈칸(죽은 말의 번호가 남아 있는 칸)으로는 갈 수 없다.
        // 단 그 칸을 지나쳐 더 멀리는 갈 수 있으므로 break가 아니라 continue다.
        if (tmpChess[tmpMAP[nr][nc]].dead == true)
            continue;

        canMove = true;

        CHESS nextTagger = pt;
        nextTagger.r = nr;
        nextTagger.c = nc;

        DFS(nextTagger, tmpMAP, tmpChess, score);
    }

    // 4) 갈 곳이 없으면 여기서 게임이 끝난다. 그때의 점수로 정답을 갱신한다.
    if (canMove == false)
    {
        if (maxAnswer < score) maxAnswer = score;
    }
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        maxAnswer = 0;

        // 술래는 (0, 0)에서 시작한다. 방향은 첫 말을 먹으면서 정해지므로 아무 값이나 무방.
        tagger.r = 0;
        tagger.c = 0;
        tagger.dir = 0;
        tagger.dead = false;

        DFS(tagger, MAP, chess, 0);

        printf("%d\n", maxAnswer);
    }

    return 0;
}
