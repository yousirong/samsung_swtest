/*
	[코드트리] 2017 상반기 오후 1번 - 자율주행 자동차 (연습 버전)

	■ 요약
	  현재 칸 청소 -> 왼쪽부터 네 방향 확인 -> 갈 곳이 없으면 방향 유지한 채 후진,
	  후진할 칸이 인도면 멈춘다. 마지막에 청소한 칸 수를 센다.
	  자세한 설명은 코드트리_2017_상반기오후1번_자율주행자동차.cpp 참고.
*/
#include <stdio.h>	            // scanf, printf, putchar 사용

#define MAX (50+5)              // N, M 최대 50 + 여유

#define ROAD (0)                // 청소 안 한 도로
#define STREET (1)              // 인도
#define MARK (2)                // 청소한 도로

int T;                          // 테스트 케이스 수
int N, M, R, C, D;              // 격자 크기, 위치, 방향
int MAP[MAX][MAX];              // 격자

// 북 동 남 서
int dr[] = { -1,0,1,0 };        // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };        // 방향별 열 변화량

void input()
{
	scanf("%d %d %d %d %d", &N, &M, &R, &C, &D);   // 격자 크기, 위치, 방향 입력

	for (int r = 0; r < N; r++)          // 모든 행
		for (int c = 0; c < M; c++)      // 모든 열
			scanf("%d", &MAP[r][c]);     // 칸 상태 입력
}

void printMap() // for debug
{
	for (int r = 0; r < N; r++)          // 모든 행
	{
		for (int c = 0; c < M; c++)      // 모든 열
			printf("%d ", MAP[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void simulate()
{
	int changeDir[] = { 3,0,1,2 };       // 왼쪽 회전 표

	while (1)                            // 멈출 때까지
	{
		MAP[R][C] = MARK;                // 현재 칸 청소

		int nDir = changeDir[D];         // 왼쪽 방향
		int nr = R + dr[nDir];           // 왼쪽 칸 행
		int nc = C + dc[nDir];           // 왼쪽 칸 열

		if (MAP[nr][nc] == ROAD)         // 왼쪽이 청소 안 한 도로면
		{
			D = nDir;                    // 회전
			R = nr;                      // 전진
			C = nc;
		}
		else                             // 왼쪽이 막혔으면
		{
			int i;                       // 확인한 방향 수
			for (i = 0; i < 4; i++)      // 네 방향 확인
			{
				D = nDir;                // 회전
				int nr = R + dr[D];      // 앞 칸 행
				int nc = C + dc[D];      // 앞 칸 열

				if (MAP[nr][nc] == ROAD) // 청소할 도로면
				{
					R = nr;              // 전진
					C = nc;
					break;               // 확인 끝
				}
				nDir = changeDir[D];     // 한 번 더 왼쪽
			}

			if (i == 4)                  // 네 방향 모두 실패
			{
				nr = R - dr[D];          // 방향 유지한 채 뒤 칸
				nc = C - dc[D];

				R = nr;                  // 후진
				C = nc;

				if (MAP[R][C] == STREET) break;   // 인도면 멈춤
			}
		}
	}
}

int getAnswer()
{
	int sum = 0;                         // 청소한 칸 수
	for (int r = 0; r < N; r++)          // 모든 행
		for (int c = 0; c < M; c++)      // 모든 열
			if (MAP[r][c] == MARK) sum++;   // 청소한 칸이면 센다
	return sum;                          // 개수 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		printf("%d\n", getAnswer());     // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
