// BOJ 15651 - N과 M (3)
// https://www.acmicpc.net/problem/15651
// 
// 문제 요약
// 1부터 N까지의 숫자 중에서
// 길이 M의 수열을 만든다.
//
// 특징
// - 같은 숫자를 여러 번 골라도 된다. (중복 허용)
// - 순서가 다르면 다른 경우로 본다. (순서 중요)
//
// 예)
// N = 3, M = 2
// 가능한 결과:
// 1 1
// 1 2
// 1 3
// 2 1
// 2 2
// 2 3
// 3 1
// 3 2
// 3 3
//
// 핵심 아이디어
// - 중복 순열이므로 visit 배열이 필요 없다.
// - 각 자리(depth)마다 1부터 N까지를 모두 다시 선택할 수 있다.

#include <stdio.h>

#define MAX (7 + 3)

int N, M;
int num_of_cases[MAX];   // 현재까지 만든 수열

// 현재 완성된 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 중복 순열 생성
// depth = 현재 몇 번째 자리를 채우는지
void DFS(int depth)
{
	// 길이 M 수열이 완성되면 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 현재 자리에는 1 ~ N 중 아무 숫자나 올 수 있다
	for (int i = 1; i <= N; i++)
	{
		num_of_cases[depth] = i;

		// 중복 허용이므로 visit 체크 없이 바로 다음 자리로 진행
		DFS(depth + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0);

	return 0;
}

// BOJ 15656 - N과 M (7)
// https://www.acmicpc.net/problem/15656
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