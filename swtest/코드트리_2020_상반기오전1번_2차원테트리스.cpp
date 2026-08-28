/*
    [코드트리] 2020 상반기 오전 1번 - 2차원 테트리스

    ■ 문제 요약
      4 x 4 크기의 대기 구역에 블록이 하나씩 놓이고, 그 블록이 두 보드에 동시에 떨어진다.

        빨간 보드 : 4행 x 6열. 블록이 대기 구역에서 "오른쪽"으로 끝까지 밀려간다.
        노란 보드 : 6행 x 4열. 블록이 대기 구역에서 "아래"로 끝까지 떨어진다.

      블록 종류
        1 : 1 x 1
        2 : 1 x 2 (가로)
        3 : 2 x 1 (세로)

      블록을 놓은 뒤 매번 아래 두 가지를 처리한다.

        1) 진한 칸(각 보드의 안쪽 4줄) 중 한 줄이 꽉 차면 그 줄을 지우고 1점을 얻는다.
        2) 연한 칸(입구 쪽 2줄)에 블록이 남아 있으면, 그 줄 수만큼
           반대쪽 끝 줄이 밀려나 사라지고 전체가 그쪽으로 한 칸씩 밀린다.

      블록을 모두 처리한 뒤 총점과 보드에 남은 블록 칸 수를 출력한다.

    ■ 좌표 배치 (이 코드의 핵심 설계)
      두 보드를 따로 두지 않고 하나의 배열 MAP에 ㄱ자 모양으로 얹었다.

              열 0 1 2 3 | 4 5 6 7 8 9 |10
        행 0    +-------+-------------+ W
        행 1    | 대기  |  빨간 보드  | W      <- 오른쪽으로 밀려간다
        행 2    | 구역  |             | W
        행 3    +-------+-------------+ W
        행 4    | 노                              |
        행 5    | 란     ^                        |
        행 6    | 보     |  아래로 떨어진다        |
        행 7    | 드                              |
        행 8    |                                 |
        행 9    +---------------------------------+
        행10     W W W W

      즉 행 0~3 x 열 0~3 이 두 보드가 공유하는 대기 구역이고,
      거기서 오른쪽으로 뻗은 부분이 빨간 보드, 아래로 뻗은 부분이 노란 보드다.

        START(4) ~ END(9) 가 각 보드의 6줄이고
          연한 칸 : START, START+1        (입구 쪽 2줄)
          진한 칸 : START+2 ~ END         (안쪽 4줄, 점수 대상)

      배열 바깥쪽에 WALL(-1)을 박아 두었기 때문에 블록이 끝까지 밀려가는 while 루프가
      좌표 범위 검사 없이 "다음 칸이 0이 아니면 멈춤" 한 줄로 끝난다.
      (블록도 벽도 모두 0이 아니므로 같은 조건으로 걸린다)

    ■ 줄 삭제 : deleteRed / deleteYellow
      col 열을 지우고 그보다 입구 쪽에 있던 것들을 한 칸씩 안쪽으로 민 뒤, START를 비운다.

        점수로 지울 때 : deleteRed(꽉 찬 열)  -> 그 줄이 사라지고 앞쪽이 밀려 들어온다
        밀려날 때      : deleteRed(END)       -> 가장 안쪽 줄이 밖으로 밀려 사라진다

      두 경우 모두 "한 줄 없애고 나머지를 안쪽으로 한 칸 민다"는 같은 동작이라
      함수 하나를 col만 바꿔 재사용한다.

    ■ 왜 두 번씩 호출하는가
        getScoreRed()를 두 번 부르는 이유
          가로 2칸짜리 블록 하나가 두 줄을 동시에 채울 수 있어서 최대 2줄이 지워진다.

        checkRed() -> deleteRed(END) 를 두 번 부르는 이유
          연한 칸은 두 줄인데 checkRed는 안쪽 연한 줄(START+1)만 본다.
          한 번 밀리면 바깥 연한 줄(START)에 있던 블록이 START+1로 옮겨 오므로,
          같은 검사를 한 번 더 하면 두 줄을 모두 처리한 것과 같아진다.
          START를 따로 검사할 필요가 없는 것이 이 구조의 장점이다.

    ■ 마지막 집계
      남은 블록 수는 진한 칸만 세면 된다. 연한 칸은 위 처리 때문에 항상 비어 있기 때문이다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (10 + 3)
#define MAX_K (10000 + 500)

// 각 보드의 줄 번호 범위 (연한 칸 2줄 + 진한 칸 4줄)
#define START (4)
#define END (9)

// 벽 표시
#define WALL (-1)

int T;

int K;                  // 블록 개수
int MAP[MAX][MAX];      // 대기 구역 + 두 보드를 함께 담은 배열

// 입력 블록 정보
int TYPE[MAX_K];
int R[MAX_K];
int C[MAX_K];

/*
    입력 + 벽 세우기
*/
void input()
{
    for (int r = 0; r < MAX; r++)
        for (int c = 0; c < MAX; c++)
            MAP[r][c] = 0;

    // 빨간 보드의 오른쪽 끝 벽 (여기서 블록이 멈춘다)
    MAP[0][10] = MAP[1][10] = MAP[2][10] = MAP[3][10] = WALL;

    // 노란 보드의 아래쪽 끝 벽
    MAP[10][0] = MAP[10][1] = MAP[10][2] = MAP[10][3] = WALL;

    scanf("%d", &K);

    for (int k = 0; k < K; k++)
        scanf("%d %d %d", &TYPE[k], &R[k], &C[k]);
}

/*
    디버그용: 전체 배열 출력 (ㄱ자 배치를 눈으로 확인할 때 유용)
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
    빨간 보드 : (sr, sc)에서 오른쪽으로 끝까지 민다

    멈춤 조건은 "다음에 차지할 칸이 0이 아니다"이다.
    벽도 다른 블록도 모두 0이 아니므로 한 조건으로 처리된다.
*/
void moveRed(int type, int sr, int sc)
{
    int c = sc;

    if (type == 1)
    {
        // 한 칸짜리 : 바로 오른쪽만 보면 된다
        while (1)
        {
            if (MAP[sr][c + 1] != 0) break;
            c++;
        }

        MAP[sr][c] = 1;
    }
    else if (type == 2)
    {
        // 가로 2칸 : c와 c+1을 차지하므로, 진행 방향의 다음 칸은 c+2다
        while (1)
        {
            if (MAP[sr][c + 2] != 0) break;
            c++;
        }

        MAP[sr][c] = MAP[sr][c + 1] = 1;
    }
    else if (type == 3)
    {
        // 세로 2칸 : 두 행이 함께 오른쪽으로 가므로 두 행 모두 다음 칸이 비어야 한다
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
    노란 보드 : (sr, sc)에서 아래로 끝까지 떨어뜨린다

    moveRed와 완전히 같은 구조이고 방향만 아래쪽이다.
    다만 가로/세로 블록의 역할이 서로 바뀐다는 점에 주의.
*/
void moveYellow(int type, int sr, int sc)
{
    int r = sr;

    if (type == 1)
    {
        while (1)
        {
            if (MAP[r + 1][sc] != 0) break;
            r++;
        }

        MAP[r][sc] = 1;
    }
    else if (type == 2)
    {
        // 가로 2칸 : 두 열이 함께 내려가므로 두 열 모두 아래가 비어야 한다
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
        // 세로 2칸 : r과 r+1을 차지하므로 진행 방향의 다음 칸은 r+2다
        while (1)
        {
            if (MAP[r + 2][sc] != 0) break;
            r++;
        }

        MAP[r][sc] = MAP[r + 1][sc] = 1;
    }
}

/*
    빨간 보드에서 col 열을 없애고, 입구 쪽에 있던 열들을 한 칸씩 안쪽으로 민다.
    비게 된 START 열은 0으로 만든다.

    점수 삭제와 "밀려남" 처리에 모두 쓰인다 (col만 다르다).
*/
void deleteRed(int col)
{
    for (int c = col; c > START; c--)
        for (int r = 0; r < 4; r++)
            MAP[r][c] = MAP[r][c - 1];

    MAP[0][START] = MAP[1][START] = MAP[2][START] = MAP[3][START] = 0;
}

/*
    노란 보드에서 row 행을 없애고 위쪽 행들을 한 칸씩 아래로 민다.
*/
void deleteYellow(int row)
{
    for (int r = row; r > START; r--)
        for (int c = 0; c < 4; c++)
            MAP[r][c] = MAP[r - 1][c];

    MAP[START][0] = MAP[START][1] = MAP[START][2] = MAP[START][3] = 0;
}

/*
    빨간 보드 점수 처리

    진한 칸(START+2 ~ END) 중 4칸이 모두 찬 열을 하나 찾아 지우고 1점을 돌려준다.
    한 번에 한 줄만 처리하므로, 두 줄이 찰 수 있는 상황에서는 두 번 호출한다.
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
    노란 보드 점수 처리 (빨간 보드와 행/열만 바뀐 같은 코드)
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
    빨간 보드의 안쪽 연한 줄(START+1)에 블록이 있는지

    바깥 연한 줄(START)은 따로 보지 않는다.
    한 번 밀리면 START에 있던 블록이 START+1로 옮겨 오므로,
    이 검사를 두 번 하면 연한 칸 두 줄이 모두 처리된다.
*/
bool checkRed()
{
    for (int r = 0; r < 4; r++)
        if (MAP[r][START + 1] == 1) return true;

    return false;
}

/*
    노란 보드의 안쪽 연한 줄(START+1 행)에 블록이 있는지
*/
bool checkYellow()
{
    for (int c = 0; c < 4; c++)
        if (MAP[START + 1][c] == 1) return true;

    return false;
}

/*
    블록을 하나씩 처리하는 전체 시뮬레이션

    순서가 정해져 있다 : 배치 -> 점수 줄 삭제 -> 연한 칸 밀어내기
*/
void simulate()
{
    int score = 0;

    for (int k = 0; k < K; k++)
    {
        // 같은 블록이 두 보드에 동시에 놓인다
        moveRed(TYPE[k], R[k], C[k]);
        moveYellow(TYPE[k], R[k], C[k]);

        // 가로 2칸 블록이 두 줄을 동시에 채울 수 있어 최대 2줄까지 지워진다
        score += getScoreRed();
        score += getScoreRed();

        score += getScoreYellow();
        score += getScoreYellow();

        // 연한 칸이 두 줄이므로 두 번 검사한다 (위 checkRed 설명 참고)
        if (checkRed() == true) deleteRed(END);
        if (checkRed() == true) deleteRed(END);

        if (checkYellow() == true) deleteYellow(END);
        if (checkYellow() == true) deleteYellow(END);
    }

    int blockCount = 0;

    // 연한 칸은 위 처리로 항상 비어 있으므로 진한 칸만 세면 된다
    for (int c = START + 2; c <= END; c++)
        for (int r = 0; r < 4; r++)
            blockCount += MAP[r][c];

    for (int r = START + 2; r <= END; r++)
        for (int c = 0; c < 4; c++)
            blockCount += MAP[r][c];

    printf("%d\n%d\n", score, blockCount);
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();
        simulate();
    }

    return 0;
}
