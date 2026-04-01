// BOJ 2606 - 바이러스 (BFS)
// https://www.acmicpc.net/problem/2606

// BFS (너비 우선 탐색)
// 시작 노드에서 가까운 노드부터 차례대로 탐색하는 알고리즘

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)  // 최대 노드 수 (문제 조건 100 + 여유)

int V, E;               // V: 정점(컴퓨터 수), E: 간선(연결 수)
int MAP[MAX][MAX];      // 인접 행렬 (연결 관계 저장)

int queue[MAX];         // BFS에서 사용할 큐 (배열로 구현)
bool visit[MAX];        // 방문 여부 체크 배열

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
    scanf("%d %d", &V, &E);  // 정점 개수, 간선 개수 입력

    for (int i = 0; i < E; i++) // 간선 개수만큼 반복
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
// BFS 함수
// ---------------------------
void BFS(int node)
{
    int rp, wp;   // rp: read pointer (꺼낼 위치), wp: write pointer (넣을 위치)
    int count;    // 감염된 컴퓨터 수 (1번 제외)

    rp = wp = 0;  // 큐 초기화

    // 시작 노드를 큐에 삽입
    queue[wp++] = node;

    visit[node] = true; // 시작 노드 방문 처리

    count = 0; // 감염된 컴퓨터 수 초기화

    // 큐가 빌 때까지 반복
    while (rp < wp)
    {
        int out = queue[rp++]; // 큐에서 하나 꺼냄

        // 현재 노드(out)와 연결된 모든 노드 확인
        for (int c = 1; c <= V; c++)
        {
            // 연결 안 되어 있거나 이미 방문한 경우 skip
            if (MAP[out][c] == 0 || visit[c] == true)
                continue;

            // 새롭게 감염된 컴퓨터
            count++;

            // 큐에 추가 (다음에 탐색하기 위해)
            queue[wp++] = c;

            // 방문 처리 (중복 방문 방지)
            visit[c] = true;
        }
    }

    // 최종 감염된 컴퓨터 수 출력
    printf("%d\n", count);
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
    input();   // 입력 받기

    BFS(1);    // 1번 컴퓨터부터 탐색 시작

    return 0;
}