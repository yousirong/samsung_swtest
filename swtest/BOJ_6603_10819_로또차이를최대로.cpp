// 이 파일에는 두 문제 풀이 흔적이 같이 들어 있다.
// 위쪽의 BOJ 6603 코드는 보관용으로 주석 처리되어 있고,
// 실제로 동작하는 코드는 아래 BOJ 10819 풀이이다.


// ==================================================
// BOJ 6603 - 로또 (보관용 코드, 현재 실행 안 됨)
// ==================================================

#include <stdio.h>

#define MAX (13 + 3)

int N, M;

int num_of_cases[MAX];   // 현재 뽑은 조합(6개)
int number[MAX];         // 입력으로 주어진 숫자들

// 입력 함수
void input()
{
	scanf("%d", &N);

	for (int i = 1; i <= N; i++)
		scanf("%d", &number[i]);
}

// 현재 만들어진 조합 출력
void printCases()
{
	for (int i = 0; i < 6; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// DFS로 조합 생성
// depth : 현재 몇 개 뽑았는지
// start : 이번에 뽑기 시작할 번호 인덱스
void DFS(int depth, int start)
{
	// 6개를 다 뽑았으면 출력
	if (depth == 6)
	{
		printCases();
		return;
	}

	// start부터 N까지 하나씩 고르기
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = number[i];
		DFS(depth + 1, i + 1);  // 다음 숫자는 현재보다 뒤에서만 고름
	}
}

int main()
{
	while (1)
	{
		input();
		if (N == 0) break;   // 0 입력이면 종료

		DFS(0, 1);

		putchar('\n');
	}

	return 0;
}



// --------------------------------------------------
// BOJ 10819 - 차이를 최대로
// --------------------------------------------------
//
// 문제 요약
// 주어진 N개의 수를 적절한 순서로 재배열했을 때
//
// |A[0] - A[1]| + |A[1] - A[2]| + ... + |A[N-2] - A[N-1]|
//
// 의 값을 최대로 만드는 문제이다.
//
// 핵심 아이디어
// - N이 크지 않으므로 모든 순열을 만들어 볼 수 있다.
// - DFS(백트래킹)로 가능한 모든 순열을 생성한다.
// - 순열이 완성될 때마다 식의 값을 계산해서 최댓값을 갱신한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (8 + 3)

int N;                   // 숫자 개수

int num_of_cases[MAX];   // 현재 DFS로 만들고 있는 순열
bool visit[MAX];         // 해당 숫자를 이미 사용했는지 체크
int number[MAX];         // 입력 숫자들

int maxAnswer;           // 최댓값 저장

// ---------------------------
// 디버그용: 현재 순열 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < N; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 절댓값 함수
// stdlib.h의 abs를 직접 안 쓰고 구현
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;
}

// ---------------------------
// 현재 순열의 점수 계산
//
// |A[0]-A[1]| + |A[1]-A[2]| + ... 를 계산
// ---------------------------
int calculate()
{
	int sum = 0;

	for (int i = 0; i < N - 1; i++)
		sum += abs(num_of_cases[i] - num_of_cases[i + 1]);

	return sum;
}

// ---------------------------
// DFS(백트래킹)로 순열 생성
//
// depth : 현재 몇 번째 자리를 채우고 있는지
// ---------------------------
void DFS(int depth)
{
	// 순열이 완성된 경우
	if (depth == N)
	{
		// 현재 순열의 점수 계산
		int tmp = calculate();

		// 최댓값 갱신
		if (maxAnswer < tmp)
			maxAnswer = tmp;

		return;
	}

	// 아직 사용하지 않은 숫자를 하나 선택해서
	// 현재 depth 자리에 배치
	for (int i = 0; i < N; i++)
	{
		// 이미 사용한 숫자는 건너뜀
		if (visit[i] == true)
			continue;

		// 현재 depth 위치에 number[i] 배치
		num_of_cases[depth] = number[i];

		// 사용 처리
		visit[i] = true;

		// 다음 자리 채우기
		DFS(depth + 1);

		// 백트래킹: 사용 해제
		visit[i] = false;
	}
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	// 숫자 개수 입력
	scanf("%d", &N);

	// 숫자들 입력
	for (int i = 0; i < N; i++)
		scanf("%d", &number[i]);

	// 최댓값 초기화
	maxAnswer = -1;

	// DFS로 모든 순열 탐색 시작
	DFS(0);

	// 정답 출력
	printf("%d\n", maxAnswer);

	return 0;
}