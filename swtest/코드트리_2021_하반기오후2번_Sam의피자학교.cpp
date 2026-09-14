/*
	[코드트리] 2021 하반기 오후 2번 - Sam의 피자학교
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/sam-pizza-school/description
	(백준 23291 "어항 정리"와 같은 문제다. 어항 -> 피자 도우, 물고기 -> 밀가루)

	■ 문제 요약
	  N개(N은 4의 배수)의 피자 도우가 한 줄로 놓여 있고 위치마다 밀가루 양이 주어진다.
	  아래 1 ~ 5 과정을 "1회"로 보고, 밀가루 양의 (최댓값 - 최솟값)이 k 이하가 될 때까지 반복한다.

	    1) 밀가루 추가 : 밀가루 양이 가장 작은 위치에 1씩 넣는다. (여러 개면 모두)
	    2) 도우 말기   : 맨 처음 위치부터 하나씩 접어 위로 올리며 말아 간다.
	                    "세워진 묶음"을 통째로 시계 방향 90도 돌려 오른쪽 바닥 위에 올리는 식이다.
	                    바닥에 있는 밀가루보다 위에 올릴 밀가루의 너비가 더 넓으면 중단한다.
	    3) 꾹 누르기   : 상하좌우 인접한 두 양 a, b에 대해 d = |a - b| / 5 (몫)를
	                    큰 쪽에서 빼고 작은 쪽에 더한다. 모든 위치에서 동시에 일어난다.
	                    그 뒤 "열이 작은 것부터, 같은 열이면 행이 큰 것(아래)부터" 왼쪽에 나열해
	                    다시 1자로 편다.
	    4) 반 접기 x 2 : 반으로 접어 올리고(1줄 -> 2줄), 그것을 다시 반으로 접는다(2줄 -> 4줄).
	    5) 꾹 누르기   : 3)을 한 번 더 한다.

	  k 이하가 되기 위한 최소 반복 횟수를 출력한다.

	  입력 예제
	      8 4 / 1 10 4 13 8 3 1 7   -> 1
	      8 2 / 1 10 4 13 8 3 1 7   -> 3     (이 코드로 두 예제 모두 맞는 것을 확인했다)

	■ 풀이 방침 : "바닥은 항상 N행"으로 고정하고 위로 쌓는다
	  pizza[N][1..N]이 바닥 줄이고, 말거나 접은 도우는 N-1, N-2, ... 행으로 올라간다.
	  빈 칸은 0이다. 밀가루는 매번 최소 1씩 늘어나므로 도우가 있는 칸은 0이 될 수 없어,
	  0을 "도우 없음" 표시로 써도 헷갈리지 않는다.

	  바닥 줄로 되돌리는 일은 press()가 맡는다. 말기/접기가 격자를 어떤 모양으로 만들든
	  press()가 "왼쪽 열부터, 아래에서 위로" 읽어 N행에 다시 펴 주므로
	  roll()과 fold()는 시작할 때 항상 한 줄이라고 가정할 수 있다.

	■ 말기(roll)의 규칙성
	  묶음의 (높이 x 너비)와 시작 열이 아래처럼 늘어난다.

	      start : 1  2  3  5  7  10 ...
	      크기  : 1x1, 2x1, 2x2, 3x2, 3x3, 4x3 ...   (높이 x 너비)

	  - 크기는 높이, 너비가 번갈아 1씩 커진다    -> count가 짝수면 height++, 홀수면 width++
	  - 시작 열은 방금 올린 묶음의 너비만큼 밀린다 -> start += (count / 2 + 1)
	  - 묶음을 올렸을 때 오른쪽 끝(start + width + height - 1)이 N을 넘으면 멈춘다

	  돌리는 좌표는 (r, c) -> (N - width + (c - start), start + width + (N - r)) 이다.
	  "원래 열의 오른쪽 거리"가 새 행이 되고, "원래 바닥에서의 높이"가 새 열의 오른쪽 거리가 된다.

	■ 주의할 점
	  [확인 필요] 처음 입력부터 이미 (최댓값 - 최솟값) <= K여도 이 코드는 한 번 연습한 뒤 검사하므로
	  최소 1을 출력한다. (입력 "4 5 / 3 3 3 3"에 대해 이 코드는 1을 출력한다.)
	  문제 본문은 "k 이하가 되기 위한 최소 반복 횟수"라고만 해서 이 경우 0인지 1인지 명시하지 않는다.
	  예제 두 개는 둘 다 처음에 조건을 만족하지 않는 입력이라 이 부분은 판단할 수 없다.
*/
#include <stdio.h>

#define MAX (100+10)
#define INF (0x7fff0000)

int T;

int N, K;
int pizza[MAX][MAX]; // 바닥이 N행, 위로 쌓일수록 행 번호가 작아진다 (0 = 도우 없음)

struct RC
{
	int r;
	int c;
};

// 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

void input()
{
	scanf("%d %d", &N, &K);

	// 테두리(0행, N+1행/열)까지 0으로 비워 둔다.
	// press()에서 이웃 칸을 경계 검사 없이 읽기 때문에 테두리가 0이어야 한다.
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			pizza[r][c] = 0;

	// 처음에는 전부 바닥 줄(N행)에 놓인다.
	for (int c = 1; c <= N; c++)
		scanf("%d", &pizza[N][c]);
}

void printMap() // for debug
{
	for(int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d", pizza[r][c]);
		putchar('\n');
	}
	putchar('\n');

}

// 1) 밀가루가 가장 적은 도우 "전부"에 1씩 넣는다.
void addFlour()
{
	int min = INF;
	for (int c = 1; c <= N; c++)
		if (pizza[N][c] < min) min = pizza[N][c];

	for (int c = 1; c <= N; c++)
		if (pizza[N][c] == min) pizza[N][c]++;
}

// 2) 도우 말기
void roll()
{
	// start, 높이 x 너비
	// 1, 1 x 1
	// 2, 2 x 1
	// 3, 2 x 2
	// 5, 3 x 2
	// 7, 3 x 3
	// 10, 4 x 3

	int start, height, width;
	start = height = width = 1;

	int count = 0;
	while (1)
	{
		// 묶음을 돌려 올리면 오른쪽 끝이 start + width + height - 1 열이 된다.
		// 그게 N을 넘으면 바닥에 올릴 자리가 없으므로 멈춘다.
		if (start + height + width - 1 > N) break;

		// 밀가루 이동
		// 묶음 [start, start + width) x [N - height + 1, N]을 시계 방향 90도 돌려
		// 바로 오른쪽(start + width 열부터) 바닥 위에 올린다.
		//   c가 오른쪽으로 갈수록 -> 올라간 뒤 아래쪽 행 (nr 증가)
		//   r이 위로 갈수록      -> 올라간 뒤 오른쪽 열 (nc 증가)
		for (int c = start; c < start + width; c++)
		{
			for (int r = N; r > N - height; r--)
			{
				int nr, nc;
				nr = N - width + (c - start);
				nc = start + width + (N - r);

				// for debug
				// printf("%d %d -> %d %d\n", r, c, nr, nc);

				pizza[nr][nc] = pizza[r][c];
				pizza[r][c] = 0;
			}
		}

		// 돌렸으므로 다음 묶음은 (옛 너비 x 옛 높이 + 1줄)이 된다.
		// 결과적으로 높이와 너비가 번갈아 1씩 커진다.
		if (count % 2 == 0) height++;
		else width++;
		// 방금 옮긴 묶음의 너비만큼 시작 열이 밀린다. (1, 1, 2, 2, 3, 3, ...)
		start += (count / 2 + 1) ;

		count++;
	}
}

// 3), 5) 누르기 : 인접 차이 / 5 만큼 동시에 옮긴 뒤 한 줄로 편다.
void press()
{
	// "동시에" 옮겨야 하므로 결과는 tmpPizza에 모은다.
	// pizza를 바로 고치면 앞에서 옮긴 값이 뒤 칸 계산에 섞인다.
	int tmpPizza[MAX][MAX] = { 0 };
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (pizza[r][c] == 0) continue;
			int flour = pizza[r][c];

			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				// 도우가 없는 칸(테두리 포함)과는 주고받지 않는다.
				if (pizza[nr][nc] == 0) continue;

				// 두 칸 쌍은 양쪽에서 한 번씩 보게 되므로
				// "많은 쪽이 줄 때"만 처리하면 중복 없이 한 번만 옮겨진다.
				if (pizza[r][c] > pizza[nr][nc])
				{
					int diff = (pizza[r][c] - pizza[nr][nc]) / 5;

					flour -= diff;
					tmpPizza[nr][nc] += diff;
				}
			}
			tmpPizza[r][c] += flour;
		}
	}

	// 한 줄로 펴기 : 왼쪽 열부터, 같은 열은 바닥(N행)에서 위로.
	// 바닥 줄 index는 항상 1 이상 c 이하이므로, 같은 N행에 덮어써도
	// 아직 읽지 않은 tmpPizza를 망가뜨리지 않는다 (읽는 쪽이 tmpPizza라 애초에 안전).
	int index = 1;
	for (int c = 1; c <= N; c++)
	{
		if (tmpPizza[N][c] == 0) continue;

		int sr = N;
		while (1)
		{
			if (tmpPizza[sr][c] == 0) break; // 이 열의 위쪽 끝
			pizza[N][index++] = tmpPizza[sr][c];
			sr--;
		}
	}
	// 바닥 줄을 뺀 나머지는 비운다.
	for (int r = 1; r <= N - 1; r++)
		for (int c = 1; c <= N; c++)
			pizza[r][c] = 0;
}

// 4) 반 접기 두 번
void fold()
{
	// 한 번 접기
	// 왼쪽 절반(1 ~ N/2)을 180도 돌려 오른쪽 절반 위(N-1행)에 올린다.
	// 180도 회전이므로 N/2열 -> N/2+1열, 1열 -> N열로 좌우가 뒤집힌다.
	int sc = (N / 2) + 1;
	for (int c = N / 2; c >= 1; c--)
	{
		pizza[N - 1][sc++] = pizza[N][c];
		pizza[N][c] = 0;
	}

	// 두 번 접기
	// 이제 (N-1, N)행 x (N/2+1 ~ N)열의 2줄 묶음이다.
	// 그중 왼쪽 절반(N/2+1 ~ 3N/4)을 180도 돌려 오른쪽 절반(3N/4+1 ~ N) 위에 올린다.
	//   N-1행 -> N-2행, N행 -> N-3행 (위아래 뒤집힘)
	//   왼쪽 열일수록 -> 오른쪽 열 (nc = N부터 감소, 좌우 뒤집힘)
	int nr = N - 2;
	for (int r = N - 1; r <= N; r++)
	{
		int ec = (N / 4) * 3;
		int nc = N;
		for (int c = (N / 2) + 1; c <= ec; c++)
		{
			// for debug
			// printf("%d %d -> %d %d\n", r, c, nr, nc);

			pizza[nr][nc] = pizza[r][c];
			pizza[r][c] = 0;
			nc--;
		}
		nr--;
	}
}

// 바닥 줄의 (최댓값 - 최솟값) <= K 인가
bool check()
{
	int max = 0;
	int min = INF;

	for (int c = 1; c <= N; c++)
	{
		if (pizza[N][c] < min) min = pizza[N][c];
		if (pizza[N][c] > max) max = pizza[N][c];
	}

	return (max - min) <= K;
}

int simulate()
{
	int count = 0;
	while (1)
	{
		addFlour();
		roll();
		press();
		fold();
		press();

		count++;

		// [확인 필요] 연습을 먼저 하고 검사한다 -> 처음부터 조건을 만족해도 1을 반환한다.
		// (백준 23291 기준이면 루프 앞에서 먼저 검사해 0을 반환해야 한다)
		if (check() == true)  break;
	}
	return count;
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
