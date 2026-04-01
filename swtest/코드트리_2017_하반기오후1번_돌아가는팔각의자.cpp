// 코드트리 2017 하반기 오후 1번 - 돌아가는 팔각의자
// https://www.codetree.ai/training-field/frequent-problems/problems/rounding-eight-angle
// 문제 요약
// 4개의 팔각의자가 일렬로 놓여 있고,
// 각 의자는 8개의 톱니 상태(0 또는 1)를 가진다.
//
// 회전 규칙
// - 어떤 의자를 회전시키면
// - 양옆 의자와 맞닿은 톱니를 비교해서
// - 서로 다르면 옆 의자도 반대 방향으로 회전한다.
// - 같으면 그 방향으로는 더 이상 전파되지 않는다.
//
// 인덱스 의미(이 코드 기준)
// chair[number][1] : 12시 방향 톱니
// chair[number][3] : 오른쪽 맞닿는 톱니
// chair[number][7] : 왼쪽 맞닿는 톱니
//
// 방향 값
//  1 : 시계 방향
// -1 : 반시계 방향
//
// 목표
// K번 회전 명령을 모두 수행한 뒤 점수를 계산한다.

#include <stdio.h>

#define MAX (100 + 10)

#define CLOCKWISE (1)
#define COUTNER_CLOCKWISE (-1)

int T;

int K;                     // 회전 명령 개수
int chair[5][10];          // 1~4번 의자, 각 의자 톱니는 1~8 사용
int chairNumber[MAX];      // k번째 명령에서 회전시킬 의자 번호
int directions[MAX];       // k번째 명령에서 회전 방향

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	// 4개의 의자 입력
	// 각 의자는 8개의 톱니 상태를 가짐
	for (int number = 1; number <= 4; number++)
	{
		for (int index = 1; index <= 8; index++)
		{
			scanf("%1d", &chair[number][index]);
		}
	}

	// 회전 명령 개수 입력
	scanf("%d", &K);

	// 각 회전 명령 입력
	for (int i = 0; i < K; i++)
	{
		scanf("%d %d", &chairNumber[i], &directions[i]);
	}
}

// ---------------------------
// 하나의 의자를 실제로 회전시키는 함수
//
// number    : 의자 번호
// direction : 회전 방향
//
// 반시계 방향(-1)
//   [1][2][3][4][5][6][7][8]
// ->[2][3][4][5][6][7][8][1]
//
// 시계 방향(1)
//   [1][2][3][4][5][6][7][8]
// ->[8][1][2][3][4][5][6][7]
// ---------------------------
void rotate(int number, int direction)
{
	int tmp;

	// 반시계 방향 회전
	if (direction == COUTNER_CLOCKWISE)
	{
		tmp = chair[number][1];

		for (int index = 1; index <= 7; index++)
			chair[number][index] = chair[number][index + 1];

		chair[number][8] = tmp;
	}
	// 시계 방향 회전
	else
	{
		tmp = chair[number][8];

		for (int index = 8; index >= 2; index--)
			chair[number][index] = chair[number][index - 1];

		chair[number][1] = tmp;
	}
}

// ---------------------------
// 전체 회전 시뮬레이션
// ---------------------------
void simulate()
{
	// K개의 회전 명령을 순서대로 처리
	for (int k = 0; k < K; k++)
	{
		// check[i] = i번 의자가 이번 명령에서 회전할 방향
		//  0이면 회전 안 함
		//  1이면 시계 방향
		// -1이면 반시계 방향
		int check[5 + 1] = { 0 };

		int target = chairNumber[k];   // 이번에 직접 회전시키는 의자
		int direction = directions[k]; // 그 방향

		// 시작 의자는 무조건 해당 방향으로 회전
		check[target] = direction;

		// ----------------------------------------
		// 오른쪽으로 회전 전파 검사
		//
		// 현재 의자의 오른쪽 톱니(3번)와
		// 오른쪽 의자의 왼쪽 톱니(7번)를 비교
		// 서로 다르면 반대 방향으로 회전 전파
		// ----------------------------------------
		for (int right = target; right <= 3; right++)
		{
			if (chair[right][3] != chair[right + 1][7])
				check[right + 1] = check[right] * (-1);
			else
				break;
		}

		// ----------------------------------------
		// 왼쪽으로 회전 전파 검사
		//
		// 현재 의자의 왼쪽 톱니(7번)와
		// 왼쪽 의자의 오른쪽 톱니(3번)를 비교
		// 서로 다르면 반대 방향으로 회전 전파
		// ----------------------------------------
		for (int left = target; left >= 2; left--)
		{
			if (chair[left][7] != chair[left - 1][3])
				check[left - 1] = check[left] * (-1);
			else
				break;
		}

		// ----------------------------------------
		// 전파 여부가 다 계산된 뒤 한꺼번에 회전
		//
		// 중요:
		// 회전 전파를 계산할 때는 "현재 상태" 기준이어야 하므로
		// 검사 중간에 바로 회전시키면 안 된다.
		// 따라서 먼저 check 배열에 저장하고,
		// 마지막에 실제 회전을 수행한다.
		// ----------------------------------------
		for (int i = 1; i <= 4; i++)
		{
			if (check[i] != 0)
				rotate(i, check[i]);
		}
	}
}

// ---------------------------
// 최종 점수 계산
//
// 1번 의자 12시 톱니가 1이면 1점
// 2번 의자 12시 톱니가 1이면 2점
// 3번 의자 12시 톱니가 1이면 4점
// 4번 의자 12시 톱니가 1이면 8점
// ---------------------------
int getScore()
{
	// 일반화 버전 예시 (주석 처리된 원본)
	//
	// int sum, mul;
	// sum = 0;
	// mul = 1;
	// for (int i = 1; i <= 4; i++)
	// {
	//     sum += mul * chair[i][1];
	//     mul *= 2;
	// }

	return (chair[1][1] * 1) +
		(chair[2][1] * 2) +
		(chair[3][1] * 4) +
		(chair[4][1] * 8);
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

		simulate();

		printf("%d\n", getScore());
	}

	return 0;
}