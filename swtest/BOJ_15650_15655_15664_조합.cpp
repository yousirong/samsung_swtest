// BOJ 15650 - N과 M (2)
//
// 문제 요약
// 1부터 N까지 숫자 중에서
// 중복 없이 M개를 골라
// 오름차순으로 출력하는 문제이다.
//
// 핵심 아이디어
// - 조합은 순서가 중요하지 않다.
// - 따라서 DFS에서 start 인자를 두고
//   다음 숫자는 현재 숫자보다 뒤에서만 고르게 한다.
// - 이렇게 하면 자동으로 오름차순이 되고,
//   같은 조합을 여러 번 만들지 않게 된다.
//
// 예)
// N=4, M=2
// 가능한 출력:
// 1 2
// 1 3
// 1 4
// 2 3
// 2 4
// 3 4

#include <stdio.h>

#define MAX (8 + 3)

int N, M;
int num_of_cases[MAX];   // 현재까지 만든 조합

// 현재 조합 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 조합 만들기
// depth : 현재 몇 개 골랐는지
// start : 이번에 고를 수 있는 시작 숫자
void DFS(int depth, int start)
{
	// M개를 모두 골랐으면 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// start부터 N까지 하나씩 선택
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = i;

		// 조합이므로 다음 숫자는 i보다 뒤에서만 선택
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0, 1);

	return 0;
}

// BOJ 15655 - N과 M (6)
//
// 문제 요약
// 입력으로 주어진 N개의 숫자 중에서
// 중복 없이 M개를 골라
// 오름차순 조합 형태로 출력하는 문제이다.
//
// 핵심 아이디어
// - DFS 구조는 15650과 거의 같다.
// - 차이점은 1~N이 아니라 입력 배열 number[]의 실제 값을 사용한다.
// - 사전순 출력을 위해 먼저 number[]를 정렬한다.

#include <stdio.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 현재 조합
int number[MAX];         // 입력으로 주어진 숫자들

// 현재 조합 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 조합 생성
// depth : 현재 몇 개 골랐는지
// start : 이번에 선택 가능한 시작 인덱스
void DFS(int depth, int start)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	for (int i = start; i <= N; i++)
	{
		// 입력 숫자 자체를 넣는다
		num_of_cases[depth] = number[i];

		// 조합이므로 다음은 i보다 뒤에서만 선택
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	// 입력 숫자 받기
	for (int i = 1; i <= N; i++)
		scanf("%d", &number[i]);

	// 사전순 출력을 위해 정렬
	for (int i = 1; i <= N - 1; i++)
	{
		for (int k = i + 1; k <= N; k++)
		{
			if (number[i] > number[k])
			{
				int tmp = number[i];
				number[i] = number[k];
				number[k] = tmp;
			}
		}
	}

	DFS(0, 1);

	return 0;
}

// BOJ 15656 - N과 M (7)
//
// 문제 요약
// 입력으로 주어진 N개의 숫자 중에서
// 길이 M의 수열을 만든다.
//
// 특징
// - 같은 숫자를 여러 번 골라도 된다. (중복 허용)
// - 순서가 다르면 다른 경우로 본다. (순서 중요)
// - 출력은 사전순이어야 하므로 입력 숫자를 먼저 정렬한다.
//
// 예)
// 입력 숫자: 9 7 1
// 정렬 후: 1 7 9
//
// M = 2 이면 가능한 결과:
// 1 1
// 1 7
// 1 9
// 7 1
// 7 7
// 7 9
// 9 1
// 9 7
// 9 9
//
// 핵심 아이디어
// - DFS 구조는 15651과 거의 같다.
// - 차이점은 1~N이 아니라 number[i]라는 실제 입력값을 넣는다는 점이다.
// - 중복 허용이므로 visit 배열은 필요 없다.

#include <stdio.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];   // 현재까지 만든 수열
int number[MAX];         // 입력으로 주어진 숫자들

// 현재 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 중복 순열 생성
void DFS(int depth)
{
	// 길이 M 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 현재 자리에는 입력 숫자들 중 아무 값이나 다시 올 수 있다
	for (int i = 1; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		// 중복 허용이므로 사용 체크 없이 다음 자리 진행
		DFS(depth + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	// 입력 숫자 받기
	for (int i = 1; i <= N; i++)
		scanf("%d", &number[i]);

	// 사전순 출력을 위해 정렬
	for (int i = 1; i <= N - 1; i++)
	{
		for (int k = i + 1; k <= N; k++)
		{
			if (number[i] > number[k])
			{
				int tmp = number[i];
				number[i] = number[k];
				number[k] = tmp;
			}
		}
	}

	DFS(0);

	return 0;
}