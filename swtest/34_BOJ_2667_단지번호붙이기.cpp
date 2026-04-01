// 백준 2667 단지번호붙이기
// BFS(너비 우선 탐색)으로 각 아파트 단지의 크기를 구하는 코드
// https://www.acmicpc.net/problem/2667
#include <stdio.h>
#include <stdbool.h>

#define MAX (25 + 5)   // 문제의 최대 N은 25, 경계 여유를 주기 위해 +5

int N;                 // 지도의 크기 (N x N)

int MAP[MAX][MAX];     // 아파트 지도 저장 배열 (1: 집, 0: 빈칸)
bool visit[MAX][MAX];  // 방문 여부 체크 배열

int answers[MAX * MAX];
// 각 단지의 집 개수를 저장하는 배열
// 최악의 경우 모든 칸이 따로 떨어져 있으면 단지 수가 N*N까지 가능

// 좌표를 저장하기 위한 구조체
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

RC queue[MAX * MAX];
// BFS용 큐
// 모든 칸이 한 번씩 들어갈 수 있으므로 최대 크기는 N*N 정도면 충분

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 디버그용: visit 배열 출력
// ---------------------------
void printVisit()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d", &N);   // 지도의 크기 입력

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%1d", &MAP[r][c]);
			// %1d를 사용하면 붙어 있는 숫자를 한 자리씩 입력받을 수 있음
			// 예: 0110100 같은 문자열을 숫자 하나씩 읽어옴
		}
	}
}

// ---------------------------
// BFS 함수
// 시작 좌표 (r, c)가 속한 단지의 크기를 반환
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;
	// rp: read pointer  -> 큐에서 꺼낼 위치
	// wp: write pointer -> 큐에 넣을 위치
	// BFS가 끝난 뒤 wp 값이 곧 단지의 크기(방문한 집 수)가 됨

	rp = wp = 0;  // 큐 초기화

	// 시작 좌표를 큐에 삽입
	queue[wp].r = r;
	queue[wp++].c = c;

	// 시작 좌표 방문 처리
	visit[r][c] = true;

	// 큐가 빌 때까지 반복
	while (rp < wp)
	{
		// 큐에서 현재 좌표 하나 꺼냄
		RC out = queue[rp++];

		// 현재 좌표에서 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;  // next row, next col

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 집이 아니거나(0), 이미 방문했다면 건너뜀
			if (MAP[nr][nc] == 0 || visit[nr][nc] == true)
				continue;

			// 방문 가능한 집이면 큐에 추가
			queue[wp].r = nr;
			queue[wp++].c = nc;

			// 방문 처리
			visit[nr][nc] = true;
		}
	}

	// 큐에 들어간 총 원소 수 = 이 단지의 집 개수
	return wp;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	input();   // 지도 입력

	int ansCount = 0;   // 총 단지 수

	// 모든 칸을 순회하면서
	// 아직 방문하지 않은 집(1)을 만나면 BFS 시작
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 1 && visit[r][c] == false)
			{
				answers[ansCount++] = BFS(r, c);
				// BFS가 반환한 단지 크기를 answers 배열에 저장
				// ansCount를 증가시켜 총 단지 수 기록
			}
		}
	}

	// 총 단지 수 출력
	printf("%d\n", ansCount);

	// 단지 크기를 오름차순 정렬
	for (int i = 0; i < ansCount - 1; i++)
	{
		for (int k = i + 1; k < ansCount; k++)
		{
			if (answers[i] > answers[k])
			{
				int tmp = answers[i];
				answers[i] = answers[k];
				answers[k] = tmp;
			}
		}
	}

	// 정렬된 단지 크기 출력
	for (int i = 0; i < ansCount; i++)
		printf("%d\n", answers[i]);

	return 0;
}