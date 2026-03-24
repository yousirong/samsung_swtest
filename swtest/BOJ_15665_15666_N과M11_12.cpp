// BOJ 15665 - N과 M (11)
//
// 문제 요약
// 입력으로 주어진 숫자들 중에서 길이 M의 수열을 만든다.
//
// 특징
// - 입력에 같은 값이 여러 번 들어올 수 있다.
// - 하지만 결과에서 같은 수열이 여러 번 출력되면 안 된다.
// - 따라서 먼저 "서로 다른 값만" 추려서 정렬한 뒤 DFS를 돌린다.
//
// 이 문제의 성격
// - 중복 순열
// - 같은 값을 여러 번 뽑아도 됨
// - 순서가 다르면 다른 수열
//
// 예)
// 입력: 9 7 9
// 중복 제거 후: 7 9
//
// M = 2 이면 출력:
// 7 7
// 7 9
// 9 7
// 9 9

#include <stdio.h>
#include <stdbool.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];     // 현재까지 만든 수열
int number[MAX];           // 중복 제거 후 남긴 서로 다른 값들
bool check[10000 + 100];   // 어떤 값이 이미 number[]에 들어갔는지 확인

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
	// 길이 M 수열 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 현재 자리에 서로 다른 값들 중 하나를 선택
	for (int i = 1; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		// 중복 선택 허용이므로 visit 같은 배열은 필요 없음
		DFS(depth + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;

	// 입력값을 받으면서
	// 같은 값은 한 번만 number[]에 저장
	for (int i = 1; i <= N; i++)
	{
		int value;
		scanf("%d", &value);

		// 아직 저장하지 않은 값이면 추가
		if (check[value] == false)
		{
			number[count++] = value;
			check[value] = true;
		}
	}

	// 이제 N을 "서로 다른 값의 개수"로 다시 사용
	N = count - 1;

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

	// DFS 시작
	DFS(0);

	return 0;
}

// BOJ 15666 - N과 M (12)
//
// 문제 요약
// 입력으로 주어진 숫자들 중에서 길이 M의 수열을 만든다.
//
// 특징
// - 입력에 같은 값이 여러 번 들어올 수 있다.
// - 하지만 결과에서 같은 수열이 여러 번 출력되면 안 된다.
// - 따라서 먼저 "서로 다른 값만" 추려서 정렬한다.
//
// 이 문제의 성격
// - 중복 조합
// - 같은 값을 여러 번 뽑아도 됨
// - 순서는 중요하지 않음
//
// 따라서 핵심은:
// DFS(depth + 1, i)
//
// 즉, 현재 고른 값 i를 다음에도 다시 고를 수 있고,
// start를 사용해서 앞쪽 값으로 되돌아가지 않게 한다.
//
// 예)
// 입력: 9 7 9
// 중복 제거 후: 7 9
//
// M = 2 이면 출력:
// 7 7
// 7 9
// 9 9

#include <stdio.h>
#include <stdbool.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];     // 현재까지 만든 수열
int number[MAX];           // 중복 제거 후 남긴 서로 다른 값들
bool check[10000 + 100];   // 어떤 값이 이미 number[]에 들어갔는지 확인

// 현재 완성된 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 중복 조합 생성
// depth : 현재 몇 번째 자리를 채우는지
// start : 이번에 고를 수 있는 시작 인덱스
void DFS(int depth, int start)
{
	// 길이 M 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// start부터 N까지 선택
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		// 중복 허용 조합이므로
		// 다음에도 i부터 다시 선택 가능
		DFS(depth + 1, i);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;

	// 입력값을 받으면서 같은 값은 한 번만 number[]에 저장
	for (int i = 1; i <= N; i++)
	{
		int value;
		scanf("%d", &value);

		if (check[value] == false)
		{
			number[count++] = value;
			check[value] = true;
		}
	}

	// 이후 N은 "서로 다른 값의 개수"로 다시 사용
	N = count - 1;

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

	// DFS 시작
	DFS(0, 1);

	return 0;
}