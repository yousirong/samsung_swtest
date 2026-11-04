/*
	[코드트리] 2015 하반기 2번 - 2개의 사탕 (연습 버전)

	■ 요약
	  방향 수열(길이 10, 직전과 같은 방향 제외)을 DFS로 만들고, 수열마다 두 사탕을 굴려 본다.
	  빨강만 구멍에 빠지면 그때까지의 횟수로 최솟값을 갱신한다.
	  자세한 설명은 코드트리_2015_하반기2번_2개의사탕.cpp 참고.
*/
#include <stdio.h>                // scanf, printf, putchar 사용

#define MAX (10+5)                // 보드 최대 10 + 여유
#define INF (0x7fff0000)          // 실패 표시용 큰 값

int T;                            // 테스트 케이스 수
int N, M;                         // 보드 크기

char MAP[MAX][MAX];               // 보드 (# 벽, O 구멍, . 빈칸)

int num_of_cases[10 + 5];         // DFS로 만든 방향 수열

struct RC                         // 좌표 한 쌍
{
	int r;                        // 행
	int c;                        // 열
};

RC RED, BLUE;                     // 사탕의 처음 위치

int minAnswer;                    // 최소 기울이기 횟수

int dr[] = { -1,0,1,0 };          // ↑ → ↓ ← 행 변화량
int dc[] = { 0,1,0,-1 };          // ↑ → ↓ ← 열 변화량

void input()
{
	scanf("%d %d", &N, &M);              // 보드 크기 입력

	for (int r = 0; r < N; r++)          // 모든 행
	{
		for (int c = 0; c < M; c++)      // 모든 열
		{
			scanf(" %c", &MAP[r][c]);    // 한 글자 입력

			if (MAP[r][c] == 'R')        // 빨간 사탕
			{
				RED.r = r;               // 위치 저장 (행)
				RED.c = c;               // 위치 저장 (열)

				MAP[r][c] = '.';         // 보드에서는 빈칸으로
			}
			else if (MAP[r][c] == 'B')   // 파란 사탕
			{
				BLUE.r = r;              // 위치 저장 (행)
				BLUE.c = c;              // 위치 저장 (열)

				MAP[r][c] = '.';         // 보드에서는 빈칸으로
			}
		}
	}
}

void printMap()
{
	MAP[RED.r][RED.c] = 'R';             // 빨간 사탕 잠시 표시
	MAP[BLUE.r][BLUE.c] = 'B';           // 파란 사탕 잠시 표시

	for (int r = 0; r < N; r++)          // 모든 행
		printf("%s\n", MAP[r]);          // 한 줄 출력
	putchar('\n');                       // 빈 줄

	MAP[RED.r][RED.c] = '.';             // 표시 되돌리기
	MAP[BLUE.r][BLUE.c] = '.';           // 표시 되돌리기
}

void printCases()
{
	for (int i = 0; i < 10; i++) printf("%d", num_of_cases[i]);   // 방향 수열 출력
	putchar('\n');                       // 줄바꿈
}

int simulate()
{
	RC red, blue;                        // 이번 시뮬레이션의 사탕 위치

	red = RED;                           // 처음 위치에서 시작
	blue = BLUE;

	for (int i = 0; i < 10; i++)         // 최대 10번 기울이기
	{
		int direction = num_of_cases[i]; // 이번 방향

		while (1)                        // 더 못 움직일 때까지
		{
			int red_nr = red.r + dr[direction];     // 빨강 다음 행
			int red_nc = red.c + dc[direction];     // 빨강 다음 열
			int blue_nr = blue.r + dr[direction];   // 파랑 다음 행
			int blue_nc = blue.c + dc[direction];   // 파랑 다음 열

			bool checkWallRed = MAP[red_nr][red_nc] == '#';          // 빨강 앞이 벽인가
			bool checkWallBlue = MAP[blue_nr][blue_nc] == '#';       // 파랑 앞이 벽인가
			bool checkCandyRed = (red_nr == blue.r) && (red_nc == blue.c);     // 빨강 앞에 파랑이 있는가
			bool checkCandyBlue = (blue_nr == red.r) && (blue_nc == red.c);    // 파랑 앞에 빨강이 있는가

			if (checkWallRed == true && checkWallBlue == true) break;   // 둘 다 막힘

			if (checkWallRed == false && checkWallBlue == false)        // 둘 다 갈 수 있음
			{
				red.r = red_nr;          // 빨강 이동
				red.c = red_nc;

				blue.r = blue_nr;        // 파랑 이동
				blue.c = blue_nc;
			}

			else if (checkWallRed == true && checkCandyBlue == false)   // 빨강만 막힘
			{
				blue.r = blue_nr;        // 파랑만 이동
				blue.c = blue_nc;
			}
			else if (checkWallBlue == true && checkCandyRed == false)   // 파랑만 막힘
			{
				red.r = red_nr;          // 빨강만 이동
				red.c = red_nc;
			}
			else                         // 서로 막고 있음
				break;

			if (MAP[red.r][red.c] == 'O')    // 빨강이 구멍에 빠짐
			{
				while (MAP[blue.r][blue.c] != '#')   // 파랑을 마저 굴려 본다
				{
					if (MAP[blue.r][blue.c] == 'O') return INF;   // 파랑도 빠지면 실패

					blue.r += dr[direction];     // 파랑 한 칸 더
					blue.c += dc[direction];
				}

				return i + 1;            // 빨강만 빠짐 -> 성공
			}

			if (MAP[blue.r][blue.c] == 'O') return INF;   // 파랑이 먼저 빠짐 -> 실패
		}
	}

	return INF;                          // 10번 안에 실패
}

void DFS(int depth, int direction)
{
	if (depth == 10)                     // 수열 완성
	{
		//printCases();

		int tmp = simulate();            // 시뮬레이션 결과
		if (tmp < minAnswer) minAnswer = tmp;   // 최솟값 갱신
		return;                          // 되돌아간다
	}

	for (int i = 0; i < 4; i++)          // 4방향 후보
	{
		if (i == direction) continue;    // 직전과 같은 방향은 건너뜀

		num_of_cases[depth] = i;         // 이번 방향 기록

		DFS(depth + 1, i);               // 다음 순번
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		minAnswer = INF;                 // 최솟값 초기화

		DFS(0, -1);                      // 수열 만들기 시작

		if (minAnswer == INF) printf("-1\n");    // 실패
		else printf("%d\n", minAnswer);          // 최소 횟수 출력
	}
	return 0;                            // 프로그램 정상 종료

}
