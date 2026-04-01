// 코드트리 2018 상반기 오후 2번 - 병원 거리 최소화하기
// https://www.codetree.ai/training-field/frequent-problems/problems/min-of-hospital-distance
// 문제 요약
// N x N 격자에
// - 사람(1)
// - 병원(2)
// 이 주어진다.
//
// 병원들 중 정확히 M개를 선택했을 때,
// 모든 사람에 대해 "가장 가까운 선택된 병원까지의 거리"를 구하고,
// 그 거리들의 합을 최소로 만드는 문제이다.
//
// 거리 정의
// - 맨해튼 거리(Manhattan Distance)
// - |r1 - r2| + |c1 - c2|
//
// 핵심 아이디어
// 1. 병원 후보들 중 M개를 고르는 모든 조합을 DFS로 만든다.
// 2. 각 조합마다 모든 사람에 대해
//    선택된 병원들 중 가장 가까운 병원까지의 거리를 계산한다.
// 3. 그 거리 합의 최솟값을 갱신한다.
//
// 즉,
// "병원 M개 고르는 조합 DFS" + "거리 합 계산"
// 구조의 완전탐색 문제이다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (50 + 10)
#define MAX_HOSPITAL (13 + 5)
#define INF (0x7fff0000)

// 칸의 의미
#define PERSON   (1)
#define HOSPITAL (2)

int T;

int N, M;                  // N: 격자 크기, M: 선택할 병원 수
int MAP[MAX][MAX];         // 격자 정보

int num_of_cases[MAX];     // DFS로 고른 병원 인덱스들

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC person[MAX * 2];               // 사람 좌표 목록
RC hospital[MAX_HOSPITAL];        // 병원 좌표 목록
int pcnt, hcnt;                   // 사람 수, 병원 수

int minAnswer;                    // 거리 합의 최소값

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	pcnt = hcnt = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);

			// 사람 좌표 저장
			if (MAP[r][c] == PERSON)
			{
				person[pcnt].r = r;
				person[pcnt++].c = c;
			}
			// 병원 좌표 저장
			else if (MAP[r][c] == HOSPITAL)
			{
				hospital[hcnt].r = r;
				hospital[hcnt++].c = c;
			}
		}
	}
}

// ---------------------------
// 디버그용 맵 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 디버그용: 현재 고른 병원 인덱스 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < M; i++)
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
// 맨해튼 거리 계산
//
// (r1, c1) 와 (r2, c2) 사이 거리
// = |r1-r2| + |c1-c2|
// ---------------------------
int getDistance(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2);
}

// ---------------------------
// 현재 DFS로 선택한 병원 조합(num_of_cases[])에 대해
// 전체 거리 합 계산
//
// 과정
// 1. 어떤 병원이 선택되었는지 selectedHospital[]에 표시
// 2. 각 사람마다
//    선택된 병원들 중 가장 가까운 병원까지 거리(minDistance)를 찾음
// 3. 그 값을 모두 더한 합을 반환
// ---------------------------
int calculate()
{
	bool selectedHospital[MAX_HOSPITAL] = { 0 };

	// 현재 선택된 병원들 표시
	for (int i = 0; i < M; i++)
		selectedHospital[num_of_cases[i]] = true;

	int sum = 0;

	// 모든 사람에 대해
	for (int i = 0; i < pcnt; i++)
	{
		int minDistance = INF;

		// 선택된 병원들 중 가장 가까운 곳 찾기
		for (int k = 0; k < hcnt; k++)
		{
			if (selectedHospital[k] == false)
				continue;

			int distance = getDistance(
				person[i].r, person[i].c,
				hospital[k].r, hospital[k].c
			);

			minDistance = (distance < minDistance) ? distance : minDistance;
		}

		// 이 사람의 최소 거리 누적
		sum += minDistance;
	}

	return sum;
}

// ---------------------------
// DFS로 병원 중 M개를 고르는 조합 생성
//
// depth : 현재 몇 개의 병원을 골랐는지
// start : 다음에 선택할 수 있는 시작 인덱스
//
// 조합이므로 DFS(depth+1, i+1) 형태를 사용한다.
// ---------------------------
void DFS(int depth, int start)
{
	// 병원 M개를 모두 고른 경우
	if (depth == M)
	{
		// printCases();

		int tmp = calculate();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	// 병원 후보들 중에서 하나씩 선택
	for (int i = start; i < hcnt; i++)
	{
		num_of_cases[depth] = i;

		// 다음 병원은 현재보다 뒤에서만 고름
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

		// 병원 M개 고르는 모든 조합 탐색
		DFS(0, 0);

		printf("%d\n", minAnswer);
	}

	return 0;
}