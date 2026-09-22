/*
    [BOJ] 2606 - 바이러스 (BFS 풀이)

	[프로그래머스 제출용]  원본 : swtest/25_BOJ_2606_바이러스BFS.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).

    https://www.acmicpc.net/problem/2606

    ■ 문제 요약
      컴퓨터 V대가 네트워크로 연결되어 있고, 연결 정보 E개가 주어진다.
      1번 컴퓨터가 웜 바이러스에 걸렸을 때, 1번을 통해 감염되는 컴퓨터의 수를 출력한다.
      (1번 자기 자신은 제외한다)

    ■ 풀이 방침
      "1번이 속한 연결 요소의 크기 - 1"을 구하면 된다.
      같은 파일 세트의 DFS 풀이(24번)와 답은 완전히 같고, 훑는 순서만 다르다.

      BFS(너비 우선 탐색)는 시작점에서 가까운 정점부터 차례대로 방문한다.
      큐에 넣은 순서대로 꺼내며 이웃을 확장하는 방식이라 재귀가 필요 없다.

    ■ 자료구조
      V <= 100 이라 인접 행렬로 충분하다. 무방향 간선이므로 양쪽 모두 표시한다.

      큐는 배열 하나와 포인터 두 개로 만든다.
        rp : 다음에 꺼낼 위치
        wp : 다음에 넣을 위치
        rp < wp 인 동안 큐에 처리할 원소가 남아 있다.

      각 정점은 visit 덕분에 큐에 딱 한 번만 들어가므로 queue[MAX]면 충분하다.

    ■ 주의할 점
      방문 표시는 "큐에서 꺼낼 때"가 아니라 "큐에 넣을 때" 해야 한다.
      꺼낼 때 표시하면, 같은 정점이 여러 이웃을 통해 큐에 중복으로 들어가
      개수가 부풀려진다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용
#include <stdbool.h>

using namespace std;

#define MAX (100 + 10)  // 최대 정점 수 (문제 조건 100 + 여유)

int V, E;               // V: 정점(컴퓨터) 수, E: 간선(연결) 수
int MAP[MAX][MAX];      // 인접 행렬. MAP[a][b] == 1 이면 a-b 연결

int queue[MAX];         // BFS용 큐 (배열로 구현)
bool visit[MAX];        // 방문(감염) 여부
int infected;           // [추가] BFS 결과(감염 수)를 담아 두는 전역

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다
void input(int v, const vector<vector<int>>& edges)
{
    V = v;                        // [수정] scanf("%d %d", &V, &E) 대체
    E = (int)edges.size();

    // [추가] 재호출 대비 : 인접 행렬과 방문 표시를 비운다
    for (int a = 1; a <= V; a++)
    {
        visit[a] = false;
        for (int b = 1; b <= V; b++) MAP[a][b] = 0;
    }
    infected = 0;

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
// BFS
//
// node에서 시작해 퍼져 나가며, 새로 감염된 정점 수를 세어 출력한다.
// ---------------------------
void BFS(int node)
{
    int rp, wp;   // rp: 꺼낼 위치, wp: 넣을 위치
    int count;    // 새로 감염된 컴퓨터 수 (시작 정점 제외)

    rp = wp = 0;  // 빈 큐

    // 시작 정점을 큐에 넣고 방문 표시
    queue[wp++] = node;
    visit[node] = true;

    count = 0;

    // 큐가 빌 때까지 (처리할 원소가 남아 있는 동안)
    while (rp < wp)
    {
        int out = queue[rp++]; // 큐에서 하나 꺼낸다

        // out과 연결된 정점을 1번부터 V번까지 확인
        for (int c = 1; c <= V; c++)
        {
            // 연결이 없거나 이미 감염된 정점은 건너뛴다
            if (MAP[out][c] == 0 || visit[c] == true)
                continue;

            // c를 이번에 새로 감염시켰다
            count++;

            // 나중에 c의 이웃도 봐야 하므로 큐에 넣는다
            queue[wp++] = c;

            // 넣는 순간 바로 방문 표시 (중복 삽입 방지)
            visit[c] = true;
        }
    }

    // 시작 정점을 제외한 감염 수
    infected = count;   // [수정] printf -> 전역에 담아 solution()이 반환한다
}

// ---------------------------
// 메인
// ---------------------------
// [수정] main() -> solution()
int solution(int v, vector<vector<int>> edges)
{
    input(v, edges);

    BFS(1);    // 1번 컴퓨터에서 감염 시작

    return infected;   // [수정] BFS 안의 printf 대신 전역에 담아 둔 값을 반환
}
