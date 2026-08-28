/*
	[코드트리] 2017 하반기 오후 2번 - 연산자 배치하기
	https://www.codetree.ai/training-field/frequent-problems/problems/arrange-operator

	■ 문제 요약
	  숫자 N개가 순서대로 주어지고, 쓸 수 있는 +, -, * 의 개수가 각각 주어진다.
	  (연산자 개수의 합은 정확히 N-1개다)

	  숫자의 순서는 바꿀 수 없고, 숫자 사이에 연산자를 배치해 만든 식의
	  결과의 최솟값과 최댓값을 출력한다.

	  연산자 우선순위는 무시하고 항상 앞에서부터 차례대로 계산한다.

	      a op1 b op2 c op3 d  ->  (((a op1 b) op2 c) op3 d)

	  (백준 14888 "연산자 끼워넣기"에서 나눗셈만 빠진 형태다)

	■ 풀이 방침 : 연산자 배치 완전탐색 (백트래킹)
	  자리마다 어떤 연산자를 넣을지 재귀로 정한다. 30번 파일의 순열과 같은 골격인데,
	  "각 연산자를 쓸 수 있는 개수가 정해져 있다"는 점이 다르다.

	    visit[i]    : i번 연산자를 지금까지 몇 번 썼는지 (bool이 아니라 카운터)
	    possible[i] : i번 연산자를 쓸 수 있는 최대 개수

	    if (visit[i] == possible[i]) continue;   // 재고 소진, 이 자리에 못 씀

	  이 구조는 30번 파일의 15663(중복 있는 원소의 순열)과 완전히 같다.

	  백트래킹의 짝을 반드시 지켜야 한다.
	      visit[i]++;  ->  DFS(depth + 1);  ->  visit[i]--;

	  연산자가 N-1개(최대 10개)이므로 경우의 수는 많아야 3^10 = 59,049 수준이다.

	■ 주의할 점
	  1) 숫자가 N개면 연산자는 N-1개다. 재귀 종료 조건이 depth == N - 1인 이유다.
	  2) 최솟값/최댓값을 함께 구하므로 각각 INF, -INF로 초기화한다.
	     결과가 음수일 수 있어서 maxAnswer를 0으로 두면 안 된다.
*/

#include <stdio.h>

#define MAX (11 + 5)
#define INF (0x7fff0000)

// 연산자 번호
#define PLUS     (0)
#define MINUS    (1)
#define MULTIPLY (2)

int T;

int N;                  // 숫자 개수
int num_of_cases[MAX];  // 자리별로 고른 연산자 번호 (길이 N-1)
int visit[MAX];         // visit[i] : i번 연산자를 지금까지 쓴 횟수
int possible[3];        // 쓸 수 있는 최대 개수 [+, -, *]
int number[MAX];        // 입력 숫자들

int minAnswer, maxAnswer;

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int i = 0; i < N; i++)
		scanf("%d", &number[i]);

	// possible[0] = '+' 개수, possible[1] = '-' 개수, possible[2] = '*' 개수
	for (int i = 0; i < 3; i++)
		scanf("%d", &possible[i]);
}

// ---------------------------
// 디버그용: 현재 연산자 배치 출력 (연산자는 N-1개)
// ---------------------------
void printCases()
{
	for (int i = 0; i < N - 1; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 배치대로 식의 값을 계산
//
// 우선순위 없이 앞에서부터 순서대로 누적한다.
// ---------------------------
int calculate()
{
	int sum = number[0];

	for (int i = 0; i < N - 1; i++)
	{
		if (num_of_cases[i] == PLUS)
			sum += number[i + 1];
		else if (num_of_cases[i] == MINUS)
			sum -= number[i + 1];
		else if (num_of_cases[i] == MULTIPLY)
			sum *= number[i + 1];
	}

	return sum;
}

// ---------------------------
// DFS : 자리마다 연산자를 하나씩 정한다
//
//   depth : 지금 정하려는 연산자 자리 (0부터, 총 N-1개)
// ---------------------------
void DFS(int depth)
{
	// 연산자를 다 배치했으면 식이 하나 완성된 것
	if (depth == N - 1)
	{
		// printCases();

		int tmp = calculate();

		if (maxAnswer < tmp)
			maxAnswer = tmp;

		if (minAnswer > tmp)
			minAnswer = tmp;

		return;
	}

	// 세 종류를 모두 시도하되, 남은 개수가 있는 것만
	for (int i = 0; i < 3; i++)
	{
		// 이 연산자는 이미 쓸 수 있는 만큼 다 썼다
		if (visit[i] == possible[i])
			continue;

		num_of_cases[depth] = i;

		visit[i]++;        // 하나 소비

		DFS(depth + 1);

		visit[i]--;        // 반납 (백트래킹)
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// 결과가 음수일 수 있으므로 양쪽 끝에서 시작한다
		minAnswer = INF;
		maxAnswer = -INF;

		DFS(0);

		printf("%d %d\n", minAnswer, maxAnswer);
	}

	return 0;
}
