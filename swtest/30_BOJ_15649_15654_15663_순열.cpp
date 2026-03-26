// BOJ 15649 - N과 M (1)
//
// 문제 요약
// 1부터 N까지 숫자 중에서
// 서로 다른 숫자 M개를 골라
// 순서를 고려한 수열을 모두 출력하는 문제이다.
//
// 예)
// N=3, M=2
// 출력:
// 1 2
// 1 3
// 2 1
// 2 3
// 3 1
// 3 2
//
// 핵심 아이디어
// - DFS(백트래킹)로 한 자리씩 수를 채운다.
// - 이미 사용한 숫자는 다시 쓰면 안 되므로 visit 배열로 체크한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 현재까지 만든 수열
bool visit[MAX];         // 숫자 i를 이미 사용했는지 표시

// 현재 만들어진 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 수열 생성
// depth = 현재 몇 번째 자리를 채우고 있는지
void DFS(int depth)
{
	// 길이 M의 수열이 완성되면 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 1부터 N까지 숫자를 하나씩 시도
	for (int i = 1; i <= N; i++)
	{
		// 이미 사용한 숫자는 중복 불가
		if (visit[i] == true)
			continue;

		// 현재 자리에 i 배치
		num_of_cases[depth] = i;

		// 사용 처리
		visit[i] = true;

		// 다음 자리 채우기
		DFS(depth + 1);

		// 백트래킹: 사용 해제
		visit[i] = false;
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0);

	return 0;
}

// BOJ 15654 - N과 M (5)
//
// 문제 요약
// 입력으로 주어진 N개의 숫자 중에서
// 서로 다른 숫자 M개를 골라
// 순서를 고려한 수열을 모두 출력하는 문제이다.
//
// 차이점
// - 15649는 1~N 자체를 사용
// - 15654는 입력 배열 number[]에 들어 있는 실제 값을 사용
//
// 핵심 아이디어
// - DFS 구조는 15649와 거의 같다.
// - 단, 현재 자리에 i가 아니라 number[i]를 넣는다.
// - 사전순 출력을 위해 먼저 입력값을 정렬한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 현재 만든 수열
bool visit[MAX];         // i번째 입력 숫자를 사용했는지
int number[MAX];         // 입력으로 주어진 숫자들

// 현재 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 수열 생성
void DFS(int depth)
{
	// 길이 M 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 입력 숫자들 중 하나 선택
	for (int i = 1; i <= N; i++)
	{
		// 이미 사용한 값이면 건너뜀
		if (visit[i] == true)
			continue;

		// 실제 입력값을 현재 자리에 넣음
		num_of_cases[depth] = number[i];

		// 사용 처리
		visit[i] = true;

		// 다음 자리 채우기
		DFS(depth + 1);

		// 백트래킹
		visit[i] = false;
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

// BOJ 15663 - N과 M (9)
//
// 문제 요약
// 입력 숫자들 중에는 같은 값이 여러 번 들어올 수 있다.
// 이 숫자들로 길이 M의 수열을 만들되,
// 같은 수열은 한 번만 출력해야 한다.
//
// 예)
// 입력: 1 1 2
// M=2
//
// 가능한 결과:
// 1 1
// 1 2
// 2 1
//
// 핵심 아이디어
// - 같은 값이 여러 번 입력될 수 있으므로
//   "서로 다른 값 목록"과 "각 값의 개수"로 압축해서 관리한다.
// - number[]   : 서로 다른 값들만 저장
// - possible[] : 각 값이 몇 번 등장했는지 저장
// - visit[]    : 현재 DFS에서 그 값을 몇 번 사용했는지 저장
//
// 주의
// 이 코드에서 visit[i]는 true/false가 아니라
// "i번째 서로 다른 값을 몇 번 사용했는지"를 뜻한다.

#include <stdio.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];      // 현재까지 만든 수열
int visit[MAX];             // i번째 서로 다른 값을 현재까지 몇 번 사용했는지
int possible[10000 + 500];  // 값 value가 입력에서 몇 번 나왔는지
int number[MAX];            // 서로 다른 값들만 저장

// 현재 수열 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 수열 생성
void DFS(int depth)
{
	// 길이 M 완성 시 출력
	if (depth == M)
	{
		printCases();
		return;
	}

	// 서로 다른 값들 중 하나 선택
	for (int i = 1; i <= N; i++)
	{
		// 해당 값을 이미 가능한 횟수만큼 다 사용했다면 건너뜀
		if (visit[i] == possible[number[i]])
			continue;

		// 현재 자리에 값 배치
		num_of_cases[depth] = number[i];

		// 사용 횟수 증가
		visit[i]++;

		// 다음 자리 채우기
		DFS(depth + 1);

		// 백트래킹: 사용 횟수 복구
		visit[i]--;
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;

	// 입력값을 받으면서
	// 같은 값은 number[]에 한 번만 저장하고
	// possible[value]에 등장 횟수 누적
	for (int i = 1; i <= N; i++)
	{
		int value;
		scanf("%d", &value);

		// 처음 등장한 값이면 number[]에 추가
		if (possible[value] == 0)
			number[count++] = value;

		// 등장 횟수 증가
		possible[value]++;
	}

	// 이후 N은 "서로 다른 값의 개수"로 다시 사용
	N = count - 1;

	// 서로 다른 값들을 정렬해 사전순 출력 보장
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