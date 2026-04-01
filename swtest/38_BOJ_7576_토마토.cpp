// BOJ 7576 - 토마토
// https://www.acmicpc.net/problem/7576
// 문제 요약
// 상자 안의 토마토들이 있고,
// 하루가 지나면 익은 토마토의 상하좌우에 있는 안 익은 토마토가 익는다.
//
// 값의 의미
//  1 : 처음부터 익은 토마토
//  0 : 아직 익지 않은 토마토
// -1 : 빈 칸
//
// 목표
// 모든 토마토가 다 익는 데 걸리는 최소 날짜를 구한다.
// 만약 끝까지 익지 못하는 토마토가 있으면 -1을 출력한다.
//
// 핵심 아이디어
// - 처음부터 익어 있는 토마토가 여러 개일 수 있으므로
//   이들을 모두 시작점으로 넣는 "멀티 소스 BFS"를 사용한다.
// - BFS가 진행되면서
//   새로 익는 칸에 "현재 칸 값 + 1"을 저장하여
//   며칠째에 익었는지를 MAP에 직접 기록한다.

#include <stdio.h>

#define MAX (1000 + 100)

int M, N;               // M: 열 개수, N: 행 개수
int MAP[MAX][MAX];      // 토마토 상자 상태

// 좌표를 저장하는 구조체
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

RC queue[MAX * MAX];    // BFS용 큐

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &M, &N);

	// 바깥 테두리를 -1로 채워 둔다.
	// 이렇게 하면 별도의 범위 검사 없이
	// 바깥을 "빈 칸(막힌 칸)"처럼 처리할 수 있다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
		{
			MAP[r][c] = -1;
		}
	}

	// 실제 토마토 상자 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 디버그용 출력 함수
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// BFS 함수
// ---------------------------
// 처음부터 익어 있는 모든 토마토를 큐에 넣고 시작하는
// 멀티 소스 BFS
void BFS()
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 처음부터 익은 토마토(값이 1인 칸)를
	// 전부 큐에 넣는다.
	// 즉, 여러 시작점에서 동시에 BFS를 시작하는 형태
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] == 1)
			{
				queue[wp].r = r;
				queue[wp++].c = c;
			}
		}
	}

	// 큐가 빌 때까지 BFS 진행
	while (rp < wp)
	{
		// 현재 익은 토마토 하나 꺼냄
		RC out = queue[rp++];

		// 상하좌우 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 아직 안 익은 토마토(0)에게만 익음 전파 가능
			// -1(빈 칸), 1 이상(이미 익은 칸)은 건너뜀
			if (MAP[nr][nc] != 0)
				continue;

			// 새로 익는 토마토를 큐에 넣음
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 현재 칸이 x일 때, 다음 칸은 x+1일째 익음
			// 즉 날짜 정보를 MAP에 직접 저장
			MAP[nr][nc] = MAP[out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 정답 계산 함수
// ---------------------------
int getAnswer()
{
	int maxAnswer = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 끝까지 0이 남아 있으면
			// 익지 못한 토마토가 있다는 뜻
			if (MAP[r][c] == 0)
				return -1;

			// 가장 늦게 익은 날짜 찾기
			if (maxAnswer < MAP[r][c])
				maxAnswer = MAP[r][c];
		}
	}

	// 처음부터 익은 토마토 값이 1이므로
	// 실제 걸린 날짜는 최대값 - 1
	return maxAnswer - 1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();   // 입력 받기

	BFS();     // 토마토 익히기

	printf("%d\n", getAnswer());   // 결과 출력

	return 0;
}