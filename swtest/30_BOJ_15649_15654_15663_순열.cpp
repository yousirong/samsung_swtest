/*
	[BOJ] 15649 N과 M (1) / 15654 N과 M (5) / 15663 N과 M (9) - 순열
	https://www.acmicpc.net/problem/15649
	https://www.acmicpc.net/problem/15654
	https://www.acmicpc.net/problem/15663

	■ 이 파일에 대하여
	  세 문제의 풀이가 차례로 들어 있는 정리용 파일이다.
	  main과 전역 변수가 각각 있으므로 한꺼번에 컴파일되지는 않는다.
	  필요한 것만 남기고 나머지는 주석 처리해서 쓴다.

	■ 공통 주제 : 순열 (순서 O, 중복 X)
	  후보를 매 자리에서 1 ~ N 전부 훑되, "이미 쓴 것"을 visit로 걸러 낸다.
	  중복순열(28번 파일)과 코드가 거의 같고 visit 한 겹만 얹힌 형태다.

	  백트래킹의 기본 짝은 아래 세 줄이다.

	      visit[i] = true;      // 내가 쓰는 중
	      DFS(depth + 1);       // 더 깊이
	      visit[i] = false;     // 반납 (이 줄을 빼먹으면 뒤쪽 경우가 통째로 사라진다)

	■ 세 문제의 관계
	    15649 : 후보가 1 ~ N.               visit는 bool.
	    15654 : 후보가 입력으로 주어진 N개.  visit는 bool. 사전순 위해 먼저 정렬.
	    15663 : 후보에 같은 값이 여러 번 들어올 수 있다.
	            같은 수열은 한 번만 출력해야 하므로 처리 방식이 달라진다.

	■ 15663의 핵심 아이디어
	  값이 중복될 수 있을 때 그냥 15654처럼 풀면 같은 수열이 여러 번 나온다.
	  예를 들어 입력이 1 1 2 라면, 두 개의 1은 서로 구분되지 않는데도
	  "첫 번째 1을 쓴 경우"와 "두 번째 1을 쓴 경우"가 따로 생성돼 1 2 가 두 번 출력된다.

	  그래서 후보를 "서로 다른 값 목록 + 각 값의 개수"로 압축한다.

	      number[]   : 서로 다른 값만 모아 정렬한 목록
	      possible[v]: 값 v가 입력에 몇 번 등장했는지
	      visit[i]   : 지금 재귀 경로에서 number[i]를 몇 번 썼는지 (bool이 아니라 카운터!)

	  각 값은 "등장한 횟수만큼만" 쓸 수 있으므로 조건이 이렇게 된다.

	      if (visit[i] == possible[number[i]]) continue;   // 재고 소진, 못 씀

	  후보 목록에 같은 값이 한 번씩만 있으니 같은 수열이 두 번 만들어질 일이 없고,
	  개수 제한 덕분에 있는 만큼만 쓰게 된다.
*/

// ====================================================================
// BOJ 15649 - N과 M (1)
//
//   1부터 N까지에서 서로 다른 수 M개를 골라 만든 수열을 전부 출력한다 (순서 구분).
//
//   예) N = 3, M = 2
//       1 2 / 1 3 / 2 1 / 2 3 / 3 1 / 3 2   (총 3P2 = 6가지)
// ====================================================================

#include <stdio.h>
#include <stdbool.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 만들고 있는 수열
bool visit[MAX];         // 숫자 i를 지금 경로에서 이미 썼는지

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 순열 생성
//   depth : 지금 채우려는 자리 번호 (0부터)
void DFS(int depth)
{
	// M자리를 다 채웠으면 한 경우 완성
	if (depth == M)
	{
		printCases();
		return;
	}

	// 순서를 구분하므로 매번 1 ~ N 전부를 후보로 본다 (작은 수부터 = 사전순)
	for (int i = 1; i <= N; i++)
	{
		// 앞 자리에서 이미 쓴 수는 다시 쓸 수 없다
		if (visit[i] == true)
			continue;

		num_of_cases[depth] = i;

		visit[i] = true;     // 사용 중 표시

		DFS(depth + 1);      // 다음 자리로

		visit[i] = false;    // 반납 (백트래킹) - 형제 가지에서 다시 쓸 수 있게
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	DFS(0);

	return 0;
}

// ====================================================================
// BOJ 15654 - N과 M (5)
//
//   입력으로 주어진 서로 다른 N개의 수에서 M개를 골라 만든 수열을 사전순으로 출력한다.
//
//   15649와 재귀 구조는 같고, 자리에 담는 값이 i 대신 number[i]다.
//   visit는 여전히 "값"이 아니라 "인덱스" 기준으로 관리한다.
// ====================================================================

#include <stdio.h>
#include <stdbool.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];   // 만들고 있는 수열
bool visit[MAX];         // i번째 후보를 썼는지 (값이 아니라 인덱스 기준)
int number[MAX];         // 후보로 주어진 수들 (1-based)

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 순열 생성
void DFS(int depth)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	for (int i = 1; i <= N; i++)
	{
		if (visit[i] == true)
			continue;

		// 15649와 유일하게 다른 줄
		num_of_cases[depth] = number[i];

		visit[i] = true;

		DFS(depth + 1);

		visit[i] = false;    // 반납 (백트래킹)
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

// ====================================================================
// BOJ 15663 - N과 M (9)
//
//   입력에 같은 값이 여러 번 들어올 수 있다.
//   길이 M의 수열을 만들되, 같은 수열은 한 번만 출력한다. 출력은 사전순.
//
//   예) 입력 1 1 2, M = 2
//       1 1 / 1 2 / 2 1        (1 2 가 두 번 나오면 안 된다)
//
//   위 헤더에서 설명한 "서로 다른 값 + 개수" 압축 방식으로 처리한다.
//   여기서 visit[i]는 bool이 아니라 "number[i]를 지금까지 몇 번 썼는지"인 점에 주의.
// ====================================================================

#include <stdio.h>

#define MAX (8 + 3)

int N, M;

int num_of_cases[MAX];      // 만들고 있는 수열
int visit[MAX];             // number[i]를 현재 경로에서 몇 번 썼는지 (카운터)
int possible[10000 + 500];  // possible[v] = 값 v가 입력에 등장한 횟수
int number[MAX];            // 서로 다른 값들만 모은 목록

// 완성된 수열 한 줄 출력
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// 재귀로 (중복 없는) 순열 생성
void DFS(int depth)
{
	if (depth == M)
	{
		printCases();
		return;
	}

	// 후보 목록에는 서로 다른 값이 한 번씩만 들어 있으므로
	// 같은 수열이 두 번 만들어질 수 없다.
	for (int i = 1; i <= N; i++)
	{
		// 이 값을 이미 있는 개수만큼 다 썼다면 더는 쓸 수 없다
		if (visit[i] == possible[number[i]])
			continue;

		num_of_cases[depth] = number[i];

		visit[i]++;       // 이 값을 하나 소비

		DFS(depth + 1);

		visit[i]--;       // 반납 (백트래킹)
	}
}

int main()
{
	scanf("%d %d", &N, &M);

	int count = 1;   // number[]에 채워 넣을 다음 위치 (1-based)

	// 입력을 받으면서 값별 등장 횟수를 세고, 처음 보는 값만 목록에 추가한다
	for (int i = 1; i <= N; i++)
	{
		int value;
		scanf("%d", &value);

		// 아직 한 번도 안 나온 값이면 서로 다른 값 목록에 넣는다
		if (possible[value] == 0)
			number[count++] = value;

		possible[value]++;
	}

	// 여기서부터 N의 의미가 바뀐다 : "입력 개수"가 아니라 "서로 다른 값의 개수"
	N = count - 1;

	// 사전순 출력을 위해 서로 다른 값 목록을 오름차순 정렬
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
