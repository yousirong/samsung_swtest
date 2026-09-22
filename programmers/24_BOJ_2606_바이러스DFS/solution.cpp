/*
	[BOJ] 2606 - 바이러스 (DFS 풀이)
	원본 : swtest/24_BOJ_2606_바이러스DFS.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.

	https://www.acmicpc.net/problem/2606

	■ 문제 요약
	  컴퓨터 V대가 네트워크로 연결되어 있고, 연결 정보 E개가 주어진다.
	  1번 컴퓨터가 웜 바이러스에 걸렸을 때, 1번을 통해 감염되는 컴퓨터의 수를 출력한다.
	  (1번 자기 자신은 제외한다)

	■ 풀이 방침
	  결국 "1번이 속한 연결 요소(connected component)의 크기 - 1"을 구하는 문제다.
	  1번에서 시작해 그래프를 한 번 훑으면서 새로 방문하는 정점을 세면 된다.

	  여기서는 DFS(깊이 우선 탐색)를 재귀로 구현했다.
	  한 방향으로 갈 수 있는 데까지 들어갔다가, 막히면 되돌아와 다른 가지를 본다.

	■ 자료구조
	  V <= 100 으로 아주 작으므로 인접 리스트 대신 인접 행렬을 썼다.
	    MAP[a][b] == 1  <=>  a와 b가 연결됨
	  간선이 방향 없는 연결이므로 입력마다 양쪽 모두 1로 표시한다.

	  탐색 비용은 정점마다 모든 열을 훑으므로 O(V^2) = 10,000 수준이라 넉넉하다.

	■ 주의할 점
	  1) visit 표시는 DFS에 들어가자마자 한다.
	     그래야 사이클이 있어도 같은 정점에 다시 들어가지 않는다.
	  2) count는 "새로 감염시킨 이웃"을 발견한 시점에 올린다.
	     시작 정점 1번은 이 방식으로는 절대 세어지지 않으므로
	     문제가 요구하는 "1번 제외" 조건이 자동으로 맞춰진다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용
#include <stdbool.h>

#define MAX (100 + 10)  // 최대 정점 수 (문제 조건 100 + 여유)

int V, E;               // V: 정점(컴퓨터) 수, E: 간선(연결) 수
int MAP[MAX][MAX];      // 인접 행렬. MAP[a][b] == 1 이면 a-b 연결

bool visit[MAX];        // 방문(감염) 여부
int count;              // 새로 감염된 컴퓨터 수 (1번 제외)

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다
void input(int v, const std::vector<std::vector<int>>& edges)
{
	V = v;                        // [수정] scanf("%d %d", &V, &E) 대체
	E = (int)edges.size();

	// [추가] 재호출 대비 : 인접 행렬과 방문 표시를 비운다
	for (int a = 1; a <= V; a++)
	{
		visit[a] = false;
		for (int b = 1; b <= V; b++) MAP[a][b] = 0;
	}
	count = 0;

	for (int i = 0; i < E; i++)
	{
		int n1 = edges[i][0];   // [수정] scanf 대체
		int n2 = edges[i][1];

		// 무방향 그래프이므로 양방향 모두 표시
		MAP[n1][n2] = 1;
		MAP[n2][n1] = 1;
	}
}

// ---------------------------
// 디버그용: 인접 행렬 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= V; r++)
	{
		for (int c = 1; c <= V; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// DFS (재귀)
//
// node를 감염 처리하고, 아직 감염되지 않은 이웃으로 계속 파고든다.
// ---------------------------
void DFS(int node)
{
	// 들어오자마자 방문 표시. 사이클이 있어도 재진입하지 않게 하는 핵심.
	visit[node] = true;

	// node와 연결된 정점을 1번부터 V번까지 확인
	for (int c = 1; c <= V; c++)
	{
		// 연결이 없거나 이미 감염된 정점은 건너뛴다
		if (MAP[node][c] == 0 || visit[c] == true)
			continue;

		// 여기 도달했다는 것은 c를 이번에 새로 감염시켰다는 뜻
		count++;

		// c 방향으로 더 깊이 들어간다
		DFS(c);
	}
}

// ---------------------------
// 메인
// ---------------------------
// [수정] main() -> solution()
int solution(int v, std::vector<std::vector<int>> edges)
{
	input(v, edges);

	DFS(1);    // 1번 컴퓨터에서 감염 시작

	// 1번을 제외한 감염된 컴퓨터 수
	return count;   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int v, e;
	scanf("%d %d", &v, &e);

	std::vector<std::vector<int>> edges(e, std::vector<int>(2));
	for (int i = 0; i < e; i++) scanf("%d %d", &edges[i][0], &edges[i][1]);

	int ans = solution(v, edges);

#ifdef REPEAT_TEST
	int ans2 = solution(v, edges);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);

	return 0;
}
#endif
