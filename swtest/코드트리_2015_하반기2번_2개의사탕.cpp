// 코드트리 2015 하반기 2번 - 2개의 사탕
// https://www.codetree.ai/training-field/frequent-problems/problems/two-candies
// 문제 요약
// 빨간 사탕(R)과 파란 사탕(B)이 있는 보드에서
// 상/하/좌/우로 기울이는 동작을 최대 10번까지 수행할 수 있다.
//
// 목표
// - 빨간 사탕만 구멍(O)에 빠뜨리고
// - 파란 사탕은 빠지지 않게 하는
// 최소 기울이기 횟수를 구하는 문제
//
// 핵심 아이디어
// 1. 가능한 기울이기 방향 수열(길이 10)을 DFS로 전부 만들어 본다.
// 2. 만들어진 방향 수열을 simulate()로 실제 보드에 적용해 본다.
// 3. 빨간 사탕만 빠지는 경우의 최소 횟수를 정답으로 갱신한다.
//
// 이 코드의 특징
// - BFS가 아니라 "방향 수열 완전탐색 + 시뮬레이션" 방식
// - 연속해서 같은 방향으로 기울이는 경우는 의미가 적으므로 DFS에서 제외
// - 빨간 사탕이 성공해도, 같은 방향으로 파란 사탕이 이후 구멍에 빠질 수 있으면 실패 처리

#include <stdio.h>
#include <stdbool.h>

#define MAX (10 + 5)
#define INF (0x7fff0000)

int T;

int N, M;                 // 보드 크기
char MAP[MAX][MAX];       // 보드 정보

int num_of_cases[10 + 5]; // DFS로 만든 방향 수열 (최대 10번)

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC RED, BLUE;             // 초기 빨간 사탕 위치, 파란 사탕 위치

int minAnswer;            // 최소 정답

// 방향 배열: ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			scanf(" %c", &MAP[r][c]);

			// 빨간 사탕 위치 저장
			if (MAP[r][c] == 'R')
			{
				RED.r = r;
				RED.c = c;

				// 보드에서는 빈칸처럼 취급하기 위해 '.'로 바꿈
				MAP[r][c] = '.';
			}
			// 파란 사탕 위치 저장
			else if (MAP[r][c] == 'B')
			{
				BLUE.r = r;
				BLUE.c = c;

				// 마찬가지로 빈칸 처리
				MAP[r][c] = '.';
			}
		}
	}
}

// ---------------------------
// 디버그용 출력 함수
// 현재 RED, BLUE를 MAP 위에 잠시 올려서 출력
// ---------------------------
void printMap()
{
	MAP[RED.r][RED.c] = 'R';
	MAP[BLUE.r][BLUE.c] = 'B';

	for (int r = 0; r < N; r++)
		printf("%s\n", MAP[r]);
	putchar('\n');

	// 다시 원상복구
	MAP[RED.r][RED.c] = '.';
	MAP[BLUE.r][BLUE.c] = '.';
}

// ---------------------------
// 디버그용: 현재 방향 수열 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < 10; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 현재 num_of_cases[0..9] 방향 수열을 실제로 실행해 보는 함수
//
// 반환값
// - 빨간 사탕만 성공적으로 빠졌으면: 그때까지 사용한 기울이기 횟수
// - 실패하면: INF
// ---------------------------
int simulate()
{
	RC red, blue;

	// 초기 위치에서 시뮬레이션 시작
	red = RED;
	blue = BLUE;

	// 최대 10번 기울이기
	for (int i = 0; i < 10; i++)
	{
		int direction = num_of_cases[i];

		// 한 방향으로 기울이면
		// 두 사탕이 더 이상 움직일 수 없을 때까지 계속 이동
		while (1)
		{
			// 빨간 사탕의 다음 위치
			int red_nr = red.r + dr[direction];
			int red_nc = red.c + dc[direction];

			// 파란 사탕의 다음 위치
			int blue_nr = blue.r + dr[direction];
			int blue_nc = blue.c + dc[direction];

			// 빨간 사탕의 다음 칸이 벽인지
			bool checkWallRed = MAP[red_nr][red_nc] == '#';

			// 파란 사탕의 다음 칸이 벽인지
			bool checkWallBlue = MAP[blue_nr][blue_nc] == '#';

			// 빨간 사탕의 다음 칸이 현재 파란 사탕 위치인지
			// 즉, 파란 사탕 때문에 못 가는지
			bool checkCandyRed = (red_nr == blue.r) && (red_nc == blue.c);

			// 파란 사탕의 다음 칸이 현재 빨간 사탕 위치인지
			bool checkCandyBlue = (blue_nr == red.r) && (blue_nc == red.c);

			// 디버그용
			// RED = red; BLUE = blue;
			// printMap();

			// ----------------------------------------
			// 경우 1) 둘 다 다음 칸이 벽이면 더 못 움직이므로 정지
			// ----------------------------------------
			if (checkWallRed == true && checkWallBlue == true)
				break;

			// ----------------------------------------
			// 경우 2) 둘 다 다음 칸이 벽이 아니면 둘 다 1칸 이동
			// ----------------------------------------
			if (checkWallRed == false && checkWallBlue == false)
			{
				red.r = red_nr;
				red.c = red_nc;

				blue.r = blue_nr;
				blue.c = blue_nc;
			}
			// ----------------------------------------
			// 경우 3) 빨간 사탕은 벽 때문에 못 가고,
			//         파란 사탕만 이동 가능하며,
			//         파란 사탕 다음 칸이 빨간 사탕 자리도 아니면 이동
			// ----------------------------------------
			else if (checkWallRed == true && checkCandyBlue == false)
			{
				blue.r = blue_nr;
				blue.c = blue_nc;
			}
			// ----------------------------------------
			// 경우 4) 파란 사탕은 벽 때문에 못 가고,
			//         빨간 사탕만 이동 가능하며,
			//         빨간 사탕 다음 칸이 파란 사탕 자리도 아니면 이동
			// ----------------------------------------
			else if (checkWallBlue == true && checkCandyRed == false)
			{
				red.r = red_nr;
				red.c = red_nc;
			}
			// ----------------------------------------
			// 그 외의 경우는 서로 막고 있거나 애매한 충돌 상황이므로 정지
			// ----------------------------------------
			else
			{
				break;
			}

			// ----------------------------------------
			// 빨간 사탕이 구멍에 빠진 경우
			// ----------------------------------------
			if (MAP[red.r][red.c] == 'O')
			{
				// 같은 방향으로 파란 사탕도 계속 굴러가서
				// 구멍에 빠질 수 있는지 끝까지 확인
				while (MAP[blue.r][blue.c] != '#')
				{
					if (MAP[blue.r][blue.c] == 'O')
						return INF;   // 파란 사탕도 빠지면 실패

					blue.r += dr[direction];
					blue.c += dc[direction];
				}

				// 빨간 사탕만 빠졌다면 성공
				return i + 1;   // 사용한 기울이기 횟수
			}

			// ----------------------------------------
			// 파란 사탕이 먼저/같이 구멍에 빠지면 실패
			// ----------------------------------------
			if (MAP[blue.r][blue.c] == 'O')
				return INF;
		}
	}

	// 10번 안에 성공 못 하면 실패
	return INF;
}

// ---------------------------
// DFS로 길이 10의 방향 수열 생성
//
// depth     : 현재 몇 번째 방향까지 정했는지
// direction : 직전에 선택한 방향
// ---------------------------
void DFS(int depth, int direction)
{
	// 길이 10 수열이 완성되면 시뮬레이션 수행
	if (depth == 10)
	{
		// printCases();

		int tmp = simulate();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	// 4방향 중 하나 선택
	for (int i = 0; i < 4; i++)
	{
		// 연속해서 같은 방향은 제외
		// 예: 위 다음 위
		// 보통 의미가 없거나 중복 상태를 많이 만든다
		if (i == direction)
			continue;

		num_of_cases[depth] = i;

		DFS(depth + 1, i);
	}
}

int main()
{
	// 원래 테스트케이스 입력이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		// DFS로 모든 방향 수열 탐색
		DFS(0, -1);

		// 정답 출력
		if (minAnswer == INF)
			printf("-1\n");
		else
			printf("%d\n", minAnswer);
	}

	return 0;
}