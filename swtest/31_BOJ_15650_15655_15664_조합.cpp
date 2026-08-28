/*
	[BOJ] 15650 N과 M (2) / 15655 N과 M (6) - 조합
	https://www.acmicpc.net/problem/15650
	https://www.acmicpc.net/problem/15655

	■ 이 파일에 대하여
	  여러 문제의 풀이가 차례로 들어 있는 정리용 파일이다.
	  main과 전역 변수가 각각 있으므로 한꺼번에 컴파일되지는 않는다.
	  필요한 것만 남기고 나머지는 주석 처리해서 쓴다.

	  ※ 파일 이름에는 15664가 붙어 있지만, 아래 세 번째 블록의 코드는
	    실제로는 15656(N과 M (7), 중복순열) 풀이이고 28번 파일의 후반부와 같은 내용이다.
	    15664(N과 M (10))는 "중복 있는 수 목록에서 비내림차순 조합 + 같은 수열 제거"라
	    이 코드로는 풀리지 않는다. 세 번째 블록 위에 다시 적어 두었다.

	■ 공통 주제 : 조합 (순서 X, 중복 X)
	  순서를 무시한다는 것은 "같은 집합을 한 번만 만든다"는 뜻이고,
	  그러려면 항상 증가하는 순서로만 뽑으면 된다. 이를 위해 start 인자를 둔다.

	      for (i = start; i <= N; i++)
	          DFS(depth + 1, i + 1);     // 다음은 반드시 i보다 뒤에서

	  다음 start를 i+1로 넘기는 이 한 줄이 조합의 전부다.
	  i로 넘기면 중복조합(29번 파일), start 없이 매번 1부터 돌면 중복순열(28번 파일),
	  start 없이 visit로 거르면 순열(30번 파일)이 된다.

	  덤으로 결과가 항상 오름차순이라 문제의 출력 조건도 자동으로 맞는다.
*/

// ====================================================================
// BOJ 15650 - N과 M (2)
//
//   1부터 N까지에서 중복 없이 M개를 골라 오름차순으로 출력한다.
//
//   예) N = 4, M = 2
//       1 2 / 1 3 / 1 4 / 2 3 / 2 4 / 3 4   (총 4C2 = 6가지)
// ====================================================================

#include <stdio.h>

#define MAX (8 + 3)

int N, M;
int num_of_cases[MAX];   // 만들고 있는 조합

// 완성된 조합 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 조합 생성
//   depth : 지금까지 몇 개 골랐는지
//   start : 이번에 고를 수 있는 최소 숫자
void DFS(int depth, int start)
{
	// M개를 다 골랐으면 한 경우 완성
	if (depth == M)
	{
		printCases();
		return;
	}

	// 앞에서 고른 수보다 뒤에 있는 수만 본다
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = i;

		// 다음 시작값을 i + 1로 넘기는 것이 "조합"의 핵심.
		// (i로 넘기면 같은 수를 다시 고를 수 있게 되어 중복조합이 된다)
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0, 1);

	return 0;
}

// ====================================================================
// BOJ 15655 - N과 M (6)
//
//   입력으로 주어진 서로 다른 N개의 수에서 M개를 골라 오름차순 조합으로 출력한다.
//
//   15650과 재귀 구조는 같고, 담는 값이 i 대신 number[i]다.
//   start는 여전히 "값"이 아니라 "인덱스" 기준으로 움직인다.
//   후보를 미리 정렬해 두면 인덱스 증가가 곧 값 증가라서 오름차순 출력이 보장된다.
// ====================================================================

#include <stdio.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 만들고 있는 조합
int number[MAX];         // 후보로 주어진 수들 (1-based)

// 완성된 조합 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 조합 생성
//   start : 이번에 고를 수 있는 최소 인덱스
void DFS(int depth, int start)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	for (int i = start; i <= N; i++)
	{
		// 15650과 유일하게 다른 줄
		num_of_cases[depth] = number[i];

		DFS(depth + 1, i + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	// 후보 수 입력
	for (int i = 1; i <= N; i++)
		scanf("%d", &number[i]);

	// 사전순(= 오름차순) 출력을 위한 정렬 (선택 정렬)
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

// ====================================================================
// BOJ 15656 - N과 M (7)  ※ 중복순열이며, 조합이 아니다
// https://www.acmicpc.net/problem/15656
//
//   입력으로 주어진 N개의 수 중에서 중복을 허용해 길이 M인 수열을 사전순으로 출력한다.
//
//   예) 입력 9 7 1 -> 정렬 후 1 7 9, M = 2 이면
//       1 1 / 1 7 / 1 9 / 7 1 / 7 7 / 7 9 / 9 1 / 9 7 / 9 9
//
//   중복도 허용하고 순서도 구분하므로 start도 visit도 필요 없고,
//   매 자리에서 후보 전체를 처음부터 다시 돈다. 28번 파일의 후반부와 같은 코드다.
//
//   [주의] 이 파일 이름에 들어 있는 15664(N과 M (10))는 다른 문제다.
//          15664는 입력에 같은 값이 여러 번 들어올 수 있고,
//          비내림차순 조합을 만들되 같은 수열은 한 번만 출력해야 한다.
//          그러려면 30번 파일의 15663처럼 "서로 다른 값 + 등장 횟수"로 압축한 뒤
//          조합 방식(다음 시작 인덱스를 넘기는 방식)으로 뽑아야 하며,
//          아래 코드로는 풀리지 않는다.
// ====================================================================

#include <stdio.h>

#define MAX (7 + 3)

int N, M;

int num_of_cases[MAX];   // 만들고 있는 수열
int number[MAX];         // 후보로 주어진 수들 (1-based)

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 중복순열 생성
void DFS(int depth)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	// 제약이 전혀 없으므로 매 자리에서 후보 전체를 다시 돈다
	for (int i = 1; i <= N; i++)
	{
		num_of_cases[depth] = number[i];

		DFS(depth + 1);
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	// 후보 수 입력
	for (int i = 1; i <= N; i++)
		scanf("%d", &number[i]);

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
