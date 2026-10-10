#include <stdio.h>	

#define MAX (100+20)

int T;
int wheel[5][10]; /* wheel[번호 1~4][각 톱니 1~8] */
int list[6];
int number[MAX];
int direct[MAX];
int N;

void input()
{
	scanf("%d", &N);
	for (int number = 1; number <= 4; number++)
	{
		for (int index = 1; index <= 8; index++)
		{
			scanf("%d", &wheel[number][index]);
		}
	}

	for (int i = 0; i < N; i++)
		scanf("%d %d", &number[i], &direct[i]);

	return;
}

void printStatus(int step, const char* when)   // for debug
{
	printf("=== %d번째 명령 %s ===\n", step + 1, when);

	for (int k = 1; k <= 4; k++)
	{
		printf("자석%d (회전 %2d) : ", k, list[k]);
		for (int i = 1; i <= 8; i++)
		{
			if (i == 1) printf("[%d]", wheel[k][i]);                 // 화살표 위치
			else if (i == 3 || i == 7) printf("<%d>", wheel[k][i]);  // 맞닿는 날
			else printf(" %d ", wheel[k][i]);
		}
		putchar('\n');
	}

	for (int k = 1; k <= 3; k++)
		printf("  %d-%d 맞닿은 극: %d vs %d -> %s\n",
			k, k + 1, wheel[k][3], wheel[k + 1][7],
			wheel[k][3] != wheel[k + 1][7] ? "다름(전파)" : "같음");

	putchar('\n');
}

void rotate(int number, int dir)
{
	int temp;

	if (dir == -1)
	{
		temp = wheel[number][1];
		for (int index = 1; index <= 7; index++)
		{
			wheel[number][index] = wheel[number][index + 1];
		}
		wheel[number][8] = temp;
	}
	else
	{
		temp = wheel[number][8];
		for (int index = 8; index >= 2; index--)
		{
			wheel[number][index] = wheel[number][index - 1];
		}
		wheel[number][1] = temp;
	}
	return;
}

int check(int compare, int number)
{
	if (compare < 1 || compare>4) return 0;
	if (number < 1 || number>4) return 0;

	if (compare < number && wheel[number][7] != wheel[compare][3]) return 1;
	if (compare > number && wheel[number][3] != wheel[compare][7]) return 1;

	return 0;
}

void DFS(int number, int dir)
{
	//printf("  DFS(%d, %d)\n", number, dir);   // for debug

	// number를 기준으로 왼쪽 바퀴가 움직여야 하는지 체크
	if (check(number - 1, number) == 1 && list[number - 1] == 0)
	{
		list[number - 1] = dir * (-1);
		DFS(number - 1, dir * (-1));
	}

	// number를 기준으로 오른쪽 바퀴가 움직여야 하는지 체크
	if (check(number + 1, number) == 1 && list[number + 1] == 0)
	{
		list[number + 1] = dir * (-1);
		DFS(number + 1, dir * (-1));
	}
	
	return;
}

int calculate()
{
	int sum, mul;

	sum = 0;
	mul = 1;
	for (int i = 1; i <= 4; i++)
	{
		sum += mul * wheel[i][1];
		mul *= 2;
	}

	return sum;
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		int ans;

		input();

		for (int i = 0; i < N; i++)
		{
			list[number[i]] = direct[i];
			DFS(number[i], direct[i]);
			//printStatus(i, "회전 전");   // 어떤 자석이 어느 방향으로 돌기로 했는지 확인

			for (int k = 1; k <= 4; k++)
				if (list[k] != 0) rotate(k, list[k]);
			//printStatus(i, "회전 후");   // 실제 회전 결과 확인

			for (int k = 1; k <= 4; k++) list[k] = 0;
		}

		ans = calculate();

		printf("#%d %d\n", tc, ans);
	}
	return 0;
}