// 코드트리 2019 하반기 오후 1번 - 이상한 다트 게임
//
// 문제 요약
// N개의 원판이 있고, 각 원판에는 M개의 숫자가 원형으로 적혀 있다.
// 총 Q번의 회전을 수행하며, 각 회전은 다음 정보를 가진다.
//
// X : X의 배수 번호를 가진 원판들을 회전
// D : 방향 (0 = 시계방향, 1 = 반시계방향)
// K : K칸 회전
//
// 회전 후 처리 규칙
// 1. 인접하면서 같은 수를 찾는다.
//    - 같은 원판에서 양옆도 인접
//    - 원판은 원형이므로 1번 칸과 M번 칸도 인접
//    - 위/아래 원판의 같은 위치도 인접
// 2. 인접한 같은 수가 하나라도 있으면 모두 0으로 삭제
// 3. 삭제가 한 번도 일어나지 않으면,
//    남아 있는 수들의 평균을 기준으로
//    평균보다 크면 1 감소, 작으면 1 증가
//
// 목표
// Q번의 회전을 모두 수행한 뒤 원판에 남아 있는 수들의 합을 구한다.
//
// 핵심 아이디어
// - circle[r][c] : r번 원판의 c번 위치 숫자
// - rotate()     : 조건에 맞는 원판들을 회전
// - BFS()        : 인접한 같은 수를 탐색해서 삭제
// - startGame()  : 한 번의 회전 후 삭제 또는 평균 조정 수행
//
// 즉,
// "원판 회전" + "같은 수 묶음 삭제" + "없으면 평균 조정"
// 을 Q번 반복 시뮬레이션하는 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (50 + 5)

// 회전 방향 정의
#define CLOCKWISE (0)           // 시계 방향
#define COUNTER_CLOCKWISE (1)   // 반시계 방향

int T;
int N, M, Q;

// circle[r][c] = r번 원판의 c번 위치 숫자
int circle[MAX][MAX];

// BFS 방문 배열
int visit[MAX][MAX];

// 회전 명령 정보
// X[i] : i번째 명령에서 X의 배수 원판 회전
// D[i] : 방향
// K[i] : 회전 칸 수
int X[MAX];
int D[MAX];
int K[MAX];

// 좌표 저장용 구조체
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

    N : 원판 개수
    M : 각 원판에 적힌 숫자 개수
    Q : 회전 명령 수

    circle[r][c] : 초기 원판 숫자 입력
    X[i], D[i], K[i] : 각 회전 명령 입력
*/
void input()
{
    scanf("%d %d %d", &N, &M, &Q);

    // 전체 배열 0 초기화
    for (int r = 0; r < MAX; r++)
        for (int c = 0; c < MAX; c++)
            circle[r][c] = 0;

    // 원판 숫자 입력
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            scanf("%d", &circle[r][c]);

    // 회전 명령 입력
    for (int i = 1; i <= Q; i++)
        scanf("%d %d %d", &X[i], &D[i], &K[i]);
}

/*
    디버그용 출력 함수
    현재 원판 상태를 출력
*/
void printMap()
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
            printf("%d ", circle[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

/*
    회전 함수

    x : x의 배수 원판들을 회전
    d : 방향
    k : 회전 칸 수

    원판은 원형 구조이므로,
    임시 배열 arr에 현재 원판을 복사한 뒤
    회전된 위치에 다시 배치한다.
*/
void rotate(int x, int d, int k)
{
    int arr[MAX] = { 0 };

    // 반시계 방향이면 시계 방향 기준으로 -k 처리
    if (d == COUNTER_CLOCKWISE) k = -k;

    // x의 배수 원판들만 회전
    for (int r = x; r <= N; r += x)
    {
        int index = 0;

        // 현재 원판의 값을 arr에 복사
        for (int c = 1; c <= M; c++)
            arr[index++] = circle[r][c];

        // 회전 후 다시 circle에 반영
        for (int i = 0; i < index; i++)
        {
            int newIndex = (i - k) % index;

            // 음수 보정
            newIndex = (newIndex < 0) ? newIndex + index : newIndex;

            circle[r][i + 1] = arr[newIndex];
        }
    }
}

/*
    BFS 함수

    시작 위치 (r, c)와 같은 숫자로 연결된 인접 칸들을 모두 찾는다.
    연결된 칸의 개수가 2개 이상이면 모두 0으로 삭제한다.

    인접 조건
    - 상하좌우
    - 좌우는 원형 연결: 1번 칸과 M번 칸도 서로 인접

    반환값
    - true  : 삭제가 발생함
    - false : 삭제가 발생하지 않음
*/
bool BFS(int r, int c)
{
    int rp, wp;
    rp = wp = 0;

    // 시작점 큐에 삽입
    queue[wp].r = r;
    queue[wp++].c = c;

    visit[r][c] = true;

    while (rp < wp)
    {
        RC out = queue[rp++];

        // 상하좌우 탐색
        for (int i = 0; i < 4; i++)
        {
            int nr = out.r + dr[i];
            int nc = out.c + dc[i];

            // 0은 이미 삭제된 칸이므로 제외
            if (circle[nr][nc] == 0) continue;

            // 값이 다르거나 이미 방문했으면 제외
            if (circle[nr][nc] != circle[r][c] || visit[nr][nc] == true) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = true;
        }

        // 원형 구조 처리
        // c == 1이면 왼쪽 이웃은 M
        if (out.c == 1)
        {
            if (circle[r][c] == circle[out.r][M] && visit[out.r][M] == false)
            {
                queue[wp].r = out.r;
                queue[wp++].c = M;

                visit[out.r][M] = true;
            }
        }
        // c == M이면 오른쪽 이웃은 1
        else if (out.c == M)
        {
            if (circle[r][c] == circle[out.r][1] && visit[out.r][1] == false)
            {
                queue[wp].r = out.r;
                queue[wp++].c = 1;

                visit[out.r][1] = true;
            }
        }
    }

    // 연결된 칸이 2개 이상이면 삭제
    if (wp > 1)
    {
        for (int i = 0; i < wp; i++)
        {
            int rr = queue[i].r;
            int cc = queue[i].c;

            circle[rr][cc] = 0;
        }
        return true;
    }

    return false;
}

/*
    평균 조정 함수

    삭제가 한 번도 일어나지 않았을 때 호출된다.

    남아 있는 숫자들의 평균을 구한 뒤,
    - 평균보다 작으면 +1
    - 평균보다 크면 -1
    - 평균과 같으면 그대로 둠

    주의:
    이 코드에서는 실수 평균이 아니라 정수 나눗셈 결과(avg = sum / count)를 사용한다.
*/
void makeAverage()
{
    int sum = 0;
    int count = 0;
    int avg;

    // 남아 있는 숫자들의 합과 개수 계산
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            if (circle[r][c] != 0)
            {
                sum += circle[r][c];
                count++;
            }
        }
    }

    // 남아 있는 숫자가 없으면 종료
    if (count == 0) return;

    avg = sum / count;

    // 평균과 비교해서 조정
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            if (circle[r][c] == 0) continue;

            if (circle[r][c] < avg) circle[r][c]++;
            else if (circle[r][c] > avg) circle[r][c]--;
        }
    }
}

/*
    회전 1번 후 후처리 함수

    1. visit 배열 초기화
    2. 모든 칸을 돌며 BFS 수행
    3. 삭제가 한 번이라도 일어나면 그대로 종료
    4. 삭제가 없으면 평균 조정 수행
*/
void startGame()
{
    // 방문 배열 초기화
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            visit[r][c] = false;

    bool deleteCheck = false;

    // 모든 칸 탐색
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= M; c++)
        {
            // 이미 0이거나 방문한 칸은 제외
            if (circle[r][c] == 0 || visit[r][c] == true) continue;

            bool check = BFS(r, c);
            if (check == true) deleteCheck = true;
        }
    }

    // 삭제가 한 번도 없었으면 평균 조정
    if (deleteCheck == false) makeAverage();
}

/*
    전체 시뮬레이션 함수

    Q번의 회전 명령을 차례대로 수행한다.
    각 명령마다
    1. rotate()
    2. startGame()
    순서로 진행
*/
void simulate()
{
    for (int q = 1; q <= Q; q++)
    {
        rotate(X[q], D[q], K[q]);
        startGame();
    }
}

/*
    최종 정답 계산 함수

    모든 원판에 남아 있는 숫자의 합을 반환
*/
int getAnswer()
{
    int sum = 0;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= M; c++)
            sum += circle[r][c];

    return sum;
}

int main()
{
    //scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        simulate();

        printf("%d\n", getAnswer());
    }

    return 0;
}