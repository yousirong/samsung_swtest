/*
	[SWEA] 2105 - [모의 SW 역량테스트] 디저트 카페
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5VwAr6APYDFAWu

	■ 문제 요약
	  N x N 지역의 칸마다 디저트 카페가 있고, 디저트 종류(1 ~ 100)가 적혀 있다 (4 <= N <= 20).
	  대각선으로만 움직여 사각형 모양으로 돌아 출발 카페로 돌아온다.
	  같은 종류의 디저트를 두 번 먹으면 안 되고, 카페 하나만 들르거나 왔던 길을 되돌아가면 안 된다.
	  먹을 수 있는 디저트의 최대 개수를 구한다. 투어가 불가능하면 -1. 테스트 케이스가 T개.

	■ 풀이 방침 : 출발점마다 "직진 / 시계 방향으로 꺾기" 두 갈래 DFS
	  방향을 ↘ -> ↙ -> ↖ -> ↗ 순서(시계 방향)로만 바꾼다고 정해 두면,
	  사각형 한 바퀴는 "각 방향으로 몇 칸씩 가는가"로 정해진다.

	      DFS(L, R, C, sr, sc, dir)
	        - (R, C)가 출발점이면 한 바퀴 완성 -> L로 최댓값 갱신
	        - 범위 밖이거나 먹은 디저트면 실패
	        - 아니면 먹었다고 표시하고
	            1) 같은 방향으로 한 칸 더
	            2) 다음 방향으로 꺾어서 한 칸
	          돌아오면 표시를 지운다
	        - 꺾은 횟수가 4번(dir == 4)이 되면 더 갈 방향이 없다

	  출발점은 사각형의 "맨 위 꼭짓점"으로 정한다. 그러면 첫 이동은 항상 ↘ 이고,
	  출발점의 행은 1 ~ N-2, 열은 2 ~ N-1 만 보면 된다 (아래로 2칸, 좌우로 1칸 이상 필요).

	■ 왜 사각형만 만들어지나
	  ↘ a칸, ↙ b칸 (a, b >= 1) 뒤에는 출발점보다 아래에 있어서 ↖ 로만은 돌아올 수 없다.
	  결국 ↗ 까지 꺾고 마주 보는 변의 길이가 같을 때만 출발점에 닿는다.
	  그래서 "출발점에 닿았다" = "올바른 사각형 투어"다.

	■ 주의할 점
	  - 출발점 디저트는 main()에서 미리 먹었다고 표시한다. 출발점 검사(R == sr && C == sc)가
	    디저트 검사보다 먼저라서, 돌아왔을 때 "이미 먹은 디저트"로 막히지 않는다.
	  - 투어가 하나도 없으면 MAXANS가 초기값 -1 그대로 출력된다.
	  - [확인 필요] dir == 3에서 꺾는 호출은 dr[4], dc[4]를 읽는다. 배열 크기가 4라서 범위 밖 읽기다.
	    호출된 DFS가 dir == 4 검사로 바로 끝나 그 값을 쓰지는 않으므로 결과는 맞지만,
	    엄밀히는 정의되지 않은 동작이다. dr/dc 끝에 0을 하나 더 두면 깔끔해진다.
*/
#include <stdio.h>

#define MAX (25+5)

int T;
int N;

int MAP[MAX][MAX];        // 칸마다 디저트 종류

int dessert[100 + 20];    // dessert[종류] == 1 : 이번 투어에서 이미 먹음
int MAXANS;               // 먹은 디저트 수의 최댓값 (투어가 없으면 -1)

// 순서대로 오른쪽 아래, 왼쪽 아래, 왼쪽 위, 오른쪽 위
int dr[] = { 1,1,-1,-1 };
int dc[] = { 1,-1,-1,1 };

void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

// L : 지금까지 먹은 수, (R, C) : 지금 칸, (sr, sc) : 출발점, dir : 지금 진행 방향
void DFS(int L, int R, int C, int sr, int sc, int dir)
{
	if (dir == 4) return;   // 네 방향을 다 썼다

	// 출발점으로 돌아왔다 -> 한 바퀴 완성
	if (R == sr && C == sc)
	{
		if (MAXANS < L) MAXANS = L;
		return;
	}

	// MAP의 범위를 벗어나는 경우
	if (R<1 || C<1 || R>N || C>N) return;

	if (dessert[MAP[R][C]] == 0)
	{
		dessert[MAP[R][C]] = 1;

		// 현재 방향으로 진행
		DFS(L + 1, R + dr[dir], C + dc[dir], sr, sc, dir);

		// 시계 방향으로 진행
		// (dir == 3이면 dr[4], dc[4]를 읽는다. 위 [확인 필요] 참고)
		DFS(L + 1, R + dr[dir+1], C + dc[dir+1], sr, sc, dir+1);

		dessert[MAP[R][C]] = 0;   // 원상복구
	}
	else
		return;   // 같은 디저트를 또 먹게 된다
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		MAXANS = -1;
		// 출발점 = 사각형의 맨 위 꼭짓점
		for (int r = 1; r <= N - 2; r++)
		{
			for (int c = 2; c <= N - 1; c++)
			{
				dessert[MAP[r][c]] = 1;            // 출발점 디저트를 먹고
				DFS(1, r + 1, c + 1, r, c, 0);     // 첫 칸은 무조건 ↘
				dessert[MAP[r][c]] = 0;
			}
		}
		printf("#%d %d\n", tc, MAXANS);
	}
	return 0;
}