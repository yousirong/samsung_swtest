/*
	[코드트리] 2021 상반기 오후 1번 - 나무 타이쿤
	https://www.codetree.ai/training-field/frequent-problems/problems/tree-tycoon

	■ 문제 요약
	  N x N 격자에 나무가 심겨 있고(MAP[r][c] = 그루 수), 격자의 "왼쪽 아래 2x2" 칸
	  (N,1) (N,2) (N-1,1) (N-1,2) 네 곳에 영양제가 놓인 상태로 시작한다.
	  M번의 사이클을 도는데, 각 사이클마다 방향 d와 거리 p가 주어지고 아래 4단계를 수행한다.

	    1) 이동 : 모든 영양제가 방향 d로 p칸 이동한다.
	              격자를 벗어나면 반대편에서 다시 들어온다(토러스 = 상하좌우가 이어진 격자).
	    2) 성장 : 영양제가 놓인 칸의 나무가 1그루 성장한다.
	    3) 번식 : (2)의 성장이 전부 끝난 뒤, 영양제가 놓인 각 칸을 기준으로
	              "대각선 4방향" 인접 칸 중 나무가 1그루 이상 있는 칸의 개수만큼 추가로 성장한다.
	    4) 재배치 : 영양제가 놓여 있던 칸을 제외하고, 나무가 2그루 이상인 모든 칸에서
	              나무를 2그루 베어내고 그 자리에 새 영양제를 놓는다(기존 영양제는 사라진다).

	  M번의 사이클이 끝난 뒤 격자에 남은 나무 그루 수의 총합을 출력한다.

	■ 방향 번호(문제 정의) : 1 →, 2 ↗, 3 ↑, 4 ↖, 5 ←, 6 ↙, 7 ↓, 8 ↘
	  아래 dr8/dc8은 인덱스를 그대로 쓰기 위해 0번을 더미로 비워 둔다.

	■ 풀이 방침
	  격자 값(MAP)과 영양제 좌표 목록(supplement)만 들고 시뮬레이션을 그대로 옮긴다.
	  4단계가 "영양제 칸 = 이번 턴에 영양제가 있던 칸"을 제외한다는 점,
	  3단계가 2단계 완료 후에 일괄 판정된다는 점이 핵심 함정이다.
*/

#include <stdio.h>

#define MAX_N (50 + 5)		// N <= 50, 격자 바깥 테두리(0행/0열, N+1행/N+1열)까지 여유
#define MAX_M (100 + 10)	// M <= 100

int T;					// 테스트케이스 수 (이 문제는 단일 케이스라 1로 고정)

int N, M;				// 격자 크기, 사이클 수
int MAP[MAX_N][MAX_N];	// MAP[r][c] = (r, c)에 심긴 나무 그루 수, 1-based
int D[MAX_M];			// m번째 사이클의 이동 방향(1~8)
int P[MAX_M];			// m번째 사이클의 이동 거리

struct RC				// 좌표 한 쌍
{
	int r;
	int c;
};

// 현재 영양제가 놓인 칸들의 목록. 최대 N*N개까지 늘어날 수 있다.
RC supplement[MAX_N * MAX_N];
int scnt;				// 현재 영양제 개수

// 3단계 "번식" 판정에 쓰는 대각선 4방향 : ↖, ↗, ↘, ↙
int dr4[] = { -1, -1, 1, 1 };
int dc4[] = { -1, 1, 1, -1 };

// 1단계 "이동"에 쓰는 8방향. 0번 인덱스는 더미(문제의 방향 번호 1~8을 그대로 쓰기 위함)
// -, →, ↗, ↑, ↖, ←, ↙, ↓, ↘
int dr8[] = { 0, 0, -1, -1, -1, 0, 1, 1, 1 };
int dc8[] = { 0, 1, 1, 0, -1, -1, -1, 0, 1 };

void input()
{
	scanf("%d %d", &N, &M);

	// 테두리(0행/0열, N+1행/N+1열)까지 0으로 초기화.
	// 이렇게 해두면 grow()에서 대각선 이웃을 볼 때 범위 검사를 생략할 수 있다.
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = 0;

	// 초기 나무 배치
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	// 각 사이클의 (방향, 거리)
	for (int m = 0; m < M; m++)
		scanf("%d %d", &D[m], &P[m]);

	// 초기 영양제 위치 : 격자의 왼쪽 아래 2x2 영역
	//   (N-1, 1) (N-1, 2)
	//   (N  , 1) (N  , 2)
	supplement[0].r = N; supplement[0].c = 1;
	supplement[1].r = N; supplement[1].c = 2;
	supplement[2].r = N - 1; supplement[2].c = 1;
	supplement[3].r = N - 1; supplement[3].c = 2;
	scnt = 4;
}

void printMap() // for debug : 현재 격자 상태를 눈으로 확인할 때 사용
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// [1단계] 모든 영양제를 방향 dir로 size칸 이동시킨다. 격자는 상하좌우가 이어져 있다.
void moveSupplement(int dir, int size)
{
	for (int i = 0; i < scnt; i++)
	{
		int nr, nc;

		// size가 N보다 클 수 있으므로 size % N으로 줄인다.
		// 한 바퀴(N칸) 돌면 제자리이므로 결과가 같다. 실제 이동량은 0 ~ N-1.
		nr = supplement[i].r + dr8[dir] * (size % N);
		nc = supplement[i].c + dc8[dir] * (size % N);

		// 이동 전 좌표가 [1, N]이고 변화량이 [-(N-1), N-1]이므로
		// 결과는 [2-N, 2N-1] 범위. 따라서 아래처럼 "한 번만" 보정해도 항상 [1, N]에 들어온다.
		// (while이 필요 없는 이유가 여기에 있다. size % N을 빼먹으면 이 보정이 깨진다.)
		if (nr > N) nr -= N;
		if (nc > N) nc -= N;
		if (nr < 1) nr += N;
		if (nc < 1) nc += N;

		supplement[i].r = nr;
		supplement[i].c = nc;
	}
}

// [2단계] 영양제가 놓인 칸의 나무를 1그루씩 성장시킨다.
void insert()
{
	for (int i = 0; i < scnt; i++)
	{
		int r, c;

		r = supplement[i].r;
		c = supplement[i].c;

		MAP[r][c]++;
	}
}

// [3단계] 영양제 칸마다 대각선 4방향 이웃 중 나무가 있는 칸 수만큼 추가 성장시킨다.
void grow()
{
	for (int s = 0; s < scnt; s++)
	{
		int r, c;

		r = supplement[s].r;
		c = supplement[s].c;

		int count = 0;
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = r + dr4[i];
			nc = c + dc4[i];

			// 테두리를 0으로 채워 뒀으므로 격자 밖(0행/0열, N+1행/N+1열)은 자동으로 "나무 없음" 처리된다.
			// !!x 는 x가 0이면 0, 0이 아니면 1 → "나무가 1그루 이상인가"를 0/1로 바꾸는 관용 표현.
			// if (MAP[nr][nc] > 0) count++;
			count += !!MAP[nr][nc];
		}

		// 값을 곧바로 더해도(in-place) 문제가 없는 이유:
		// grow()가 값을 바꾸는 칸은 "영양제 칸"뿐인데, 그 칸들은 insert()에서 이미 +1 되어
		// 전부 1그루 이상인 상태다. 즉 0 -> 양수로 바뀌는 칸이 없으므로
		// 뒤에 처리되는 영양제의 "나무가 있는가" 판정 결과가 달라지지 않는다.
		MAP[r][c] += count;
	}
}

// (r, c)가 이번 턴에 영양제가 놓여 있던 칸인지 확인
bool check(int r, int c)
{
	for (int i = 0; i < scnt; i++)
		if (supplement[i].r == r && supplement[i].c == c)
			return true;

	return false;
}

// [4단계] 영양제 칸을 제외하고 나무가 2그루 이상인 칸에서 2그루를 베어내고,
//         그 칸들을 다음 턴의 영양제 위치로 삼는다.
void makeSupplement()
{
	// 새 영양제 목록을 임시 배열에 모은다.
	// supplement를 곧바로 덮어쓰면 아래 check()가 참조하는 "현재 영양제 목록"이 망가진다.
	RC tmpSupplement[MAX_N * MAX_N] = { 0 };

	int tcnt = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] < 2) continue;	// 나무가 2그루 미만이면 벨 수 없다

			// 이번 턴에 특수 영양제를 투입한 좌표는 제외한다
			if (check(r, c) == true) continue;

			MAP[r][c] -= 2;					// 나무 2그루를 베어내고

			tmpSupplement[tcnt].r = r;		// 그 자리에 새 영양제를 놓는다
			tmpSupplement[tcnt++].c = c;
		}
	}

	// 임시 목록을 실제 영양제 목록으로 교체(기존 영양제는 모두 사라진다)
	for (int i = 0; i < tcnt; i++)
		supplement[i] = tmpSupplement[i];

	scnt = tcnt;
}

// M번의 사이클을 순서대로 수행한다. 각 단계의 순서가 곧 문제 규칙이다.
void simulate()
{
	for (int m = 0; m < M; m++)
	{
		moveSupplement(D[m], P[m]);	// 1) 이동
		insert();					// 2) 성장(+1)
		grow();						// 3) 대각선 이웃 수만큼 추가 성장
		makeSupplement();			// 4) 벌목 후 영양제 재배치
	}
}

// 격자에 남은 나무 그루 수의 총합
int getAnswer()
{
	int sum = 0;
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			sum += MAP[r][c];

	return sum;
}

int main()
{
	// 이 문제는 테스트케이스가 하나뿐이다.
	// scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();

		printf("%d\n", getAnswer());
	}

	return 0;
}
