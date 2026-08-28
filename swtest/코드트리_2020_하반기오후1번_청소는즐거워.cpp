/*
	[코드트리] 2020 하반기 오후 1번 - 청소는 즐거워
	(백준 20057 "마법사 상어와 토네이도"와 같은 문제다  https://www.acmicpc.net/problem/20057)

	■ 문제 요약
	  N x N 격자의 각 칸에 모래가 쌓여 있다(N은 홀수).
	  토네이도가 격자 정가운데에서 시작해 왼쪽으로 한 칸 이동하고,
	  이후 달팽이(나선) 모양을 그리며 격자 왼쪽 위 칸에 도달할 때까지 움직인다.

	  토네이도가 어떤 칸으로 이동하면 그 칸의 모래가 주변 칸들로 정해진 비율만큼 흩어진다.
	  비율의 합은 45%이고, 남은 모래(α)는 이동 방향 바로 앞 칸으로 간다.
	  격자 밖으로 나간 모래의 총량을 출력한다.

	■ 두 부분으로 나눠 생각한다
	    (1) 토네이도가 지나갈 경로를 미리 만든다        -> makeSnail()
	    (2) 경로를 따라가며 칸마다 모래를 흩뿌린다      -> clean()

	  경로를 먼저 다 만들어 두면 시뮬레이션 본체가 아주 단순해진다.

	■ (1) 달팽이 경로 만들기
	  16번 파일(BOJ 1913 달팽이)과 같은 방식이다.

	    방향 순서 : ← → ↓ → → → ↑ 로 순환 (dr/dc의 0, 1, 2, 3)
	    구간 길이 : 1, 1, 2, 2, 3, 3, ... (두 구간마다 1씩 증가)

	  지나간 좌표를 track[]에 순서대로 쌓아 둔다.
	  그다음 이웃한 두 좌표의 차이를 보고 track[i].dir(그 칸에서 다음 칸으로 갈 때의 방향)를 채운다.
	  좌표를 먼저 만들고 방향을 나중에 계산하는 편이, 만들면서 방향까지 기록하는 것보다 덜 헷갈린다.

	  마지막에 tcnt = N * N 으로 맞추는 이유는, 나선을 도는 루프가 격자 밖으로 한 칸 넘칠 수 있어서
	  실제로 쓸 구간만 N*N개로 잘라 두기 위함이다.

	■ (2) 모래 흩뿌리기
	  비율은 5 x 5 표 ratio[방향][r][c]로 미리 적어 두었다.
	  표의 가운데 (2, 2)가 토네이도가 있는 칸이고, 나머지 칸이 상대 좌표다.

	    dust = 토네이도 칸의 모래 * 비율 / 100    (정수 나눗셈, 버림)

	  각 칸의 몫을 계산해 원래 칸에서 빼 나가고, 다 빼고 남은 것이 α다.
	  α는 이동 방향 바로 앞 칸, 즉 표에서 (2 + dr, 2 + dc) 자리로 간다.

	  중요한 점은 계산과 반영을 분리한 것이다.
	    - 먼저 tmpMAP(5x5)에 각 칸이 받을 양을 모두 계산해 둔다
	    - 그다음 한꺼번에 MAP에 더한다
	  이렇게 해야 흩뿌리는 도중에 바뀐 값이 다음 계산에 섞이지 않는다.

	  반영 단계에서 격자 밖으로 나가는 칸의 몫은 MAP에 더하지 않고 따로 합산해 돌려준다.
	  이 값이 곧 정답이다.

	■ 주의할 점
	  simulate()에서 청소하는 칸은 track[i]가 아니라 track[i+1]이다.
	  토네이도는 "이동한 뒤" 그 칸의 모래를 흩뿌리기 때문이다.
	  방향은 track[i].dir(그 칸에서 나가는 방향)을 쓴다.
*/

#include <stdio.h>

#define MAX (500 + 20)

int T;

int N;
int MAP[MAX][MAX];     // 각 칸의 모래 양
int snail[MAX][MAX];   // 달팽이 순서 번호 (디버그용)

// 토네이도의 경로 한 칸
struct RCD
{
	int r;
	int c;
	int dir;   // 이 칸에서 다음 칸으로 갈 때의 방향
};

RCD track[MAX * MAX];
int tcnt;

// 방향 배열 : ←(0), ↓(1), →(2), ↑(3)
int dr[] = { 0, 1, 0, -1 };
int dc[] = { -1, 0, 1, 0 };

void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

/*
	모래가 흩어지는 비율 표 (단위 %)

	ratio[방향][r][c] 에서 (2, 2)가 토네이도가 있는 칸이고
	r, c는 각각 -2 ~ +2의 상대 좌표에 대응한다.

	한 방향의 합은 45%이고, 나머지 55% 근처(정확히는 나눗셈 후 남은 전부)가 α로 간다.
	α의 자리는 표에 0으로 비워 두고 clean()에서 따로 채운다.

	네 표는 같은 모양을 방향에 맞춰 돌려 놓은 것이다.
*/
// ←, ↓, →, ↑
int ratio[4][5][5] =
{
	{
		{0,  0, 2, 0, 0},
		{0, 10, 7, 1, 0},
		{5,  0, 0, 0, 0},
		{0, 10, 7, 1, 0},
		{0,  0, 2, 0, 0},
	},
	{
		{0,  0, 0,  0, 0},
		{0,  1, 0,  1, 0},
		{2,  7, 0,  7, 2},
		{0, 10, 0, 10, 0},
		{0,  0, 5,  0, 0},
	},
	{
		{0, 0, 2,  0, 0},
		{0, 1, 7, 10, 0},
		{0, 0, 0,  0, 5},
		{0, 1, 7, 10, 0},
		{0, 0, 2,  0, 0},
	},
	{
		{0,  0, 5,  0, 0},
		{0, 10, 0, 10, 0},
		{2,  7, 0,  7, 2},
		{0,  1, 0,  1, 0},
		{0,  0, 0,  0, 0},
	}
};

/*
	토네이도가 지나갈 달팽이 경로를 만든다

	가운데에서 시작해 ← ↓ → ↑ 순서로 돌며, 구간 길이는 1,1,2,2,3,3,... 로 늘어난다.
	좌표를 track[]에 쌓은 뒤, 이웃한 좌표의 차이로 각 칸의 진행 방향을 채운다.
*/
void makeSnail()
{
	int sr, sc, direction;
	int index, size;

	sr = sc = (N + 1) / 2;   // 정가운데에서 시작
	direction = 0;           // 첫 방향은 왼쪽
	index = 1;
	size = 0;

	snail[sr][sc] = index++;
	track[tcnt].r = sr;
	track[tcnt++].c = sc;

	for (int i = 0; i < 2 * N - 1; i++)
	{
		// 두 구간마다 길이가 1씩 늘어난다
		if (i % 2 == 0) size++;

		for (int s = 0; s < size; s++)
		{
			int nr, nc;

			nr = sr + dr[direction];
			nc = sc + dc[direction];

			snail[nr][nc] = index++;

			sr = nr;
			sc = nc;

			track[tcnt].r = nr;
			track[tcnt++].c = nc;
		}

		direction++;

		if (direction == 4) direction = 0;
	}

	// 마지막 구간이 격자 밖으로 한 칸 넘칠 수 있으므로 실제로 쓸 N*N개만 남긴다
	tcnt = N * N;

	// 좌표 차이로 진행 방향을 역산한다.
	// 만들면서 방향을 같이 기록하는 것보다 이렇게 나중에 채우는 편이 실수가 적다.
	for (int i = 0; i < tcnt - 1; i++)
	{
		int r, c, nr, nc;

		r = track[i].r;
		c = track[i].c;
		nr = track[i + 1].r;
		nc = track[i + 1].c;

		if (nc - c == -1) track[i].dir = 0; // 왼쪽
		else if (nc - c == 1) track[i].dir = 2; // 오른쪽
		else if (nr - r == -1) track[i].dir = 3; // 위쪽
		else if (nr - r == 1) track[i].dir = 1; // 아래쪽
	}

	track[tcnt - 1].dir = 0;   // 마지막 칸에서는 더 나가지 않으므로 아무 값이나 무방

	// 아래는 경로를 눈으로 확인할 때만 쓰는 디버그 코드다 (return으로 막아 둠)
	return;
	printMap(snail);
	for (int i = 0; i < tcnt; i++)
		printf("%d] (%d, %d) / %d\n", i, track[i].r, track[i].c, track[i].dir);
}

/*
	(sr, sc)의 모래를 dir 방향 기준 비율대로 흩뿌린다

	  반환값 : 이번에 격자 밖으로 나간 모래의 양
*/
int clean(int sr, int sc, int dir)
{
	int center = MAP[sr][sc];    // 흩뿌리기 전의 모래 양 (비율 계산의 기준)
	int tmpMAP[5][5] = { 0 };    // 각 칸이 받을 양을 먼저 여기에 모은다

	// 1) 비율대로 몫을 계산하고 원래 칸에서 그만큼 뺀다
	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			int tr, tc, dust;

			tr = sr + r - 2;
			tc = sc + c - 2;

			// 비율은 항상 "흩뿌리기 전 값"인 center를 기준으로 한다
			dust = center * ratio[dir][r][c] / 100;

			tmpMAP[r][c] = dust;
			MAP[sr][sc] -= dust;
		}
	}

	// 2) 다 빼고 남은 것이 α다. 이동 방향 바로 앞 칸으로 간다.
	tmpMAP[2 + dr[dir]][2 + dc[dir]] = MAP[sr][sc];
	MAP[sr][sc] = 0;   // 토네이도가 지나간 칸에는 모래가 남지 않는다

	// 3) 계산이 모두 끝난 뒤에 한꺼번에 반영한다
	int ret = 0;
	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			int tr, tc;

			tr = sr + r - 2;
			tc = sc + c - 2;

			// 격자 밖으로 나간 몫은 MAP에 넣지 않고 따로 모은다 (이게 정답이 된다)
			if (tr < 1 || tc < 1 || tr > N || tc > N) ret += tmpMAP[r][c];
			else MAP[tr][tc] += tmpMAP[r][c];
		}
	}

	return ret;
}

/*
	경로를 따라 끝까지 이동하며 모래를 흩뿌린다
*/
int simulate()
{
	int outDust = 0;
	for (int i = 0; i < tcnt - 1; i++)
	{
		int r, c, dir;

		// 토네이도는 "이동한 뒤" 그 칸을 흩뿌리므로 대상 칸은 track[i + 1]이다
		r = track[i + 1].r;
		c = track[i + 1].c;

		// 방향은 track[i]에서 track[i+1]로 갈 때의 방향
		dir = track[i].dir;

		outDust += clean(r, c, dir);
	}

	return outDust;
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		makeSnail();

		printf("%d\n", simulate());
	}

	return 0;
}
