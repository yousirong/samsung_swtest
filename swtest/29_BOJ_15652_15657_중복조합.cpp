// BOJ 15652 - N과 M (4)
//
// 문제 요약
// 1부터 N까지 숫자 중에서
// 중복을 허용하여 M개를 고르되,
// 비내림차순(오름차순과 유사) 형태로 출력하는 문제이다.
//
// 특징
// - 중복 허용: 같은 숫자를 여러 번 골라도 된다.
// - 순서 중요하지 않음: 1 2 와 2 1 은 같은 경우로 본다.
// - 따라서 결과는 항상 앞에서 뒤로 갈수록 작아지지 않는 형태가 된다.
//
// 예)
// N=3, M=2
// 가능한 출력:
// 1 1
// 1 2
// 1 3
// 2 2
// 2 3
// 3 3
//
// 핵심 아이디어
// - 조합이므로 start를 사용한다.
// - 중복 허용이므로 다음 DFS에서도 i부터 다시 고를 수 있다.
//   즉, DFS(depth + 1, i) 가 핵심이다.

#include <stdio.h>

#define MAX (7 + 3)

int N, M;
int num_of_cases[MAX];   // 현재까지 만든 수열(조합)

// 현재 완성된 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 중복 조합 생성
// depth : 현재 몇 개 골랐는지
// start : 이번에 선택 가능한 시작 숫자
void DFS(int depth, int start)
{
	// 길이 M이 완성되면 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// start부터 N까지 선택 가능
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = i;

		// 중복 허용이므로 다음에도 i를 다시 선택 가능
		DFS(depth + 1, i);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0, 1);

	return 0;
}

// BOJ 15657 - N과 M (8)
//
// 문제 요약
// 입력으로 주어진 N개의 숫자 중에서
// 중복을 허용하여 M개를 고르되,
// 비내림차순 형태로 출력하는 문제이다.
//
// 특징
// - 중복 허용: 같은 값을 여러 번 골라도 됨
// - 순서 중요하지 않음: 앞에서 뒤로 갈수록 작아지지 않게 출력
// - 사전순 출력을 위해 먼저 입력 숫자를 정렬해야 함
//
// 예)
// 입력 숫자: 9 7 1
// 정렬 후:   1 7 9
//
// M=2 이면 가능한 출력:
// 1 1
// 1 7
// 1 9
// 7 7
// 7 9
// 9 9
//
// 핵심 아이디어
// - DFS 구조는 15652와 거의 같다.
// - 차이점은 i 자체를 넣는 것이 아니라 입력값 number[i]를 넣는다는 점
// - 중복 허용 조합이므로 DFS(depth + 1, i) 형태 사용

#include <stdio.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];   // 현재까지 만든 수열
int number[MAX];         // 입력으로 주어진 숫자들

// 현재 완성된 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 중복 조합 생성
// depth : 현재 몇 개 골랐는지
// start : 이번에 선택 가능한 시작 인덱스
void DFS(int depth, int start)
{
	// 길이 M 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// start부터 N까지 선택 가능
	for (int i = start; i <= N; i++)
	{
		// 이번 문제는 실제 입력값을 넣는다
		num_of_cases[depth] = number[i];

		// 중복 허용 조합이므로 다음에도 i부터 다시 선택 가능
		DFS(depth + 1, i);
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