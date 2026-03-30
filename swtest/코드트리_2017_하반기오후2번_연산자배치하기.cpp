// 코드트리 2017 하반기 오후 2번 - 연산자 배치하기
//
// 문제 요약
// 숫자 N개가 순서대로 주어지고,
// 사용할 수 있는 연산자 개수(+, -, *)가 주어진다.
//
// 숫자의 순서는 바꿀 수 없고,
// 숫자 사이에 연산자들을 배치하여 만들 수 있는 식의 결과 중
// 최솟값과 최댓값을 구하는 문제이다.
//
// 예를 들어 숫자가
//   a b c d
// 라면 식은
//   (((a op1 b) op2 c) op3 d)
// 처럼 왼쪽부터 차례대로 계산한다.
//
// 핵심 아이디어
// - 숫자 사이에는 총 N-1개의 연산자가 들어간다.
// - 각 자리에 어떤 연산자를 넣을지 DFS(백트래킹)로 모두 탐색한다.
// - 단, 연산자마다 사용할 수 있는 개수가 정해져 있으므로
//   visit[]로 현재까지 사용한 횟수를 관리한다.
// - 완성된 연산자 배치에 대해 결과를 계산하고
//   최솟값, 최댓값을 갱신한다.

#include <stdio.h>

#define MAX (11 + 5)
#define INF (0x7fff0000)

// 연산자 종류를 번호로 정의
#define PLUS     (0)
#define MINUS    (1)
#define MULTIPLY (2)

int T;

int N;                  // 숫자 개수
int num_of_cases[MAX];  // 현재까지 고른 연산자 배치
int visit[MAX];         // 각 연산자를 현재까지 몇 번 사용했는지
int possible[3];        // 사용할 수 있는 최대 개수: [+, -, *]
int number[MAX];        // 입력 숫자들

int minAnswer, maxAnswer;  // 결과의 최솟값, 최댓값

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d", &N);

	// 숫자 입력
	for (int i = 0; i < N; i++)
		scanf("%d", &number[i]);

	// 사용할 수 있는 연산자 개수 입력
	// possible[0] = + 개수
	// possible[1] = - 개수
	// possible[2] = * 개수
	for (int i = 0; i < 3; i++)
		scanf("%d", &possible[i]);
}

// ---------------------------
// 디버그용: 현재 연산자 배치 출력
// 숫자가 N개면 연산자는 N-1개 필요
// ---------------------------
void printCases()
{
	for (int i = 0; i < N - 1; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 num_of_cases[]에 저장된 연산자 배치대로
// 식의 결과를 계산하는 함수
//
// 계산 방식
// sum = number[0]에서 시작해서
// num_of_cases[i]에 따라 number[i+1]을 차례대로 계산
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
// DFS로 연산자 배치 생성
//
// depth = 현재 몇 번째 연산자를 고르고 있는지
//
// 숫자가 N개면 연산자는 N-1개 필요하므로
// depth가 N-1이 되면 하나의 식이 완성된 것이다.
// ---------------------------
void DFS(int depth)
{
	// 연산자 N-1개를 모두 배치했으면 결과 계산
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

	// 세 종류의 연산자 중 하나 선택
	for (int i = 0; i < 3; i++)
	{
		// 이미 해당 연산자를 가능한 개수만큼 다 사용했으면 건너뜀
		if (visit[i] == possible[i])
			continue;

		// 현재 자리에 i번 연산자 배치
		num_of_cases[depth] = i;

		// 사용 횟수 증가
		visit[i]++;

		// 다음 자리 탐색
		DFS(depth + 1);

		// 백트래킹: 사용 횟수 복구
		visit[i]--;
	}
}

int main()
{
	// 원래 테스트케이스 형식이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;
		maxAnswer = -INF;

		// 모든 연산자 배치 탐색
		DFS(0);

		printf("%d %d\n", minAnswer, maxAnswer);
	}

	return 0;
}