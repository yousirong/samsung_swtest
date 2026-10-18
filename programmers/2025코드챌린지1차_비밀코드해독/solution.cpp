/*
	[프로그래머스] 2025 프로그래머스 코드챌린지 1차 예선 - 비밀 코드 해독

	[프로그래머스 제출용]  원본 : swtest/프로그래머스_2025_코드챌린지1차예선_비밀코드해독.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	solution()은 프로그래머스 템플릿 줄을 그대로 두고 가운데 세 줄만 넣었다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://school.programmers.co.kr/learn/courses/30/lessons/388352

	■ 문제 요약
	  비밀 코드는 1 ~ n 중 서로 다른 정수 5개다 (10 <= n <= 30).
	  m번(1 ~ 10) 시도했고, 시도마다 넣은 5개 중 몇 개가 비밀 코드에 들어 있는지 응답을 받았다.
	  모든 응답과 맞는 비밀 코드 후보의 개수를 구한다.

	  입출력 예
	      n 10, 시도 5번 -> 3
	      n 15, 시도 5번 -> 5

	■ 풀이 방침 : 5개 고르는 조합을 DFS로 전부 만들어 보고, 시도마다 겹치는 개수를 확인
	  1 ~ n 중 5개 고르기는 C(30, 5) = 142,506가지뿐이다. 전부 만들어 봐도 된다.

	      DFS(L, start)
	        - L == 5 : 5개를 다 골랐다 -> 모든 시도와 맞는지 check()
	        - start ~ n 중 하나(i)를 고르고 DFS(L + 1, i + 1)
	          (i + 1부터 고르므로 오름차순 조합만 나온다 = 같은 조합을 두 번 세지 않는다)

	  삼성 기출의 "치킨 배달 / 병원 거리 최소화"에서 M개 고르는 조합 DFS와 같은 모양이다.

	■ 확인 (check)
	  고른 5개를 selected[수] = 1 로 표시해 두면, 시도의 5개 각각이 비밀 코드에 있는지 바로 안다.
	  시도마다 겹친 개수가 응답과 하나라도 다르면 탈락.
	  한 후보당 m x 5 = 최대 50번 비교 -> 전체 약 700만 번이라 충분히 빠르다.

	■ 주의할 점
	  - selected[]는 DFS에서 고를 때 1, 돌아올 때 0으로 되돌린다 (원상복구).
	  - 남은 수가 모자라면(n - i + 1 < 5 - L) 더 볼 필요가 없지만, 경우의 수가 작아서 생략했다.
*/
#include <stdio.h>
#include <string>
#include <vector>              // [추가] 프로그래머스 템플릿 그대로

using namespace std;

#define MAX_N (30+5)
#define MAX_M (10+5)

int T;

int N, M;                   // 수의 범위, 시도 횟수
int QUERY[MAX_M][5];        // 시도마다 넣은 5개
int RESP[MAX_M];            // 시도마다 받은 응답 (겹친 개수)

int selected[MAX_N];        // selected[x] == 1 : x를 비밀 코드 후보로 골랐다

int COUNT;                  // 맞는 후보 수 (템플릿의 지역 answer와 겹치지 않게 대문자)

// [수정] scanf 대신 인자로 받는다. 매개변수 이름은 템플릿과 같게 두었다 (전역은 대문자라 겹치지 않는다)
void input(int n, const vector<vector<int>>& q, const vector<int>& ans)
{
	N = n;                   // [수정] scanf 대체
	M = (int)q.size();

	for (int i = 0; i < M; i++)
		for (int j = 0; j < 5; j++)
			QUERY[i][j] = q[i][j];   // [수정] scanf 대체

	for (int i = 0; i < M; i++)
		RESP[i] = ans[i];          // [수정] scanf 대체

	for (int x = 0; x <= N; x++)
		selected[x] = 0;

	COUNT = 0;
}

// 지금 고른 5개가 모든 시도의 응답과 맞는가
int check()
{
	for (int i = 0; i < M; i++)
	{
		int cnt = 0;

		for (int j = 0; j < 5; j++)
			if (selected[QUERY[i][j]] == 1) cnt++;

		if (cnt != RESP[i]) return 0;
	}
	return 1;
}

// L개 골랐고, 다음은 start 이상에서 고른다
void DFS(int L, int start)
{
	if (L == 5)
	{
		if (check() == 1) COUNT++;
		return;
	}

	for (int i = start; i <= N; i++)
	{
		selected[i] = 1;
		DFS(L + 1, i + 1);
		selected[i] = 0;   // 원상복구
	}
}

// [수정] main() -> solution(). 템플릿 그대로 두고 가운데 세 줄만 넣었다
int solution(int n, vector<vector<int>> q, vector<int> ans) {
    int answer = 0;
    input(n, q, ans);     // [추가]
    DFS(0, 1);            // [추가]
    answer = COUNT;       // [추가]
    return answer;
}
