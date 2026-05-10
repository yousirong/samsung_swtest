// 코드트리 2020 상반기 오후 2번 - 자율주행 전기차
//
// 문제 요약
// N x N 격자 위에서 전기차가 M명의 승객을 태워 목적지까지 데려다줘야 한다.
// 전기차는 초기 배터리 C를 가지고 시작한다.
//
// 진행 규칙
// 1. 아직 도착하지 않은 승객 중 가장 가까운 승객을 찾는다.
// 2. 거리가 같다면 행 번호가 작은 승객,
//    그래도 같다면 열 번호가 작은 승객을 선택한다.
// 3. 전기차가 승객 출발지까지 이동하면 그 거리만큼 배터리를 소모한다.
// 4. 승객을 태우고 목적지까지 이동하면 그 거리만큼 배터리를 소모하지만,
//    도착 성공 시 이동한 거리의 2배가 충전된다.
//    결과적으로 코드에서는 목적지 이동 전 C -= distance 대신,
//    성공 후 C += distance 형태와 같은 효과로 처리한다.
// 5. 모든 승객을 성공적으로 데려다주면 남은 배터리를 출력한다.
// 6. 중간에 배터리가 부족하거나 도달할 수 없으면 -1을 출력한다.
//
// 핵심 아이디어
// - findPeople()       : BFS로 현재 차 위치에서 가장 가까운 승객 찾기
// - goToDestination()  : BFS로 승객 출발지에서 목적지까지 거리 계산
// - simulate()         : 승객 선택, 이동, 배터리 갱신을 M번 반복
//
// 즉,
// "가장 가까운 승객 찾기 BFS"와 "목적지까지 이동 BFS"를
// 승객 수만큼 반복하는 시뮬레이션 문제이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

#define MAX (20 + 5)
#define INF (0x7fff0000)

#define ROAD (0)
#define WALL (1)

int T;

int N, M, C;

// MAP[r][c] = 격자 정보
// 0  : 이동 가능한 길
// -1 : 벽 또는 격자 밖
int MAP[MAX][MAX];

// tmpMAP은 BFS에서 승객 위치를 임시로 표시하거나 MAP 복사용으로 사용
int tmpMAP[MAX][MAX];

// visit[r][c] = BFS 방문 여부 및 거리 저장
// 시작점을 1로 두기 때문에 실제 거리는 visit - 1
int visit[MAX][MAX];

// 좌표 구조체
struct RC
{
    int r;
    int c;
};

typedef struct RC RC;

// 전기차 현재 위치
RC car;

// BFS 큐
RC queue[MAX * MAX];

// 승객 정보 구조체
struct PEOPLE
{
    int sr;       // 승객 출발 행
    int sc;       // 승객 출발 열
    int er;       // 승객 목적지 행
    int ec;       // 승객 목적지 열
    bool check;   // true면 이미 목적지까지 데려다준 승객
};

typedef struct PEOPLE PEOPLE;

// people[i] = i번 승객 정보
PEOPLE people[MAX * MAX];

// findPeople() 결과 정보
struct INFO
{
    int peopleNumber;   // 선택된 승객 번호
    int distance;       // 전기차에서 승객 출발지까지 거리
};

typedef struct INFO INFO;

// 방향 배열: ↑, →, ↓, ←
// BFS에서는 이 순서 자체가 정답 우선순위를 직접 보장하지는 않으므로,
// 모든 후보를 확인한 뒤 거리/행/열 기준으로 비교한다.
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

/*
    입력 함수

    N : 격자 크기
    M : 승객 수
    C : 초기 배터리

    MAP은 벽을 -1로 바꿔 저장한다.
    격자 밖도 -1로 초기화해서 BFS 경계 처리를 쉽게 한다.

    이후 전기차 초기 위치와 승객 정보를 입력받는다.
*/
void input()
{
    scanf("%d %d %d", &N, &M, &C);

    // 격자 밖을 -1로 초기화
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            MAP[r][c] = -1;

    // 실제 격자 입력
    for (int r = 1; r <= N; r++)
    {
        for (int c = 1; c <= N; c++)
        {
            scanf("%d", &MAP[r][c]);

            // 입력에서 WALL은 1이지만,
            // 코드에서는 벽을 -1로 통일해서 사용
            if (MAP[r][c] == WALL) MAP[r][c] = -1;
        }
    }

    // 전기차 초기 위치
    scanf("%d %d", &car.r, &car.c);

    // 승객 정보 입력
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
    디버그용 맵 출력 함수
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
    현재 전기차 위치에서 가장 가까운 승객을 찾는 함수

    BFS로 모든 도달 가능한 칸을 탐색하면서
    아직 도착하지 않은 승객의 출발지를 만나면 후보로 비교한다.

    선택 기준
    1. 거리 짧은 승객
    2. 행 번호 작은 승객
    3. 열 번호 작은 승객

    반환값
    - peopleNumber : 선택된 승객 번호
    - distance     : 전기차에서 그 승객 출발지까지 거리
*/
INFO findPeople()
{
    int rp, wp;

    // visit 초기화
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            visit[r][c] = 0;

    // tmpMAP에 기본 MAP 복사
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            tmpMAP[r][c] = MAP[r][c];

    // 아직 태우지 않은 승객들의 출발 위치를 tmpMAP에 승객 번호로 표시
    for (int i = 1; i <= M; i++)
    {
        if (people[i].check == true) continue;

        int pr = people[i].sr;
        int pc = people[i].sc;

        tmpMAP[pr][pc] = i;

        // 전기차가 이미 승객 출발지 위에 있는 경우
        // 바로 해당 승객을 선택하고 거리는 0
        if (pr == car.r && pc == car.c)
        {
            INFO result = { i, 0 };
            return result;
        }
    }

    // BFS 시작
    rp = wp = 0;

    queue[wp].r = car.r;
    queue[wp++].c = car.c;

    visit[car.r][car.c] = 1;

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

            // 벽이거나 이미 방문한 칸이면 제외
            if (tmpMAP[nr][nc] == -1 || visit[nr][nc] != 0) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;

            // tmpMAP[nr][nc] != 0 이면 승객 출발지
            if (tmpMAP[nr][nc] != 0)
            {
                // 거리, 행, 열 기준으로 더 우선인 승객 선택
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

    // visit은 시작점을 1로 저장했으므로 실제 거리는 -1
    INFO result = { peopleNumber, minDistance - 1 };
    return result;
}

/*
    현재 전기차 위치에서 특정 목적지(er, ec)까지의 최단거리 계산

    일반 BFS로 목적지까지의 거리를 구한다.

    반환값
    - 도달 가능하면 최단거리
    - 도달 불가능하면 -1
*/
int goToDestination(int er, int ec)
{
    int rp, wp;

    // visit 초기화
    for (int r = 1; r <= N; r++)
        for (int c = 1; c <= N; c++)
            visit[r][c] = 0;

    // tmpMAP에 MAP 복사
    for (int r = 0; r <= N + 1; r++)
        for (int c = 0; c <= N + 1; c++)
            tmpMAP[r][c] = MAP[r][c];

    // BFS 시작
    rp = wp = 0;

    queue[wp].r = car.r;
    queue[wp++].c = car.c;

    visit[car.r][car.c] = 1;

    while (rp < wp)
    {
        RC out = queue[rp++];

        // 목적지 도착
        if (out.r == er && out.c == ec)
            return visit[out.r][out.c] - 1;

        for (int k = 0; k < 4; k++)
        {
            int nr = out.r + dr[k];
            int nc = out.c + dc[k];

            // 벽이거나 이미 방문한 칸이면 제외
            if (tmpMAP[nr][nc] == -1 || visit[nr][nc] != 0) continue;

            queue[wp].r = nr;
            queue[wp++].c = nc;

            visit[nr][nc] = visit[out.r][out.c] + 1;
        }
    }

    return -1;
}

/*
    전체 시뮬레이션 함수

    승객 수 M번 반복하면서
    1. 가장 가까운 승객 찾기
    2. 승객 출발지까지 이동 가능/배터리 확인
    3. 전기차 위치를 승객 출발지로 이동
    4. 승객 목적지까지 최단거리 계산
    5. 목적지까지 이동 가능/배터리 확인
    6. 배터리 갱신 및 승객 완료 처리

    반환값
    - 성공 시 남은 배터리
    - 실패 시 -1
*/
int simulate()
{
    for (int i = 0; i < M; i++)
    {
        // 남은 승객 중 가장 가까운 승객의 번호와 거리 찾기
        INFO info = findPeople();

        // 승객을 찾지 못한 경우
        if (info.peopleNumber == -1) return -1;

        // 승객 출발지까지 갈 배터리가 부족한 경우
        // 원본 코드 기준: C <= info.distance면 실패
        // 즉, 도착 후 배터리가 0이 되는 경우도 실패로 처리한다.
        if (C <= info.distance) return -1;

        // 승객 출발지까지 이동하며 배터리 소모
        C -= info.distance;

        int number = info.peopleNumber;

        // 자동차를 승객 출발지로 이동
        car.r = people[number].sr;
        car.c = people[number].sc;

        // 승객 목적지까지 거리 계산
        int capacity = goToDestination(people[number].er, people[number].ec);

        // 목적지에 도달할 수 없거나 배터리가 부족하면 실패
        if (capacity == -1 || C < capacity) return -1;

        // 문제 규칙상 목적지까지 이동 성공 시
        // 이동 거리의 2배가 충전된다.
        //
        // 실제로는:
        // C = C - capacity + capacity * 2
        // 이므로 결과적으로 C += capacity 와 같다.
        C += capacity;

        // 자동차를 목적지로 이동
        car.r = people[number].er;
        car.c = people[number].ec;

        // 해당 승객 처리 완료
        people[number].check = true;
    }

    return C;
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        printf("%d\n", simulate());
    }

    return 0;
}