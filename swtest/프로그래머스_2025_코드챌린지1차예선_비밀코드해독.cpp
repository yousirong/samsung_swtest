/*
	[프로그래머스] 2025 프로그래머스 코드챌린지 1차 예선 - 비밀 코드 해독
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
#include <stdio.h>              // scanf, printf 사용

#define MAX_N (30+5)            // n 최대 30 + 여유
#define MAX_M (10+5)            // m 최대 10 + 여유

int T;                      // 테스트 케이스 수

int N, M;                   // 수의 범위, 시도 횟수
int QUERY[MAX_M][5];        // 시도마다 넣은 5개
int RESP[MAX_M];            // 시도마다 받은 응답 (겹친 개수)

int selected[MAX_N];        // selected[x] == 1 : x를 비밀 코드 후보로 골랐다

int COUNT;                  // 맞는 후보 수 (템플릿의 지역 answer와 겹치지 않게 대문자)

void input()
{
	scanf("%d %d", &N, &M);              // 수의 범위, 시도 횟수 입력

	for (int i = 0; i < M; i++)          // 시도마다
		for (int j = 0; j < 5; j++)      // 넣은 5개
			scanf("%d", &QUERY[i][j]);   // 입력

	for (int i = 0; i < M; i++)          // 시도마다
		scanf("%d", &RESP[i]);           // 응답 입력

	for (int x = 0; x <= N; x++)         // 모든 수
		selected[x] = 0;                 // 고르지 않은 상태로

	COUNT = 0;                           // 후보 수 초기화
}

// 지금 고른 5개가 모든 시도의 응답과 맞는가
int check()
{
	for (int i = 0; i < M; i++)          // 시도마다
	{
		int cnt = 0;                     // 겹친 개수

		for (int j = 0; j < 5; j++)      // 시도한 5개 각각
			if (selected[QUERY[i][j]] == 1) cnt++;   // 후보에 있으면 센다

		if (cnt != RESP[i]) return 0;    // 응답과 다르면 탈락
	}
	return 1;                            // 모든 응답과 일치
}

// L개 골랐고, 다음은 start 이상에서 고른다
void DFS(int L, int start)
{
	if (L == 5)                          // 5개를 다 골랐으면
	{
		if (check() == 1) COUNT++;       // 맞으면 후보 수 증가
		return;                          // 되돌아간다
	}

	for (int i = start; i <= N; i++)     // start부터 N까지 후보
	{
		selected[i] = 1;                 // i를 고른다
		DFS(L + 1, i + 1);               // 다음은 i보다 큰 수에서
		selected[i] = 0;   // 원상복구
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		DFS(0, 1);                       // 0개 고른 상태, 1부터

		printf("%d\n", COUNT);           // 후보 수 출력
	}

	return 0;                            // 프로그램 정상 종료
}
