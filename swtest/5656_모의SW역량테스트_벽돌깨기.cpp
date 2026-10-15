/*
	[SWEA] 5656 - [모의 SW 역량테스트] 벽돌 깨기
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRQm6qfL0DFAUo

	■ 문제 요약
	  W x H 판(W 2 ~ 12, H 2 ~ 15)에 벽돌(숫자 1 ~ 9)이 있다. 구슬을 N번(1 ~ 4) 위에서 떨어뜨린다.
	    - 구슬은 고른 열의 맨 위 벽돌에 맞는다
	    - 맞은 벽돌은 깨지면서 상하좌우로 (숫자 - 1)칸 안의 벽돌도 함께 깬다 (연쇄)
	    - 다 깨진 뒤 빈 칸 위의 벽돌은 아래로 떨어진다
	  남는 벽돌 수의 최솟값을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 구슬마다 떨어뜨릴 열을 DFS로 전부 고른다 (최대 12^4 = 20,736가지)
	  DFS(L, map) : L번째 구슬을 떨어뜨릴 차례, 지금 판이 map
	    열마다
	      1) check : 그 열의 맨 위 벽돌 행 (없으면 -1)
	      2) 판을 tmpMAP에 복사하고 crash(깨기) -> move(떨어뜨리기)
	      3) DFS(L + 1, tmpMAP)
	  구슬 N개를 다 썼으면 남은 벽돌 수로 최솟값 갱신.
	  판을 복사해서 넘기므로 되돌리기(원상복구)가 필요 없다.

	■ crash : 깨진 벽돌에서 다시 crash를 부르는 재귀 (연쇄)
	  자기 칸을 먼저 0으로 만들고, 네 방향으로 1 ~ (size - 1)칸 안의 벽돌마다 crash를 부른다.
	  먼저 0으로 만들어 두므로 서로를 다시 부르며 끝없이 도는 일은 없다.

	■ move : 열마다 아래에서부터 벽돌을 모아 맨 아래부터 다시 쌓는다

	■ 주의할 점
	  - [버그] crash()가 연쇄를 찾을 때 지금 판(map)이 아니라 처음 입력 판(전역 MAP)을 본다.
	              if (MAP[nr][nc] > 0)crash(nr, nc, map);
	           첫 구슬은 map이 MAP과 같아서 맞지만, 둘째 구슬부터는 벽돌이 떨어져 위치가 바뀌었다.
	           처음엔 빈 칸이었다가 벽돌이 떨어져 들어온 칸을 연쇄에서 빠뜨린다.
	           (재현 : N 2, W 3, H 6
	                     0 0 0 / 0 0 0 / 2 0 2 / 0 1 0 / 2 0 0 / 0 1 0
	                   -> 3열, 1열 순서로 떨어뜨리면 다 깨진다 (답 0). 이 코드는 2)
	           -> if (map[nr][nc] > 0)crash(nr, nc, map);
	           벽(-1) 검사도 MAP으로 하지만 테두리는 바뀌지 않으므로 그쪽은 문제없다.
	  - 빈 열(sr == -1)을 고르면 구슬이 아무것도 깨지 않는다. 판을 그대로 다음 단계로 넘긴다.
	  - 테두리를 -1로 둘러 crash의 범위 검사를 대신한다.
	  - !!map[r][c] 는 0이 아니면 1, 0이면 0 (벽돌 칸 수 세기).
	  - output()은 디버그 출력용이다.
*/
#include <stdio.h>

#define MAX (15+5)

int T;
int N, W, H;        // 구슬 수, 가로, 세로
int MAP[MAX][MAX];  // 처음 입력 판. 테두리는 -1
int MINANS;

// 순서대로 왼쪽, 위, 오른쪽, 아래
int dr[] = { 0,-1,0,1 };
int dc[] = { -1,0,1,0 };


void input()
{
	scanf("%d %d %d", &N, &W, &H);

	for (int r = 0; r <= H + 1; r++)
		for (int c = 0; c <= W + 1; c++)
			MAP[r][c] = -1;

	for (int r = 1; r <= H; r++)
		for (int c = 1; c <= W; c++)
			scanf("%d", &MAP[r][c]);
}

void output(int map[MAX][MAX])   // for debug
{
	for (int r = 1; r <= H; r++)
	{
		for (int c = 1; c <= W; c++)
			printf("%d", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void copy(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 0; r <= H + 1; r++)
		for (int c = 0; c <= W + 1; c++)
			copy[r][c] = original[r][c];
}

// column 열의 맨 위 벽돌 행 (벽돌이 없으면 -1)
int check(int column, int map[MAX][MAX])
{
	for (int row = 1; row <= H; row++)
		if (map[row][column]) return row;
	return -1;
}

// (sr, sc) 벽돌을 깨고, 범위 안의 벽돌도 연쇄로 깬다
void crash(int sr, int sc, int map[MAX][MAX])
{
	int size = map[sr][sc];

	map[sr][sc] = 0;   // 먼저 비워 두어 다시 불려도 아무 일 없게

	for (int dir = 0; dir < 4; dir++)
	{
		for (int i = 1; i < size; i++)   // (size - 1)칸까지
		{
			int nr, nc;

			nr = sr + dr[dir] * i;
			nc = sc + dc[dir] * i;

			if (MAP[nr][nc] == -1) break;               // 벽

			if (MAP[nr][nc] > 0)crash(nr, nc, map);     // [버그] MAP이 아니라 map을 봐야 한다
		}
	}
}

// 빈 칸 위의 벽돌을 아래로 떨어뜨린다
void move(int map[MAX][MAX])
{
	for (int c = 1; c <= W; c++)
	{
		int list[MAX] = { 0 };
		int lcnt = 0;

		// 아래에서부터 벽돌만 모은다
		for (int r = H; r >= 1; r--)
		{
			if (map[r][c])
			{
				list[lcnt++] = map[r][c];
				map[r][c] = 0;
			}
		}

		// 맨 아래부터 다시 쌓는다
		int sr = H;
		for (int l = 0; l < lcnt; l++) map[sr--][c] = list[l];
	}
}

// 남은 벽돌 수
int count(int map[MAX][MAX])
{
	int sum = 0;
	for (int r = 1; r <= H; r++)
		for (int c = 1; c <= W; c++)
			sum += !!map[r][c];

	return sum;
}

// L번째 구슬을 떨어뜨릴 차례, 지금 판이 map
void DFS(int L, int map[MAX][MAX])
{
	if (L == N)   // 구슬을 다 썼다
	{
		int tmp = count(map);
		if (tmp < MINANS) MINANS = tmp;
		return;
	}

	for (int col = 1; col <= W; col++)
	{
		int sr = check(col, map);
		if (sr == -1)   // 빈 열 : 깨지는 것 없이 다음 구슬로
		{
			DFS(L + 1, map);
			continue;
		}
		int tmpMAP[MAX][MAX] = { 0 };

		// 복사본에서 깨고 떨어뜨린다 -> map은 그대로라 되돌릴 필요가 없다
		copy(tmpMAP, map);
		crash(sr, col, tmpMAP);
		move(tmpMAP);

		DFS(L + 1, tmpMAP);
	}
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		MINANS = 0x7fff0000;

		DFS(0, MAP);

		printf("#%d %d\n", tc, MINANS);
	}
	return 0;
}