// 코드트리 2019 상반기 오전 1번 - 시공의 돌풍
// https://www.codetree.ai/training-field/frequent-problems/problems/heros-of-storm
// 문제 요약
// N x M 격자에 먼지가 있고, 공기청정기(돌풍 기계)가 2칸 차지하고 있다.
// 매 초마다 다음 두 단계가 순서대로 일어난다.
//
// 1. 먼지 확산
//    - 각 먼지 칸은 상하좌우로 먼지를 퍼뜨린다.
//    - 퍼지는 양 = 현재 먼지 / 5
//    - 공기청정기 칸(-1)으로는 퍼지지 않는다.
//
// 2. 공기청정기 작동
//    - 위쪽 공기청정기: 반시계 방향으로 바람 순환
//    - 아래쪽 공기청정기: 시계 방향으로 바람 순환
//    - 바람 방향대로 먼지가 한 칸씩 이동한다.
//    - 공기청정기 바로 오른쪽 칸에는 깨끗한 공기가 들어가므로 0이 된다.
//
// 목표
// time초 후 남아 있는 먼지 총합을 구한다.

#include <stdio.h>

#define MAX (50 + 10)
#define TORNADO_POSITION (-1)

int T;
int N, M, time;
int MAP[MAX][MAX];      // 현재 먼지 상태
int tmpMAP[MAX][MAX];   // 확산량 임시 저장 배열

struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC tornado[2];  // 공기청정기 두 칸 위치
int tcnt;

// 방향 배열: ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &time);

	tcnt = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);

			// 공기청정기 위치 저장
			if (MAP[r][c] == TORNADO_POSITION)
			{
				tornado[tcnt].r = r;
				tornado[tcnt++].c = c;
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
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 1단계: 먼지 확산
//
// 핵심 아이디어
// - MAP를 바로 수정하면서 확산시키면
//   이미 퍼진 먼지가 또 확산되는 문제가 생긴다.
// - 그래서 "이번 초에 새로 퍼진 양"만 tmpMAP에 따로 누적한다.
// - 마지막에 MAP += tmpMAP 로 합친다.
// ---------------------------
void diffusion()
{
	// 이번 초에 퍼진 먼지 양을 저장할 tmpMAP 초기화
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			tmpMAP[r][c] = 0;

	// 모든 칸 검사
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 먼지가 없거나 공기청정기 칸이면 확산할 필요 없음
			if (MAP[r][c] <= 0) continue;

			int dust, count;
			dust = count = 0;

			// 상하좌우 4방향으로 확산 시도
			for (int i = 0; i < 4; i++)
			{
				int nr = r + dr[i];
				int nc = c + dc[i];

				// 범위 밖이면 확산 불가
				if (nr < 1 || nc < 1 || nr > N || nc > M) continue;

				// 공기청정기 칸이면 확산 불가
				if (MAP[nr][nc] == TORNADO_POSITION) continue;

				// 확산량 = 현재 먼지 / 5
				dust = MAP[r][c] / 5;

				// 확산된 양을 임시 배열에 누적
				tmpMAP[nr][nc] += dust;
				count++;
			}

			// 현재 칸에서는 count번만큼 퍼져나갔으므로 그만큼 감소
			MAP[r][c] -= (dust * count);
		}
	}

	// 확산 결과를 원본 MAP에 반영
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			MAP[r][c] += tmpMAP[r][c];
}

// ---------------------------
// 2단계: 공기청정기 작동
//
// 위쪽 공기청정기: 반시계 방향
// 아래쪽 공기청정기: 시계 방향
//
// 여기서는 "테두리 값을 한 칸씩 미는 방식"으로 구현한다.
// ---------------------------
void cleaning()
{
	int up_r = tornado[0].r;     // 위쪽 공기청정기 행
	int down_r = tornado[1].r;   // 아래쪽 공기청정기 행

	// =====================================================
	// [위쪽 공기청정기] 반시계 방향 회전
	//
	// 회전 범위:
	// (1,1) ~ (up_r, M)
	// =====================================================

	// 1) 왼쪽 세로줄을 위로 한 칸씩 당김
	//
	// 좌표 이동:
	// (up_r-1,1) <- (up_r-2,1)
	// (up_r-2,1) <- (up_r-3,1)
	// ...
	// (2,1) <- (1,1)
	//
	// 예를 들어 up_r = 4이면:
	// (3,1) <- (2,1)
	// (2,1) <- (1,1)
	for (int r = up_r - 1; r > 1; r--)
		MAP[r][1] = MAP[r - 1][1];

	// 2) 윗줄을 왼쪽으로 한 칸씩 당김
	//
	// 좌표 이동:
	// (1,1) <- (1,2)
	// (1,2) <- (1,3)
	// ...
	// (1,M-1) <- (1,M)
	for (int c = 1; c <= M - 1; c++)
		MAP[1][c] = MAP[1][c + 1];

	// 3) 오른쪽 세로줄을 아래로 한 칸씩 당김
	//
	// 좌표 이동:
	// (1,M) <- (2,M)
	// (2,M) <- (3,M)
	// ...
	// (up_r-1,M) <- (up_r,M)
	for (int r = 1; r <= up_r - 1; r++)
		MAP[r][M] = MAP[r + 1][M];

	// 4) 공기청정기 행을 오른쪽으로 한 칸씩 당김
	//
	// 좌표 이동:
	// (up_r,M) <- (up_r,M-1)
	// (up_r,M-1) <- (up_r,M-2)
	// ...
	// (up_r,2) <- (up_r,1)
	for (int c = M; c > 1; c--)
		MAP[up_r][c] = MAP[up_r][c - 1];

	// 공기청정기 바로 오른쪽 칸에는 깨끗한 공기가 들어감
	MAP[up_r][2] = 0;

	// 공기청정기 위치 자체는 -1 유지
	MAP[up_r][1] = TORNADO_POSITION;

	// =====================================================
	// [아래쪽 공기청정기] 시계 방향 회전
	//
	// 회전 범위:
	// (down_r,1) ~ (N,M)
	// =====================================================

	// 1) 왼쪽 세로줄을 아래로 한 칸씩 당김
	//
	// 좌표 이동:
	// (down_r+1,1) <- (down_r+2,1)
	// ...
	// (N-1,1) <- (N,1)
	for (int r = down_r + 1; r <= N - 1; r++)
		MAP[r][1] = MAP[r + 1][1];

	// 2) 아랫줄을 왼쪽으로 한 칸씩 당김
	//
	// 좌표 이동:
	// (N,1) <- (N,2)
	// (N,2) <- (N,3)
	// ...
	// (N,M-1) <- (N,M)
	for (int c = 1; c <= M - 1; c++)
		MAP[N][c] = MAP[N][c + 1];

	// 3) 오른쪽 세로줄을 위로 한 칸씩 당김
	//
	// 좌표 이동:
	// (N,M) <- (N-1,M)
	// (N-1,M) <- (N-2,M)
	// ...
	// (down_r+1,M) <- (down_r,M)
	for (int r = N; r > down_r; r--)
		MAP[r][M] = MAP[r - 1][M];

	// 4) 공기청정기 행을 오른쪽으로 한 칸씩 당김
	//
	// 좌표 이동:
	// (down_r,M) <- (down_r,M-1)
	// ...
	// (down_r,2) <- (down_r,1)
	for (int c = M; c > 1; c--)
		MAP[down_r][c] = MAP[down_r][c - 1];

	// 공기청정기 바로 오른쪽 칸은 0
	MAP[down_r][2] = 0;

	// 공기청정기 위치 유지
	MAP[down_r][1] = TORNADO_POSITION;
}

// ---------------------------
// 남은 먼지 총합 계산
// 공기청정기(-1)는 제외
// ---------------------------
int getAnswer()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] != TORNADO_POSITION)
				sum += MAP[r][c];
		}
	}

	return sum;
}

int main()
{
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// time초 동안 반복
		for (int t = 0; t < time; t++)
		{
			diffusion();
			cleaning();
			// printMap();
		}

		printf("%d\n", getAnswer());
	}

	return 0;
}