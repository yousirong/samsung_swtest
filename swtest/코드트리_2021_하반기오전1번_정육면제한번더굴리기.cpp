/*
	[코드트리] 2021 하반기 오전 1번 - 정육면체 한번 더 굴리기
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/cube-rounding-again/description
	(백준 23288 "주사위 굴리기 2"와 같은 문제다)

	■ 문제 요약
	  N x N 격자의 각 칸에 1 ~ 6 숫자가 있고, (1, 1)에 정육면체(주사위)가 놓여 있다.
	  처음 상태는 윗면 1, 남쪽 면 2, 동쪽 면 3 (마주 보는 면의 합은 7)이고 오른쪽(동)을 향한다.
	  M번 아래를 반복한다.

	    1) 이동 : 보는 방향으로 한 칸 굴린다. 격자 밖이면 방향을 반대로 바꿔 굴린다.
	    2) 점수 : 도착 칸과 같은 숫자로 상하좌우 연결된 칸 수 x 그 숫자를 얻는다.
	    3) 회전 : 아랫면 > 칸 숫자면 시계 방향 90도, 작으면 반시계 90도, 같으면 그대로.

	  M번 동안 얻은 점수의 합을 출력한다.

	■ 풀이 방침
	  - 점수판을 미리 만든다 : 격자는 바뀌지 않으므로 칸마다 "같은 숫자 덩어리 크기 x 숫자"를
	    처음에 BFS로 한 번만 계산해 scoreBoard에 적어 둔다.
	    BFS가 끝났을 때 큐에 들어간 원소 수(wp)가 곧 덩어리 크기다.
	  - 주사위는 6면을 전개도 모양 그대로 이름 붙여 들고 굴릴 때마다 면을 옮긴다.

	          up                (북쪽 면)
	    left  top  right        (서쪽, 윗면, 동쪽)
	          down              (남쪽 면)
	          bottom            (아랫면)

	  - 방향 전환은 표 3개(반대, 시계, 반시계)로 처리한다.

	■ 주의할 점
	  [수정됨] 예전 코드는 서쪽으로 굴릴 때 moveWest()가 아니라 moveEast()를 불러 답이 틀렸다.
	           지금 simulate()는 else if (dir == WEST) moveWest(); 로 고쳐져 있다.
	  [주석 오류] dr/dc 위 주석이 "-, 동, 서, 남, 북"인데 실제 값은 3번이 북(-1), 4번이 남(+1)이다.
	         #define(NORTH 3, SOUTH 4)과 값은 맞고 주석만 틀렸다.
*/
#include <stdio.h>             // scanf, printf, putchar 사용

#define MAX (20 +5)            // N 최대 20 + 여유

#define EAST (1)               // 동
#define WEST (2)               // 서
#define NORTH (3)              // 북
#define SOUTH (4)              // 남

int T;                         // 테스트 케이스 수
int N, M;                      // 격자 크기, 이동 횟수
int MAP[MAX][MAX];             // 칸의 숫자

bool visit[MAX][MAX];          // BFS 방문 표시
int scoreBoard[MAX][MAX]; // 그 칸에 도착했을 때 얻는 점수

// 전개도 모양으로 6면을 둔다.
struct CUBE
{
	int up;                            // 북쪽 면
	int left; int top; int right;      // 서쪽 면, 윗면, 동쪽 면
	int down;                          // 남쪽 면
	int bottom;                        // 아랫면
};

CUBE cube;                     // 주사위

struct RC                      // 좌표 한 쌍
{
	int r;                     // 행
	int c;                     // 열
};

RC queue[MAX * MAX];           // BFS용 큐

// -, 동, 서, 남, 북
// [주석 오류] 실제 순서는 -, 동, 서, 북, 남 이다. (#define NORTH 3, SOUTH 4와 일치)
int dr[] = { 0,0,0,-1,1 };     // 방향별 행 변화량
int dc[] = { 0,1,-1,0,0 };     // 방향별 열 변화량

void input()
{
	scanf("%d %d", &N, &M);              // 격자 크기, 이동 횟수 입력

	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= N; c++)     // 모든 열
			scanf("%d", &MAP[r][c]);     // 칸 숫자 입력
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%d ", map[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void printCube() // for debug
{
	printf("   %d\n", cube.up);                              // 북쪽 면
	printf("%d %d %d\n", cube.left, cube.top, cube.right);   // 서쪽, 윗면, 동쪽
	printf("   %d\n", cube.down);                            // 남쪽 면
	printf("   %d\n", cube.bottom);                          // 아랫면
}

// 굴리기 4개 : tmp에 굴리기 전 값을 받아 두고 바뀌는 4면만 옮긴다.
// tmp 인덱스 : 0 up, 1 left, 2 top, 3 right, 4 down, 5 bottom

// 동쪽으로 굴림 : 윗면 -> 동, 동 -> 아래, 아래 -> 서, 서 -> 윗면 (up, down은 그대로)
void moveEast()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };   // 굴리기 전 스냅샷

	cube.top = tmp[1];                   // 서쪽 면 -> 윗면
	cube.right = tmp[2];                 // 윗면 -> 동쪽 면
	cube.bottom = tmp[3];                // 동쪽 면 -> 아랫면
	cube.left = tmp[5];                  // 아랫면 -> 서쪽 면
}

// 서쪽으로 굴림 : 윗면 -> 서, 서 -> 아래, 아래 -> 동, 동 -> 윗면
void moveWest()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };   // 굴리기 전 스냅샷

	cube.top = tmp[3];                   // 동쪽 면 -> 윗면
	cube.right = tmp[5];                 // 아랫면 -> 동쪽 면
	cube.bottom = tmp[1];                // 서쪽 면 -> 아랫면
	cube.left = tmp[2];                  // 윗면 -> 서쪽 면
}

// 북쪽으로 굴림 : 윗면 -> 북, 북 -> 아래, 아래 -> 남, 남 -> 윗면 (left, right는 그대로)
void moveNorth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };   // 굴리기 전 스냅샷

	cube.up = tmp[2];                    // 윗면 -> 북쪽 면
	cube.top = tmp[4];                   // 남쪽 면 -> 윗면
	cube.down = tmp[5];                  // 아랫면 -> 남쪽 면
	cube.bottom = tmp[0];                // 북쪽 면 -> 아랫면
}

// 남쪽으로 굴림 : 윗면 -> 남, 남 -> 아래, 아래 -> 북, 북 -> 윗면
void moveSouth()
{
	int tmp[6] = { cube.up, cube.left, cube.top, cube.right, cube.down, cube.bottom };   // 굴리기 전 스냅샷

	cube.up = tmp[5];                    // 아랫면 -> 북쪽 면
	cube.top = tmp[0];                   // 북쪽 면 -> 윗면
	cube.down = tmp[2];                  // 윗면 -> 남쪽 면
	cube.bottom = tmp[4];                // 남쪽 면 -> 아랫면
}

// (r, c)와 같은 숫자로 연결된 덩어리를 찾아 점수(숫자 x 크기)를 덩어리 전체에 적는다.
void BFS(int r, int c)
{
	int number;                          // 덩어리의 숫자
	int rp, wp;                          // rp: 꺼낼 위치, wp: 넣을 위치

	number = MAP[r][c];                  // 시작 칸 숫자

	rp = wp = 0;                         // 빈 큐

	queue[wp].r = r;                     // 시작 행
	queue[wp++].c = c;                   // 시작 열, 삽입 위치 증가

	visit[r][c] = true;                  // 시작 칸 방문 표시

	while (rp < wp)                      // 처리할 칸이 남아 있는 동안
	{
		RC out = queue[rp++];            // 한 칸 꺼낸다

		for (int i = 1; i <= 4; i++)     // 4방향 (1 ~ 4번)
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (nr <1 || nc <1 || nr >N || nc > N) continue;   // 격자 밖

			if (MAP[nr][nc] != number || visit[nr][nc] == true) continue;   // 다른 숫자거나 방문했으면 건너뜀

			queue[wp].r = nr;            // 큐에 넣는다
			queue[wp++].c = nc;

			visit[nr][nc] = true;        // 방문 표시
		}
	}

	// 큐에 들어간 칸들이 곧 덩어리이고, wp가 그 크기다.
	for (int i = 0; i < wp; i++)
	{
		int r, c;                        // 덩어리 칸

		r = queue[i].r;                  // 행
		c = queue[i].c;                  // 열

		scoreBoard[r][c] = number * wp;  // 숫자 x 덩어리 크기
	}
}

// 모든 덩어리의 점수를 미리 계산한다. (격자는 변하지 않으므로 한 번이면 된다)
void makeScoreBoard()
{
	for (int r = 1; r <= N; r++)         // 방문 기록 초기화
		for (int c = 1; c <= N; c++)
			visit[r][c] = false;

	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
		{
			if (visit[r][c] == true) continue;   // 이미 처리한 칸은 건너뜀

			BFS(r, c);                   // 덩어리 점수 계산
		}
	}
	//printMap(scoreBoard);
}

int simulate()
{
	// 동 : 1, 서 : 2, 북 : 3, 남 : 4
	int changeDir[5] = { 0, 2, 1, 4, 3 };            // 반대 방향 표
	int changeClock[5] = { 0, 4, 3, 1, 2 };          // 시계 90도 표
	int changeCounterClock[5] = { 0, 3, 4, 2, 1 };   // 반시계 90도 표

	// 방향 전환 배열 만들기
	// changeDir[EAST] = WEST;
	// changeDir[WEST] = EAST;
	// changeDir[NORTH] = SOUTH;
	// changeDir[SOUTH] = NORTH;

	// changeClock[EAST] = SOUTH;
	// changeClock[WEST] = NORTH;
	// changeClock[NORTH] = EAST;
	// changeClock[SOUTH] = WEST;

	// changeCounterClock[EAST] = NORTH;
	// changeCounterClock[WEST] = SOUTH;
	// changeCounterClock[NORTH] = WEST;
	// changeCounterClock[SOUTH] = EAST;

	int sr, sc, dir;                     // 주사위 위치, 방향

	sr = sc = 1;                         // (1, 1)에서 시작
	dir = EAST;                          // 동쪽을 향한다

	// 처음 상태 : 윗면 1, 남 2, 동 3 -> 나머지는 7에서 뺀 값
	cube.up = 5;                                     // 북쪽 면
	cube.left = 4; cube.top = 1; cube.right = 3;     // 서쪽, 윗면, 동쪽
	cube.down = 2;                                   // 남쪽 면
	cube.bottom = 6;                                 // 아랫면

	int score = 0;                       // 총점

	for (int i = 0; i < M; i++)          // M번 이동
	{
		int nr, nc;                      // 다음 칸

		nr = sr + dr[dir];               // 다음 행
		nc = sc + dc[dir];               // 다음 열

		// 격자 밖이면 반대 방향으로 굴린다. (바뀐 방향은 계속 유지)
		if (nr<1 || nc<1 || nr>N || nc> N)
		{
			dir = changeDir[dir];        // 방향 반전
			nr = sr + dr[dir];           // 반대 방향 다음 행
			nc = sc + dc[dir];           // 반대 방향 다음 열
		}

		if (dir == EAST) moveEast();             // 동쪽으로 굴림
		else if (dir == WEST) moveWest();        // 서쪽으로 굴림
		else if (dir == NORTH) moveNorth();      // 북쪽으로 굴림
		else if (dir == SOUTH) moveSouth();      // 남쪽으로 굴림

		score += scoreBoard[nr][nc];     // 도착 칸 점수 더하기

		// 아랫면과 칸 숫자를 비교해 다음 방향을 정한다.
		int A = cube.bottom;             // 아랫면 숫자
		int B = MAP[nr][nc];             // 칸 숫자

		if (A > B) dir = changeClock[dir];               // 아랫면이 크면 시계 90도
		else if (A < B) dir = changeCounterClock[dir];   // 작으면 반시계 90도

		sr = nr;                         // 위치 갱신
		sc = nc;
	}
	return score;                        // 총점 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		// 격자가 변하지 않으므로 점수판은 한 번만 만든다.
		makeScoreBoard();

		printf("%d\n", simulate());      // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
