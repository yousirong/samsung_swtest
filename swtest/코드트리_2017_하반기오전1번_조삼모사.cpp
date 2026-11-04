/*
	[코드트리] 2017 하반기 오전 1번 - 조삼모사
	https://www.codetree.ai/training-field/frequent-problems/problems/three-at-dawn-and-four-at-dusk

	■ 문제 요약
	  N명(N은 짝수)을 정확히 N/2명씩 두 팀으로 나눈다.
	  같은 팀에 i번과 j번이 함께 있으면 그 팀의 능력치에 MAP[i][j] + MAP[j][i]가 더해진다.
	  두 팀 능력치 차이의 절댓값을 최소로 만들었을 때 그 값을 출력한다.

	  (백준 14889 "스타트와 링크"와 같은 문제다)

	■ 풀이 방침 : 절반을 고르는 조합 완전탐색
	  한 팀을 정하면 나머지는 자동으로 다른 팀이므로, 실제로 정할 것은 하나뿐이다.

	    1) DFS로 1 ~ N 중 N/2명을 고르는 조합을 만든다
	    2) 고른 사람은 morning 팀, 나머지는 dinner 팀
	    3) 두 팀의 능력치를 각각 구해 차이의 절댓값으로 최솟값을 갱신한다

	  N이 20 이하라 조합 수는 최대 20C10 = 184,756가지이고,
	  능력치 계산도 (N/2)^2 수준이라 완전탐색으로 충분하다.

	  ※ 조합이므로 순서가 다른 같은 팀은 만들어지지 않는다.
	    다만 "A팀을 고른 경우"와 "B팀을 고른 경우"는 결과가 같은데도 둘 다 생성된다.
	    1번 사람을 항상 morning에 고정하면(DFS(1, 2)로 시작) 절반으로 줄일 수 있다.

	■ 능력치 계산
	  한 팀 안의 모든 두 사람 쌍을 봐야 하므로, 팀 목록에서 i < k인 쌍만 훑는다.
	  MAP은 대칭이 아니라서 두 방향을 모두 더해야 한다.

	    팀이 {2, 3, 4} 라면 (2,3) (2,4) (3,4) 세 쌍에 대해
	    MAP[2][3]+MAP[3][2], MAP[2][4]+MAP[4][2], MAP[3][4]+MAP[4][3] 을 더한다.

	  두 팀의 인원이 똑같이 halfN명이므로, 같은 이중 for문 안에서
	  morning과 dinner의 합을 한 번에 구할 수 있다.
*/

#include <stdio.h>           // scanf, printf, putchar 사용

#define MAX (20 + 5)         // N 최대 20 + 여유
#define INF (0x7fff0000)     // 아주 큰 값 (최솟값 초기화용)

int T;               // 테스트 케이스 수

int N;               // 전체 사람 수 (짝수)
int halfN;           // 한 팀의 인원 수 = N / 2
int MAP[MAX][MAX];   // MAP[i][j] : i와 j가 같은 팀일 때 더해지는 값

int num_of_cases[MAX]; // DFS로 고른 morning 팀 사람 번호들

int minAnswer;       // 두 팀 능력치 차이의 최솟값

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d", &N);                     // 사람 수 입력

	halfN = N / 2;                       // 한 팀 인원

	for (int r = 1; r <= N; r++)         // 1번부터 N번까지
		for (int c = 1; c <= N; c++)     // 1번부터 N번까지
			scanf("%d", &MAP[r][c]);     // 궁합 값 입력
}

// ---------------------------
// 디버그용: 현재 고른 팀 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < halfN; i++)      // 고른 인원만큼
		printf("%d ", num_of_cases[i]);  // 번호 출력
	putchar('\n');                       // 줄바꿈
}

// ---------------------------
// 절댓값 (stdlib.h 없이 직접 구현)
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;             // 양수면 그대로, 아니면 부호 반전
}

// ---------------------------
// 현재 조합에 대한 두 팀 능력치 차이
//
//   1) 고른 사람을 morning으로 표시
//   2) 표시되지 않은 사람을 dinner로 모은다
//   3) 각 팀 안의 모든 쌍을 더한다
//   4) 차이의 절댓값을 돌려준다
// ---------------------------
int calculate()
{
	bool isMorning[MAX] = { 0 }; // i번 사람이 morning 팀인지
	int morning[MAX] = { 0 };    // morning 팀 사람 번호 목록
	int dinner[MAX] = { 0 };     // dinner 팀 사람 번호 목록
	int mcnt, dcnt;              // 각 팀 인원 수 (둘 다 halfN이 된다)
	int sum1, sum2;              // 두 팀의 능력치

	// 이번 조합에서 고른 사람들에 표시
	for (int i = 0; i < halfN; i++)
		isMorning[num_of_cases[i]] = true;

	// 표시 여부로 두 팀을 실제 목록으로 분리
	mcnt = dcnt = 0;                     // 인원 초기화
	for (int i = 1; i <= N; i++)         // 모든 사람
	{
		if (isMorning[i] == true)        // morning 팀이면
			morning[mcnt++] = i;         // morning 목록에
		else                             // 아니면
			dinner[dcnt++] = i;          // dinner 목록에
	}

	// 디버그용
	// printf("morning : ");
	// for (int i = 0; i < mcnt; i++) printf("%d ", morning[i]);
	// putchar('\n');
	// printf("dinner : ");
	// for (int i = 0; i < dcnt; i++) printf("%d ", dinner[i]);
	// putchar('\n'); putchar('\n');

	sum1 = sum2 = 0;                     // 능력치 초기화

	// 팀 안의 서로 다른 두 사람 쌍을 모두 훑는다 (i < k).
	// 두 팀의 인원이 같으므로 같은 루프에서 둘 다 계산할 수 있다.
	for (int i = 0; i < halfN; i++)
	{
		for (int k = i + 1; k < halfN; k++)
		{
			int mr, mc; // morning 팀의 두 사람
			int dr, dc; // dinner 팀의 두 사람

			mr = morning[i];             // morning 첫째 사람
			mc = morning[k];             // morning 둘째 사람

			dr = dinner[i];              // dinner 첫째 사람
			dc = dinner[k];              // dinner 둘째 사람

			// MAP이 대칭이 아니므로 양방향을 모두 더한다
			sum1 += (MAP[mr][mc] + MAP[mc][mr]);
			sum2 += (MAP[dr][dc] + MAP[dc][dr]);
		}
	}

	return abs(sum1 - sum2);             // 두 팀 차이의 절댓값
}

// ---------------------------
// DFS : 1 ~ N 중 halfN명을 고르는 조합 생성
//
//   depth : 지금까지 고른 인원 수
//   start : 이번에 고를 수 있는 최소 번호
// ---------------------------
void DFS(int depth, int start)
{
	// 절반을 다 골랐으면 팀 분할이 하나 완성된 것
	if (depth == halfN)
	{
		// printCases();

		int tmp = calculate();           // 이 분할의 차이
		if (tmp < minAnswer)             // 더 작으면
			minAnswer = tmp;             // 최솟값 갱신

		return;                          // 되돌아간다
	}

	for (int i = start; i <= N; i++)     // start부터 후보
	{
		num_of_cases[depth] = i;         // i번을 morning 팀으로

		// 조합이므로 다음은 i보다 뒤에서만 고른다
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		minAnswer = INF;                 // 최솟값 초기화

		DFS(0, 1);                       // 0명 고른 상태, 1번부터

		printf("%d\n", minAnswer);       // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
