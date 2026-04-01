// 코드트리 2017 상반기 오전 2번 - 외주 수익 최대화하기
// https://www.codetree.ai/training-field/frequent-problems/problems/max-of-outsourcing-profit
// 문제 요약
// 1일부터 N일까지 매일 "그 날 시작할 수 있는 외주"가 하나씩 주어진다.
// 각 외주는
// - 걸리는 기간 time[i]
// - 완료 시 얻는 수익 profit[i]
// 를 가진다.
//
// 어떤 날의 외주를 선택하면
// 그 기간 동안 다른 외주를 동시에 할 수 없다.
//
// 목표
// N일 안에 끝낼 수 있는 외주들만 골라
// 얻을 수 있는 총 수익의 최댓값을 구하는 문제
//
// 핵심 아이디어
// - 각 날짜 i에 대해
//   "그 날 외주를 선택한다(1)" / "선택하지 않는다(0)"
//   두 가지 경우를 DFS로 전부 만들어 본다.
// - 완성된 선택 결과(num_of_cases[])가
//   실제로 가능한 일정인지 check()로 검사한다.
// - 가능하다면 getProfit()으로 수익을 계산해 최댓값 갱신
//
// 이 코드는 완전탐색 방식이다.
// 즉, 1일부터 N일까지에 대해 선택/비선택 모든 경우(2^N)를 확인한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (15 + 5)

int T;

int N;                 // 총 날짜 수
int time[MAX];         // i일에 시작하는 외주의 소요 기간
int profit[MAX];       // i일에 시작하는 외주의 수익

int num_of_cases[MAX]; // i일 외주를 선택했는지 여부 (0 또는 1)

int maxAnswer;         // 가능한 최대 수익

// ---------------------------
// 입력 함수
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
// num_of_cases[i] == 1 이면 i일 외주 선택
// ---------------------------
void printCases()
{
	for (int i = 0; i <= N; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 선택 상태가 실제로 가능한 일정인지 검사
//
// 검사 방법
// - visit[d] = d일이 이미 어떤 외주에 의해 사용 중인지 표시
// - i일 외주를 선택했다면
//   i일부터 i + time[i] - 1일까지 모두 비어 있어야 함
// - 하나라도 겹치거나 N일을 넘어가면 불가능
// ---------------------------
bool check()
{
	bool visit[MAX] = { 0 };

	for (int i = 1; i <= N; i++)
	{
		// i일 외주를 선택하지 않았으면 건너뜀
		if (num_of_cases[i] == 0)
			continue;

		// i일 외주가 차지하는 날짜를 모두 확인
		for (int k = 0; k < time[i]; k++)
		{
			// 기간이 N일을 넘어서면 불가능
			if (N < i + k)
				return false;

			// 이미 다른 외주가 사용한 날짜면 겹치므로 불가능
			if (visit[i + k] == true)
				return false;

			// 해당 날짜 사용 처리
			visit[i + k] = true;
		}
	}

	// 끝까지 문제 없으면 가능한 일정
	return true;
}

// ---------------------------
// 현재 선택 상태의 총 수익 계산
//
// num_of_cases[i]가 1이면 profit[i]를 더함
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
// DFS로 모든 선택 경우 생성
//
// depth = 현재 몇 번째 날짜까지 결정했는지
//
// depth가 0이면 아직 아무 날짜도 결정 안 함
// depth가 N이면 1일 ~ N일 선택 여부가 모두 정해진 상태
// ---------------------------
void DFS(int depth)
{
	// 1일 ~ N일 선택 여부가 모두 정해진 경우
	if (depth == N)
	{
		// printCases();

		// 가능한 일정인지 확인
		if (check() == true)
		{
			// 가능하면 총 수익 계산
			int tmp = getProfit();

			// 최댓값 갱신
			if (maxAnswer < tmp)
				maxAnswer = tmp;
		}
		return;
	}

	// 현재 날짜(depth+1)에 대해
	// 0 = 선택 안 함, 1 = 선택 함
	for (int i = 0; i < 2; i++)
	{
		num_of_cases[depth + 1] = i;  // 1일차 ~ N일차 선택 여부 저장

		DFS(depth + 1);
	}
}

int main()
{
	// 원래 테스트케이스가 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	int T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		maxAnswer = 0;

		// 모든 경우의 수 탐색 시작
		DFS(0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}