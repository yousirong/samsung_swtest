#include <stdio.h>

#define MAX (1000+100)

int N;
int queue[MAX * 2];
int rp, wp;

int main()
{
	scanf("%d", &N);

	rp = wp = 0;

	for (int i = 1; i <= N; i++) queue[wp++] = i;

	for (int i = 0; i < N - 1; i++)
	{
		printf("%d ", queue[rp++]);

		queue[wp++] = queue[rp++];
	}

	printf("%d\n", queue[wp - 1]);

	return 0;
}