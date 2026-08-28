/*
	[코드트리] 2017 상반기 오후 2번 - 방화벽 설치하기
	https://www.codetree.ai/training-field/frequent-problems/problems/firewall-installation

	■ 문제 요약
	  N x M 격자가 주어진다.

	    0 : 빈 칸
	    1 : 벽
	    2 : 불(바이러스)

	  빈 칸 중 정확히 3곳에 벽을 새로 세운 뒤, 불이 상하좌우로 퍼지게 둔다.
	  불이 다 퍼지고 나서 남는 빈 칸(안전 영역)의 최대 개수를 구한다.

	  (백준 14502 "연구소"와 같은 문제다)

	■ 풀이 방침 : 조합 완전탐색 + 확산 시뮬레이션
	  벽을 어디에 세워야 좋은지는 규칙으로 알기 어렵다. 대신 격자가 8 x 8 이하로 작아서
	  "빈 칸 중 3곳을 고르는 모든 조합"을 전부 시도해 볼 수 있다.

	    1) 빈 칸 좌표를 미리 emptyRoom[] 목록으로 모아 둔다
	    2) DFS로 그 목록에서 서로 다른 인덱스 3개를 고른다 (조합)
	    3) 원본을 복사한 tmpMAP에 벽 3개를 세우고 BFS로 불을 퍼뜨린다
	    4) 남은 0의 개수를 세어 최댓값을 갱신한다

	  빈 칸이 최대 64개이므로 조합 수는 64C3 = 41,664가지이고,
	  한 번의 BFS도 64칸짜리라 전부 돌려도 가볍다.

	■ 구현 포인트
	  1) 좌표 (r, c) 두 개를 조합으로 다루면 "중복 없이 3곳"을 만들기가 번거롭다.
	     빈 칸을 1차원 목록으로 펴 두면 그냥 인덱스 조합 문제가 되어 간단해진다.
	     조합이므로 다음 시작값을 i + 1로 넘긴다.
	  2) 불은 여러 군데서 동시에 퍼지므로, 모든 불을 큐에 넣고 시작하는 멀티 소스 BFS를 쓴다.
	     여기서는 퍼지는 데 걸린 시간이 필요 없어 거리 기록 없이 값만 FIRE로 바꾼다.
	  3) 원본 MAP은 절대 건드리지 않고 매번 tmpMAP에 복사해서 실험한다.
	     그래야 다음 조합이 깨끗한 상태에서 시작한다.
	  4) countEmpty의 `sum += !tmpMAP[r][c]` 는 값이 0일 때만 1을 더하는 관용 표현이다.
*/

#include <stdio.h>

#define MAX (8 + 3)

// 칸의 상태
#define EMPTY (0)   // 빈 칸
#define WALL  (1)   // 벽
#define FIRE  (2)   // 불(바이러스)

int T;

int N, M;               // 격자 크기
int MAP[MAX][MAX];      // 원본 (건드리지 않는다)
int tmpMAP[MAX][MAX];   // 조합마다 새로 복사해 쓰는 작업용 격자

int num_of_cases[5];    // 고른 빈 칸 3개의 "인덱스"
int maxAnswer;          // 안전 영역의 최댓값

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];        // BFS용 큐
RC emptyRoom[MAX * MAX];    // 빈 칸 좌표를 1차원으로 펴 놓은 목록
int emptyCount;             // 빈 칸 개수

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%d", &MAP[r][c]);
}

// ---------------------------
// 디버그용: 작업 격자 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", tmpMAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// original -> copy 로 격자 복사
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			copy[r][c] = original[r][c];
}

// ---------------------------
// 디버그용: 현재 고른 인덱스 3개 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < 3; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 빈 칸 좌표를 미리 목록으로 수집
//
// 2차원 좌표를 1차원 인덱스로 바꿔 두면
// 이후 DFS가 "0 ~ emptyCount-1 중 3개 고르기"라는 단순한 조합 문제가 된다.
// ---------------------------
void setEmptyCount()
{
	emptyCount = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] == EMPTY)
			{
				emptyRoom[emptyCount].r = r;
				emptyRoom[emptyCount++].c = c;
			}
		}
	}
}

// ---------------------------
// 불이 다 퍼진 뒤 남은 빈 칸(안전 영역) 개수
// ---------------------------
int countEmpty()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// !x 는 x가 0일 때만 1이 된다 (빈 칸만 세기)
			sum += !tmpMAP[r][c];
		}
	}

	return sum;
}

// ---------------------------
// 현재 고른 3곳에 벽을 세우고 불을 끝까지 퍼뜨린다
// ---------------------------
void BFS()
{
	// 매번 원본에서 새로 시작
	copyMap(tmpMAP, MAP);

	// 1) 고른 인덱스 3곳에 벽 설치
	for (int i = 0; i < 3; i++)
	{
		int wr, wc;
		int index = num_of_cases[i];

		wr = emptyRoom[index].r;
		wc = emptyRoom[index].c;

		tmpMAP[wr][wc] = WALL;
	}

	// 2) 모든 불을 큐에 넣고 시작하는 멀티 소스 BFS
	int rp, wp;
	rp = wp = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (tmpMAP[r][c] == FIRE)
			{
				queue[wp].r = r;
				queue[wp++].c = c;
			}
		}
	}

	// 3) 확산
	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 격자 밖
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// 빈 칸이 아니면(벽이거나 이미 불이면) 퍼질 수 없다.
			// 값을 FIRE로 바꾸는 것이 곧 방문 표시라 visit 배열이 따로 필요 없다.
			if (tmpMAP[nr][nc] != EMPTY)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			tmpMAP[nr][nc] = FIRE;
		}
	}
}

// ---------------------------
// DFS : 빈 칸 목록에서 서로 다른 인덱스 3개를 고른다 (조합)
//
//   depth : 지금까지 고른 개수
//   start : 이번에 고를 수 있는 최소 인덱스
// ---------------------------
void DFS(int depth, int start)
{
	// 3곳을 다 골랐으면 실제로 실험해 본다
	if (depth == 3)
	{
		// printCases();

		BFS();

		int tmp = countEmpty();
		if (maxAnswer < tmp)
			maxAnswer = tmp;

		return;
	}

	for (int i = start; i < emptyCount; i++)
	{
		num_of_cases[depth] = i;

		// 조합이므로 다음은 i보다 뒤에서만 고른다 (같은 조합의 순서 바꾼 중복 제거)
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// 빈 칸 목록을 먼저 만들어 둔다
		setEmptyCount();

		maxAnswer = 0;

		DFS(0, 0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}
