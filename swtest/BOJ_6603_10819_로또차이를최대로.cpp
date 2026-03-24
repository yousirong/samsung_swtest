// BOJ 6603
/*
#include <stdio.h>

#define MAX (13+3)

int N, M;

int num_of_cases[MAX];
int number[MAX];

void input()
{
	scanf("%d", &N);

	for (int i = 1; i <= N; i++) scanf("%d", &number[i]);
}

void printCases()
{
	for (int i = 0; i < 6; i++) printf("%d ", num_of_cases[i]);
	putchar('\n');
}

void DFS(int depth, int start)
{
	if (depth == 6)
	{
		printCases();
		return;
	}

	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = number[i];
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	while (1)
	{
		input();
		if (N == 0) break;

		DFS(0, 1);

		putchar('\n');
	}

	return 0;
}
*/

// BOJ 10819
#include <stdio.h>

#define MAX (8 + 3)

int N;

int num_of_cases[MAX];
bool visit[MAX];
int number[MAX];

int maxAnswer;

void printCases()
{
	for (int i = 0; i < N; i++) printf("%d ", num_of_cases[i]);
	putchar('\n');
}

int abs(int x)
{
	return (x > 0) ? x : -x;

}

int calculate()
{
	int sum = 0;

	for (int i = 0; i < N - 1; i++)
	{
		sum += abs((num_of_cases[i] - num_of_cases[i + 1]));
	}

	return sum;
}


void DFS(int depth)
{
	if (depth == N)
	{
		//printCases();
		int tmp = calculate();
		if (maxAnswer < tmp) maxAnswer = tmp;

		return;
	}

	for (int i = 0; i < N; i++)
	{
		if (visit[i] == true) continue;

		num_of_cases[depth] = number[i];

		visit[i] = true;
		DFS(depth + 1);
		visit[i] = false;
	}
}

int main()
{
	scanf("%d", &N);

	for (int i = 0; i < N; i++) scanf("%d", &number[i]);

	maxAnswer = -1;

	DFS(0);

	printf("%d\n", maxAnswer);

	return 0;
}
