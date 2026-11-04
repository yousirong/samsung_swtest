/*
	[코드트리] 2026 상반기 오후 1번 - 아기 고래의 첫 항해
	https://www.codetree.ai/training-field/frequent-problems   ("아기 고래" 검색)
	[확인 필요] 문제 개별 주소(slug)를 찾지 못해 기출 목록 주소를 달아 두었다.

	■ 문제 요약 (코드에서 읽어 낸 규칙이다. 세부 조건은 문제 본문과 대조가 필요하다)
	  N x N 바다(0은 바다, 1은 바위)에서 아기 고래가 (R, C)에서 방향 D로 출발한다.
	  방향은 1 위, 2 아래, 3 왼쪽, 4 오른쪽이다. 지나간 칸은 "탐험한 칸"이 된다.
	  아래를 반복하며 고래가 지나간 좌표를 하나씩 출력한다.

	    1) 인접 탐험 : 지금 방향 기준으로 앞 -> 왼쪽 -> 오른쪽 -> 뒤 순서로 보며,
	       아직 탐험하지 않은 바다 칸이 있으면 그쪽으로 한 칸 가고 방향도 그쪽으로 바꾼다.
	    2) 이동 : 네 방향 모두 탐험할 곳이 없으면, 가장 가까운 "아직 탐험하지 않은 바다"로 간다.
	       (탐험한 바다는 지나갈 수 있다. 거리가 같으면 행이 작은 칸, 열이 작은 칸)
	       도착한 칸으로 들어온 방향이 새 방향이 된다.
	    3) 갈 곳이 아예 없으면 끝난다.

	■ 방향 표 (changeDirection)
	  "앞 -> 왼쪽 -> 오른쪽 -> 뒤"를 방향마다 미리 적어 둔 표다.
	      위(1)    를 볼 때 : 위, 왼쪽, 오른쪽, 아래    -> 1, 3, 4, 2
	      아래(2)  를 볼 때 : 아래, 오른쪽, 왼쪽, 위    -> 2, 4, 3, 1
	      왼쪽(3)  을 볼 때 : 왼쪽, 아래, 위, 오른쪽    -> 3, 2, 1, 4
	      오른쪽(4)을 볼 때 : 오른쪽, 위, 아래, 왼쪽    -> 4, 1, 2, 3
	  (아래를 볼 때의 "왼쪽"은 화면 기준 오른쪽이다. 고래 기준으로 생각해야 한다)

	■ 가장 가까운 바다 찾기 (getNextStep)
	  BFS로 퍼지면서 큐에 (좌표, 거리, 들어온 방향)을 담는다.
	  그중 아직 탐험하지 않은 칸 가운데 (거리, 행, 열)이 가장 작은 칸을 고른다(isPriority).
	  들어온 방향은 바로 앞 칸과의 차이로 구한다.

	■ 주의할 점
	  [확인 필요] 가장 가까운 칸을 고르는 for문이 BFS의 while 안에 있어서,
	              큐에서 하나 꺼낼 때마다 지금까지 담긴 큐 전체를 다시 훑는다.
	              결과는 while 밖에서 한 번 훑는 것과 같지만(매번 ret을 새로 계산한다)
	              시간이 (칸 수)^2 로 늘어난다. N이 크고 2단계가 자주 나오면 느릴 수 있다.
	  - 반복 횟수 상한은 N x N 이다. 바다 칸을 모두 탐험하면 그 전에 끝난다.
	  - check[][]는 input()에서 초기화하지 않는다. 한 번만 실행하면 문제없다.
	  - printMap은 0부터 돌지만 이 코드는 1부터 쓴다. 디버그용이라 결과와는 무관하다.
*/
#include <stdio.h>	                     // scanf, printf, putchar 사용

#define MAX (50+5)                       // N 최대 50 + 여유
#define INF (0x7fff0000)                 // 아주 큰 값
#define ROCK (1)                         // 바위

#define UP (1)                           // 위
#define DOWN (2)                         // 아래
#define LEFT (3)                         // 왼쪽
#define RIGHT (4)                        // 오른쪽

int T;                                   // 테스트 케이스 수

int N, R, C, D;                          // 바다 크기, 고래 행, 열, 방향

int MAP[MAX][MAX];                       // 바다(0) / 바위(1)
int check[MAX][MAX];                     // 탐험한 칸 표시

// 0, 상, 하, 좌, 우
int dr[] = { 0,-1,1,0,0 };               // 방향별 행 변화량 (1번부터 사용)
int dc[] = { 0,0,0,-1,1 };               // 방향별 열 변화량

// D = 1 -> 1, 3, 4, 2
// D = 2 -> 2, 4, 3, 1
// D = 3 -> 3, 2, 1, 4
// D = 4 -> 4, 1, 2, 3

int changeDirection[5][5] = {            // [현재 방향][순서] = 볼 방향
	{0, 0, 0, 0, 0},
	{0, 1, 3, 4, 2},
	{0, 2, 4, 3, 1},
	{0, 3, 2, 1, 4},
	{0, 4, 1, 2, 3},
};

struct RCD                               // BFS 원소
{
	int r;                               // 행
	int c;                               // 열
	int depth;                           // 거리
	int dir;                             // 들어온 방향
};

RCD queue[MAX * MAX];                    // BFS 큐
int rp, wp;                              // 큐 위치

void input()
{
	scanf("%d %d %d %d", &N, &R, &C, &D);   // 바다 크기, 고래 위치, 방향 입력

	for (int r = 1; r <= N; r++)         // 바다 입력
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX][MAX])         // for debug
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

bool explore()                           // 1단계 : 인접 탐험
{
	for (int i = 1; i <= 4; i++)         // 앞, 왼, 오, 뒤
	{
		int dir = changeDirection[D][i];
		// 앞 -> 왼쪽 -> 오른쪽 -> 뒤 순서로 본다.
		int nr, nc;                      // 다음 칸

		nr = R + dr[dir];
		nc = C + dc[dir];

		if (nr<1 || nc<1 || nr>N || nc>N) continue;   // 격자 밖
		if (MAP[nr][nc] == ROCK || check[nr][nc] == 1) continue;
		// 바위와 이미 탐험한 칸은 "인접 탐험" 대상이 아니다.

		check[nr][nc] = 1;               // 탐험 표시

		R = nr;                          // 이동
		C = nc;
		D = dir;                         // 방향 변경

		return true;                     // 성공
	}
	return false;                        // 인접 탐험 불가
}

bool isPriority(RCD a, RCD b)            // a가 더 우선이면 true
{
	if (a.depth != b.depth) return a.depth < b.depth;   // 거리가 짧은 쪽
	if (a.r != b.r) return a.r < b.r;                   // 행이 작은 쪽
	return a.c < b.c;                                   // 열이 작은 쪽
}

RCD getNextStep(int sr, int sc) //BFS
{
	RCD ret;                             // 결과
	int visit[MAX][MAX] = { 0 };         // 방문 표시

	rp = wp = 0;                         // 큐 초기화

	queue[wp].r = sr;                    // 출발 칸 넣기
	queue[wp].c = sc;
	queue[wp++].depth = 0;

	visit[sr][sc] = 1;                   // 방문 표시

	int priorityDir[] = { LEFT, DOWN, RIGHT, UP };   // 퍼지는 순서
	while (rp < wp)                      // 큐가 빌 때까지
	{
		RCD out = queue[rp++];           // 하나 꺼내기

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[priorityDir[i]];
			nc = out.c + dc[priorityDir[i]];

			if (nr<1 || nc<1 || nr>N || nc>N)continue;   // 격자 밖
			if (visit[nr][nc] == 1 || MAP[nr][nc] == ROCK) continue;
			// 이동할 때는 이미 탐험한 바다도 지나갈 수 있다 (바위만 막힌다).

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp].c = nc;
			queue[wp].depth = out.depth + 1;   // 거리

			int dir = 0;                 // 들어온 방향
			if (nr - out.r == -1) dir = UP;
			else if (nr - out.r == 1) dir = DOWN;
			else if (nc - out.c == 1) dir = RIGHT;
			else dir = LEFT;

			queue[wp++].dir = dir;       // 방향 저장
			visit[nr][nc] = 1;           // 방문 표시
		}

		ret.r = ret.c = ret.depth = INF;
		// [확인 필요] 이 고르기가 while 안에 있어 큐를 매번 다시 훑는다. 결과는 같지만 느리다.
		for (int i = 0; i < wp; i++)     // 큐에 담긴 칸 전부
		{
			RCD tmp = queue[i];

			int r, c;
			r = tmp.r;
			c = tmp.c;

			if (check[r][c] == 1)continue;   // 이미 탐험한 칸 제외

			if (isPriority(tmp, ret) == true)   // 더 우선이면
				ret = tmp;               // 갱신
		}

	}
	return ret;                          // 반환 (없으면 INF)
}

void simulate()
{
	check[R][C] = 1;                     // 출발 칸 탐험

	printf("%d %d\n", R, C);             // 출발 좌표 출력

	for (int k = 0; k < N * N; k++)      // 최대 N*N번
	{
		// 1단계 : 인접 탐험
		bool isExplore = explore();
		if (isExplore == true)
		{
			printf("%d %d\n", R, C);     // 좌표 출력
			continue;
		}

		// 2단계 : 가장 가까운 바다로 이동
		RCD next = getNextStep(R, C);

		if (next.r == INF) return;
		// 탐험할 바다가 더 없으면 끝

		R = next.r;                      // 이동
		C = next.c;
		D = next.dir;                    // 들어온 방향이 새 방향

		check[R][C] = 1;                 // 탐험 표시

		printf("%d %d\n", R, C);         // 좌표 출력
		//printMap(check);
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션 + 출력
	}
	return 0;                            // 프로그램 정상 종료
}
