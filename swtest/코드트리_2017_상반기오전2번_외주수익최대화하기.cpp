/*
	[코드트리] 2017 상반기 오전 2번 - 외주 수익 최대화하기
	https://www.codetree.ai/training-field/frequent-problems/problems/max-of-outsourcing-profit

	■ 문제 요약
	  1일부터 N일까지, 각 날짜마다 그날 시작할 수 있는 외주가 하나씩 주어진다.
	  i일 외주는 time[i]일이 걸리고 끝내면 profit[i]의 수익을 준다.

	  한 번에 하나의 외주만 할 수 있고(기간이 겹치면 안 된다),
	  N일을 넘어가서 끝나는 외주는 할 수 없다.
	  얻을 수 있는 총 수익의 최댓값을 구한다.

	  (백준 14501 "퇴사"와 같은 문제다)

	■ 풀이 방침 : 선택/비선택 완전탐색
	  N이 15 이하로 아주 작으므로, 각 날짜를 "한다 / 안 한다" 두 가지로 나눈
	  2^N가지 조합을 전부 만들어 봐도 된다.

	    1) DFS로 num_of_cases[1..N]에 0/1을 채운다
	    2) 다 채워지면 check()로 그 조합이 실제로 가능한 일정인지 본다
	    3) 가능하면 getProfit()으로 수익을 더해 최댓값을 갱신한다

	  2^15 = 32,768가지이고 검사도 가볍기 때문에 넉넉하게 통과한다.

	■ 가능한 일정인지 판정하기 : check()
	  선택한 외주들이 서로 겹치지 않고 N일 안에 끝나는지 본다.
	  날짜별 사용 여부를 visit에 표시하며 훑는 방식이다.

	    i일 외주를 골랐다면 i일부터 i + time[i] - 1일까지가 필요하므로
	      - 그 범위가 N을 넘으면 불가능
	      - 그중 이미 다른 외주가 쓴 날이 있으면 불가능
	    문제가 없으면 그 날짜들을 사용 중으로 표시하고 다음 외주를 본다.

	■ 참고 : 더 빠른 풀이 (DP)
	  N이 커지면 완전탐색은 쓸 수 없다. 이 문제는 뒤에서부터 채우는 DP로 O(N)에 풀린다.

	    dp[i] = i일부터 시작해서 얻을 수 있는 최대 수익
	    dp[i] = max( dp[i + 1],                                  // i일 외주를 안 한다
	                 profit[i] + dp[i + time[i]] )               // i일 외주를 한다 (i + time[i] - 1 <= N 일 때만)

	  여기서는 문제 크기가 작아 이해하기 쉬운 완전탐색으로 두었다.

	■ 사소한 주의
	  전역 배열 이름 time은 <time.h>의 time() 함수와 이름이 같다.
	  이 파일은 <stdio.h>만 포함해서 충돌하지 않지만, 헤더를 추가하면 문제가 될 수 있다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (15 + 5)

int T;

int N;                 // 총 날짜 수
int time[MAX];         // i일에 시작하는 외주의 소요 기간
int profit[MAX];       // i일에 시작하는 외주의 수익

int num_of_cases[MAX]; // num_of_cases[i] : i일 외주를 선택했으면 1, 아니면 0

int maxAnswer;         // 가능한 최대 수익

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int i = 1; i <= N; i++)
	{
		scanf("%d %d", &time[i], &profit[i]);
	}
}

// ---------------------------
// 디버그용: 현재 선택 상태 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i <= N; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 선택 조합이 실제로 수행 가능한 일정인지 검사
//
//   visit[d] : d일이 이미 어떤 외주에 점유되었는지
//
// 선택한 외주를 앞에서부터 하나씩 보며 필요한 날짜를 예약한다.
// 기간이 N을 넘거나 이미 예약된 날과 겹치면 그 즉시 불가능.
// ---------------------------
bool check()
{
	bool visit[MAX] = { 0 };   // 호출할 때마다 새로 비운다

	for (int i = 1; i <= N; i++)
	{
		// 고르지 않은 날은 볼 필요 없다
		if (num_of_cases[i] == 0)
			continue;

		// i일 외주가 차지하는 날짜 : i ~ i + time[i] - 1
		for (int k = 0; k < time[i]; k++)
		{
			// N일을 넘겨서 끝나면 이 외주는 할 수 없다
			if (N < i + k)
				return false;

			// 다른 외주가 이미 쓰고 있는 날이면 겹친다
			if (visit[i + k] == true)
				return false;

			visit[i + k] = true;   // 이 날을 점유
		}
	}

	return true;
}

// ---------------------------
// 현재 선택 조합의 총 수익
//
// 선택 여부가 0/1이므로 곱해서 더하면 조건문 없이 합계가 나온다.
// ---------------------------
int getProfit()
{
	int sum = 0;

	for (int i = 1; i <= N; i++)
	{
		sum += (num_of_cases[i] * profit[i]);
	}

	return sum;
}

// ---------------------------
// DFS : 날짜마다 선택/비선택을 정해 2^N가지 조합을 만든다
//
//   depth : 지금까지 정한 날짜 수 (depth일까지 결정 완료)
// ---------------------------
void DFS(int depth)
{
	// 1일 ~ N일의 선택 여부가 모두 정해졌다
	if (depth == N)
	{
		// printCases();

		// 겹치지 않고 N일 안에 끝나는 조합만 후보가 된다
		if (check() == true)
		{
			int tmp = getProfit();

			if (maxAnswer < tmp)
				maxAnswer = tmp;
		}
		return;
	}

	// 다음 날짜(depth + 1)를 0(안 함) / 1(함) 두 가지로 갈라 본다
	for (int i = 0; i < 2; i++)
	{
		num_of_cases[depth + 1] = i;

		DFS(depth + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	int T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// 아무것도 선택하지 않으면 수익 0이므로 0에서 시작하면 된다
		maxAnswer = 0;

		DFS(0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}
