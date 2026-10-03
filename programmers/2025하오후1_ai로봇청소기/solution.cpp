/*
	[코드트리] 2025 하반기 오후 1번 - AI 로봇청소기

	[프로그래머스 제출용]  원본 : swtest/코드트리_2025_하반기오후1번_ai로봇청소기.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것,
	그리고 원본 버그 4개를 고친 것만 다르다 (아래 "원본 버그" 참고).
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/ai-robot/description

	■ 문제 요약
	  N x N 격자의 각 칸에는 먼지 양이 적혀 있고, -1은 물건(지나갈 수 없음)이다.
	  로봇청소기 K대가 있고, L번 테스트한다. 한 번의 테스트는 아래 순서다.

	    1) 이동 : 1번 청소기부터 차례로, 가장 가까운 "먼지 있는 칸"으로 간다.
	       물건과 다른 청소기는 지나갈 수 없다. 거리가 같으면 행이 작은 칸, 열이 작은 칸.
	       지금 칸에 이미 먼지가 있으면 움직이지 않는다. 갈 곳이 없으면 제자리에 있는다.
	    2) 청소 : 1번 청소기부터, 자기 칸과 상하좌우 중 세 방향(한 방향은 뺀다)을 청소한다.
	       칸마다 최대 20까지 먼지를 지운다.
	       어느 방향을 뺄지는 "청소할 수 있는 먼지 합"이 가장 큰 쪽으로 정한다
	       (같으면 오른쪽 -> 아래 -> 왼쪽 -> 위 순).
	    3) 축적 : 먼지가 있는 칸마다 5씩 늘어난다.
	    4) 확산 : 먼지가 0인 칸은 상하좌우 먼지 합의 1/10(몫)만큼 먼지가 생긴다(동시에).
	    5) 출력 : 격자 전체 먼지 합을 출력한다.

	■ 풀이 방침
	  - 테두리를 WALL(-1)로 두어 격자 밖과 물건을 같은 값으로 처리한다.
	  - 이동은 BFS로 가장 가까운 먼지 칸을 찾는다. 같은 거리면 행 -> 열이 작은 칸을 고른다.
	    다른 청소기 위치는 check[][]에 찍어 두고 통과하지 못하게 한다.
	  - 청소 방향은 "다섯 칸(자기 + 상하좌우) 합에서 빠지는 한 칸을 뺀 값"이 가장 큰 쪽이다.
	    total에 다섯 칸의 min(먼지, 20)을 모두 더해 두고, 방향마다 반대쪽 한 칸을 뺀 값을 비교한다.
	  - 확산은 "동시에"이므로 tmpMAP에 모았다가 한 번에 더한다.

	■ 원본 버그 4개 — 이 사본에서는 모두 고쳤다 (고친 자리마다 // [버그수정] 표시)
	  [버그 1] input()의 scanf("%d", MAP[r][c]) 에 & 가 빠져 입력을 읽자마자 죽었다.
	           -> 이 사본은 scanf 대신 인자로 받으므로 자연히 사라졌다.
	  [버그 2] clean()이 이웃 칸을 0 밑으로 깎았을 때 자기 칸을 0으로 만들었다 (먼지가 음수로 남음).
	           -> MAP[nr][nc] = 0;
	  [버그 3] clean()이 자기 칸을 청소하지 않았다.
	           -> 자기 칸도 최대 20 지운다 (getDirection이 자기 칸까지 계산에 넣는 것과 맞춘다).
	  [버그 4] getPosition()이 갈 곳을 못 찾았을 때 { sc, sc } 를 돌려줬다.
	           -> { sr, sc } (제자리)
	  swtest/ 원본은 기록용이라 버그를 그대로 두고 [버그] 주석으로만 표시해 두었다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (30+5)
#define WALL (-1)
#define INF (0x7fff0000)

int T;
int N, K, L; // 격자 크기, 로봇 청소기의 개수, 테스트 횟수

int MAP[MAX][MAX];
bool check[MAX][MAX]; //청소기 좌표

struct RC
{
	int r;
	int c;
};

RC cleaner[50+5];

RC queue[MAX * MAX];
int rcnt;

int resultList[100 + 10];   // [추가] 테스트마다의 먼지 합 (원본은 바로 printf 했다)
int resultCount;

// 우,하,좌,상
int dr[] = { 0,1,0,-1 };
int dc[] = { 1,0,-1,0 };

// [수정] scanf 대신 인자로 받는다
// board = N x N 먼지 양 (-1은 물건), cleaners[k] = {행, 열} (1부터), l = 테스트 횟수
// (원본의 [버그 1] scanf 의 & 누락은 scanf 자체가 없어지면서 자연히 사라졌다)
void input(const vector<vector<int>>& board, const vector<vector<int>>& cleaners, int l)
{
	N = (int)board.size();        // [수정] scanf("%d %d %d", &N, &K, &L) 대체
	K = (int)cleaners.size();
	L = l;

	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = WALL;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체

	for (int k = 1; k <= K; k++)
	{
		cleaner[k].r = cleaners[k - 1][0];   // [수정] scanf 대체
		cleaner[k].c = cleaners[k - 1][1];
	}

	resultCount = 0;
}

void printMap() // for debug
{
	for (int k = 1; k <= K; k++)
		printf("%d] %d, %d\n", k, cleaner[k].r, cleaner[k].c);
	putchar('\n');

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

RC getPosition(int index)	//BFS
{
	RC ret;
	int rp, wp;
	rp = wp = 0;
	int visit[MAX][MAX] = { 0 };

	int sr = cleaner[index].r;
	int sc = cleaner[index].c;

	if (MAP[sr][sc] > 0) return { sr,sc };
	// 지금 칸에 먼지가 있으면 움직이지 않는다.

	queue[wp].r = sr;
	queue[wp++].c = sc;

	visit[sr][sc] = 1;

	ret.r = ret.c = INF;
	int minDistance = INF;
	while (rp < wp)
	{
		RC out = queue[rp++];

		// 먼지를 찾은 경우 + 다른 청소기가 위치하지 않은 경우
		if (MAP[out.r][out.c] > 0 && check[out.r][out.c] == false)
		{
			if (visit[out.r][out.c] < minDistance)
			{
				minDistance = visit[out.r][out.c];
				ret = out;
			}
			else if (visit[out.r][out.c] == minDistance)
			{
				if (out.r < ret.r) ret = out;
				else if (out.r == ret.r)
				{
					if (out.c < ret.c)
						ret = out;
				}
			}
			continue;
		}

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (MAP[nr][nc] == WALL || check[nr][nc] == true) continue;
			if (visit[nr][nc] != 0) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;	
		}

	}

	if (ret.r == INF) return { sr,sc };	// [버그수정] 원본은 { sc, sc } 였다. 갈 곳이 없으면 제자리

	return ret;
}
int min(int a, int b)
{
	return a < b ? a : b;
}

int getDirection(int index)
{
	RC target = cleaner[index];

	int total = min(MAP[target.r][target.c], 20);
	// 자기 칸 + 상하좌우 다섯 칸에서 지울 수 있는 먼지(칸마다 최대 20)를 모두 더해 둔다.
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;
		nr = target.r + dr[i];
		nc = target.c + dc[i];
		if (MAP[nr][nc] == WALL) continue;

		total += min(MAP[nr][nc], 20);
	}
	int maxDir = -1;
	int maxDust = -1;
	int changeDir[] = { 2,3,0,1 };

	for (int i = 0; i < 4; i++)
	{
		int reserve = changeDir[i];

		int nr, nc;

		nr = target.r + dr[reserve];
		nc = target.c + dc[reserve];

		int dust = total;
		// 방향 i를 바라보면 반대쪽 한 칸은 청소하지 않으므로 그만큼 뺀다.
		if (MAP[nr][nc] != WALL) dust -= min(MAP[nr][nc], 20);

		if (maxDust < dust)
			// "더 클 때만" 갱신 -> 같으면 우, 하, 좌, 상 순으로 앞선 방향이 남는다.
		{
			maxDust = dust;
			maxDir = i;
		}
	}

	return maxDir;
}

void clean(int index)
{
	int changeDir[] = { 2,3,0,1 };

	RC target = cleaner[index];
	int direction= getDirection(index);
	int reserve = changeDir[direction];

	MAP[target.r][target.c] -= 20;	// [버그수정] 원본은 자기 칸을 청소하지 않았다
	if (MAP[target.r][target.c] < 0) MAP[target.r][target.c] = 0;	// [버그수정]

	for (int i = 0; i < 4; i++)
	{
		if (i == reserve) continue;

		int nr, nc;

		nr = target.r + dr[i];
		nc = target.c + dc[i];

		if (MAP[nr][nc] == WALL) continue;

		MAP[nr][nc] -= 20;
		if (MAP[nr][nc] < 0) MAP[nr][nc] = 0;	// [버그수정] 원본은 MAP[target.r][target.c] = 0 이었다
	}
}

void addDust()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == WALL) continue;
			MAP[r][c] += 5;
		}
	}
}

void spreadDust()
{
	int tmpMAP[MAX][MAX] = { 0 };
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] != 0) continue;
			// 먼지가 0인 칸에만 생긴다. (물건 칸은 -1이라 여기서 같이 걸러진다)

			int sum = 0;
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;
				nr = r + dr[i];
				nc = c + dc[i];

				if (MAP[nr][nc] == WALL) continue;

				sum += MAP[nr][nc];
			}
			tmpMAP[r][c] = sum / 10;
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] += tmpMAP[r][c];
}

int getDust()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == WALL)continue;
			sum += MAP[r][c];
		}
	}
	return sum;
}


void simulate()
{
	for (int l = 0; l < L; l++)
	{
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= N; c++)
				check[r][c] = false;

		// 0. 청소기 좌표 체크
		for (int k = 1; k <= K; k++)
			check[cleaner[k].r][cleaner[k].c] = true;

		// 1. 청소기 이동
		for (int k = 1; k <= K; k++)
		{
			RC rc = getPosition(k);

			check[cleaner[k].r][cleaner[k].c] = false;

			cleaner[k].r = rc.r;
			cleaner[k].c = rc.c;

			check[cleaner[k].r][cleaner[k].c] = true;

		}

		// 2. 청소
		for (int k = 1; k <= K; k++) clean(k);

		// 3. 먼지 축적
		addDust();

		// 4. 먼지 확산
		spreadDust();

		// 5. 결과 출력
		resultList[resultCount++] = getDust();   // [수정] printf -> 배열에 담기
	}

}

// [수정] main() -> solution(). 테스트마다 격자 전체 먼지 합을 담아 반환한다.
vector<int> solution(vector<vector<int>> board, vector<vector<int>> cleaners, int l)
{
	input(board, cleaners, l);

	simulate();

	return vector<int>(resultList, resultList + resultCount);
}
