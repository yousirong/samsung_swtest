/*
	[프로그래머스] 2025 프로그래머스 코드챌린지 2차 예선 - 완전범죄

	[프로그래머스 제출용]  원본 : swtest/프로그래머스_2025_코드챌린지2차예선_완전범죄.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://school.programmers.co.kr/learn/courses/30/lessons/389480

	■ 문제 요약
	  물건 i를 A도둑이 훔치면 A 흔적 info[i][0]개, B도둑이 훔치면 B 흔적 info[i][1]개가 남는다 (1 ~ 3).
	  A 흔적 누적이 n 이상이거나 B 흔적 누적이 m 이상이면 붙잡힌다.
	  둘 다 안 붙잡히고 모든 물건을 훔칠 때, A 흔적 누적의 최솟값을 구한다. 불가능하면 -1.

	  제한 : 물건 1 ~ 40개, 1 <= n, m <= 120

	  입출력 예
	      [[1,2],[2,3],[2,1]], 4, 4 -> 2
	      [[1,2],[2,3],[2,1]], 1, 7 -> 0
	      [[3,3],[3,3]], 7, 1 -> 6
	      [[3,3],[3,3]], 6, 1 -> -1

	■ 풀이 방침 : 물건마다 "A가 훔친다 / B가 훔친다" 두 갈래 DFS + 방문 체크
	  그냥 DFS면 2^40 가지라 너무 많다.
	  그런데 "몇 번째 물건까지 정했고(L), A 흔적이 a, B 흔적이 b" 인 상태는
	  어떤 순서로 왔든 앞으로 할 수 있는 일이 똑같다.
	  -> visit[L][a][b]로 한 번 와 본 상태는 다시 보지 않는다.
	     상태 수는 41 x 120 x 120 = 약 59만 개뿐이다.

	      DFS(L, a, b)
	        - a >= n 또는 b >= m        -> 붙잡힘, 실패
	        - a >= answer               -> 이미 찾은 답보다 나을 수 없다 (가지치기)
	        - visit[L][a][b] == 1       -> 이미 본 상태
	        - L == 물건 수              -> 다 훔쳤다, answer = min(answer, a)
	        - A가 훔친다 : DFS(L + 1, a + A[L], b)
	        - B가 훔친다 : DFS(L + 1, a, b + B[L])

	  삼성 기출의 "DFS로 고르기 + 같은 상태 다시 안 가기" (BFS의 visit와 같은 생각)이다.

	■ 가지치기와 방문 체크를 같이 써도 되는 이유
	  가지치기로 잘린 갈래는 어차피 지금 answer보다 나을 수 없다.
	  answer는 줄어들기만 하므로, 나중에 같은 상태에 다시 와도 그 갈래가 답이 될 수는 없다.

	■ 주의할 점
	  - 흔적은 n "이상"이면 붙잡힌다. 그래서 a < n, b < m 이어야 살아 있다 (a는 0 ~ n-1).
	  - visit는 케이스마다 비워야 한다.
	  - 답을 못 찾으면 answer가 INF 그대로 -> -1을 출력한다.
*/
#include <stdio.h>
#include <string>
#include <vector>              // [추가] 프로그래머스 템플릿 그대로

using namespace std;

#define MAX_ITEM (40+5)
#define MAX_T (120+5)
#define INF (0x7fff0000)

int T;

int CNT, N, M;                      // 물건 수, A가 붙잡히는 흔적, B가 붙잡히는 흔적
int A[MAX_ITEM], B[MAX_ITEM];       // 물건마다 A / B 흔적

char visit[MAX_ITEM][MAX_T][MAX_T]; // visit[L][a][b] : 이 상태에 와 봤는가

int answer;

// [수정] scanf 대신 인자로 받는다
void input(const vector<vector<int>>& info, int n, int m)
{
	CNT = (int)info.size();   // [수정] scanf 대체
	N = n;
	M = m;

	for (int i = 0; i < CNT; i++)
	{
		A[i] = info[i][0];     // [수정] scanf 대체
		B[i] = info[i][1];
	}

	for (int L = 0; L <= CNT; L++)
		for (int a = 0; a < N; a++)
			for (int b = 0; b < M; b++)
				visit[L][a][b] = 0;

	answer = INF;
}

// L번 물건을 누가 훔칠지 정할 차례, 지금까지 A 흔적 a, B 흔적 b
void DFS(int L, int a, int b)
{
	if (a >= N || b >= M) return;     // 붙잡힘
	if (a >= answer) return;          // 가지치기
	if (visit[L][a][b] == 1) return;  // 이미 본 상태

	visit[L][a][b] = 1;

	if (L == CNT)                     // 모든 물건을 훔쳤다
	{
		if (a < answer) answer = a;
		return;
	}

	DFS(L + 1, a + A[L], b);          // A가 훔친다
	DFS(L + 1, a, b + B[L]);          // B가 훔친다
}

// [수정] main() -> solution()
int solution(vector<vector<int>> info, int n, int m)
{
	input(info, n, m);

	DFS(0, 0, 0);

	if (answer == INF) answer = -1;

	return answer;   // [수정] printf -> return
}
