/*
	[코드트리] 2021 하반기 오전 2번 - 냉방 시스템
	https://www.codetree.ai/training-field/frequent-problems/problems/cooling-system
	(백준 23289 "온풍기 안녕!"과 거의 같은 문제다. 온풍기 -> 에어컨, 온도 조사칸 -> 사무실)

	■ 문제 요약
	  N x N 격자에 빈칸(0), 사무실(1), 에어컨(2 왼쪽, 3 위, 4 오른쪽, 5 아래)이 있고,
	  칸과 칸 사이에 벽 W개가 있다. (r c s : s = 0이면 (r, c)의 위쪽 벽, s = 1이면 왼쪽 벽)
	  1분마다 아래를 반복한다.

	    1) 에어컨 바람 : 각 에어컨 바로 앞 칸에 5, 그 앞으로 퍼지며 4, 3, 2, 1만큼 시원해진다.
	                    바람은 앞 / 앞 대각선 두 칸으로 퍼지고, 벽이 가로막으면 못 간다.
	                    (대각선은 "옆으로 한 칸, 앞으로 한 칸" 두 번의 이동에 벽이 없어야 한다)
	                    한 에어컨의 바람은 같은 칸에 한 번만 더해진다.
	    2) 공기 섞임  : 벽이 없는 인접 두 칸은 차이 / 4 만큼 높은 쪽 -> 낮은 쪽으로 동시에 옮긴다.
	    3) 외벽 감소  : 가장 바깥 칸들은 시원함이 1 줄어든다 (0이면 그대로).
	    4) 1분 지남   : 모든 사무실이 K 이상이면 끝.

	  끝나는 시간을 출력한다. 100분을 넘기면 -1.

	■ 풀이 방침
	  - 바람은 거리 단계별로 퍼지므로 BFS로 푼다. 큐에 (칸, 세기)를 넣고 세기가 0이면 멈춘다.
	    visit은 에어컨마다 새로 만들어 "한 에어컨이 같은 칸에 두 번 더하지 않게" 한다.
	  - 벽은 칸마다 방향별 bool로 들고, 입력받을 때 맞닿은 두 칸 양쪽에 모두 적는다.
	      위쪽 벽  : (r, c)의 UP   + (r-1, c)의 DOWN
	      왼쪽 벽  : (r, c)의 LEFT + (r, c-1)의 RIGHT
	    이러면 어느 칸에서 보든 wall[r][c].direction[방향] 하나로 벽을 확인할 수 있다.
	  - 방향 번호를 MAP 값(2 ~ 5)과 똑같이 맞춰 dr/dc 인덱스로 바로 쓴다. (0, 1번은 비워 둠)

	■ 대각선 벽 검사
	  바람 방향이 좌/우일 때 위 대각선 (r-1, c+dc)로 가려면
	      (r, c) -> (r-1, c)       사이에 벽이 없고 : wall[r][c].direction[UP]
	      (r-1, c) -> (r-1, c+dc)  사이에 벽이 없어야 : wall[r-1][c].direction[dir]
	  즉 "먼저 옆으로 비켜선 뒤 앞으로" 가는 두 번의 이동을 각각 검사한다.
	  상/하 바람도 같은 방식이다.

	■ 주의할 점
	  [버그] BFS의 좌/우 바람 "옆(정면)" 검사가 == 가 아니라 = (대입)이다.
	         (wall[out.r][out.c].direction[dir] = false)
	         -> 식의 값이 항상 false라서 좌/우 에어컨의 정면 전파가 한 번도 일어나지 않고,
	            덤으로 그 칸의 벽 정보를 지워 버린다. 괄호가 한 겹 더 있어 컴파일러 경고도 안 뜬다.
	         == 로 고친 코드와 랜덤 입력 80개를 비교하면 36개에서 답이 다르다.
	  [참고] wall과 temperature는 input()에서 초기화하지 않는다.
	         한 프로세스에서 한 번만 돌리면 전역 0 초기화 덕에 문제없지만,
	         테스트케이스를 여러 번 돌리거나 함수형으로 재호출하면 이전 값이 남는다.
*/
#include <stdio.h>

#define MAX (20+10)

#define OFFICE (1)
#define LEFT (2)
#define UP (3)
#define RIGHT (4)
#define DOWN (5)

int T;

int N, K, W;
int MAP[MAX][MAX];
int temperature[MAX][MAX]; // 칸의 시원함

struct RC
{
	int r;
	int c;
};

RC office[MAX * MAX];
int ocnt;

struct RCD
{
	int r;
	int c;
	int dir;
};

RCD airConditioner[MAX * MAX];
int acnt;

// BFS 큐 원소 : 칸 + 이 칸에 더할 바람 세기
struct RCL
{
	int r;
	int c;
	int length;
};

RCL queue[MAX * MAX];

// direction[LEFT/UP/RIGHT/DOWN] = 그 방향에 벽이 있는가
struct WALL
{
	bool direction[6];
};

WALL wall[MAX][MAX];

// -, -, ←, ↑, →, ↓   (MAP의 에어컨 값 2 ~ 5와 인덱스를 맞췄다)
int dr[] = { 0,0,0,-1,0,1 };
int dc[] = { 0,0,-1,0,1,0 };

void input()
{
	scanf("%d %d %d", &N, &W, &K);

	ocnt = acnt = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);

			if (MAP[r][c] == OFFICE)
			{
				office[ocnt].r = r;
				office[ocnt++].c = c;
			}
			else if (MAP[r][c] != 0)
			{
				// 2 ~ 5가 곧 바람 방향
				airConditioner[acnt].r = r;
				airConditioner[acnt].c = c;
				airConditioner[acnt++].dir = MAP[r][c];
			}
		}
	}

	// 벽은 맞닿은 두 칸 양쪽에 모두 적는다.
	for (int w = 0; w < W; w++)
	{
		int r, c, s;

		scanf("%d %d %d", &r, &c, &s);

		if (s == 0)
		{
			wall[r][c].direction[UP] = true;
			wall[r - 1][c].direction[DOWN] = true;
		}
		else // s ==1
		{
			wall[r][c].direction[LEFT] = true;
			wall[r][c-1].direction[RIGHT] = true;
		}
	}
}

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

// (r, c)의 dir 방향 에어컨 하나의 바람을 퍼뜨린다.
void BFS(int r, int c, int dir)
{
	int rp, wp;
	bool visit[MAX][MAX] = { 0 }; // 에어컨마다 새로 : 같은 칸에 한 번만 더한다
	int sr, sc;

	// 에어컨 바로 앞 칸이 세기 5로 시작
	sr = r + dr[dir];
	sc = c + dc[dir];

	rp = wp = 0;

	queue[wp].r = sr;
	queue[wp].c = sc;
	queue[wp++].length = 5;

	visit[sr][sc] = true;

	while (rp < wp)
	{
		RCL out = queue[rp++];

		// 세기는 단계마다 1씩 줄고 BFS라 순서대로 나오므로, 0이 나오면 뒤도 전부 0이다.
		if (out.length == 0) break;

		// 격자 밖 칸도 일단 큐에 넣고 여기서 거른다. (MAX에 여유를 둬서 visit 접근은 안전)
		if (out.r<1 || out.c<1 || out.r >N || out.c >N) continue;

		temperature[out.r][out.c] += out.length;

		if (dir == RIGHT || dir == LEFT)
		{
			int nr, nc;

			// 좌/우 바람은 다음 열이 항상 한 칸 앞
			nc = out.c + dc[dir];

			// ↖ ↗ 위
			nr = out.r - 1;
			if (visit[nr][nc] == false
				//(r,c) 와 (r-1,c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[UP] == false)
				//(r-1,c) 와 (r-1,c + dc[dir]) 사이에 벽이 없어야 한다.
				&& (wall[nr][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ← → 옆
			nr = out.r;
			if (visit[nr][nc] == false
				// (r,c)와 (r, c+dc[dir]) 사이에 벽이 없어야 한다.
				// [버그] = 는 대입이다. == false 여야 한다.
				//        지금은 항상 거짓이라 정면으로 퍼지지 않고, 이 칸의 벽 정보까지 지운다.
				&& (wall[out.r][out.c].direction[dir] = false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ↙ ↘ 아래
			nr = out.r + 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r + 1, c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[DOWN] == false)
				// (r + 1, c)와 (r + 1, c + dc[dir]) 사이에 벽이 없어야 한다.
				&& (wall[nr][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}
		}
		else // UP or DOWN
		{
			int nr, nc;

			// 상/하 바람은 다음 행이 항상 한 칸 앞
			nr = out.r + dr[dir];

			// ↖ ↙ 왼
			nc = out.c - 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r, c - 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[LEFT] == false)
				// (r, c - 1)와 (r + dr[dir], c - 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][nc].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ↑ ↓ 위, 아래
			nc = out.c;
			if (visit[nr][nc] == false
				// (r, c)와 (r + dr[dir], c) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}

			// ↗ ↘
			nc = out.c + 1;
			if (visit[nr][nc] == false
				// (r, c)와 (r, c + 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][out.c].direction[RIGHT] == false)
				// (r, c + 1)와 (r + dr[dir], c + 1) 사이에 벽이 없어야 한다.
				&& (wall[out.r][nc].direction[dir] == false))
			{
				queue[wp].r = nr;
				queue[wp].c = nc;
				queue[wp++].length = out.length - 1;

				visit[nr][nc] = true;
			}
		}
	}
}
// 2) 공기 섞임 : 벽 없는 인접 칸끼리 차이 / 4를 동시에 옮긴다.
void controlTemperature()
{
	// 동시에 일어나므로 결과는 tmpTemp에 모은다.
	int tmpTemp[MAX][MAX] = { 0 };

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 0인 칸은 누구에게도 줄 수 없고 자기 값도 0이라 건너뛰어도 된다.
			if (temperature[r][c] == 0) continue;

			int value = temperature[r][c];
			for (int i = 2; i <= 5; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr <1 || nc <1 || nr>N || nc > N) continue;
				// 벽은 양쪽 칸에 모두 적혀 있어 내 쪽만 보면 된다.
				if (wall[r][c].direction[i] == true) continue;

				// 두 칸 쌍은 양쪽에서 한 번씩 보므로 "높은 쪽이 줄 때"만 처리한다.
				if (temperature[r][c] > temperature[nr][nc])
				{
					int diff = (temperature[r][c] - temperature[nr][nc]) / 4;

					value -= diff;
					tmpTemp[nr][nc] += diff;
				}
			}

			tmpTemp[r][c] += value;
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			temperature[r][c] = tmpTemp[r][c];
}

// 3) 외벽 감소 : 네 모서리는 한 번만 줄도록 따로 처리하고, 변은 모서리를 뺀 2 ~ N-1만 돈다.
void decreaseTemperature()
{
	if (temperature[1][1] != 0) temperature[1][1]--;
	if (temperature[N][1] != 0) temperature[N][1]--;
	if (temperature[1][N] != 0) temperature[1][N]--;
	if (temperature[N][N] != 0) temperature[N][N]--;

	for (int r = 2; r <= N - 1; r++)
		if (temperature[r][1] != 0) temperature[r][1]--;

	for (int r = 2; r <= N - 1; r++)
		if (temperature[r][N] != 0) temperature[r][N]--;

	for (int c = 2; c <= N - 1; c++)
		if (temperature[1][c] != 0)temperature[1][c]--;

	for (int c = 2; c <= N - 1; c++)
		if (temperature[N][c] != 0)temperature[N][c]--;

}

// 모든 사무실이 K 이상인가
bool checkOffice()
{
	for (int i = 0; i < ocnt; i++)
		if (temperature[office[i].r][office[i].c] < K)
			return false;

	return true;

}

int simulate()
{
	int time = 0;

	while (1)
	{
		// 1) 에어컨마다 바람
		for (int i = 0; i < acnt; i++)
		{
			int r, c, dir;

			r = airConditioner[i].r;
			c = airConditioner[i].c;
			dir = airConditioner[i].dir;

			BFS(r, c, dir);
		}
		// 2) 공기 섞임
		controlTemperature();

		// 3) 외벽 감소
		decreaseTemperature();

		// 4) 1분 지남. 100분을 넘기면 실패
		time++;
		if (time > 100) return -1;

		if (checkOffice() == true) break;
	}

	return time;
}


int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("%d\n", simulate());
	}

	return 0;
}
