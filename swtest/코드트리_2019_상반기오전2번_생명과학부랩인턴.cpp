/*
	[코드트리] 2019 상반기 오전 2번 - 생명과학부 랩 인턴
	https://www.codetree.ai/training-field/frequent-problems/problems/biology-lab-intern

	■ 문제 요약
	  N x M 격자에 곰팡이 K마리가 있다. 각 곰팡이는 이동 거리 s, 이동 방향 d, 크기 b를 가진다.
	  방향은 1 = 위, 2 = 아래, 3 = 오른쪽, 4 = 왼쪽이다.

	  인턴이 1열부터 M열까지 한 열씩 이동하며, 각 열에서 아래를 반복한다.

	    1) 그 열에서 "가장 위"에 있는 곰팡이 한 마리를 채취한다 (없으면 넘어간다)
	    2) 남아 있는 모든 곰팡이가 동시에 자기 거리만큼 이동한다
	       격자 끝에 닿으면 방향을 반대로 바꿔 계속 간다 (벽에 튕긴다)
	    3) 이동 후 한 칸에 여러 마리가 겹치면 크기가 가장 큰 한 마리만 남는다

	  채취한 곰팡이 크기의 합을 출력한다.

	  (백준 17143 "낚시왕"과 같은 문제다)

	■ 이동 거리 줄이기 (이 문제의 핵심)
	  s가 아주 클 수 있어서 한 칸씩 s번 움직이면 시간이 모자란다.
	  하지만 벽에 튕기는 운동은 주기적이라 왕복 한 바퀴를 돌면 완전히 제자리로 온다.

	    위/아래로 움직이는 곰팡이의 주기 : (N - 1) * 2
	    좌/우로 움직이는 곰팡이의 주기   : (M - 1) * 2

	  그래서 s를 주기로 나눈 나머지만큼만 움직이면 위치와 방향이 모두 같아진다.
	  나머지를 구한 뒤에는 한 칸씩 밀면서 벽에 닿을 때 방향만 뒤집어 주면 된다.

	  ※ N이 1이거나 M이 1이면 주기가 0이 되어 나눗셈에서 문제가 생긴다.
	    이 문제는 격자가 2 이상이라 걸리지 않지만, 조건이 다른 문제에 옮겨 쓸 때는 확인이 필요하다.

	■ "동시에 이동"을 만드는 방법
	  이동 결과를 mold에 바로 쓰면, 이미 옮긴 곰팡이를 다시 만나 두 번 움직이게 된다.
	  그래서 빈 tmpMold에 결과를 쌓고, 전부 옮긴 뒤에 통째로 교체한다.

	  겹침 처리도 이 과정에서 자연스럽게 해결된다.
	  도착 칸에 이미 다른 곰팡이가 있으면 크기를 비교해 큰 쪽만 남기면 된다.

	      if (tmpMold[sr][sc].size < mold[r][c].size) -> 덮어쓴다

	  방향은 이동하면서 반전됐을 수 있으므로, 저장할 때 최종 방향(dir)으로 갱신해야 한다.
	  원래 구조체의 dir을 그대로 두면 다음 턴의 이동이 틀어진다.
*/

#include <stdio.h>

#define MAX (100 + 20)

int T;
int N, M, K;

// 곰팡이 한 마리
struct MOLD
{
	int distance; // 한 턴에 움직이는 칸 수 s
	int dir;      // 이동 방향 d
	int size;     // 크기 b (0이면 그 칸은 비어 있다는 뜻)
};

typedef struct MOLD MOLD;

MOLD mold[MAX][MAX];     // 현재 상태
MOLD tmpMold[MAX][MAX];  // 이동 결과를 쌓는 곳

// 방향 배열. 방향 번호 1~4를 그대로 인덱스로 쓰기 위해 0번은 더미다.
//   1: 위, 2: 아래, 3: 오른쪽, 4: 왼쪽
int dr[] = { 0, -1, 1, 0, 0 };
int dc[] = { 0,  0, 0, 1,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &K);

	// size가 0이면 빈칸이라는 약속이므로 전부 0으로 밀어 둔다
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			mold[r][c].distance = mold[r][c].dir = mold[r][c].size = 0;

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
// 디버그용 출력 (크기만 표시)
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
// sc열에서 가장 위에 있는 곰팡이를 채취
//
//   반환값 : 잡은 곰팡이의 크기. 그 열이 비어 있으면 0.
// ---------------------------
int catchMold(int sc)
{
	// 위에서부터 내려오며 처음 만나는 것이 답이다
	for (int r = 1; r <= N; r++)
	{
		if (mold[r][sc].size != 0)
		{
			int ret = mold[r][sc].size;

			// 잡은 자리는 완전히 비운다
			mold[r][sc].size = 0;
			mold[r][sc].dir = 0;
			mold[r][sc].distance = 0;

			return ret;
		}
	}

	return 0;
}

// ---------------------------
// 남은 곰팡이를 모두 동시에 이동시킨다
// ---------------------------
void moveMold()
{
	// 결과를 쌓을 배열을 비운다
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			tmpMold[r][c].distance = tmpMold[r][c].dir = tmpMold[r][c].size = 0;

	// 방향 반전표 : 1(위) <-> 2(아래), 3(오른쪽) <-> 4(왼쪽)
	int changeDir[5] = { 0, 2, 1, 4, 3 };

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (mold[r][c].size == 0) continue;

			MOLD md = mold[r][c];
			int sr, sc, dir;

			sr = r;
			sc = c;
			dir = md.dir;

			int move = md.distance;

			// 왕복 주기로 나눈 나머지만 움직이면 결과가 같다.
			//   세로 이동 주기 = (N-1)*2, 가로 이동 주기 = (M-1)*2
			if (dir <= 2) move = md.distance % ((N - 1) * 2);
			else          move = md.distance % ((M - 1) * 2);

			// 남은 거리만큼 한 칸씩 이동하며, 벽에 부딪히면 방향을 뒤집는다
			for (int m = 0; m < move; m++)
			{
				int nr = sr + dr[dir];
				int nc = sc + dc[dir];

				// 다음 칸이 격자 밖이면 방향을 반대로 바꿔 다시 계산한다
				if (nr < 1 || nc < 1 || nr > N || nc > M)
				{
					dir = changeDir[dir];

					nr = sr + dr[dir];
					nc = sc + dc[dir];
				}

				sr = nr;
				sc = nc;
			}

			// 도착 칸에 이미 다른 곰팡이가 있으면 더 큰 쪽만 남는다
			if (tmpMold[sr][sc].size < mold[r][c].size)
			{
				tmpMold[sr][sc] = mold[r][c];

				// 이동 중에 뒤집힌 방향을 반드시 반영해야 다음 턴이 맞는다
				tmpMold[sr][sc].dir = dir;
			}
		}
	}

	// 전부 옮긴 뒤 한꺼번에 교체 (= 동시 이동)
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			mold[r][c] = tmpMold[r][c];
}

// ---------------------------
// 인턴이 1열부터 M열까지 이동하며 채취
// ---------------------------
int simulate()
{
	int sum = 0;

	for (int c = 1; c <= M; c++)
	{
		// 채취가 먼저, 이동이 나중이다. 순서를 바꾸면 답이 달라진다.
		sum += catchMold(c);

		moveMold();
	}

	return sum;
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("%d\n", simulate());
	}

	return 0;
}
