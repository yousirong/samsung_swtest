// BOJ 2161 - 카드1
//
// 카드 1 ~ N이 순서대로 놓여 있을 때
// 1. 맨 위 카드를 버리고
// 2. 그 다음 카드를 맨 아래로 옮기는
// 과정을 반복한다.
//
// 큐로 그대로 시뮬레이션하면 된다.

#include <stdio.h>

#define MAX (1000 + 100)

int N;
int queue[MAX * 2];
int rp, wp;

int main()
{
	scanf("%d", &N);

	rp = wp = 0;

	// 초기 카드 상태: 1, 2, 3, ..., N
	for (int i = 1; i <= N; i++)
		queue[wp++] = i;

	// 마지막 카드 하나가 남을 때까지 반복
	for (int i = 0; i < N - 1; i++)
	{
		// 1) 맨 앞 카드를 버림
		printf("%d ", queue[rp++]);

		// 2) 다음 카드를 맨 뒤로 보냄
		queue[wp++] = queue[rp++];
	}

	// 마지막으로 남은 카드 출력
	printf("%d\n", queue[wp - 1]);

	return 0;
}
