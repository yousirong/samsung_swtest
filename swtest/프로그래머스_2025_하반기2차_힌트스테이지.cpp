/*
	[프로그래머스] 2025 카카오 하반기 2차 - 힌트 스테이지
	https://school.programmers.co.kr/learn/courses/30/lessons/468377

	■ 문제 요약
	  n개의 스테이지(1번 -> n번)를 순서대로 모두 해결한다.
	  i번 스테이지는 i번 힌트권을 j장 쓰면 cost[i][j]의 비용이 든다 (j가 클수록 싸다).
	  한 스테이지에 쓸 수 있는 힌트권은 최대 n - 1장이다. 처음에는 힌트권이 없다.

	  마지막 스테이지를 뺀 각 스테이지에서는 힌트 번들을 최대 1개 살 수 있다.
	  i번 스테이지의 번들은 가격 hint[i][0]이고, i+1번 이상의 힌트권 k장이 들어 있다.
	  (같은 번호가 여러 장 들어 있을 수 있다)

	  모든 스테이지를 해결하는 최소 비용을 구한다.

	  제한 : 2 <= n <= 16, 2 <= k + 1 <= 20

	  입출력 예
	      예제 1 -> 810,  예제 2 -> 1080,  예제 3 -> 1180
	      예제 4 -> 267789,  예제 5 -> 3004,  예제 6 -> 3426

	■ 풀이 방침 : 번들을 "산다 / 안 산다"로 나누는 DFS
	  결정할 것은 "각 스테이지에서 번들을 살까 말까" 하나뿐이다.
	  번들은 n - 1개(최대 15개)라 경우의 수는 2^15 = 32,768가지다. 전부 해 봐도 된다.

	      DFS(stage, sum)
	        1) 지금까지 모은 stage번 힌트권을 (최대 n - 1장까지) 전부 써서 이 스테이지를 푼다
	        2) 마지막 스테이지면 끝 -> 최솟값 갱신
	        3) 번들을 안 사고 다음 스테이지로
	        4) 번들을 사고(힌트권을 더하고) 다음 스테이지로, 돌아와서 힌트권을 되돌린다

	  삼성 기출의 연산자 배치, 조삼모사 같은 "고르기 백트래킹"과 같은 모양이다.

	■ 힌트권은 있는 만큼 다 쓰면 된다
	  cost[i][j] > cost[i][j+1] 이라 힌트권을 한 장 더 쓰면 항상 싸진다.
	  그리고 i번 힌트권은 i번 스테이지에서만 쓸 수 있어서, 안 쓰고 남겨 봐야 쓸 곳이 없다.
	  그러니 "몇 장 쓸까"는 고민할 필요가 없다. min(가진 장수, n - 1)장을 쓴다.

	■ 순서가 꼬이지 않는 이유
	  i번 스테이지의 번들에는 i+1번 이상의 힌트권만 들어 있다.
	  그래서 번들을 사는 즉시 ticket[]에 더해 두면, 그 힌트권이 쓰일 스테이지에 도착했을 때
	  이미 들어와 있다. 스테이지를 앞에서부터 차례로 보는 DFS와 딱 맞는다.

	■ 가지치기
	  비용은 모두 양수라 sum은 줄어들지 않는다.
	  지금까지의 sum이 이미 찾아 둔 answer 이상이면 더 볼 필요가 없다.
	  (가지치기가 없어도 3만 가지라 충분히 빠르지만, 습관처럼 넣어 둔다)

	■ 주의할 점
	  - 스테이지 번호는 1부터 쓴다 (힌트권 번호가 1 ~ n 이라 그대로 맞춘다).
	  - 번들 가격이 0일 수도 있다 (그룹 #2). 그래도 "산다/안 산다" 둘 다 보므로 문제없다.
	  - 힌트권이 n - 1장보다 많이 모여도 n - 1장까지만 쓴다.
*/
#include <stdio.h>

#define MAX_cost (16+4)   // 스테이지 수 최대 16
#define MAX_hint (20+4)   // 번들 한 줄 길이(가격 + 힌트권 k장) 최대 20
#define INF (0x7fffffff)

int T;

int N;                             // 스테이지 수
int COST[MAX_cost][MAX_cost];      // COST[i][j] : i번 스테이지에서 힌트권 j장을 썼을 때 비용
int price[MAX_cost];               // i번 스테이지 번들 가격
int bundle[MAX_cost][MAX_hint];    // i번 스테이지 번들에 든 힌트권 번호들
int bundleSize[MAX_cost];          // 번들에 든 힌트권 장수 (k)

int ticket[MAX_cost];              // 지금 들고 있는 i번 힌트권 장수
int answer;

void input()
{
	scanf("%d", &N);

	for (int i = 1; i <= N; i++)
		for (int j = 0; j < N; j++)
			scanf("%d", &COST[i][j]);

	int k;
	scanf("%d", &k);   // 번들 하나에 든 힌트권 장수

	for (int i = 1; i <= N - 1; i++)
	{
		scanf("%d", &price[i]);

		bundleSize[i] = k;
		for (int j = 0; j < k; j++)
			scanf("%d", &bundle[i][j]);
	}

	for (int i = 0; i <= N; i++)
		ticket[i] = 0;

	answer = INF;
}

// stage번 스테이지를 풀 차례이고, 지금까지 쓴 비용이 sum이다
void DFS(int stage, int sum)
{
	// 가지치기 : 이미 찾은 답보다 비싸면 더 볼 필요가 없다
	if (sum >= answer) return;

	// 1) 가진 힌트권을 최대 n - 1장까지 전부 쓴다 (쓸수록 싸고, 남겨도 쓸 곳이 없다)
	int use = ticket[stage];
	if (use > N - 1) use = N - 1;

	sum += COST[stage][use];

	// 2) 마지막 스테이지면 끝
	if (stage == N)
	{
		if (sum < answer) answer = sum;
		return;
	}

	// 3) 이 스테이지의 번들을 사지 않는다
	DFS(stage + 1, sum);

	// 4) 번들을 산다 : 힌트권을 더하고 내려갔다가, 돌아오면 되돌린다
	for (int j = 0; j < bundleSize[stage]; j++)
		ticket[bundle[stage][j]]++;

	DFS(stage + 1, sum + price[stage]);

	for (int j = 0; j < bundleSize[stage]; j++)
		ticket[bundle[stage][j]]--;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		DFS(1, 0);

		printf("%d\n", answer);
	}

	return 0;
}
