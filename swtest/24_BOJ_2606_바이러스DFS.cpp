// BOJ 2606 - 바이러스 (DFS)
// https://www.acmicpc.net/problem/2606

// DFS (깊이 우선 탐색)
// 한 방향으로 끝까지 탐색한 후, 더 이상 갈 곳이 없으면 되돌아오는 방식

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)  // 최대 노드 수 (문제 조건 100 + 여유)

int V, E;               // V: 정점(컴퓨터 수), E: 간선(연결 수)
int MAP[MAX][MAX];      // 인접 행렬 (연결 관계 저장)

bool visit[MAX];        // 방문 여부 체크 배열
int count;              // 감염된 컴퓨터 수 (1번 제외)

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &V, &E);  // 정점 개수, 간선 개수 입력

	for (int i = 0; i < E; i++)
	{
		int n1, n2;
		scanf("%d %d", &n1, &n2); // 연결된 두 정점 입력

		// 무방향 그래프이므로 양쪽 모두 연결 표시
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
// DFS 함수 (재귀)
// ---------------------------
void DFS(int node)
{
	// 현재 노드 방문 처리
	visit[node] = true;

	// 현재 노드와 연결된 모든 노드 확인
	for (int c = 1; c <= V; c++)
	{
		// 연결이 없거나 이미 방문한 경우 건너뜀
		if (MAP[node][c] == 0 || visit[c] == true)
			continue;

		// 새롭게 감염된 컴퓨터
		count++;

		// 해당 노드로 깊이 우선 탐색 진행 (재귀 호출)
		DFS(c);
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();   // 입력 받기

	DFS(1);    // 1번 컴퓨터부터 탐색 시작

	// 최종 감염된 컴퓨터 수 출력 (1번 제외)
	printf("%d\n", count);

	return 0;
}