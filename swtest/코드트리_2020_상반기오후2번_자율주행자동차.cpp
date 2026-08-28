/*
    [코드트리] 2020 상반기 오후 2번 - 자율주행 전기차
    (백준 19238 "스타트 택시"와 같은 문제다  https://www.acmicpc.net/problem/19238)

    ■ 문제 요약
      N x N 격자에 빈칸(0)과 벽(1)이 있고, 전기차가 배터리 C를 가지고 시작한다.
      승객 M명이 각각 출발지와 목적지를 가지고 있다.

      아래를 M번 반복한다.

        1) 아직 태우지 않은 승객 중 "가장 가까운" 승객을 고른다.
           거리가 같으면 행 번호가 작은 쪽, 그것도 같으면 열 번호가 작은 쪽.
        2) 그 승객의 출발지까지 이동한다. 이동 거리만큼 배터리가 준다.
        3) 승객을 태우고 목적지까지 이동한다. 이동 거리만큼 배터리가 줄지만,
           도착에 성공하면 그 거리의 두 배가 충전된다.

      모두 성공하면 남은 배터리를, 도중에 실패하면 -1을 출력한다.

    ■ 풀이 방침 : BFS 두 종류를 번갈아 돌린다
      거리를 최단 경로로 재야 하고 벽이 있으므로 맨해튼 거리가 아니라 BFS가 필요하다.

        findPeople()      : 차 위치에서 BFS를 돌려 "가장 가까운 승객"을 고른다
        goToDestination() : 승객 출발지에서 BFS를 돌려 목적지까지의 거리를 잰다

      이 두 개를 승객 수만큼 반복하면 끝난다.

    ■ 가장 가까운 승객 고르기
      승객마다 따로 BFS를 돌릴 필요가 없다. 차에서 한 번만 BFS를 돌리면
      모든 칸까지의 거리가 한꺼번에 나오기 때문이다.

      그래서 BFS를 돌리기 전에 tmpMAP의 승객 출발지 칸에 "승객 번호"를 적어 둔다.
      그러면 BFS가 어떤 칸을 처음 방문할 때 그 칸이 승객 자리인지 바로 알 수 있다.

      우선순위(거리 -> 행 -> 열)는 방문 순서에 기대지 않고 명시적으로 비교한다.
      BFS는 거리가 작은 칸부터 꺼내지만, 같은 거리 안에서의 순서는 큐에 들어간 순서라
      행/열 우선순위와 일치한다는 보장이 없기 때문이다.

    ■ 거리를 1부터 세는 이유
      visit이 방문 여부와 거리를 겸한다. 0을 "미방문"으로 쓰므로 시작 칸을 1로 두고,
      실제 거리를 낼 때 1을 뺀다.

    ■ 배터리 계산
      목적지 이동은 "거리만큼 쓰고 거리의 두 배를 받는다"이므로
      결과적으로 거리만큼 늘어난다. 그래서 코드에서는 C += capacity 한 줄이면 된다.

          C - capacity + capacity * 2  ==  C + capacity

    ■ [확인 필요] 태우러 갈 때의 배터리 조건
      승객에게 가는 부분은 `if (C <= info.distance) return -1;` 로,
      배터리와 거리가 정확히 같아 도착과 동시에 0이 되는 경우도 실패로 본다.
      반면 목적지로 갈 때는 `if (C < capacity)` 라 같은 상황을 성공으로 처리한다.
      두 조건이 서로 다르다는 점은 한 번 짚어 볼 만하다.

      백준 19238 기준으로는 두 곳 모두 "부족할 때만" 실패이므로 앞쪽도 `C < info.distance` 가 맞다.
      코드트리 쪽 조건이 다를 수 있으니 문제 설명을 확인하고 맞춰 두는 편이 좋다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (20 + 5)
#define INF (0x7fff0000)

#define ROAD (0)
#define WALL (1)

int T;

int N, M, C;

// MAP[r][c] : 0이면 길, -1이면 벽 (격자 바깥도 -1)
int MAP[MAX][MAX];

// BFS용 작업 배열. MAP을 복사한 뒤 승객 위치를 덧그리는 데 쓴다.
int tmpMAP[MAX][MAX];

// visit[r][c] : 방문 여부 + 거리 (0이면 미방문, 시작 칸이 1)
int visit[MAX][MAX];

// 좌표 한 쌍
struct RC
{
    int r;
    int c;
};

typedef struct RC RC;

RC car;                // 전기차의 현재 위치
RC queue[MAX * MAX];   // BFS용 큐

// 승객 한 명
struct PEOPLE
{
    int sr;       // 출발지 행
    int sc;       // 출발지 열
    int er;       // 목적지 행
    int ec;       // 목적지 열
    bool check;   // 이미 데려다줬는지
};

typedef struct PEOPLE PEOPLE;

PEOPLE people[MAX * MAX];

// findPeople()의 결과
struct INFO
{
    int peopleNumber;   // 고른 승객 번호 (없으면 -1)
    int distance;       // 그 승객까지의 거리
};

typedef struct INFO INFO;

// 방향 배열: ↑, →, ↓, ←
// 우선순위는 아래 비교식으로 따로 처리하므로 이 순서 자체는 답에 영향을 주지 않는다.
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

/*
    입력
*/
void input()
{
    scanf("%d %d %d", &N, &M, &C);

    // 격자 바깥을 벽(-1)으로 채워 두면 BFS에서 범위 검사가 필요 없다
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = -1;

    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            scanf("%d", &MAP[r][c]);

            // 입력의 벽은 1이지만, 바깥 테두리와 똑같이 -1로 통일해 둔다.
            // 그러면 "벽인가"를 -1 하나로 검사할 수 있다.
            if (MAP[r][c] == WALL) MAP[r][c] = -1;
        }
    }

    scanf("%d %d", &car.r, &car.c);

    for (int m = 1; m <= M; m++)
    {
        int sr, sc, er, ec;

        scanf("%d %d %d %d", &sr, &sc, &er, &ec);

        people[m].sr = sr;
        people[m].sc = sc;
        people[m].er = er;
        people[m].ec = ec;
        people[m].check = false;
    }
}

/*
    디버그용 맵 출력
*/
void printMap(int map[MAX][MAX])
{
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
            printf("%2d ", map[r][c]);
        putchar('\n');
    }
    putchar('\n');
}

/*
    차 위치에서 BFS를 한 번 돌려 가장 가까운 승객을 고른다

      우선순위 : 거리 -> 행 번호 -> 열 번호 (모두 작은 쪽)
*/
INFO findPeople()
{
    int rp, wp;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            visit[r][c] = 0;

    // 원본을 건드리지 않기 위해 작업용 배열에 복사 (테두리까지 포함)
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            tmpMAP[r][c] = MAP[r][c];

    // 아직 안 태운 승객의 출발지에 "승객 번호"를 적어 둔다.
    // 길은 0, 벽은 -1이므로 0이 아닌 양수면 곧 승객 자리가 된다.
    for (int i = 1; i <= M; i++)
    {
        if (people[i].check == true) continue;

        int pr = people[i].sr;
        int pc = people[i].sc;

        tmpMAP[pr][pc] = i;

        // 차가 이미 승객 위에 서 있으면 BFS를 돌릴 것도 없이 거리 0이다.
        // (BFS는 시작 칸을 후보로 검사하지 않으므로 이 처리가 필요하다)
        if (pr == car.r && pc == car.c)
        {
            INFO result = { i, 0 };
            return result;
        }
    }

    rp = wp = 0;

    queue[wp].r = car.r;
    queue[wp++].c = car.c;

    visit[car.r][car.c] = 1;   // 0은 미방문이므로 1부터 센다

    int minDistance = INF;
    int minR = INF;
    int minC = INF;
    int peopleNumber = -1;

    while (rp < wp)
    {
        RC out = queue[rp++];

        for (int k = 0; k < 4; k++)
        {
            int nr = out.r + dr[k];
            int nc = out.c + dc[k];

            if (tmpMAP[nr][nc] == -1 || visit[nr][nc] != 0) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;

            // 0이 아니면 승객이 서 있는 칸이다
            if (tmpMAP[nr][nc] != 0)
            {
                // 거리 -> 행 -> 열 순으로 명시적으로 비교한다.
                // BFS의 방문 순서만 믿으면 같은 거리 안에서의 우선순위가 보장되지 않는다.
                if ((visit[nr][nc] < minDistance)
                    || (visit[nr][nc] == minDistance && nr < minR)
                    || (visit[nr][nc] == minDistance && nr == minR && nc < minC))
                {
                    peopleNumber = tmpMAP[nr][nc];
                    minDistance = visit[nr][nc];
                    minR = nr;
                    minC = nc;
                }
            }
        }
    }

    // 시작 칸을 1로 뒀으므로 실제 거리는 -1
    // (승객을 못 찾았으면 peopleNumber가 -1이고, 호출한 쪽에서 먼저 걸러낸다)
    INFO result = { peopleNumber, minDistance - 1 };
    return result;
}

/*
    차 위치에서 (er, ec)까지의 최단 거리

      도달할 수 없으면 -1
*/
int goToDestination(int er, int ec)
{
    int rp, wp;

    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            visit[r][c] = 0;

    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            tmpMAP[r][c] = MAP[r][c];

    rp = wp = 0;

    queue[wp].r = car.r;
    queue[wp++].c = car.c;

    visit[car.r][car.c] = 1;

    while (rp < wp)
    {
        RC out = queue[rp++];

        // 처음 꺼낸 목적지가 최단이다 (출발지 == 목적지인 경우 0이 나온다)
        if (out.r == er && out.c == ec)
            return visit[out.r][out.c] - 1;

        for (int k = 0; k < 4; k++)
        {
            int nr = out.r + dr[k];
            int nc = out.c + dc[k];

            if (tmpMAP[nr][nc] == -1 || visit[nr][nc] != 0) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;
        }
    }

    return -1;
}

/*
    승객 M명을 차례로 처리
*/
int simulate()
{
    for (int i = 0; i < M; i++)
    {
        // 1) 가장 가까운 승객 고르기
        INFO info = findPeople();

        // 아무도 도달할 수 없으면 실패
        if (info.peopleNumber == -1) return -1;

        // 2) 태우러 가는 데 배터리가 모자라면 실패
        //    (파일 상단 [확인 필요] 참고 : 이 조건은 <= 로 되어 있다)
        if (C <= info.distance) return -1;

        C -= info.distance;

        int number = info.peopleNumber;

        // 차를 승객 출발지로 옮긴다
        car.r = people[number].sr;
        car.c = people[number].sc;

        // 3) 목적지까지의 거리
        int capacity = goToDestination(people[number].er, people[number].ec);

        // 갈 수 없거나 배터리가 모자라면 실패
        if (capacity == -1 || C < capacity) return -1;

        // 거리만큼 쓰고 두 배를 받으므로 결과적으로 거리만큼 늘어난다
        C += capacity;

        car.r = people[number].er;
        car.c = people[number].ec;

        people[number].check = true;
    }

    return C;
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        printf("%d\n", simulate());
    }

    return 0;
}
