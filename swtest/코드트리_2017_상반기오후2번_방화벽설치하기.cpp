// 코드트리 2017 상반기 오후 2번 - 방화벽 설치
//
// 문제 요약
// 연구소 맵에서
// 0 : 빈 칸
// 1 : 벽
// 2 : 불(또는 바이러스)
// 이 주어진다.
//
// 빈 칸 3곳에 새로 벽을 설치한 뒤,
// 불이 상하좌우로 퍼졌을 때
// 남는 안전 영역(0의 개수)의 최댓값을 구하는 문제이다.
//
// 핵심 아이디어
// 1. 빈 칸들 중 3곳을 고르는 모든 조합을 DFS로 생성한다.
// 2. 각 조합마다 실제로 벽을 세우고 BFS로 불을 퍼뜨린다.
// 3. 퍼진 뒤 남아 있는 빈 칸 수를 세어 최댓값을 갱신한다.
//
// 즉,
// "벽 3개 놓는 조합 탐색(DFS)" + "불 확산 시뮬레이션(BFS)"
// 을 결합한 완전탐색 문제이다.

#include <stdio.h>

#define MAX (8 + 3)

// 칸의 상태 정의
#define EMPTY (0)   // 빈 칸
#define WALL  (1)   // 벽
#define FIRE  (2)   // 불(또는 바이러스)

int T;

int N, M;               // 맵 크기
int MAP[MAX][MAX];      // 원본 맵
int tmpMAP[MAX][MAX];   // 시뮬레이션용 복사 맵

int num_of_cases[5];    // DFS로 고른 "벽을 세울 빈 칸 인덱스 3개"
int maxAnswer;          // 안전 영역 최대값

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];        // BFS용 큐
RC emptyRoom[MAX * MAX];    // 빈 칸 좌표 목록 저장
int emptyCount;             // 빈 칸 개수

// 4방향: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%d", &MAP[r][c]);
}

// ---------------------------
// 디버그용 tmpMAP 출력
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
// 배열 복사 함수
// copy <- original
// ---------------------------
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			copy[r][c] = original[r][c];
}

// ---------------------------
// 디버그용: 현재 벽 선택 조합 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < 3; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 원본 맵에서 빈 칸들을 미리 수집
//
// emptyRoom[i] = i번째 빈 칸의 좌표
// emptyCount   = 빈 칸 총 개수
//
// 이후 DFS에서는 빈 칸 좌표를 직접 탐색하는 대신
// 이 목록의 인덱스를 조합으로 뽑는다.
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
// tmpMAP에서 남아 있는 빈 칸(안전 영역) 개수 세기
// ---------------------------
int countEmpty()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// tmpMAP[r][c] == 0 이면 빈 칸
			sum += !tmpMAP[r][c];
		}
	}

	return sum;
}

// ---------------------------
// 현재 선택된 3개의 벽 배치(num_of_cases[])에 대해
// 실제로 벽을 세우고 BFS로 불을 퍼뜨리는 함수
// ---------------------------
void BFS()
{
	// 원본 맵을 tmpMAP에 복사해서 시뮬레이션 시작
	copyMap(tmpMAP, MAP);

	// ----------------------------------------
	// 1. 선택된 3곳에 벽 설치
	// num_of_cases[i]는 emptyRoom 배열의 인덱스
	// ----------------------------------------
	for (int i = 0; i < 3; i++)
	{
		int wr, wc;
		int index = num_of_cases[i];

		wr = emptyRoom[index].r;
		wc = emptyRoom[index].c;

		tmpMAP[wr][wc] = WALL;
	}

	// ----------------------------------------
	// 2. 현재 tmpMAP에서 모든 FIRE(2)를 큐에 넣는다.
	//    즉, 멀티 소스 BFS 시작
	// ----------------------------------------
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

	// ----------------------------------------
	// 3. BFS로 불 확산
	// ----------------------------------------
	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 범위 밖이면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > M)
				continue;

			// 빈 칸이 아니면(벽/불이면) 확산 불가
			if (tmpMAP[nr][nc] != EMPTY)
				continue;

			// 불 확산
			queue[wp].r = nr;
			queue[wp++].c = nc;

			tmpMAP[nr][nc] = FIRE;
		}
	}
}

// ---------------------------
// DFS로 빈 칸 중 3곳을 고르는 조합 생성
//
// depth : 현재 몇 개의 벽을 골랐는지
// start : 다음에 선택할 수 있는 시작 인덱스
//
// 조합이므로 DFS(depth+1, i+1) 형태를 사용한다.
// ---------------------------
void DFS(int depth, int start)
{
	// 벽 3개를 모두 골랐으면
	// 실제로 BFS 시뮬레이션 수행
	if (depth == 3)
	{
		// printCases();

		BFS();

		int tmp = countEmpty();
		if (maxAnswer < tmp)
			maxAnswer = tmp;

		return;
	}

	// 빈 칸 목록에서 하나씩 고르기
	for (int i = start; i < emptyCount; i++)
	{
		num_of_cases[depth] = i;

		// 다음 벽은 현재보다 뒤에서만 선택
		DFS(depth + 1, i + 1);
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

		// 빈 칸 목록 미리 수집
		setEmptyCount();

		maxAnswer = 0;

		// 벽 3개 조합 탐색 시작
		DFS(0, 0);

		printf("%d\n", maxAnswer);
	}

	return 0;
}