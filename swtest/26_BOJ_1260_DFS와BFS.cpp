// BOJ 1260 - DFS와 BFS
//
// 문제 요약
// 하나의 그래프에 대해
// 1. DFS 방문 순서
// 2. BFS 방문 순서
// 를 출력하는 문제
//
// 조건
// - 항상 "번호가 작은 정점부터" 방문해야 한다.
//
// 핵심 아이디어
// - 인접 행렬을 사용하면 1번부터 N번까지 순서대로 탐색 가능
// - DFS: 재귀 사용 (깊이 우선)
// - BFS: 큐 사용 (너비 우선)

#include <stdio.h>
#include <stdbool.h>

#define MAX (1000 + 50)

int N, M, V;          // N: 정점 수, M: 간선 수, V: 시작 정점
int MAP[MAX][MAX];    // 인접 행렬 (연결 여부 저장)

int queue[MAX * MAX]; // BFS용 큐
bool visit[MAX];      // 방문 여부 체크 배열

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &V);

	// 간선 정보 입력
	for (int i = 1; i <= M; i++)
	{
		int n1, n2;

		scanf("%d %d", &n1, &n2);

		// 무방향 그래프이므로 양쪽 모두 연결
		MAP[n1][n2] = 1;
		MAP[n2][n1] = 1;
	}
}

// ---------------------------
// 디버그용 인접 행렬 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// DFS (Depth-First Search)
// 깊이 우선 탐색
//
// 특징:
// - 현재 노드 방문 → 가능한 노드로 계속 깊게 내려감
// - 더 이상 갈 곳이 없으면 돌아옴 (재귀)
// ---------------------------
void DFS(int node)
{
	// 현재 노드 방문 처리
	visit[node] = true;

	// 방문 순서 출력
	printf("%d ", node);

	// 1번부터 N번까지 순서대로 확인 (작은 번호 우선)
	for (int c = 1; c <= N; c++)
	{
		// 연결 안 되어 있거나 이미 방문했으면 건너뜀
		if (MAP[node][c] == 0 || visit[c] == true)
			continue;

		// 연결된 노드로 재귀 호출 (깊이 탐색)
		DFS(c);
	}
}

// ---------------------------
// BFS (Breadth-First Search)
// 너비 우선 탐색
//
// 특징:
// - 가까운 노드부터 탐색
// - 큐(Queue)를 사용
// ---------------------------
void BFS(int node)
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 시작 노드를 큐에 삽입
	queue[wp++] = node;

	// 시작 노드 방문 처리
	visit[node] = true;

	// 큐가 빌 때까지 반복
	while (rp < wp)
	{
		// 큐에서 하나 꺼냄
		int out = queue[rp++];

		// 방문 순서 출력
		printf("%d ", out);

		// 인접한 모든 노드 확인
		for (int c = 1; c <= N; c++)
		{
			// 연결 안 되어 있거나 이미 방문했으면 건너뜀
			if (MAP[out][c] == 0 || visit[c] == true)
				continue;

			// 큐에 추가
			queue[wp++] = c;

			// 방문 처리 (중복 방지)
			visit[c] = true;
		}
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();  // 입력 받기

	// DFS 수행
	DFS(V);
	putchar('\n');

	// BFS 전에 visit 배열 초기화
	// DFS에서 이미 사용했기 때문에 다시 false로 초기화 필요
	for (int i = 1; i <= N; i++)
		visit[i] = false;

	// BFS 수행
	BFS(V);
	putchar('\n');

	return 0;
}