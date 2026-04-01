// BOJ 2194 - 유닛 이동시키기
// https://www.acmicpc.net/problem/2194
// 문제 요약
// A x B 크기의 직사각형 유닛을
// 격자 안에서 상, 하, 좌, 우로 한 칸씩 이동시켜
// 시작 위치 (sr, sc) 에서 도착 위치 (er, ec) 까지의
// 최소 이동 횟수를 구하는 문제이다.
//
// 핵심 아이디어
// - BFS를 사용하면 최소 이동 횟수를 구할 수 있다.
// - BFS의 상태는 "유닛 전체"가 아니라
//   유닛의 "왼쪽 위 좌표 (r, c)" 하나로 표현한다.
// - 어떤 좌표 (r, c)에 유닛을 놓을 수 있으려면
//   1) 유닛 전체가 맵 안에 들어가야 하고
//   2) A x B 영역 안에 장애물이 하나도 없어야 한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (500 + 50)

int N, M;              // N: 행 크기, M: 열 크기
int A, B;              // 유닛의 세로 크기 A, 가로 크기 B
int K;                 // 장애물 개수
int sr, sc, er, ec;    // 시작 좌표, 도착 좌표 (유닛의 왼쪽 위 기준)

int MAP[MAX][MAX];     // 맵 정보 (1: 장애물, 0: 빈칸)
int visit[MAX][MAX];   // 방문 체크 + 이동 횟수 저장

// 좌표를 저장할 구조체
struct RC
{
	int r;   // row
	int c;   // col
};

typedef struct RC RC;

RC queue[MAX * MAX];   // BFS용 큐

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d %d %d", &N, &M, &A, &B, &K);

	// 장애물 위치 입력
	for (int k = 0; k < K; k++)
	{
		int r, c;

		scanf("%d %d", &r, &c);

		// 장애물이 있는 칸은 1로 표시
		MAP[r][c] = 1;
	}

	// 시작 좌표와 도착 좌표 입력
	// 모두 "유닛의 왼쪽 위 좌표" 기준
	scanf("%d %d %d %d", &sr, &sc, &er, &ec);
}

// ---------------------------
// 디버그용 맵 출력 함수
// ---------------------------
void printMap(int map[MAX][MAX])
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
// 현재 유닛의 왼쪽 위 좌표가 (sr, sc)일 때
// A x B 직사각형 내부에 장애물이 없는지 검사
//
// 즉, 유닛이 차지하는 모든 칸을 확인해서
// 하나라도 장애물이 있으면 false
// 모두 비어 있으면 true 반환
// ---------------------------
bool isEmpty(int sr, int sc)
{
	for (int r = sr; r <= sr + A - 1; r++)
	{
		for (int c = sc; c <= sc + B - 1; c++)
		{
			if (MAP[r][c] == 1)
				return false;
		}
	}

	return true;
}

// ---------------------------
// BFS 함수
// 시작 좌표 (r, c)에서 도착 좌표까지의
// 최소 이동 횟수를 반환
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치

	rp = wp = 0;

	// 시작 위치 방문 처리
	// 시작점을 1로 두고,
	// 이후 한 칸 이동할 때마다 +1 해서 거리 저장
	visit[r][c] = 1;

	// 시작 위치를 큐에 삽입
	queue[wp].r = r;
	queue[wp++].c = c;

	// 큐가 빌 때까지 BFS 수행
	while (rp < wp)
	{
		// 현재 위치 하나 꺼냄
		RC out = queue[rp++];

		// 목표 위치에 도착하면
		// 시작점을 1로 뒀으므로 실제 이동 횟수는 -1
		if (out.r == er && out.c == ec)
			return visit[out.r][out.c] - 1;

		// 4방향(상, 우, 하, 좌) 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			// 다음에 이동할 유닛의 왼쪽 위 좌표
			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// ---------------------------
			// 1. 유닛 전체가 맵 안에 들어가야 함
		 //
			// 유닛의 왼쪽 위가 (nr, nc)이면
			// 오른쪽 아래 끝 좌표는
			// (nr + A - 1, nc + B - 1)
		 //
			// 이 범위가 맵을 벗어나면 이동 불가
			// ---------------------------
			if (nr < 1 || nc < 1 || (nr + A - 1) > N || (nc + B - 1) > M)
				continue;

			// ---------------------------
			// 2. 해당 위치에 유닛을 놓았을 때
			//    내부에 장애물이 있으면 이동 불가
			//
			// 3. 이미 방문한 위치면 중복 탐색 방지
			// ---------------------------
			if (isEmpty(nr, nc) == false || visit[nr][nc] != 0)
				continue;

			// 이동 가능한 위치이면 큐에 넣음
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 현재 위치까지의 거리 + 1
			visit[nr][nc] = visit[out.r][out.c] + 1;
		}
	}

	// 도착할 수 없는 경우
	return -1;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();   // 입력 받기

	// 시작 위치에서 도착 위치까지의 최소 이동 횟수 출력
	printf("%d\n", BFS(sr, sc));

	return 0;
}