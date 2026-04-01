// 코드트리 2019 상반기 오전 2번 - 생명과학부 랩 인턴
//
// 문제 요약
// N x M 격자에 여러 마리의 곰팡이가 있다.
// 각 곰팡이는
// - 이동 거리(distance)
// - 이동 방향(dir)
// - 크기(size)
// 를 가진다.
//
// 진행 규칙
// 1. 인턴은 1열부터 M열까지 차례대로 이동한다.
// 2. 각 열에 도착하면, 그 열에서 가장 위에 있는 곰팡이 1마리를 채취한다.
// 3. 그 다음 모든 곰팡이가 동시에 이동한다.
// 4. 이동 후 같은 칸에 여러 마리 곰팡이가 모이면 가장 큰 곰팡이만 남는다.
//
// 목표
// 인턴이 채취한 곰팡이 크기의 총합을 구하는 문제
//
// 핵심 아이디어
// - catchMold(c) : c열에서 가장 위 곰팡이 채취
// - moveMold()   : 모든 곰팡이 이동 + 충돌 처리
// - 이를 1열부터 M열까지 반복
//
// 이 코드는 곰팡이 1마리 정보를 구조체 MOLD로 관리하고,
// tmpMold 배열을 사용해서 "동시 이동"을 구현한다.

#include <stdio.h>

#define MAX (100 + 20)

int T;
int N, M, K;   // N: 행 수, M: 열 수, K: 곰팡이 수

// 하나의 곰팡이 정보
struct MOLD
{
	int distance; // 이동 거리 s
	int dir;      // 이동 방향 d
	int size;     // 곰팡이 크기 b
};

typedef struct MOLD MOLD;

MOLD mold[MAX][MAX];      // 현재 격자 상태
MOLD tmpMold[MAX][MAX];   // 이동 후 상태를 임시 저장

// 방향 배열
// 0은 사용 안 함
// 1: 위, 2: 아래, 3: 오른쪽, 4: 왼쪽
int dr[] = { 0, -1, 1, 0, 0 };
int dc[] = { 0,  0, 0, 1,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &K);

	// 먼저 모든 칸을 빈칸 상태로 초기화
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			mold[r][c].distance = 0;
			mold[r][c].dir = 0;
			mold[r][c].size = 0;
		}
	}

	// K마리 곰팡이 입력
	for (int k = 0; k < K; k++)
	{
		int r, c, s, d, b;
		scanf("%d %d %d %d %d", &r, &c, &s, &d, &b);

		mold[r][c].distance = s;
		mold[r][c].dir = d;
		mold[r][c].size = b;
	}
}

// ---------------------------
// 디버그용 출력 함수
// 현재는 size만 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", mold[r][c].size); // 필요하면 dir, distance로 바꿔서 확인 가능
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 1단계: 곰팡이 채취
//
// 인턴이 현재 열 sc에 있을 때,
// 위에서 아래로 내려가며 가장 먼저 만나는 곰팡이를 채취한다.
//
// 반환값:
// - 채취한 곰팡이의 크기
// - 없으면 0
// ---------------------------
int catchMold(int sc)
{
	// 위쪽 행부터 탐색
	for (int r = 1; r <= N; r++)
	{
		// 곰팡이가 있으면 채취
		if (mold[r][sc].size != 0)
		{
			int ret = mold[r][sc].size;

			// 채취 후 해당 칸은 빈칸
			mold[r][sc].size = 0;
			mold[r][sc].distance = 0;
			mold[r][sc].dir = 0;

			return ret;
		}
	}

	return 0;
}

// ---------------------------
// 2단계: 모든 곰팡이 이동
//
// 주의:
// 곰팡이들은 동시에 이동해야 하므로
// 바로 mold에 덮어쓰면 안 된다.
// 따라서 tmpMold에 새 상태를 기록한 뒤
// 마지막에 mold = tmpMold 로 바꾼다.
//
// 또한 이동 후 같은 칸에 여러 마리가 모이면
// 가장 큰 곰팡이만 남긴다.
// ---------------------------
void moveMold()
{
	// tmpMold를 빈칸 상태로 초기화
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			tmpMold[r][c].distance = 0;
			tmpMold[r][c].dir = 0;
			tmpMold[r][c].size = 0;
		}
	}

	// 방향 반전용 배열
	// 1(위) <-> 2(아래)
	// 3(오른쪽) <-> 4(왼쪽)
	int changeDir[5] = { 0, 2, 1, 4, 3 };

	// 현재 존재하는 모든 곰팡이를 이동
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 빈칸이면 무시
			if (mold[r][c].size == 0)
				continue;

			MOLD md = mold[r][c];

			int sr, sc, dir;
			sr = r;
			sc = c;
			dir = md.dir;

			int move = md.distance;

			// ----------------------------------------
			// 실제 이동 시뮬레이션
			// 한 칸씩 move번 이동
			// 벽을 만나면 방향 반대로 바꿔서 계속 이동
			// ----------------------------------------
			for (int m = 0; m < move; m++)
			{
				int nr = sr + dr[dir];
				int nc = sc + dc[dir];

				// 범위를 벗어나면 방향 반전
				if (nr < 1 || nc < 1 || nr > N || nc > M)
				{
					dir = changeDir[dir];

					nr = sr + dr[dir];
					nc = sc + dc[dir];
				}

				// 한 칸 이동
				sr = nr;
				sc = nc;
			}

			// ----------------------------------------
			// 이동이 끝난 위치가 (sr, sc)
			// 같은 칸에 이미 다른 곰팡이가 들어와 있다면
			// 더 큰 곰팡이만 남긴다.
			// ----------------------------------------
			if (tmpMold[sr][sc].size < mold[r][c].size)
			{
				tmpMold[sr][sc] = mold[r][c];
				tmpMold[sr][sc].dir = dir; // 이동 후 최종 방향 반영
			}
		}
	}

	// tmpMold를 실제 상태로 반영
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			mold[r][c] = tmpMold[r][c];
		}
	}
}

// ---------------------------
// 전체 시뮬레이션
//
// 인턴은 1열부터 M열까지 차례대로 이동한다.
// 각 열마다:
// 1. 곰팡이 채취
// 2. 곰팡이 전체 이동
//
// 반환값:
// 총 채취한 곰팡이 크기 합
// ---------------------------
int simulate()
{
	int sum = 0;

	// 인턴이 1열부터 M열까지 이동
	for (int c = 1; c <= M; c++)
	{
		// 현재 열에서 가장 위 곰팡이 채취
		sum += catchMold(c);

		// 곰팡이 전체 이동
		moveMold();
	}

	return sum;
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

		printf("%d\n", simulate());
	}

	return 0;
}