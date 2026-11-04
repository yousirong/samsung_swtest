/*
	[코드트리] 2024 하반기 오전 1번 - 미지의 공간 탈출
	https://www.codetree.ai/training-field/frequent-problems/problems/escape-from-the-unknown-space

	■ 문제 요약
	  N x N 평지 위에 한 변이 M인 정육면체(시간의 벽)가 놓여 있다.
	  평지에는 빈칸(0), 장애물(1), 탈출구(4)가 있고 정육면체가 놓인 자리는 3으로 표시된다.
	  정육면체의 다섯 면(동, 서, 남, 북, 윗면)에도 각각 M x M 격자가 있고, 윗면에는 타임머신(2)이 있다.

	  타임머신에서 출발해 정육면체 면을 타고 내려와 평지로 나온 뒤 탈출구까지 가는
	  최소 이동 횟수를 구한다. 이동은 상하좌우 한 칸이고 장애물은 지날 수 없다.

	  여기에 "시간 이상 현상" F개가 있다. 각각 (행, 열, 방향, 속도 v)로 주어지며
	  평지의 그 칸에서 시작해 방향으로 퍼져 나간다. 시작 칸은 0초, 한 칸 갈 때마다 v초씩 늘어난다.
	  해당 칸은 그 시각이 되면 막히므로, 도착 시각이 그보다 빠를 때만 지나갈 수 있다.
	  탈출할 수 없으면 -1을 출력한다.

	■ 풀이 방침 : 면을 잇는 표(next)를 미리 만들고, 평면 BFS처럼 푼다
	  가장 어려운 부분은 "면의 경계를 넘어갈 때 어느 면의 어느 칸으로 가는가"다.
	  이것을 next[면][행][열][방향]에 미리 적어 두면(preprocess),
	  BFS 본체는 평면에서 하는 것과 똑같아진다.

	    - 옆면끼리(동 -> 북 -> 서 -> 남 -> 동)는 좌우로 이어진다.
	    - 윗면과 옆면은 방향에 따라 좌표가 뒤집히기도 한다 (M + 1 - i).
	    - 옆면의 아래 끝(M행)은 평지로 내려간다. 정육면체가 놓인 칸(3)의 바로 바깥 칸과 이어진다.
	      평지에서 정육면체로 들어가는 방향도 같은 표에 반대로 적어 둔다.

	  이렇게 여섯 장의 격자를 한 덩어리로 묶고 나면, 남는 것은
	  "장애물 피하기 + 시간 이상 현상보다 먼저 지나가기"뿐이다.

	■ 시간 이상 현상 퍼뜨리기 (makeTimeWall)
	  시작 칸에 1을 넣고(0초가 아니라 1로 두어 "값이 있음"과 구분한다),
	  방향으로 한 칸씩 가며 v씩 더한 값을 적는다.
	  벽이나 탈출구를 만나거나 평지를 벗어나면 멈춘다.
	  정육면체 칸(3)을 만나면 next 표를 따라 옆면으로 올라가며 계속 퍼진다.

	■ BFS에서 시간을 비교하는 방법
	  visit에는 "이동 횟수 + 1"이 들어간다(시작 칸이 1).
	  그래서 도착 시각은 visit - 1 이고, 마지막에 visit[BOTTOM][출구] - 1 을 답으로 낸다.
	  시간 이상 현상이 있는 칸은 아래 조건일 때만 지나간다.

	      visit[지금] + 1 < TIME_WALL[다음]

	  즉 "그 칸이 막히는 시각보다 먼저 도착"해야 한다.

	■ 주의할 점
	  - 전역 이름 next, end 는 <iterator> / <algorithm> 의 std::next, std::end 와 겹친다.
	    지금은 헤더를 stdio.h만 쓰고 using namespace std; 도 없어서 문제가 없다.
	    프로그래머스 제출용으로 옮길 때는 이름을 바꿔야 한다.
	  - 입력의 시간 이상 현상 좌표는 0-based로 들어오므로 +1 해서 저장한다.
	  - 평지 좌표계와 면 좌표계의 크기가 다르다(N vs M). 경계 검사에서 어느 쪽인지 늘 구분해야 한다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (20+5)                       // N 최대 20 + 여유

#define EAST (0)                         // 동쪽 면
#define WEST (1)                         // 서쪽 면
#define SOUTH (2)                        // 남쪽 면
#define NORTH (3)                        // 북쪽 면
#define TOP (4)                          // 윗면
#define BOTTOM (5)                       // 평지

#define EMPTY (0)                        // 빈칸
#define WALL (1)                         // 장애물
#define TIME_MACHINE (2)                 // 타임머신
#define CUBE (3)                         // 정육면체가 놓인 칸
#define EXIT (4)                         // 탈출구

#define RIGHT (0)                        // →
#define LEFT (1)                         // ←
#define DOWN (2)                         // ↓
#define UP  (3)                          // ↑

int T;                                   // 테스트 케이스 수

int N, M, F;                             // 평지 크기, 정육면체 크기, 이상 현상 수
int MAP[6][MAX][MAX];                    // 면별 격자
int visit[6][MAX][MAX];                  // 면별 방문 (이동 횟수 + 1)

int TIME_WALL[6][MAX][MAX];              // 면별 막히는 시각 (0이면 현상 없음)

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC start, end;                           // 타임머신 위치, 탈출구 위치

struct PRC                               // 면 + 좌표
{
	int p; // plane
	int r;                               // 행
	int c;                               // 열
};

PRC queue[MAX * MAX * 6];                // BFS 큐
PRC next[6][MAX][MAX][4];                // 경계를 넘을 때 도착하는 (면, 행, 열)

struct TIME_INFO                         // 시간 이상 현상 정보
{
	int p;                               // 면
	int r;                               // 행
	int c;                               // 열
	int d;                               // 방향
	int v;                               // 속도
};

TIME_INFO timeInfo[10 + 3];              // 이상 현상 목록

// 우, 좌, 하, 상
int dr[] = { 0,0,1,-1 };                 // 방향별 행 변화량
int dc[] = { 1,-1,0,0 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &F);       // 평지 크기, 정육면체 크기, 현상 수 입력

	//init
	for (int i = 0; i < 6; i++)          // 모든 면
		for (int r = 0; r < MAX; r++)
			for (int c = 0; c < MAX; c++)
				MAP[i][r][c] = visit[i][r][c] = TIME_WALL[i][r][c] = 0;   // 초기화

	for (int r = 1; r <= N; r++)         // 평지 입력
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[BOTTOM][r][c]);   // 칸 값 입력

			if (MAP[BOTTOM][r][c] == EXIT)     // 탈출구면
			{
				end.r = r;               // 위치 저장
				end.c = c;
			}
		}
	}


	for (int i = 0; i < 5; i++)          // 동, 서, 남, 북, 윗면 순서로
	{
		for (int r = 1; r <= M; r++)
		{
			for (int c = 1; c <= M; c++)
			{
				scanf("%d", &MAP[i][r][c]);   // 면 격자 입력

				if (MAP[TOP][r][c] == TIME_MACHINE)   // 윗면의 타임머신이면
				{
					start.r = r;         // 출발 위치 저장
					start.c = c;
				}
			}
		}
	}

	// 시간 이상 현상
	for (int f = 0; f < F; f++)
	{
		int r, c, d, v;                  // 행, 열, 방향, 속도

		scanf("%d %d %d %d", &r, &c, &d, &v);   // 입력

		timeInfo[f].p = BOTTOM;          // 평지에서 시작
		timeInfo[f].r = r + 1;
		// 입력이 0-based라 1-based로 맞춘다.
		timeInfo[f].c = c + 1;
		timeInfo[f].d = d;               // 방향
		timeInfo[f].v = v;               // 속도
	}
}

void printMap(int map[MAX][MAX], int size)	// for debug
{
	for (int r = 1; r <= size; r++)      // 모든 행
	{
		for (int c = 1; c <= size; c++)  // 모든 열
			printf("%2d ", map[r][c]);   // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void printMapAll(int map[6][MAX][MAX]) // for debug
{
	printf("BOTTOM\n");                  // 평지
	printMap(map[BOTTOM], N);

	printf("EAST\n");                    // 동쪽 면
	printMap(map[EAST], N);

	printf("WEST\n");                    // 서쪽 면
	printMap(map[WEST], N);

	printf("SOUTH\n");                   // 남쪽 면
	printMap(map[SOUTH], N);

	printf("NORTH\n");                   // 북쪽 면
	printMap(map[NORTH], N);

	printf("TOP\n");                     // 윗면
	printMap(map[TOP], N);
}

void preprocess()                        // 면 사이 연결 표(next) 만들기
{
	for (int i = 1; i <= M; i++)         // 윗면과 옆면 연결
	{
		// TOP (1, i)에서 ↑ 가면 NORTH (1, M + 1 - i)
		next[TOP][1][i][UP].p = NORTH;
		next[TOP][1][i][UP].r = 1;
		next[TOP][1][i][UP].c = M+1-i;

		// NORTH (1, i)에서 ↑ 가면 TOP (1, M + 1 - i)
		next[NORTH][1][i][UP].p = TOP;
		next[NORTH][1][i][UP].r = 1;
		next[NORTH][1][i][UP].c = M+1-i;

		// ------------------------------------- //

		// TOP (M, i)에서 ↓ 가면 SOUTH (1, i)
		next[TOP][M][i][DOWN].p = SOUTH;
		next[TOP][M][i][DOWN].r = 1;
		next[TOP][M][i][DOWN].c = i;

		// SOUTH (1, i)에서 ↑ 가면 TOP (M, i)
		next[SOUTH][1][i][UP].p = TOP;
		next[SOUTH][1][i][UP].r = M;
		next[SOUTH][1][i][UP].c = i;

		// ------------------------------------- //

		// TOP (i, M)에서 → 가면 EAST (1, M + 1 - i)
		next[TOP][i][M][RIGHT].p = EAST;
		next[TOP][i][M][RIGHT].r = 1;
		next[TOP][i][M][RIGHT].c = M + 1 - i;

		// EAST (1, i)에서 ↑ 가면 TOP (M + 1 - i, M)
		next[EAST][1][i][UP].p = TOP;
		next[EAST][1][i][UP].r = M + 1 - i;
		next[EAST][1][i][UP].c = M;

		// ------------------------------------- //

		// TOP (i, 1)에서 ← 가면 WEST (1, i)
		next[TOP][i][1][LEFT].p = WEST;
		next[TOP][i][1][LEFT].r = 1;
		next[TOP][i][1][LEFT].c = i;

		// WEST (1, i)에서 ↑ 가면 TOP (i, 1)
		next[WEST][1][i][UP].p = TOP;
		next[WEST][1][i][UP].r = i;
		next[WEST][1][i][UP].c = 1;
	}

	// →
	for (int i = 1; i <= M; i++)         // 옆면끼리 오른쪽 연결
	{
		// SOUTH (i, M)에서 → 가면 EAST (i, 1)
		next[SOUTH][i][M][RIGHT].p = EAST;
		next[SOUTH][i][M][RIGHT].r = i;
		next[SOUTH][i][M][RIGHT].c = 1;

		// EAST (i, M)에서 → 가면 NORTH (i, 1)
		next[EAST][i][M][RIGHT].p = NORTH;
		next[EAST][i][M][RIGHT].r = i;
		next[EAST][i][M][RIGHT].c = 1;

		// NORTH (i, M)에서 → 가면 WEST (i, 1)
		next[NORTH][i][M][RIGHT].p = WEST;
		next[NORTH][i][M][RIGHT].r = i;
		next[NORTH][i][M][RIGHT].c = 1;

		// WEST (i, M)에서 → 가면 SOUTH (i, 1)
		next[WEST][i][M][RIGHT].p = SOUTH;
		next[WEST][i][M][RIGHT].r = i;
		next[WEST][i][M][RIGHT].c = 1;
	}

	// ←
	for (int i = 1; i <= M; i++)         // 옆면끼리 왼쪽 연결
	{
		// SOUTH (i, 1)에서 ← 가면 WEST (i, M)
		next[SOUTH][i][1][LEFT].p = WEST;
		next[SOUTH][i][1][LEFT].r = i;
		next[SOUTH][i][1][LEFT].c = M;

		// WEST (i, 1)에서 ← 가면 NORTH (i, M)
		next[WEST][i][1][LEFT].p = NORTH;
		next[WEST][i][1][LEFT].r = i;
		next[WEST][i][1][LEFT].c = M;

		// NORTH (i, 1)에서 ← 가면 EAST (i, M)
		next[NORTH][i][1][LEFT].p = EAST;
		next[NORTH][i][1][LEFT].r = i;
		next[NORTH][i][1][LEFT].c = M;

		// EAST (i, 1)에서 ← 가면 SOUTH (i, M)
		next[EAST][i][1][LEFT].p = SOUTH;
		next[EAST][i][1][LEFT].r = i;
		next[EAST][i][1][LEFT].c = M;
	}

	int index;                           // 옆면 아래 줄의 열 번호

	// BOTTOM, WEST
	index = 1;                           // 서쪽 면은 1열부터
	for (int r = 1; r <= N; r++)         // 위 행부터
	{
		for (int c = 1; c <= N; c++)     // 왼쪽부터 정육면체 칸 찾기
		{
			if (MAP[BOTTOM][r][c] == CUBE)   // 행의 가장 왼쪽 정육면체 칸
			{
				// BOTTOM → WEST
				next[BOTTOM][r][c - 1][RIGHT].p = WEST;
				next[BOTTOM][r][c - 1][RIGHT].r = M;
				next[BOTTOM][r][c - 1][RIGHT].c = index;

				// BOTTOM ← WEST = (↓)
				next[WEST][M][index][DOWN].p = BOTTOM;
				next[WEST][M][index][DOWN].r = r;
				next[WEST][M][index][DOWN].c = c - 1;

				index++;                 // 다음 열

				break;                   // 다음 행으로
			}
		}
	}

	// BOTTOM, EAST
	index = M;                           // 동쪽 면은 M열부터 거꾸로
	for (int r = 1; r <= N; r++)         // 위 행부터
	{
		for (int c = N; c >= 1; c--)     // 오른쪽부터 정육면체 칸 찾기
		{
			if (MAP[BOTTOM][r][c] == CUBE)   // 행의 가장 오른쪽 정육면체 칸
			{
				// EAST ← BOTTOM
				next[BOTTOM][r][c + 1][LEFT].p = EAST;
				next[BOTTOM][r][c + 1][LEFT].r = M;
				next[BOTTOM][r][c + 1][LEFT].c = index;

				// EAST → BOTTOM = (↓)
				next[EAST][M][index][DOWN].p = BOTTOM;
				next[EAST][M][index][DOWN].r = r;
				next[EAST][M][index][DOWN].c = c + 1;

				index--;                 // 다음 열

				break;                   // 다음 행으로
			}
		}
	}

	// BOTTOM, NORTH
	index = M;                           // 북쪽 면은 M열부터 거꾸로
	for (int c = 1; c <= N; c++)         // 왼쪽 열부터
	{
		for (int r = 1; r <= N; r++)     // 위쪽부터 정육면체 칸 찾기
		{
			if (MAP[BOTTOM][r][c] == CUBE)   // 열의 가장 위 정육면체 칸
			{
				// BOTTOM
				//   ↓
				// NORTH = ↓
				next[BOTTOM][r - 1][c][DOWN].p = NORTH;
				next[BOTTOM][r - 1][c][DOWN].r = M;
				next[BOTTOM][r - 1][c][DOWN].c = index;

				// BOTTOM
				//   ↑
				// NORTH = ↓
				next[NORTH][M][index][DOWN].p = BOTTOM;
				next[NORTH][M][index][DOWN].r = r - 1;
				next[NORTH][M][index][DOWN].c = c;

				index--;                 // 다음 열
				break;                   // 다음 열로
			}
		}
	}

	// BOTTOM, SOUTH
	index = 1;                           // 남쪽 면은 1열부터
	for (int c = 1; c <= N; c++)         // 왼쪽 열부터
	{
		for (int r = N; r >= 1; r--)     // 아래쪽부터 정육면체 칸 찾기
		{
			if (MAP[BOTTOM][r][c] == CUBE)   // 열의 가장 아래 정육면체 칸
			{
				// SOUTH
				//   ↑
				// BOTTOM = ↑
				next[BOTTOM][r + 1][c][UP].p = SOUTH;
				next[BOTTOM][r + 1][c][UP].r = M;
				next[BOTTOM][r + 1][c][UP].c = index;

				// SOUTH
				//   ↓
				// BOTTOM = ↓
				next[SOUTH][M][index][DOWN].p = BOTTOM;
				next[SOUTH][M][index][DOWN].r = r + 1;
				next[SOUTH][M][index][DOWN].c = c;

				index++;                 // 다음 열
				break;                   // 다음 열로
			}
		}
	}
}

void makeTimeWall()                      // 시간 이상 현상 퍼뜨리기
{
	for (int f = 0; f < F; f++)          // 현상별
	{
		int p, r, c, d, v;               // 면, 행, 열, 방향, 속도

		p = timeInfo[f].p;
		r = timeInfo[f].r;
		c = timeInfo[f].c;
		d = timeInfo[f].d;
		v = timeInfo[f].v;

		TIME_WALL[p][r][c] = 1;
		// 시작 칸은 1로 둔다 (0은 "현상 없음"을 뜻하므로).

		while (1)                        // 막힐 때까지 퍼짐
		{
			int np, nr, nc;              // 다음 칸

			np = p;                      // 같은 면에서
			nr = r + dr[d];              // 한 칸 진행
			nc = c + dc[d];

			if (p == BOTTOM && MAP[BOTTOM][nr][nc] == CUBE)
			// 평지에서 정육면체를 만나면 next 표를 따라 옆면으로 올라간다.
			{
				np = next[BOTTOM][r][c][d].p;
				nr = next[BOTTOM][r][c][d].r;
				nc = next[BOTTOM][r][c][d].c;
			}
			else if ((p != BOTTOM) && (nr<1 || nc<1 || nr>M || nc>M))   // 면의 경계를 넘으면
			{
				np = next[p][r][c][d].p;     // 연결된 면으로
				nr = next[p][r][c][d].r;
				nc = next[p][r][c][d].c;
			}

			if((np== BOTTOM)&& (nr<1|| nc<1|| nr>N || nc>N)) break;   // 평지 밖이면 멈춤
			if (MAP[np][nr][nc] == WALL) break;                       // 장애물이면 멈춤
			if (nr == end.r && nc == end.c) break;
			// 탈출구에는 시간 이상 현상이 퍼지지 않는다.

			TIME_WALL[np][nr][nc] = TIME_WALL[p][r][c] + v;   // v초 뒤에 막힘

			p = np;                      // 한 칸 진행
			r = nr;
			c = nc;
		}
	}
}

int BFS(int r, int c)                    // 타임머신에서 탈출구까지 최소 이동
{
	int rp, wp;                          // 큐 위치
	rp = wp = 0;                         // 큐 초기화

	queue[wp].p = TOP;                   // 윗면에서 시작
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[TOP][r][c] = 1;                // 시작 칸 방문 (이동 0 + 1)

	while (rp < wp)                      // 큐가 빌 때까지
	{
		PRC out = queue[rp++];           // 하나 꺼내기

		if (out.p == BOTTOM && out.r == end.r && out.c == end.c)   // 탈출구 도착
			return visit[BOTTOM][out.r][out.c] - 1;
			// visit은 "이동 횟수 + 1"이라 1을 빼면 실제 이동 횟수다.

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int np, nr, nc;              // 다음 칸

			np = out.p;                  // 같은 면에서
			nr = out.r +dr[i];           // 한 칸 진행
			nc = out.c +dc[i];

			if (out.p == BOTTOM && MAP[BOTTOM][nr][nc] == CUBE)   // 평지에서 정육면체로 들어가면
			{
				np = next[BOTTOM][out.r][out.c][i].p;   // 옆면으로
				nr = next[BOTTOM][out.r][out.c][i].r;
				nc = next[BOTTOM][out.r][out.c][i].c;
			}
			else if ((out.p != BOTTOM) && (nr < 1 || nc < 1 || nr > M || nc > M))   // 면의 경계를 넘으면
			{
				np = next[out.p][out.r][out.c][i].p;    // 연결된 면으로
				nr = next[out.p][out.r][out.c][i].r;
				nc = next[out.p][out.r][out.c][i].c;
			}

			if((np== BOTTOM) && (nr<1||nc<1|| nr>N|| nc>N)) continue;   // 평지 밖

			if (MAP[np][nr][nc] == WALL || visit[np][nr][nc] != 0)	continue;   // 장애물이거나 방문함

			// 시간의 벽
			if(TIME_WALL[np][nr][nc]!=0
				&& visit[out.p][out.r][out.c] + 1 >= TIME_WALL[np][nr][nc]) continue;
			// 그 칸이 막히는 시각보다 먼저 도착해야 지나갈 수 있다.

			queue[wp].p = np;            // 큐에 넣기
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[np][nr][nc] = visit[out.p][out.r][out.c] + 1;   // 이동 횟수 기록
		}
	}

	return -1;                           // 탈출 불가
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		preprocess();                    // 면 연결 표 만들기

		makeTimeWall();                  // 시간 이상 현상 퍼뜨리기

		//printf("MAP\n"); printMapAll(MAP);

		//printf("TIME WALL\n"); printMapAll(TIME_WALL);


		printf("%d\n", BFS(start.r, start.c));   // 결과 출력

		//printf("visit\n"); printMapAll(visit);
	}

	return 0;                            // 프로그램 정상 종료
}
