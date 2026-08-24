// 코드트리 2020 하반기 오후 2번 - 회전하는 빙하
//
// 문제 요약
// 2^N x 2^N 크기의 격자에 얼음 양이 주어진다.
// Q번의 명령이 주어지고, 각 명령은 L 값을 가진다.
//
// 매 명령마다 진행 규칙
// 1. 격자를 2^L x 2^L 크기의 부분 격자로 나눈다.
// 2. 각 부분 격자를 시계 방향으로 90도 회전한다.
// 3. 회전이 끝난 뒤, 인접한 얼음 칸이 3개 미만인 얼음 칸은 얼음이 1 감소한다.
//
// 모든 명령이 끝난 뒤
// 1. 남아 있는 얼음의 총합
// 2. 가장 큰 얼음 덩어리의 크기
// 를 출력한다.
//
// 핵심 아이디어
// - rotate()    : 부분 격자를 시계 방향 90도 회전
// - meltIce()   : 주변 얼음 칸 개수를 확인해서 녹일 칸 표시 후 한 번에 감소
// - BFS()       : 연결된 얼음 덩어리 크기 계산
// - simulate()  : Q번의 회전과 녹이기를 반복
//
// 즉,
// "부분 격자 회전 -> 얼음 녹이기 -> 최종 합/최대 덩어리 계산"
// 을 수행하는 시뮬레이션 + BFS 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (64 + 5)

int T;

int N, Q, MAP_SIZE;

// MAP[r][c] = 현재 얼음 양
int MAP[MAX][MAX];

// 회전할 때 임시로 부분 격자를 복사하는 배열
int tmpMAP[MAX][MAX];

// L[q] = q번째 명령의 회전 레벨
int L[1000 + 50];

// BFS 방문 배열
bool visit[MAX][MAX];

// 좌표 구조체
struct RC
{
    int r;
    int c;
};

typedef struct RC RC;

// BFS 큐
RC queue[MAX * MAX];

// 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

/*
    입력 함수

    N : 격자 크기 지수
    Q : 명령 개수

    실제 격자 크기는 2^N 이므로,
    MAP_SIZE = 1 << N 으로 계산한다.
*/
void input()
{
    scanf("%d %d", &N, &Q);

    MAP_SIZE = 1 << N;

    // 얼음 양 입력
    for (int r = 1; r <= MAP_SIZE; r++)
        for (int c = 1; c <= MAP_SIZE; c++)
            scanf("%d", &MAP[r][c]);

    // 회전 명령 입력
    for (int q = 0; q < Q; q++)
        scanf("%d", &L[q]);
}

/*
    디버그용 맵 출력 함수
*/
void printMap()
{
    for (int r = 1; r <= MAP_SIZE; r++)
    {
        for (int c = 1; c <= MAP_SIZE; c++)
            printf("%d ", MAP[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

/*
    부분 격자 회전 함수

    시작 좌표 (sr, sc)에서 size x size 크기의 부분 격자를
    시계 방향으로 90도 회전한다.

    원본 코드에서는 부분 격자를 4개의 사분면으로 나누고,
    각 사분면을 이동시키는 방식으로 회전한다.

    size = 2^L
    half = size / 2

    사분면 이동
    - 3번 사분면 -> 1번 사분면
    - 1번 사분면 -> 2번 사분면
    - 2번 사분면 -> 4번 사분면
    - 4번 사분면 -> 3번 사분면
*/
void rotate(int sr, int sc, int size)
{
    int half = size / 2;

    // 회전 전 부분 격자를 tmpMAP에 복사
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            tmpMAP[r][c] = MAP[sr + r][sc + c];

    // 3 -> 1
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r][sc + c] = tmpMAP[r + half][c];

    // 1 -> 2
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r][sc + c + half] = tmpMAP[r][c];

    // 2 -> 4
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r + half][sc + c + half] = tmpMAP[r][c + half];

    // 4 -> 3
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r + half][sc + c] = tmpMAP[r + half][c + half];
}

/*
    얼음 녹이기 함수

    모든 칸에 대해 동시에 판단한다.

    규칙
    - 현재 칸에 얼음이 있고,
    - 상하좌우 4칸 중 얼음이 있는 칸이 3개 미만이면
      현재 칸의 얼음 양을 1 감소시킨다.

    주의
    - 녹는 여부는 현재 상태 기준으로 동시에 결정되어야 한다.
    - 그래서 ICE 배열에 먼저 표시한 뒤,
      마지막에 한 번에 감소시킨다.
*/
void meltIce()
{
    int ICE[MAX][MAX] = { 0 };

    for (int r = 1; r <= MAP_SIZE; r++)
    {
        for (int c = 1; c <= MAP_SIZE; c++)
        {
            // 얼음이 없는 칸은 무시
            if (MAP[r][c] == 0) continue;

            int count = 0;

            for (int i = 0; i < 4; i++)
            {
                int nr = r + dr[i];
                int nc = c + dc[i];

                // MAP[nr][nc]가 0이면 얼음이 없는 칸
                // 격자 밖도 전역 배열 초기값 0이므로 얼음 없는 칸처럼 처리됨
                count += !MAP[nr][nc];
            }

            // 얼음이 없는 인접 칸이 2개 이상이면,
            // 얼음이 있는 인접 칸은 2개 이하라는 뜻
            // 즉, 인접 얼음 칸이 3개 미만이므로 녹는다.
            if (count >= 2)
                ICE[r][c] = 1;
        }
    }

    // 녹을 칸들을 한 번에 반영
    for (int r = 1; r <= MAP_SIZE; r++)
        for (int c = 1; c <= MAP_SIZE; c++)
            MAP[r][c] -= ICE[r][c];
}

/*
    전체 시뮬레이션 함수

    Q개의 명령을 순서대로 수행한다.

    각 명령마다
    1. level = L[q]
    2. divide = 2^level 크기의 부분 격자로 나눔
    3. 각 부분 격자를 회전
    4. 얼음 녹이기 수행
*/
void simulate()
{
    for (int q = 0; q < Q; q++)
    {
        int level = L[q];
        int divide = 1 << level;

        // 모든 divide x divide 부분 격자 회전
        for (int r = 1; r <= MAP_SIZE; r += divide)
            for (int c = 1; c <= MAP_SIZE; c += divide)
                rotate(r, c, divide);

        // 회전 후 얼음 녹이기
        meltIce();
    }
}

/*
    남아 있는 얼음 총합 계산 함수
*/
int getIceCount()
{
    int sum = 0;

    for (int r = 1; r <= MAP_SIZE; r++)
        for (int c = 1; c <= MAP_SIZE; c++)
            sum += MAP[r][c];

    return sum;
}

/*
    BFS 함수

    시작 칸 (r, c)에서 연결된 얼음 덩어리의 크기를 구한다.

    연결 기준
    - 상하좌우로 인접
    - 얼음 양이 1 이상인 칸만 연결됨

    반환값
    - 현재 덩어리의 칸 개수
*/
int BFS(int r, int c)
{
    int rp, wp;

    rp = wp = 0;

    queue[wp].r = r;
    queue[wp++].c = c;

    visit[r][c] = true;

    while (rp < wp)
    {
        RC out = queue[rp++];

        for (int i = 0; i < 4; i++)
        {
            int nr = out.r + dr[i];
            int nc = out.c + dc[i];

            // 얼음이 없거나 이미 방문한 칸이면 제외
            if (MAP[nr][nc] == 0 || visit[nr][nc] == true) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = true;
        }
    }

    return wp;
}

/*
    가장 큰 얼음 덩어리 크기 계산 함수

    모든 칸을 확인하면서,
    방문하지 않은 얼음 칸을 발견하면 BFS를 수행한다.
*/
int getGroupCount()
{
    // 방문 배열 초기화
    for (int r = 1; r <= MAP_SIZE; r++)
        for (int c = 1; c <= MAP_SIZE; c++)
            visit[r][c] = false;

    int max = 0;

    for (int r = 1; r <= MAP_SIZE; r++)
    {
        for (int c = 1; c <= MAP_SIZE; c++)
        {
            // 얼음이 없거나 이미 방문한 칸은 제외
            if (MAP[r][c] == 0 || visit[r][c] == true) continue;

            int tmp = BFS(r, c);

            if (max < tmp) max = tmp;
        }
    }

    return max;
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        simulate();

        int iceCount = getIceCount();
        int groupCount = getGroupCount();

        printf("%d\n%d\n", iceCount, groupCount);
    }

    return 0;
}