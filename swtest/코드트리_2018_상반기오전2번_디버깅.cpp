// 코드트리 2018 상반기 오전 2번 - 디버깅
// https://www.codetree.ai/training-field/frequent-problems/problems/debugging
// BOJ 15684 - 사다리 조작 (더 강한 test case)
// https://www.acmicpc.net/problem/15684
// 문제 요약
// 세로선 C개, 가로 위치 R개가 있는 사다리 게임이 주어진다.
// 현재 놓여 있는 가로선 M개에 대해,
// 필요한 경우 가로선을 최대 3개까지 추가해서
// "각 세로선 번호가 그대로 자기 자신으로 도착"하도록 만들 수 있는지 확인한다.
//
// 목표
// 추가해야 하는 가로선의 최소 개수를 구한다.
// 불가능하면 -1 출력.
//
// 핵심 아이디어
// 1. 현재 상태에서 이미 조건을 만족하는지 check()로 확인
// 2. 아니라면 가로선을 1개, 2개, 3개 추가하는 모든 조합을 DFS로 시도
// 3. 가능한 순간의 추가 개수를 답으로 반환
//
// 이 코드는
// "사다리 시뮬레이션(check)" + "가로선 추가 조합 DFS"
// 구조의 완전탐색 문제이다.

#include <stdio.h>
#include <stdbool.h>

#define MAX_H (30 + 5)
#define MAX_N (10 + 5)

int T;

int C, M, R;                // C = 세로선 수, M = 기존 가로선 수, R = 행 수
int MAP[MAX_H][MAX_N];      // MAP[r][c] == 1 이면 (r,c)와 (r,c+1)을 잇는 가로선 존재

int num_of_cases[MAX_H * MAX_N]; // (현재 코드에서는 실제 사용 안 함, 디버그 흔적용)

// 가로선을 놓을 수 있는 후보 좌표 저장용 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC ladder[MAX_H * MAX_N];
int lcnt;                   // 후보 가로선 개수

bool PASS;                  // 현재 setup 개수로 성공 가능한 경우가 있는지 표시

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &C, &M, &R);

	// 맵 초기화
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
		{
			MAP[r][c] = 0;
		}
	}

	// 기존 가로선 입력
	// MAP[r][c] = 1 이면 c번 세로선과 c+1번 세로선을 연결
	for (int i = 0; i < M; i++)
	{
		int r, c;
		scanf("%d %d", &r, &c);

		MAP[r][c] = 1;
	}
}

// ---------------------------
// 디버그용 사다리 맵 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 배열 복사 함수
// 현재 코드에서는 check() 안의 옛 시도 흔적으로 남아 있음
// ---------------------------
void copyMap(int copy[MAX_H][MAX_N], int original[MAX_H][MAX_N])
{
	for (int i = 1; i <= R; i++)
	{
		for (int k = 1; k <= C; k++)
		{
			copy[i][k] = original[i][k];
		}
	}
}

// ---------------------------
// 디버그용 선택 출력
// 현재 코드에서는 실사용하지 않음
// ---------------------------
void printCases()
{
	for (int i = 0; i < lcnt; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 MAP 상태가 목표 조건을 만족하는지 검사
//
// 목표 조건:
// 각 시작 세로선 c가 끝까지 내려갔을 때
// 최종 도착 세로선도 c여야 한다.
//
// 검사 방법:
// start[c] = 현재 c번 위치에 있는 시작 번호
// 처음에는 start[c] = c
//
// 각 행을 위에서 아래로 내려가며
// 가로선이 있으면 두 번호를 swap 한다.
//
// 마지막에 start[c] == c 가 모두 성립하면 성공
// ---------------------------
bool check()
{
	// 아래 주석 블록은 tmpMAP을 따로 써서 검사하던 흔적
	// 현재는 DFS에서 MAP에 직접 설치/복구하고 있으므로 필요 없음

	// copyMap(tmpMAP, MAP);
	//
	// // setup 만큼 사다리 설치
	// for (int i = 0; i < setup; i++)
	// {
	//     int r, c;
	//     r = ladder[num_of_cases[i]].r;
	//     c = ladder[num_of_cases[i]].c;
	//     tmpMAP[r][c] = 1;
	// }
	//
	// for (int r = 1; r <= R; r++)
	//     for (int c = 1; c <= C; c++)
	//         if (tmpMAP[r][c] == 1 && tmpMAP[r][c + 1] == 1)
	//             return false;

	int start[MAX_N] = { 0 };

	// 시작 번호 초기화
	for (int c = 1; c <= C; c++)
		start[c] = c;

	// 위에서 아래로 내려가며 가로선이 있으면 swap
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
		{
			if (MAP[r][c] == 0)
				continue;

			int tmp = start[c];
			start[c] = start[c + 1];
			start[c + 1] = tmp;
		}
	}

	// 마지막에 자기 번호로 돌아왔는지 확인
	for (int c = 1; c <= C; c++)
	{
		if (start[c] != c)
			return false;
	}

	return true;
}

// ---------------------------
// DFS로 가로선을 setup개 추가하는 모든 경우 탐색
//
// depth : 현재 몇 개를 추가했는지
// start : 다음 후보를 탐색할 시작 인덱스
// setup : 이번 탐색에서 총 몇 개를 추가할 것인지
//
// 중요한 점:
// - 조합 탐색이므로 DFS(depth+1, i+1)
// - 인접한 위치에는 가로선을 놓을 수 없으므로 검사 필요
// - DFS에서 MAP에 직접 1로 놓고, 끝나면 0으로 되돌리는 백트래킹 사용
// ---------------------------
void DFS(int depth, int start, int setup)
{
	// 필요한 개수만큼 다 추가했으면 현재 상태 검사
	if (depth == setup)
	{
		// printCases();

		if (check() == true)
			PASS = true;

		return;
	}

	for (int i = start; i < lcnt; i++)
	{
		// num_of_cases[depth] = i;  // 옛 흔적, 현재는 직접 설치해서 불필요

		int r = ladder[i].r;
		int c = ladder[i].c;

		// 현재 위치 양옆에 이미 가로선이 있으면 설치 불가
		// (가로선은 서로 붙어 있을 수 없음)
		if (MAP[r][c - 1] == 1 || MAP[r][c + 1] == 1)
			continue;

		// 가로선 설치
		MAP[r][c] = 1;

		// 다음 가로선 선택
		DFS(depth + 1, i + 1, setup);

		// 백트래킹: 원상복구
		MAP[r][c] = 0;
	}
}

// ---------------------------
// 전체 시뮬레이션
//
// 0개 추가로 가능한지 먼저 확인
// 안 되면 1개, 2개, 3개 추가를 차례대로 시도
// 가장 먼저 가능한 개수를 반환
// 끝까지 안 되면 -1
// ---------------------------
int simulate()
{
	// 이미 만족하면 0
	if (check() == true)
		return 0;

	// 1개, 2개, 3개 추가 시도
	for (int setup = 1; setup <= 3; setup++)
	{
		DFS(0, 0, setup);

		if (PASS == true)
			return setup;
	}

	return -1;
}

// ---------------------------
// 가로선을 새로 놓을 수 있는 후보 위치 수집
//
// 조건:
// - 현재 위치에 가로선이 없어야 함
// - 오른쪽 위치에도 없어야 함
// - 왼쪽 위치에도 없어야 함
//
// 즉, 해당 위치에 가로선을 놓아도
// 인접 가로선과 붙지 않는 곳만 후보로 저장
// ---------------------------
void getEmptyLadder()
{
	lcnt = 0;

	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C - 1; c++)
		{
			if ((MAP[r][c] == 0 && MAP[r][c + 1] == 0)
				&& (MAP[r][c] == 0 && MAP[r][c - 1] == 0))
			{
				ladder[lcnt].r = r;
				ladder[lcnt++].c = c;
			}
		}
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

		// 설치 가능한 후보 위치들 미리 수집
		getEmptyLadder();

		PASS = false;

		printf("%d\n", simulate());
	}

	return 0;
}