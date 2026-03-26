// 코드트리 2017 하반기 오전 1번 - 조삼모사
//
// 문제 요약
// N명의 사람을 두 팀으로 나누되,
// 각 팀의 인원 수는 정확히 N/2명이어야 한다.
//
// 사람 i와 j가 같은 팀에 있을 때의 시너지 값이 MAP[i][j] 로 주어진다.
// 한 팀의 팀 능력치는
// 같은 팀에 속한 모든 두 사람 쌍 (i, j)에 대해
// MAP[i][j] + MAP[j][i] 를 모두 더한 값이다.
//
// 목표
// 두 팀의 능력치 차이의 절댓값을 최소로 만드는 팀 분할을 구하는 문제이다.
//
// 핵심 아이디어
// 1. N명 중 절반(N/2명)을 한 팀(예: morning 팀)으로 고르는 모든 조합을 DFS로 만든다.
// 2. 나머지 사람들은 자동으로 다른 팀(dinner 팀)이 된다.
// 3. 두 팀의 능력치를 계산하고,
//    그 차이의 절댓값의 최솟값을 갱신한다.
//
// 즉,
// "사람 절반 뽑는 조합 DFS" + "각 팀 시너지 계산"
// 구조의 완전탐색 문제이다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (20 + 5)
#define INF (0x7fff0000)

int T;

int N;               // 전체 사람 수
int halfN;           // 한 팀의 인원 수 = N / 2
int MAP[MAX][MAX];   // 시너지 배열

int num_of_cases[MAX]; // DFS로 고른 morning 팀 사람 번호들

int minAnswer;       // 능력치 차이의 최소값

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d", &N);

	halfN = N / 2;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

// ---------------------------
// 디버그용: 현재 고른 morning 팀 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < halfN; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 절댓값 함수
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;
}

// ---------------------------
// 현재 num_of_cases[]에 저장된 morning 팀 기준으로
// 두 팀의 능력치 차이를 계산
//
// 과정
// 1. morning 팀에 속한 사람 표시
// 2. 나머지를 dinner 팀으로 분리
// 3. 각 팀의 시너지 합 계산
// 4. |sum1 - sum2| 반환
// ---------------------------
int calculate()
{
	bool isMorning[MAX] = { 0 }; // i번 사람이 morning 팀인지 표시
	int morning[MAX] = { 0 };    // morning 팀 사람 목록
	int dinner[MAX] = { 0 };     // dinner 팀 사람 목록
	int mcnt, dcnt;              // 각 팀 인원 수
	int sum1, sum2;              // 두 팀의 시너지 합

	// 현재 DFS에서 고른 사람들을 morning 팀으로 표시
	for (int i = 0; i < halfN; i++)
		isMorning[num_of_cases[i]] = true;

	// 전체 사람을 보며 morning / dinner로 나누기
	mcnt = dcnt = 0;
	for (int i = 1; i <= N; i++)
	{
		if (isMorning[i] == true)
			morning[mcnt++] = i;
		else
			dinner[dcnt++] = i;
	}

	// 디버그용
	// printf("morning : ");
	// for (int i = 0; i < mcnt; i++) printf("%d ", morning[i]);
	// putchar('\n');
	// printf("dinner : ");
	// for (int i = 0; i < dcnt; i++) printf("%d ", dinner[i]);
	// putchar('\n'); putchar('\n');

	sum1 = sum2 = 0;

	// 각 팀 내부의 모든 두 사람 쌍(i, k)에 대해
	// MAP[a][b] + MAP[b][a] 를 더한다.
	//
	// 예: 팀이 2,3,4 이면
	// (2,3), (2,4), (3,4) 의 조합을 모두 본다.
	for (int i = 0; i < halfN; i++)
	{
		for (int k = i + 1; k < halfN; k++)
		{
			int mr, mc; // morning 팀의 두 사람
			int dr, dc; // dinner 팀의 두 사람

			mr = morning[i];
			mc = morning[k];

			dr = dinner[i];
			dc = dinner[k];

			// morning 팀 시너지 누적
			sum1 += (MAP[mr][mc] + MAP[mc][mr]);

			// dinner 팀 시너지 누적
			sum2 += (MAP[dr][dc] + MAP[dc][dr]);
		}
	}

	// 두 팀 시너지 차이의 절댓값 반환
	return abs(sum1 - sum2);
}

// ---------------------------
// DFS로 N명 중 halfN명을 고르는 조합 생성
//
// depth : 현재 몇 명 골랐는지
// start : 다음에 선택할 수 있는 시작 번호
//
// 조합이므로 DFS(depth+1, i+1) 형태 사용
// ---------------------------
void DFS(int depth, int start)
{
	// halfN명을 모두 고르면 한 팀 완성
	if (depth == halfN)
	{
		// printCases();

		int tmp = calculate();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	// start부터 N까지 사람 번호 하나씩 선택
	for (int i = start; i <= N; i++)
	{
		num_of_cases[depth] = i;

		// 다음 사람은 현재보다 뒤에서만 고름
		DFS(depth + 1, i + 1);
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

		// N명 중 halfN명을 고르는 모든 조합 탐색
		DFS(0, 1);

		printf("%d\n", minAnswer);
	}

	return 0;
}