/*
    [코드트리] 2019 하반기 오전 2번 - 이상한 윷놀이
    (백준 17837 "새로운 게임 2"와 같은 문제다  https://www.acmicpc.net/problem/17837)

    ■ 문제 요약
      N x N 체스판의 각 칸에 색이 칠해져 있다.

        0 : 흰색, 1 : 빨간색, 2 : 파란색

      말 K개가 놓여 있고 각각 방향을 하나씩 가진다 (1: 오른쪽, 2: 왼쪽, 3: 위, 4: 아래).
      한 턴에 1번 말부터 K번 말까지 순서대로 아래 규칙으로 움직인다.

        - 말이 움직일 때는 그 말 "위에 쌓인 말들"이 통째로 함께 움직인다.
        - 흰색 칸으로 가면 쌓인 순서를 그대로 유지한 채 올려놓는다.
        - 빨간색 칸으로 가면 쌓인 순서를 뒤집어서 올려놓는다.
        - 파란색 칸이거나 판을 벗어나면 방향을 반대로 바꾸고 한 번 더 시도한다.
          바꾼 방향도 파란색이면 그 말은 이번 턴에 움직이지 않는다.
        - 어느 칸이든 말이 4개 이상 쌓이면 그 즉시 게임이 끝난다.

      게임이 끝나는 턴 번호를 출력한다. 1000턴 안에 끝나지 않으면 -1.

    ■ 자료 구조 : 칸마다 스택, 말마다 자기 높이
      한 칸에 여러 말이 쌓이고, "특정 말부터 그 위쪽 전부"를 잘라 옮겨야 한다.
      그래서 칸을 배열 스택으로 관리하고, 말은 자기가 몇 번째 층인지 기억한다.

        board[r][c][i] : (r, c)에 쌓인 아래에서 i번째 말의 번호
        index[r][c]    : (r, c)에 쌓인 말의 개수 (스택의 top)
        horse[n].pos   : n번 말이 자기 칸에서 몇 번째 층인지

      이렇게 두면 "n번 말부터 위쪽 전부"가 board[r][c][ horse[n].pos ... index[r][c]-1 ]
      구간으로 딱 떨어진다. 옮긴 뒤 출발 칸은 index를 pos로 줄이기만 하면 정리가 끝난다.

    ■ 흰색과 빨간색의 차이는 훑는 방향뿐
      moveWhite는 front -> back-1 (아래에서 위) 순서로 옮기고,
      moveRed는 back-1 -> front (위에서 아래) 순서로 옮긴다.
      옮기는 쪽은 그냥 순서대로 쌓으므로, 읽는 순서만 뒤집으면 "순서 뒤집기"가 된다.

      옮길 때마다 그 말의 좌표와 pos를 새 값으로 갱신해 주는 것을 빼먹으면 안 된다.
      말의 pos가 틀어지면 다음 턴에 엉뚱한 구간을 잘라 옮기게 된다.

    ■ 경계 처리
      판 바깥을 전부 파란색으로 채워 둔다.
      그러면 "판을 벗어남"과 "파란 칸"을 따로 구분할 필요 없이 한 가지 규칙으로 처리된다.

    ■ 주의할 점
      방향을 반대로 바꾸는 것은 실패했을 때 "영구적으로" 바뀌는 것이다.
      다시 파란색이라 못 움직이더라도 바뀐 방향은 그대로 유지된다.
      그래서 horse[h].dir에 먼저 반영한 뒤 재시도한다.
*/

#include <stdio.h>

// 배열 최대 크기 (N 최대 12 + 여유)
#define MAX (12+5)

// 칸의 색
#define WHITE (0)   // 그대로 이동
#define RED   (1)   // 뒤집어서 이동
#define BLUE  (2)   // 방향 반전 후 재시도 (판 바깥도 이 값으로 채운다)

int T;
int N, K;           // N: 판 크기, K: 말 개수
int MAP[MAX][MAX];  // 칸의 색

// 말 하나
struct HORSE
{
    int r;    // 현재 행
    int c;    // 현재 열
    int dir;  // 방향 (1: 오른쪽, 2: 왼쪽, 3: 위, 4: 아래)
    int pos;  // 자기 칸의 스택에서 몇 번째 층인지 (0이 맨 아래)
};

HORSE horse[10 + 3]; // 1번 ~ K번 말
int hcnt;            // 마지막 말 번호 + 1 (1-based로 쓰기 위함)

int board[MAX][MAX][10 + 3];  // board[r][c][i] : 아래에서 i번째 말의 번호
int index[MAX][MAX];          // 그 칸에 쌓인 말의 개수

// 방향별 이동량. 방향 번호 1~4를 그대로 인덱스로 쓰려고 0번은 더미다.
int dr[] = { 0,  0, 0, -1, 1 };
int dc[] = { 0,  1,-1,  0, 0 };

void input()
{
    scanf("%d %d", &N, &K);

    // 판 바깥까지 전부 파란색으로 채운다.
    // "판을 벗어남"을 파란 칸과 똑같이 처리할 수 있어 경계 검사가 사라진다.
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = BLUE;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            index[r][c] = 0;

    hcnt = 1; // 말 번호는 1번부터
    for (int k = 0; k < K; k++)
    {
        int r, c, dir;
        scanf("%d %d %d", &r, &c, &dir);

        horse[hcnt].r = r;
        horse[hcnt].c = c;
        horse[hcnt].dir = dir;

        // 그 칸의 스택 맨 위에 올린다. 입력 순서대로 쌓인다.
        int& position = index[r][c];
        board[r][c][position] = hcnt;  // 칸은 말 번호를 기억하고
        horse[hcnt].pos = position;    // 말은 자기 층수를 기억한다
        position++;

        hcnt++;
    }
}

// 디버그용: 칸마다 쌓인 말 출력
void printBoard()
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            int idx = index[r][c];
            if (idx == 0) continue;

            printf("%d, %d: ", r, c);
            for (int i = 0; i < idx; i++)
                printf("%d ", board[r][c][i]);
            putchar('\n');
        }
    }
    putchar('\n');
}

// 흰색 칸으로 이동 : 쌓인 순서를 그대로 유지
//
// number번 말과 그 위에 쌓인 말들을 아래에서 위 순서로 옮긴다.
void moveWhite(int number)
{
    HORSE h = horse[number];
    int sr = h.r, sc = h.c, dir = h.dir;
    int nr = sr + dr[dir];
    int nc = sc + dc[dir];

    int front = h.pos;              // 옮길 구간의 시작 층
    int& back = index[sr][sc];      // 출발 칸의 스택 크기
    int& nextBack = index[nr][nc];  // 도착 칸의 스택 크기

    // 아래 -> 위 순서로 읽어 그대로 쌓으므로 순서가 유지된다
    for (int k = front; k < back; k++)
    {
        int selectedNumber = board[sr][sc][k];

        // 옮기는 말의 정보도 함께 갱신해야 다음 턴이 어긋나지 않는다
        horse[selectedNumber].r = nr;
        horse[selectedNumber].c = nc;
        horse[selectedNumber].pos = nextBack;

        board[nr][nc][nextBack++] = selectedNumber;
    }

    back = front; // 출발 칸에는 front 아래쪽만 남는다
}

// 빨간색 칸으로 이동 : 쌓인 순서를 뒤집어서
//
// moveWhite와 완전히 같고 읽는 방향만 반대다.
void moveRed(int number)
{
    HORSE h = horse[number];
    int sr = h.r, sc = h.c, dir = h.dir;
    int nr = sr + dr[dir];
    int nc = sc + dc[dir];

    int front = h.pos;
    int& back = index[sr][sc];
    int& nextBack = index[nr][nc];

    // 위 -> 아래 순서로 읽어 쌓으면 결과적으로 순서가 뒤집힌다
    for (int k = back - 1; k >= front; k--)
    {
        int selectedNumber = board[sr][sc][k];

        horse[selectedNumber].r = nr;
        horse[selectedNumber].c = nc;
        horse[selectedNumber].pos = nextBack;

        board[nr][nc][nextBack++] = selectedNumber;
    }

    back = front;
}

int simulation()
{
    // 방향 반전표 : 1(오른쪽) <-> 2(왼쪽), 3(위) <-> 4(아래)
    int changeDir[5] = { 0, 2, 1, 4, 3 };

    for (int i = 1; i < 1000; i++)
    {
        // 한 턴은 1번 말부터 K번 말까지 순서대로
        for (int h = 1; h < hcnt; h++)
        {
            int sr = horse[h].r;
            int sc = horse[h].c;
            int dir = horse[h].dir;

            int nr = sr + dr[dir];
            int nc = sc + dc[dir];

            if (MAP[nr][nc] == WHITE)
            {
                moveWhite(h);
            }
            else if (MAP[nr][nc] == RED)
            {
                moveRed(h);
            }
            else if (MAP[nr][nc] == BLUE)
            {
                // 파란 칸(또는 판 바깥)이면 방향을 반대로 "바꿔 둔 채" 다시 시도한다.
                // 재시도가 실패해도 바뀐 방향은 그대로 남는다.
                dir = changeDir[dir];
                horse[h].dir = dir;

                nr = sr + dr[dir];
                nc = sc + dc[dir];

                if (MAP[nr][nc] == WHITE)      moveWhite(h);
                else if (MAP[nr][nc] == RED)   moveRed(h);
                // 반대쪽도 파란색이면 이번 턴에는 움직이지 않는다
            }

            // 방금 말이 도착한 칸에 4개 이상 쌓였으면 그 턴에서 게임 끝
            if (index[nr][nc] >= 4) return i;
        }
    }

    return -1; // 1000턴 안에 끝나지 않았다
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    T = 1;
    for (int tc = 1; tc <= T; tc++)
    {
        input();
        printf("%d\n", simulation());
    }
    return 0;
}
