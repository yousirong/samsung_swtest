/*
	[프로그래머스] 2025 카카오 하반기 1차 - 바이러스 파이프
	https://school.programmers.co.kr/learn/challenges   ("바이러스 파이프" 검색)
	[확인 필요] 문제 개별 주소(lessons 번호)를 찾지 못해 코딩테스트 연습 목록 주소를 달아 두었다.

	■ 문제 요약
	  1 ~ n번 배양체가 n - 1개의 파이프로 이어진 트리다. 파이프 종류는 A(1), B(2), C(3).
	  처음에는 모든 파이프가 닫혀 있고, infection번 배양체 하나만 감염돼 있다.

	  한 번의 행동 = "한 종류의 파이프를 전부 열었다가 닫는다".
	  열려 있는 동안 감염은 열린 파이프를 타고 계속 번진다 (한 칸이 아니라 닿는 데까지 전부).
	  행동을 최대 k번 해서 감염된 배양체 수의 최댓값을 구한다.

	  제한 : 2 <= n <= 100, 1 <= k <= 10

	  입출력 예
	      예제 1 -> 6,  예제 2 -> 7

	■ 풀이 방침 : 여는 순서를 DFS로 전부 해 본다
	  결정할 것은 "몇 번째 행동에서 어떤 종류를 열까" 뿐이다.
	  종류는 3개, 행동은 최대 10번이라 많아야 3^10 = 59,049가지다. 전부 해 봐도 된다.

	      DFS(depth, lastType, count)
	        1) 지금 감염 수로 answer 갱신
	        2) depth == K 면 끝
	        3) 종류 t = A, B, C 를 하나씩 골라
	             - 감염 상태를 백업해 두고
	             - t 종류 파이프를 열어 감염을 퍼뜨린다 (BFS)
	             - DFS(depth + 1, t, count + 새로 감염된 수)
	             - 돌아오면 백업으로 되돌린다

	  삼성 기출의 "방향을 고르고 시뮬레이션 -> 원상복구" 백트래킹(2048 게임, 구슬 탈출 등)과 같은 모양이다.

	■ 감염 퍼뜨리기 = 감염된 노드 전부를 시작점으로 넣는 BFS
	  t 종류가 열리면 감염된 노드에서 t 파이프로 이어진 노드가 감염되고,
	  새로 감염된 노드에서 또 t 파이프를 타고 번진다. 그래서 감염된 노드를 전부 큐에 넣고
	  "t 종류 간선만 따라가는" BFS를 돌리면 된다. (다른 종류 간선은 벽으로 본다)

	■ 가지치기 2개
	  - 같은 종류를 연달아 여는 것은 의미가 없다. 첫 번째에서 이미 닿는 데까지 다 퍼졌다.
	    -> 직전에 연 종류(lastType)는 건너뛴다. 3갈래가 2갈래가 되어 3 * 2^9 = 1,536가지로 준다.
	  - 열었는데 새로 감염된 게 0이면 상태가 그대로이고 행동만 하나 버린 것이다.
	    -> 그 가지는 더 내려가지 않는다. (안 열고 다른 종류를 여는 쪽이 항상 같거나 낫다)

	■ 주의할 점
	  - 트리라서 간선이 n - 1개뿐이다. 인접 리스트를 배열로 직접 만든다 (노드마다 이웃 번호 + 파이프 종류).
	  - 감염 상태 백업은 DFS 지역 배열에 한다. 깊이가 최대 10이라 100칸 x 10 이면 충분하다.
	  - 행동을 k번 "꼭" 해야 하는 게 아니라 "최대" k번이다. 그래서 매 깊이에서 answer를 갱신한다.
*/
#include <stdio.h>

#define MAX (100+5)

int T;

int N, START, K;              // 배양체 수, 처음 감염된 배양체, 최대 행동 수

int adj[MAX][MAX];            // adj[v][i]  : v의 i번째 이웃
int adjType[MAX][MAX];        // adjType[v][i] : 그 이웃과 잇는 파이프 종류 (1 ~ 3)
int adjCnt[MAX];              // v의 이웃 수

int infected[MAX];            // 1이면 감염

int queue[MAX];
int rp, wp;

int answer;

void input()
{
	scanf("%d %d", &N, &START);

	for (int v = 1; v <= N; v++)
		adjCnt[v] = 0;

	for (int i = 0; i < N - 1; i++)
	{
		int x, y, type;

		scanf("%d %d %d", &x, &y, &type);

		// 양방향으로 넣는다
		adj[x][adjCnt[x]] = y;
		adjType[x][adjCnt[x]++] = type;

		adj[y][adjCnt[y]] = x;
		adjType[y][adjCnt[y]++] = type;
	}

	scanf("%d", &K);

	for (int v = 1; v <= N; v++)
		infected[v] = 0;

	infected[START] = 1;

	answer = 0;
}

// type 종류 파이프를 열었다 닫는다. 새로 감염된 배양체 수를 돌려준다.
int spread(int type)
{
	int added = 0;

	rp = wp = 0;

	// 감염된 노드 전부가 시작점이다
	for (int v = 1; v <= N; v++)
		if (infected[v] == 1) queue[wp++] = v;

	while (rp < wp)
	{
		int out = queue[rp++];

		for (int i = 0; i < adjCnt[out]; i++)
		{
			int next = adj[out][i];

			if (adjType[out][i] != type) continue;   // 닫힌 파이프
			if (infected[next] == 1) continue;

			infected[next] = 1;
			added++;

			queue[wp++] = next;
		}
	}

	return added;
}

// depth번 행동을 했고, 직전에 연 종류가 lastType, 지금 감염 수가 count
void DFS(int depth, int lastType, int count)
{
	if (count > answer) answer = count;

	if (depth == K) return;
	if (count == N) return;   // 전부 감염됐으면 더 볼 필요가 없다

	for (int t = 1; t <= 3; t++)
	{
		if (t == lastType) continue;   // 같은 종류를 연달아 여는 건 의미가 없다

		int backup[MAX];
		for (int v = 1; v <= N; v++) backup[v] = infected[v];

		int added = spread(t);

		if (added > 0) DFS(depth + 1, t, count + added);   // 0이면 행동만 버린 것

		for (int v = 1; v <= N; v++) infected[v] = backup[v];
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		DFS(0, 0, 1);

		printf("%d\n", answer);
	}

	return 0;
}
