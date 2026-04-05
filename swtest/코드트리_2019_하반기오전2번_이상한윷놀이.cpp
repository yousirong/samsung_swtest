// 코드트리 2019 하반기 오전 2번 - 이상한 윷놀이
//
// 문제 요약
// N x N 체스판 위에 K개의 말이 있다.
// 각 말은 위치(r, c), 방향(dir)을 가진다.
//
// 칸의 색깔
// 0 : 흰색
// 1 : 빨간색
// 2 : 파란색
//
// 규칙
// 1. 1번 말부터 K번 말까지 순서대로 이동한다.
// 2. 어떤 말이 이동할 때, 그 말 위에 쌓여 있는 말들도 함께 이동한다.
// 3. 흰색 칸으로 이동하면 쌓인 순서를 그대로 유지한다.
// 4. 빨간색 칸으로 이동하면 쌓인 순서를 뒤집어서 이동한다.
// 5. 파란색 칸 또는 범위 밖이면 방향을 반대로 바꾸고,
//    한 번 더 이동을 시도한다.
//    그래도 파란색이면 이동하지 않는다.
// 6. 어떤 칸에 말이 4개 이상 쌓이면 게임 종료
//
// 목표
// 게임이 몇 번째 턴에 끝나는지 구한다.
// 1000턴 안에 안 끝나면 -1 출력
//
// 핵심 아이디어
// - board[r][c][] : 해당 칸에 쌓인 말 번호들을 아래에서 위 순서대로 저장
// - index[r][c]   : 그 칸에 현재 몇 개의 말이 쌓였는지
// - horse[i].pos  : 그 말이 자기 칸 스택에서 몇 번째 위치인지
//
// 즉, 각 칸을 "스택(배열)"처럼 관리하면서
// 말 번호를 직접 옮기는 방식으로 구현한 코드이다.
#include <stdio.h> 

// 배열 최대 크기 (N 최대 12 + 여유 5)
#define MAX (12+5) 

// 칸의 색상 정의
#define WHITE (0)   // 흰색: 그냥 이동
#define RED   (1)   // 빨간색: 뒤집어서 이동
#define BLUE  (2)   // 파란색: 방향 반전 후 이동 (그래도 BLUE면 정지)

int T;       // 테스트케이스 수 (현재 고정 1)
int N, K;    // N: 보드 크기, K: 말 수
int MAP[MAX][MAX];  // 각 칸의 색상 저장

// 말(Horse) 구조체
struct HORSE
{
    int r;    // 현재 행
    int c;    // 현재 열
    int dir;  // 현재 방향 (1:오른쪽, 2:왼쪽, 3:위, 4:아래)
    int pos;  // 해당 칸의 스택에서 자신의 위치(인덱스)
};

HORSE horse[10 + 3]; // 말 배열 (1번~K번)
int hcnt;            // 말의 총 개수 + 1 (1-based)

// board[r][c][i]: (r,c) 칸에 쌓인 i번째 말의 번호
int board[MAX][MAX][10 + 3];
// index[r][c]: (r,c) 칸에 현재 쌓인 말의 수 (스택의 top 인덱스)
int index[MAX][MAX];

// 방향별 행/열 이동량 (0:없음, 1:오른쪽, 2:왼쪽, 3:위, 4:아래)
int dr[] = { 0,  0, 0, -1, 1 };
int dc[] = { 0,  1,-1,  0, 0 };

void input()
{
    scanf("%d %d", &N, &K);

    // 보드 전체를 BLUE로 초기화 (경계 처리: 벽 = BLUE)
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = BLUE;

    // 실제 N×N 칸의 색상 입력
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            scanf("%d", &MAP[r][c]);

    // 각 칸의 스택 카운트 초기화
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            index[r][c] = 0;

    hcnt = 1; // 말 번호는 1번부터 시작
    for (int k = 0; k < K; k++)
    {
        int r, c, dir;
        scanf("%d %d %d", &r, &c, &dir);

        horse[hcnt].r = r;
        horse[hcnt].c = c;
        horse[hcnt].dir = dir;

        // 해당 칸 스택에 이 말을 추가
        int& position = index[r][c];
        board[r][c][position] = hcnt;  // 스택에 말 번호 저장
        horse[hcnt].pos = position;    // 말의 스택 내 위치 기록
        position++;                    // 스택 크기 증가

        hcnt++;
    }
}

// 디버그용: 각 칸에 쌓인 말 출력
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

// 흰색 칸 이동: 스택 순서 그대로 다음 칸에 쌓기
// number번 말부터 그 위에 쌓인 말들을 모두 이동
void moveWhite(int number)
{
    HORSE h = horse[number];
    int sr = h.r, sc = h.c, dir = h.dir;
    int nr = sr + dr[dir];
    int nc = sc + dc[dir];

    int front = h.pos;         // 이동 시작 인덱스 (number번 말의 위치)
    int& back = index[sr][sc]; // 출발 칸의 스택 크기 (참조)
    int& nextBack = index[nr][nc]; // 도착 칸의 스택 크기 (참조)

    // front부터 back-1까지 (아래→위 순서) 그대로 다음 칸에 추가
    for (int k = front; k < back; k++)
    {
        int selectedNumber = board[sr][sc][k];

        horse[selectedNumber].r = nr;
        horse[selectedNumber].c = nc;
        horse[selectedNumber].pos = nextBack;

        board[nr][nc][nextBack++] = selectedNumber;
    }
    back = front; // 출발 칸의 스택 크기를 줄임 (front 이전까지만 남김)
}

// 빨간색 칸 이동: 스택 순서를 뒤집어서 다음 칸에 쌓기
// number번 말부터 위에 쌓인 말들을 역순으로 이동
void moveRed(int number)
{
    HORSE h = horse[number];
    int sr = h.r, sc = h.c, dir = h.dir;
    int nr = sr + dr[dir];
    int nc = sc + dc[dir];

    int front = h.pos;         // 이동 시작 인덱스
    int& back = index[sr][sc]; // 출발 칸 스택 크기 (참조)
    int& nextBack = index[nr][nc]; // 도착 칸 스택 크기 (참조)

    // back-1부터 front까지 (위→아래 역순) 다음 칸에 추가
    for (int k = back - 1; k >= front; k--)
    {
        int selectedNumber = board[sr][sc][k];

        horse[selectedNumber].r = nr;
        horse[selectedNumber].c = nc;
        horse[selectedNumber].pos = nextBack;

        board[nr][nc][nextBack++] = selectedNumber;
    }
    back = front; // 출발 칸 스택 줄임
}

int simulation()
{
    // 방향 반전 테이블: 1↔2 (좌우), 3↔4 (상하)
    int changeDir[5] = { 0, 2, 1, 4, 3 };

    // 최대 1000턴 반복
    for (int i = 1; i < 1000; i++)
    {
        // 1번 말부터 K번 말 순서대로 이동
        for (int h = 1; h < hcnt; h++)
        {
            int sr = horse[h].r;
            int sc = horse[h].c;
            int dir = horse[h].dir;

            int nr = sr + dr[dir];
            int nc = sc + dc[dir];

            if (MAP[nr][nc] == WHITE)
            {
                // 흰색: 순서 그대로 이동
                moveWhite(h);
            }
            else if (MAP[nr][nc] == RED)
            {
                // 빨간색: 역순으로 이동
                moveRed(h);
            }
            else if (MAP[nr][nc] == BLUE)
            {
                // 파란색(또는 경계): 방향 반전 후 재시도
                dir = changeDir[dir];
                horse[h].dir = dir;

                nr = sr + dr[dir];
                nc = sc + dc[dir];

                if (MAP[nr][nc] == WHITE)      moveWhite(h);
                else if (MAP[nr][nc] == RED)   moveRed(h);
                // 반전 후에도 BLUE면 이동하지 않음
            }

            // 이동 후 해당 칸에 4개 이상 쌓이면 현재 턴 반환
            if (index[nr][nc] >= 4) return i;
        }
    }

    return -1; // 1000턴 내에 조건 미달 시 -1 반환
}

int main()
{
    T = 1; // 테스트케이스 고정 (문제에 따라 scanf로 변경 가능)
    for (int tc = 1; tc <= T; tc++)
    {
        input();
        printf("%d\n", simulation());
    }
    return 0;
}