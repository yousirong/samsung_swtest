/*
	[BOJ] 15665 N과 M (11) / 15666 N과 M (12)
	https://www.acmicpc.net/problem/15665
	https://www.acmicpc.net/problem/15666

	■ 이 파일에 대하여
	  두 문제의 풀이가 나란히 들어 있는 정리용 파일이다.
	  main과 전역 변수가 각각 있으므로 한꺼번에 컴파일되지는 않는다.
	  필요한 쪽만 남기고 다른 쪽은 주석 처리해서 쓴다.

	■ 두 문제의 공통 조건
	    - 입력 N개의 수에 같은 값이 여러 번 들어올 수 있다
	    - 같은 값을 몇 번이든 다시 골라도 된다 (중복 선택에 개수 제한이 없다)
	    - 같은 수열은 한 번만 출력해야 한다
	    - 출력은 사전순

	■ 핵심 아이디어 : 입력을 먼저 "서로 다른 값"으로 줄인다
	  중복 선택에 횟수 제한이 없으므로, 입력에 7이 세 번 들어오든 한 번 들어오든
	  만들 수 있는 수열의 집합은 똑같다. 따라서 중복 값은 그냥 버리면 된다.

	      check[v]  : 값 v를 이미 목록에 넣었는지
	      number[]  : 서로 다른 값만 모은 목록 (이후 오름차순 정렬)
	      N         : 중복 제거 후 "서로 다른 값의 개수"로 의미가 바뀐다

	  후보에 같은 값이 하나씩만 남으므로 같은 수열이 두 번 생성될 수 없고,
	  중복 제거를 위한 별도의 처리가 재귀 안에 전혀 필요 없어진다.

	  ※ 30번 파일의 15663(N과 M (9))과 헷갈리기 쉽다.
	    15663은 "입력에 있는 개수만큼만" 쓸 수 있어서 등장 횟수를 세어 관리해야 하지만,
	    여기 15665 / 15666은 개수 제한이 없어서 그냥 중복 제거만 하면 된다.

	■ 두 문제의 차이
	    15665 : 순서를 구분한다 -> 중복순열. start 없이 매 자리에서 후보 전체를 돈다.
	    15666 : 비내림차순 출력 -> 중복조합. start를 두고 DFS(depth + 1, i)로 넘긴다.
*/

// ====================================================================
// BOJ 15665 - N과 M (11)  : 중복순열
//
//   예) 입력 9 7 9  ->  중복 제거 후 7 9, M = 2 이면
//       7 7 / 7 9 / 9 7 / 9 9
// ====================================================================

#include <stdio.h>
#include <stdbool.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];     // 만들고 있는 수열
int number[MAX];           // 중복 제거 후 남은 서로 다른 값들 (1-based)
bool check[10000 + 100];   // check[v] : 값 v를 이미 number[]에 넣었는지

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 중복순열 생성
//   depth : 지금 채우려는 자리 번호
void DFS(int depth)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	// 순서를 구분하고 중복도 허용하므로 매 자리에서 후보 전체를 다시 돈다
	for (int i = 1; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		// visit도 start도 필요 없다.
		// 후보가 이미 서로 다른 값들이므로 같은 수열이 두 번 나올 일도 없다.
		DFS(depth + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;   // number[]에 채울 다음 위치 (1-based)

	// 입력을 받으면서 처음 보는 값만 목록에 추가 (중복 제거)
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

	// 여기서부터 N의 의미가 바뀐다 : "입력 개수"가 아니라 "서로 다른 값의 개수"
	N = count - 1;

	// 사전순 출력을 위한 오름차순 정렬 (선택 정렬)
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

// ====================================================================
// BOJ 15666 - N과 M (12)  : 중복조합
//
//   15665와 조건은 같고 출력이 비내림차순이어야 한다는 점만 다르다.
//   그래서 start 인자를 두고 다음 자리 시작값을 i로 넘긴다. (i + 1로 넘기면 일반 조합)
//
//   예) 입력 9 7 9  ->  중복 제거 후 7 9, M = 2 이면
//       7 7 / 7 9 / 9 9        (9 7 은 비내림차순이 아니라 생성되지 않는다)
// ====================================================================

#include <stdio.h>
#include <stdbool.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];     // 만들고 있는 수열
int number[MAX];           // 중복 제거 후 남은 서로 다른 값들 (1-based)
bool check[10000 + 100];   // check[v] : 값 v를 이미 number[]에 넣었는지

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 중복조합 생성
//   depth : 지금 채우려는 자리 번호
//   start : 이번에 고를 수 있는 최소 인덱스
void DFS(int depth, int start)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	// 앞에서 고른 인덱스보다 앞으로는 돌아가지 않는다 -> 비내림차순 보장
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		// 다음 시작값이 i이므로 같은 값을 다시 고를 수 있다 (중복 허용)
		DFS(depth + 1, i);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;

	// 중복 제거하며 서로 다른 값만 수집
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

	// N의 의미 전환 : 서로 다른 값의 개수
	N = count - 1;

	// 사전순 출력을 위한 오름차순 정렬 (선택 정렬)
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
