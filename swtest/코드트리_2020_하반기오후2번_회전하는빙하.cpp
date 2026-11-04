/*
    [코드트리] 2020 하반기 오후 2번 - 회전하는 빙하
    (백준 20058 "마법사 상어와 파이어스톰"과 같은 문제다  https://www.acmicpc.net/problem/20058)

    ■ 문제 요약
      2^N x 2^N 격자의 각 칸에 얼음이 쌓여 있다. 명령 Q개가 주어지고 각 명령은 값 L을 가진다.

      명령 하나는 아래 두 단계다.

        1) 격자를 2^L x 2^L 크기의 부분 격자로 나눠, 각 부분 격자를 시계 방향 90도 회전한다.
        2) 회전이 끝난 뒤, 얼음이 있는 칸 중 상하좌우로 인접한 얼음 칸이 3개 미만인 칸은
           얼음이 1 줄어든다. (모든 칸이 동시에 판정된다)

      명령을 모두 수행한 뒤 남은 얼음의 총합과 가장 큰 얼음 덩어리의 칸 수를 출력한다.

    ■ 구성
        rotate()         : 부분 격자 하나를 회전
        meltIce()        : 얼음 녹이기 (동시 판정)
        getIceCount()    : 남은 얼음 총합
        BFS/getGroupCount() : 가장 큰 덩어리 크기

    ■ 녹이기를 "동시에" 처리하기
      MAP을 보면서 바로 빼 버리면, 먼저 녹은 칸 때문에 뒤에 판정하는 칸의 이웃 수가 달라진다.
      그래서 ICE 배열에 "이번에 녹을 칸"을 먼저 표시해 두고, 전부 판정한 뒤 한꺼번에 뺀다.

      이웃을 셀 때는 얼음이 "있는" 칸이 아니라 "없는" 칸을 센다.

          count += !MAP[nr][nc];        // 값이 0이면 1을 더한다
          if (count >= 2) 녹는다;       // 없는 칸이 2개 이상 = 있는 칸이 2개 이하 = 3개 미만

      격자 바깥도 전역 배열이라 0이므로 자연히 "얼음 없음"으로 취급되어 경계 검사가 필요 없다.

    ■ [주의] rotate()가 실제 90도 회전과 다르다
      지금 구현은 부분 격자를 사분면 4개로 나눠 사분면끼리 자리만 돌린다.

          3사분면 -> 1사분면, 1 -> 2, 2 -> 4, 4 -> 3

      size가 2일 때는(사분면이 각각 한 칸) 이것이 정확히 90도 회전과 같다.
      하지만 size가 4 이상이면 사분면 "안쪽"이 함께 돌지 않아 결과가 달라진다.

        4 x 4 예시
             1  2  3  4          올바른 90도 회전        지금 코드의 결과
             5  6  7  8            13  9  5  1             9 10  1  2
             9 10 11 12            14 10  6  2            13 14  5  6
            13 14 15 16            15 11  7  3            11 12  3  4
                                   16 12  8  4            15 16  7  8

      L이 0이나 1인 입력에서는 문제가 드러나지 않지만, L이 2 이상이면 틀린 답이 나온다.
      고치려면 사분면 이동 대신 회전 공식을 그대로 쓰면 된다.
      (같은 폴더 15번 파일 BOJ 20327의 clockwisedivide와 동일한 형태다)

          for (int r = 0; r < size; r++)
              for (int c = 0; c < size; c++)
                  MAP[sr + r][sc + c] = tmpMAP[size - 1 - c][r];

      코드는 그대로 두었으니, 문제의 회전 정의를 확인한 뒤 위 형태로 바꿔 두는 것을 권한다.
*/

#include <stdio.h>               // scanf, printf, putchar 사용

#define MAX (64 + 5)             // 2^6 = 64 + 여유

int T;                           // 테스트 케이스 수

int N, Q, MAP_SIZE;              // 지수 N, 명령 수, 실제 한 변 길이

// MAP[r][c] : 그 칸의 얼음 양
int MAP[MAX][MAX];

// 회전할 부분 격자를 잠시 담아 두는 배열 (0-based로 쓴다)
int tmpMAP[MAX][MAX];

// L[q] : q번째 명령의 레벨
int L[1000 + 50];

bool visit[MAX][MAX];            // BFS 방문 표시

// 좌표 한 쌍
struct RC
{
    int r;                       // 행
    int c;                       // 열
};

typedef struct RC RC;            // struct RC를 RC로 줄여 쓰기

RC queue[MAX * MAX];             // BFS용 큐

// 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };      // 방향별 행 변화량
int dc[] = { 0, 1, 0,-1 };       // 방향별 열 변화량

/*
    입력
*/
void input()
{
    scanf("%d %d", &N, &Q);              // 지수 N, 명령 수 입력

    // 실제 격자 한 변의 길이는 2^N
    MAP_SIZE = 1 << N;

    for (int r = 1; r <= MAP_SIZE; r++)      // 모든 행
        for (int c = 1; c <= MAP_SIZE; c++)  // 모든 열
            scanf("%d", &MAP[r][c]);         // 얼음 양 입력

    for (int q = 0; q < Q; q++)          // 명령마다
        scanf("%d", &L[q]);              // 레벨 입력
}

/*
    디버그용 맵 출력
*/
void printMap()
{
    for (int r = 1; r <= MAP_SIZE; r++)      // 모든 행
    {
        for (int c = 1; c <= MAP_SIZE; c++)  // 모든 열
            printf("%d ", MAP[r][c]);        // 값 출력
        putchar('\n');                       // 행 끝 줄바꿈
    }
    putchar('\n');                           // 빈 줄
}

/*
    (sr, sc)에서 시작하는 size x size 부분 격자를 회전

    현재 구현은 사분면 4개를 통째로 돌리는 방식이다.
    size가 2일 때만 실제 90도 회전과 일치한다는 점에 주의
    (파일 상단 [주의] 항목 참고).
*/
void rotate(int sr, int sc, int size)
{
    int half = size / 2;                 // 사분면 한 변 길이

    // 회전 전 상태를 0-based로 떠 놓는다 (덮어쓰기 사고 방지)
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            tmpMAP[r][c] = MAP[sr + r][sc + c];

    // 3사분면(좌하) -> 1사분면(좌상)
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r][sc + c] = tmpMAP[r + half][c];

    // 1사분면(좌상) -> 2사분면(우상)
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r][sc + c + half] = tmpMAP[r][c];

    // 2사분면(우상) -> 4사분면(우하)
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r + half][sc + c + half] = tmpMAP[r][c + half];

    // 4사분면(우하) -> 3사분면(좌하)
    for (int r = 0; r < half; r++)
        for (int c = 0; c < half; c++)
            MAP[sr + r + half][sc + c] = tmpMAP[r + half][c + half];
}

/*
    얼음 녹이기 (모든 칸을 동시에 판정)
*/
void meltIce()
{
    int ICE[MAX][MAX] = { 0 };   // 이번에 녹을 칸 표시

    for (int r = 1; r <= MAP_SIZE; r++)      // 모든 행
    {
        for (int c = 1; c <= MAP_SIZE; c++)  // 모든 열
        {
            if (MAP[r][c] == 0) continue;   // 얼음이 없으면 녹을 것도 없다

            int count = 0;               // 얼음 없는 이웃 수

            for (int i = 0; i < 4; i++)  // 4방향
            {
                int nr = r + dr[i];      // 이웃 행
                int nc = c + dc[i];      // 이웃 열

                // 얼음이 "없는" 이웃을 센다.
                // 격자 바깥은 전역 배열이라 0이므로 자동으로 없는 칸이 된다.
                count += !MAP[nr][nc];
            }

            // 없는 이웃이 2개 이상 = 있는 이웃이 2개 이하 = 3개 미만 -> 녹는다
            if (count >= 2)
                ICE[r][c] = 1;
        }
    }

    // 판정이 모두 끝난 뒤 한꺼번에 반영해야 "동시에" 녹는 것이 된다
    for (int r = 1; r <= MAP_SIZE; r++)
        for (int c = 1; c <= MAP_SIZE; c++)
            MAP[r][c] -= ICE[r][c];      // 녹을 칸은 1 감소
}

/*
    Q개의 명령을 순서대로 수행
*/
void simulate()
{
    for (int q = 0; q < Q; q++)          // 명령마다
    {
        int level = L[q];                // 이번 레벨
        int divide = 1 << level;   // 부분 격자 한 변의 길이 2^L

        // 부분 격자의 왼쪽 위 좌표만 골라 순회한다
        for (int r = 1; r <= MAP_SIZE; r += divide)
            for (int c = 1; c <= MAP_SIZE; c += divide)
                rotate(r, c, divide);    // 부분 격자 회전

        meltIce();                       // 얼음 녹이기
    }
}

/*
    남아 있는 얼음 총합
*/
int getIceCount()
{
    int sum = 0;                         // 얼음 합

    for (int r = 1; r <= MAP_SIZE; r++)      // 모든 행
        for (int c = 1; c <= MAP_SIZE; c++)  // 모든 열
            sum += MAP[r][c];                // 더한다

    return sum;                          // 합 반환
}

/*
    (r, c)가 속한 얼음 덩어리의 칸 수

    얼음이 1 이상인 칸끼리 상하좌우로 이어지면 한 덩어리다.
    큐에 넣을 때 방문 표시를 하므로 wp가 곧 덩어리의 크기가 된다.
*/
int BFS(int r, int c)
{
    int rp, wp;                          // rp: 꺼낼 위치, wp: 넣을 위치

    rp = wp = 0;                         // 빈 큐

    queue[wp].r = r;                     // 시작 행
    queue[wp++].c = c;                   // 시작 열, 삽입 위치 증가

    visit[r][c] = true;                  // 시작 칸 방문 표시

    while (rp < wp)                      // 처리할 칸이 남아 있는 동안
    {
        RC out = queue[rp++];            // 한 칸 꺼낸다

        for (int i = 0; i < 4; i++)      // 4방향
        {
            int nr = out.r + dr[i];      // 이웃 행
            int nc = out.c + dc[i];      // 이웃 열

            // 얼음이 없거나(격자 밖 포함) 이미 센 칸이면 건너뛴다
            if (MAP[nr][nc] == 0 || visit[nr][nc] == true) continue;

            queue[wp].r = nr;            // 큐에 넣는다
            queue[wp++].c = nc;

            visit[nr][nc] = true;        // 방문 표시
        }
    }

    return wp;                           // 덩어리 크기
}

/*
    가장 큰 얼음 덩어리의 크기
*/
int getGroupCount()
{
    for (int r = 1; r <= MAP_SIZE; r++)      // 방문 기록 초기화
        for (int c = 1; c <= MAP_SIZE; c++)
            visit[r][c] = false;

    int max = 0;                         // 가장 큰 덩어리

    for (int r = 1; r <= MAP_SIZE; r++)      // 모든 행
    {
        for (int c = 1; c <= MAP_SIZE; c++)  // 모든 열
        {
            if (MAP[r][c] == 0 || visit[r][c] == true) continue;   // 얼음 없거나 이미 센 칸

            int tmp = BFS(r, c);         // 덩어리 크기

            if (max < tmp) max = tmp;    // 최댓값 갱신
        }
    }

    return max;                          // 최댓값 반환
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
    {
        input();                         // 입력

        simulate();                      // 명령 수행

        int iceCount = getIceCount();    // 남은 얼음 합
        int groupCount = getGroupCount();   // 가장 큰 덩어리

        printf("%d\n%d\n", iceCount, groupCount);   // 결과 출력
    }

    return 0;                            // 프로그램 정상 종료
}
