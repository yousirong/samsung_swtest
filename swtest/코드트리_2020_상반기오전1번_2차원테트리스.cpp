// 코드트리 2020 상반기 오전 1번 - 2차원 테트리스
//
// 문제 요약
// 블록 K개가 차례대로 주어진다.
// 각 블록은 타입(type)과 기준 좌표(r, c)를 가진다.
//
// 블록 타입
// 1 : 1x1 블록
// 2 : 1x2 가로 블록
// 3 : 2x1 세로 블록
//
// 하나의 블록이 주어질 때마다
// - 빨간 보드(오른쪽 방향으로 떨어지는 보드)
// - 노란 보드(아래 방향으로 떨어지는 보드)
// 에 각각 동시에 블록을 놓는다.
//
// 규칙
// 1. 블록은 해당 보드의 진행 방향 끝까지 이동한다.
// 2. 진한 영역(점수 영역)에서 한 줄이 가득 차면 삭제하고 점수를 얻는다.
// 3. 연한 영역에 블록이 존재하면, 그 수만큼 반대쪽 끝줄을 밀어낸다.
// 4. 모든 블록을 처리한 뒤,
//    최종 점수와 남아 있는 블록 수를 출력한다.
//
// 핵심 아이디어
// - 하나의 10x10 배열 MAP 안에
//   왼쪽 아래 노란 보드와 오른쪽 위 빨간 보드를 함께 표현
// - moveRed()    : 빨간 보드에 블록 배치
// - moveYellow() : 노란 보드에 블록 배치
// - getScoreRed(), getScoreYellow() : 가득 찬 줄 삭제 및 점수 획득
// - checkRed(), checkYellow()       : 연한 칸 검사
//
// 즉,
// "블록 배치 -> 점수 줄 삭제 -> 연한 칸 처리"
// 를 블록마다 반복하는 시뮬레이션 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (10 + 3)
#define MAX_K (10000 + 500)

// 각 보드의 유효 영역
// START~END 구간을 사용해서 빨간/노란 보드의 진한 칸과 연한 칸을 표현
#define START (4)
#define END (9)

// 벽 표시
#define WALL (-1)

int T;

int K;                  // 블록 개수
int MAP[MAX][MAX];      // 전체 보드 배열

// 입력 블록 정보
int TYPE[MAX_K];
int R[MAX_K];
int C[MAX_K];

/*
    입력 함수

    MAP 전체를 0으로 초기화한 뒤,
    경계 처리를 위해 일부 위치를 WALL로 설정한다.

    이후 K개의 블록 정보를 입력받는다.
    TYPE[k] : 블록 타입
    R[k], C[k] : 기준 좌표
*/
void input()
{
    for (int r = 0; r < MAX; r++)
        for (int c = 0; c < MAX; c++)
            MAP[r][c] = 0;

    // 빨간 보드 오른쪽 경계
    MAP[0][10] = MAP[1][10] = MAP[2][10] = MAP[3][10] = WALL;

    // 노란 보드 아래쪽 경계
    MAP[10][0] = MAP[10][1] = MAP[10][2] = MAP[10][3] = WALL;

    scanf("%d", &K);

    for (int k = 0; k < K; k++)
        scanf("%d %d %d", &TYPE[k], &R[k], &C[k]);
}

/*
    디버그용 출력 함수
    현재 MAP 전체 상태를 확인할 때 사용
*/
void printMap()
{
    for (int r = 0; r < 11; r++)
    {
        for (int c = 0; c < 11; c++)
            printf("%d ", MAP[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

/*
    빨간 보드에 블록을 배치하는 함수

    빨간 보드는 오른쪽으로 밀려 들어간다고 생각하면 된다.
    sr, sc에서 시작해서 더 이상 갈 수 없을 때까지 오른쪽으로 이동한다.

    type 1 : 한 칸 블록
    type 2 : 가로 2칸 블록
    type 3 : 세로 2칸 블록
*/
void moveRed(int type, int sr, int sc)
{
    int c = sc;

    if (type == 1)
    {
        // 1x1 블록: 오른쪽 끝까지 이동
        while (1)
        {
            if (MAP[sr][c + 1] != 0) break;
            c++;
        }

        MAP[sr][c] = 1;
    }
    else if (type == 2)
    {
        // 1x2 가로 블록:
        // 오른쪽 한 칸 더 나아간 위치까지 두 칸이 모두 비어 있어야 이동 가능
        while (1)
        {
            if (MAP[sr][c + 2] != 0) break;
            c++;
        }

        MAP[sr][c] = MAP[sr][c + 1] = 1;
    }
    else if (type == 3)
    {
        // 2x1 세로 블록:
        // 위아래 두 칸이 함께 오른쪽으로 이동해야 하므로
        // 두 행 모두 다음 칸이 비어 있어야 이동 가능
        while (1)
        {
            if (MAP[sr][c + 1] != 0) break;
            if (MAP[sr + 1][c + 1] != 0) break;
            c++;
        }

        MAP[sr][c] = MAP[sr + 1][c] = 1;
    }
}

/*
    노란 보드에 블록을 배치하는 함수

    노란 보드는 아래쪽으로 떨어진다고 생각하면 된다.
    sr, sc에서 시작해서 더 이상 갈 수 없을 때까지 아래로 이동한다.

    type 1 : 한 칸 블록
    type 2 : 가로 2칸 블록
    type 3 : 세로 2칸 블록
*/
void moveYellow(int type, int sr, int sc)
{
    int r = sr;

    if (type == 1)
    {
        // 1x1 블록: 아래 끝까지 이동
        while (1)
        {
            if (MAP[r + 1][sc] != 0) break;
            r++;
        }

        MAP[r][sc] = 1;
    }
    else if (type == 2)
    {
        // 1x2 가로 블록:
        // 두 칸이 함께 아래로 내려가야 하므로
        // 아래 두 칸이 모두 비어 있어야 이동 가능
        while (1)
        {
            if (MAP[r + 1][sc] != 0) break;
            if (MAP[r + 1][sc + 1] != 0) break;
            r++;
        }

        MAP[r][sc] = MAP[r][sc + 1] = 1;
    }
    else if (type == 3)
    {
        // 2x1 세로 블록:
        // 세로 블록이라 가장 아래 칸 기준으로 검사해야 하므로
        // r+2 위치가 비어 있어야 한 칸 더 이동 가능
        while (1)
        {
            if (MAP[r + 2][sc] != 0) break;
            r++;
        }

        MAP[r][sc] = MAP[r + 1][sc] = 1;
    }
}

/*
    빨간 보드에서 열 하나를 삭제하는 함수

    col 열이 꽉 찼을 때 호출된다.
    해당 열을 없애고, 그 왼쪽에 있는 열들을 한 칸씩 오른쪽으로 민다.
    START 위치는 새로 빈칸(0)으로 만든다.
*/
void deleteRed(int col)
{
    for (int c = col; c > START; c--)
        for (int r = 0; r < 4; r++)
            MAP[r][c] = MAP[r][c - 1];

    MAP[0][START] = MAP[1][START] = MAP[2][START] = MAP[3][START] = 0;
}

/*
    노란 보드에서 행 하나를 삭제하는 함수

    row 행이 꽉 찼을 때 호출된다.
    해당 행을 없애고, 그 위에 있는 행들을 한 칸씩 아래로 민다.
    START 위치는 새로 빈칸(0)으로 만든다.
*/
void deleteYellow(int row)
{
    for (int r = row; r > START; r--)
        for (int c = 0; c < 4; c++)
            MAP[r][c] = MAP[r - 1][c];

    MAP[START][0] = MAP[START][1] = MAP[START][2] = MAP[START][3] = 0;
}

/*
    빨간 보드에서 점수 획득 여부를 확인하는 함수

    진한 영역(START+2 ~ END)의 각 열을 검사해서
    4칸이 모두 차 있으면 그 열을 삭제하고 1점을 반환한다.

    한 번 호출당 한 줄만 삭제하도록 작성되어 있다.
*/
int getScoreRed()
{
    for (int c = START + 2; c <= END; c++)
    {
        int cnt = 0;
        for (int r = 0; r < 4; r++)
            cnt += MAP[r][c];

        if (cnt == 4)
        {
            deleteRed(c);
            return 1;
        }
    }

    return 0;
}

/*
    노란 보드에서 점수 획득 여부를 확인하는 함수

    진한 영역(START+2 ~ END)의 각 행을 검사해서
    4칸이 모두 차 있으면 그 행을 삭제하고 1점을 반환한다.

    한 번 호출당 한 줄만 삭제하도록 작성되어 있다.
*/
int getScoreYellow()
{
    for (int r = START + 2; r <= END; r++)
    {
        int cnt = 0;
        for (int c = 0; c < 4; c++)
            cnt += MAP[r][c];

        if (cnt == 4)
        {
            deleteYellow(r);
            return 1;
        }
    }

    return 0;
}

/*
    빨간 보드의 연한 칸(START+1 열)에 블록이 존재하는지 확인

    true  : 연한 칸에 블록이 있음
    false : 없음
*/
bool checkRed()
{
    for (int r = 0; r < 4; r++)
        if (MAP[r][START + 1] == 1) return true;

    return false;
}

/*
    노란 보드의 연한 칸(START+1 행)에 블록이 존재하는지 확인

    true  : 연한 칸에 블록이 있음
    false : 없음
*/
bool checkYellow()
{
    for (int c = 0; c < 4; c++)
        if (MAP[START + 1][c] == 1) return true;

    return false;
}

/*
    전체 시뮬레이션 함수

    블록을 하나씩 처리하면서
    1. 빨간 보드에 배치
    2. 노란 보드에 배치
    3. 점수 줄 삭제
    4. 연한 칸 처리
    를 반복한다.

    마지막에는
    - 총 점수
    - 남아 있는 블록 수
    를 출력한다.
*/
void simulate()
{
    int score = 0;

    for (int k = 0; k < K; k++)
    {
        // 같은 블록을 빨간/노란 보드에 각각 배치
        moveRed(TYPE[k], R[k], C[k]);
        moveYellow(TYPE[k], R[k], C[k]);

        // 빨간 보드 점수 처리
        // 최대 2줄까지 가능하다고 보고 두 번 검사
        score += getScoreRed();
        score += getScoreRed();

        // 노란 보드 점수 처리
        // 최대 2줄까지 가능하다고 보고 두 번 검사
        score += getScoreYellow();
        score += getScoreYellow();

        // 빨간 보드 연한 칸 처리
        // 최대 2칸까지 밀릴 수 있으므로 두 번 검사
        if (checkRed() == true) deleteRed(END);
        if (checkRed() == true) deleteRed(END);

        // 노란 보드 연한 칸 처리
        // 최대 2칸까지 밀릴 수 있으므로 두 번 검사
        if (checkYellow() == true) deleteYellow(END);
        if (checkYellow() == true) deleteYellow(END);
    }

    int blockCount = 0;

    // 빨간 보드의 진한 영역에 남은 블록 수 계산
    for (int c = START + 2; c <= END; c++)
        for (int r = 0; r < 4; r++)
            blockCount += MAP[r][c];

    // 노란 보드의 진한 영역에 남은 블록 수 계산
    for (int r = START + 2; r <= END; r++)
        for (int c = 0; c < 4; c++)
            blockCount += MAP[r][c];

    printf("%d\n%d\n", score, blockCount);
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();
        simulate();
    }

    return 0;
}