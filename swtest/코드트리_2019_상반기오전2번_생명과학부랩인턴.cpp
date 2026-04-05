// 코드트리 2019 상반기 오전 2번 - 생명과학부 랩 인턴
// https://www.codetree.ai/training-field/frequent-problems/problems/biology-lab-intern
// 문제 요약
// N x M 격자에 여러 마리의 곰팡이가 있다.
// 각 곰팡이는
// - 이동 거리(distance)
// - 이동 방향(dir)
// - 크기(size)
// 를 가진다.
//
// 진행 순서
// 1. 인턴은 1열부터 M열까지 차례대로 이동한다.
// 2. 현재 열에서 가장 위에 있는 곰팡이 1마리를 채취한다.
// 3. 남아 있는 모든 곰팡이가 동시에 이동한다.
// 4. 이동 후 한 칸에 여러 마리 곰팡이가 모이면 가장 큰 곰팡이만 남는다.
//
// 목표
// 인턴이 채취한 곰팡이 크기의 총합을 구한다.
//
// 핵심 아이디어
// - catchMold(c): c열에서 가장 위 곰팡이를 잡는다.
// - moveMold(): 모든 곰팡이를 동시에 이동시킨다.
// - tmpMold 배열을 사용해서 "동시 이동"을 구현한다.
// - 이동 거리가 매우 클 수 있으므로 주기(mod)를 이용해 최적화한다.

#include <stdio.h>

#define MAX (100 + 20)

int T;
int N, M, K;

// 한 마리 곰팡이 정보
struct MOLD
{
	int distance; // 움직이는 거리 s
	int dir;      // 이동 방향 d
	int size;     // 곰팡이 크기 b
};

typedef struct MOLD MOLD;

MOLD mold[MAX][MAX];     // 현재 상태
MOLD tmpMold[MAX][MAX];  // 이동 후 상태를 임시 저장

// 방향 번호
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

	// 먼저 모든 칸을 빈칸으로 초기화
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			mold[r][c].distance = mold[r][c].dir = mold[r][c].size = 0;

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
// 디버그용 출력
// 현재는 size만 출력
// 필요하면 dir, distance로 바꿔 확인 가능
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", mold[r][c].size);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 2단계: 곰팡이 채취
//
// 인턴이 현재 열 sc에 있을 때,
// 위에서 아래로 내려가며 가장 먼저 만나는 곰팡이를 잡는다.
//
// 반환값:
// - 잡은 곰팡이의 크기
// - 없으면 0
// ---------------------------
int catchMold(int sc)
{
	// 위쪽 행부터 확인
	for (int r = 1; r <= N; r++)
	{
		// 곰팡이가 있으면 채취
		if (mold[r][sc].size != 0)
		{
			int ret = mold[r][sc].size;

			// 채취 후 해당 칸은 빈칸 처리
			mold[r][sc].size = 0;
			mold[r][sc].dir = 0;
			mold[r][sc].distance = 0;

			return ret;
		}
	}

	return 0;
}

// ---------------------------
// 3단계: 곰팡이 이동
//
// 모든 곰팡이는 동시에 이동해야 하므로
// 바로 mold에 덮어쓰면 안 된다.
// 따라서 tmpMold에 이동 결과를 기록한 후
// 마지막에 mold = tmpMold 로 반영한다.
// ---------------------------
void moveMold()
{
	// tmpMold를 빈 상태로 초기화
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			tmpMold[r][c].distance = tmpMold[r][c].dir = tmpMold[r][c].size = 0;

	// 방향 반전용 배열
	// 1(위) <-> 2(아래)
	// 3(오른쪽) <-> 4(왼쪽)
	int changeDir[5] = { 0, 2, 1, 4, 3 };

	// 현재 존재하는 모든 곰팡이를 하나씩 이동
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 빈칸이면 무시
			if (mold[r][c].size == 0) continue;

			MOLD md = mold[r][c];
			int sr, sc, dir;

			// 현재 곰팡이의 시작 위치와 방향
			sr = r;
			sc = c;
			dir = md.dir;

			int move = md.distance;

			// ---------------------------
			// 이동 거리 최적화
		//
			// 위/아래 이동은 세로 길이 N에서 왕복 주기:
			//   (N - 1) * 2
		//
			// 왼쪽/오른쪽 이동은 가로 길이 M에서 왕복 주기:
			//   (M - 1) * 2
		//
			// 예를 들어 세로로 1000칸 움직여도
			// 실제로는 왕복 주기만큼 나눈 나머지만 이동하면 결과가 같다.
			// ---------------------------
			if (dir <= 2) move = md.distance % ((N - 1) * 2);
			else          move = md.distance % ((M - 1) * 2);

			// ---------------------------
			// 4-1. 곰팡이 이동
			// 한 칸씩 move번 이동
			// ---------------------------
			for (int m = 0; m < move; m++)
			{
				int nr = sr + dr[dir];
				int nc = sc + dc[dir];

				// ---------------------------
				// 4-2. 다음 칸이 범위 밖이면 방향 반전
				// ---------------------------
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

			// ---------------------------
			// 5. 이동 후 충돌 처리
			//
			// 같은 칸 (sr, sc)에 여러 마리가 모이면
			// 크기가 가장 큰 곰팡이만 살아남는다.
			// ---------------------------
			if (tmpMold[sr][sc].size < mold[r][c].size)
			{
				tmpMold[sr][sc] = mold[r][c];
				tmpMold[sr][sc].dir = dir; // 이동 후 최종 방향 반영
			}
		}
	}

	// tmpMold를 실제 상태에 반영
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			mold[r][c] = tmpMold[r][c];
}

// ---------------------------
// 전체 시뮬레이션
//
// 인턴은 1열부터 M열까지 이동한다.
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

	// 1. 인턴은 첫 번째 열부터 탐색 시작
	for (int c = 1; c <= M; c++)
	{
		// 2. 현재 열에서 가장 위 곰팡이 채취
		sum += catchMold(c);

		// 3. 곰팡이 이동
		moveMold();
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

		printf("%d\n", simulate());
	}

	return 0;
}