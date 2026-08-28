/*
	[코드트리] 2015 하반기 2번 - 2개의 사탕
	https://www.codetree.ai/training-field/frequent-problems/problems/two-candies

	■ 문제 요약
	  N x M 보드에 빨간 사탕(R)과 파란 사탕(B)이 하나씩 있고, 벽(#)과 구멍(O)이 있다.
	  보드를 상/하/좌/우 중 한 방향으로 기울이면 두 사탕이 그 방향으로
	  더 이상 갈 수 없을 때까지 동시에 굴러간다.

	  최대 10번 기울여서 "빨간 사탕만" 구멍에 빠뜨려야 한다.
	  파란 사탕이 같이 빠지거나 먼저 빠지면 실패다.
	  성공하는 최소 기울이기 횟수를 구하고, 10번 안에 불가능하면 -1을 출력한다.

	  (백준 13460 "구슬 탈출 2"와 사실상 같은 문제다)

	■ 풀이 방침 : 방향 수열 완전탐색 + 시뮬레이션
	  기울이는 횟수가 최대 10번뿐이므로, 방향을 어떻게 고를지 전부 만들어 봐도 된다.

	    1) DFS로 길이 10짜리 방향 수열을 만든다
	    2) 완성된 수열을 simulate()로 보드에 그대로 적용해 본다
	    3) 성공하면 그때까지 쓴 횟수를 최솟값과 비교해 갱신한다

	  가지치기를 하나 넣는데, "직전과 같은 방향"은 고르지 않는다.
	  이미 그 방향으로 끝까지 굴러가 붙어 있는 상태라 한 번 더 기울여도 아무것도 바뀌지 않기 때문이다.
	  덕분에 경우의 수가 4^10(약 105만)에서 4 * 3^9(약 7.9만)로 줄어든다.

	  수열은 항상 길이 10으로 만들되, simulate()가 도중에 성공하면
	  그 시점의 횟수(i + 1)를 돌려주므로 뒤쪽 방향은 자연히 무시된다.

	■ 시뮬레이션의 핵심 : 두 사탕이 서로를 막는다
	  한 번 기울이면 두 사탕이 동시에 굴러가는데, 서로 겹칠 수 없어서 경우를 나눠야 한다.
	  이 코드는 매 한 칸마다 아래 네 가지로 분기한다.

	    1) 둘 다 앞이 벽    -> 더 못 움직이므로 정지
	    2) 둘 다 앞이 벽 아님 -> 둘 다 한 칸 이동
	    3) 빨강만 벽에 막힘  -> 파랑이 빨강 자리로 가는 게 아니라면 파랑만 이동
	    4) 파랑만 벽에 막힘  -> 빨강이 파랑 자리로 가는 게 아니라면 빨강만 이동
	    그 외(서로 밀고 있는 상태) -> 정지

	  이렇게 한 칸씩 밀면 "앞선 사탕이 멈추면 뒤 사탕도 그 뒤에 멈춘다"가 자동으로 처리된다.

	■ 구멍 판정의 순서가 중요하다
	  빨간 사탕이 구멍에 들어간 시점에 파란 사탕은 아직 굴러가는 중일 수 있다.
	  그래서 빨강이 빠지면 곧바로 성공이라고 하면 안 되고,
	  파란 사탕을 같은 방향으로 벽에 닿을 때까지 마저 굴려 보며
	  도중에 구멍을 만나면 실패로 되돌린다.
	  반대로 파란 사탕이 먼저 빠지면 그 자리에서 실패다.

	■ 구현 포인트
	  입력을 읽을 때 R과 B는 좌표만 기억하고 보드에서는 '.'로 지운다.
	  사탕은 계속 움직이므로 보드에 남겨 두면 위치가 꼬인다.
	  보드에는 벽(#), 구멍(O), 빈칸(.)만 남는다. 바깥 테두리가 벽이라 범위 검사도 필요 없다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (10 + 5)
#define INF (0x7fff0000)

int T;

int N, M;                 // 보드 크기
char MAP[MAX][MAX];       // 보드 (# 벽, O 구멍, . 빈칸)

int num_of_cases[10 + 5]; // DFS로 만든 방향 수열 (길이 10)

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC RED, BLUE;             // 사탕의 "처음" 위치 (시뮬레이션마다 여기서 다시 출발)

int minAnswer;            // 지금까지 찾은 최소 기울이기 횟수

// 방향 배열: ↑(0), →(1), ↓(2), ←(3)
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			scanf(" %c", &MAP[r][c]);

			// 사탕은 위치만 따로 들고, 보드에서는 빈칸으로 지운다.
			// 사탕이 계속 이동하기 때문에 보드에 남겨 두면 상태가 어긋난다.
			if (MAP[r][c] == 'R')
			{
				RED.r = r;
				RED.c = c;

				MAP[r][c] = '.';
			}
			else if (MAP[r][c] == 'B')
			{
				BLUE.r = r;
				BLUE.c = c;

				MAP[r][c] = '.';
			}
		}
	}
}

// ---------------------------
// 디버그용: 사탕을 잠시 얹어서 보드 출력
// ---------------------------
void printMap()
{
	MAP[RED.r][RED.c] = 'R';
	MAP[BLUE.r][BLUE.c] = 'B';

	for (int r = 0; r < N; r++)
		printf("%s\n", MAP[r]);
	putchar('\n');

	// 출력용으로만 얹었으므로 곧바로 되돌린다
	MAP[RED.r][RED.c] = '.';
	MAP[BLUE.r][BLUE.c] = '.';
}

// ---------------------------
// 디버그용: 현재 방향 수열 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < 10; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 방향 수열대로 실제로 기울여 본다
//
//   성공(빨강만 구멍) : 사용한 기울이기 횟수
//   실패              : INF
// ---------------------------
int simulate()
{
	RC red, blue;

	// 매번 초기 위치에서 새로 시작한다 (전역 RED/BLUE는 건드리지 않는다)
	red = RED;
	blue = BLUE;

	// 정해진 순서대로 최대 10번 기울인다
	for (int i = 0; i < 10; i++)
	{
		int direction = num_of_cases[i];

		// 한 번 기울이면 더 이상 못 움직일 때까지 한 칸씩 계속 민다
		while (1)
		{
			// 두 사탕이 각각 가려는 다음 칸
			int red_nr = red.r + dr[direction];
			int red_nc = red.c + dc[direction];

			int blue_nr = blue.r + dr[direction];
			int blue_nc = blue.c + dc[direction];

			// 앞이 벽인가
			bool checkWallRed = MAP[red_nr][red_nc] == '#';
			bool checkWallBlue = MAP[blue_nr][blue_nc] == '#';

			// 앞이 상대 사탕이 서 있는 자리인가 (서로 막는 경우)
			bool checkCandyRed = (red_nr == blue.r) && (red_nc == blue.c);
			bool checkCandyBlue = (blue_nr == red.r) && (blue_nc == red.c);

			// 디버그용
			// RED = red; BLUE = blue;
			// printMap();

			// 1) 둘 다 벽에 막힘 -> 이번 기울이기는 여기서 끝
			if (checkWallRed == true && checkWallBlue == true)
				break;

			// 2) 둘 다 갈 수 있음 -> 나란히 한 칸씩 이동
			if (checkWallRed == false && checkWallBlue == false)
			{
				red.r = red_nr;
				red.c = red_nc;

				blue.r = blue_nr;
				blue.c = blue_nc;
			}
			// 3) 빨강만 벽에 막힘 -> 파랑이 빨강 자리로 밀고 들어가는 게 아니라면 파랑만 이동
			else if (checkWallRed == true && checkCandyBlue == false)
			{
				blue.r = blue_nr;
				blue.c = blue_nc;
			}
			// 4) 파랑만 벽에 막힘 -> 빨강이 파랑 자리로 밀고 들어가는 게 아니라면 빨강만 이동
			else if (checkWallBlue == true && checkCandyRed == false)
			{
				red.r = red_nr;
				red.c = red_nc;
			}
			// 그 외 : 서로를 막고 있어 아무도 못 움직인다
			else
			{
				break;
			}

			// ----- 빨간 사탕이 구멍에 들어간 경우 -----
			if (MAP[red.r][red.c] == 'O')
			{
				// 아직 파란 사탕은 굴러가는 중일 수 있다.
				// 같은 방향으로 벽에 닿을 때까지 마저 굴려 보고, 구멍을 만나면 실패.
				while (MAP[blue.r][blue.c] != '#')
				{
					if (MAP[blue.r][blue.c] == 'O')
						return INF;   // 파랑도 빠짐 -> 실패

					blue.r += dr[direction];
					blue.c += dc[direction];
				}

				// 빨강만 빠졌다 -> 성공. 지금까지 기울인 횟수는 i + 1번.
				return i + 1;
			}

			// ----- 파란 사탕이 (먼저) 구멍에 들어간 경우 -> 즉시 실패 -----
			if (MAP[blue.r][blue.c] == 'O')
				return INF;
		}
	}

	// 10번을 다 써도 성공하지 못했다
	return INF;
}

// ---------------------------
// DFS : 길이 10의 방향 수열을 모두 만든다
//
//   depth     : 지금 정하려는 순번
//   direction : 직전에 고른 방향 (첫 호출은 -1)
// ---------------------------
void DFS(int depth, int direction)
{
	// 수열이 완성되면 실제로 굴려 본다
	if (depth == 10)
	{
		// printCases();

		int tmp = simulate();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	for (int i = 0; i < 4; i++)
	{
		// 직전과 같은 방향은 건너뛴다.
		// 이미 그 방향 끝까지 굴러가 있어서 다시 기울여도 상태가 바뀌지 않는다.
		if (i == direction)
			continue;

		num_of_cases[depth] = i;

		DFS(depth + 1, i);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		// 가능한 모든 방향 수열을 만들어 시뮬레이션
		DFS(0, -1);

		// 한 번도 성공하지 못했으면 -1
		if (minAnswer == INF)
			printf("-1\n");
		else
			printf("%d\n", minAnswer);
	}

	return 0;
}
