/*
	[프로그래머스] 2025 프로그래머스 코드챌린지 1차 예선 - 지게차와 크레인
	https://school.programmers.co.kr/learn/courses/30/lessons/388353

	■ 문제 요약
	  n x m 창고(2 <= n, m <= 50)에 알파벳 대문자 컨테이너가 꽉 차 있다.
	  출고 요청이 순서대로 온다 (최대 100개).
	    - "A"  (한 글자) : 지게차. 창고 바깥과 이어진 A만 꺼낸다.
	                      (4면 중 하나라도 바깥과 연결된 빈 칸에 닿아 있으면 접근 가능)
	    - "AA" (두 글자) : 크레인. A를 전부 꺼낸다.
	  모든 요청 뒤 남은 컨테이너 수를 구한다.

	  입출력 예
	      ["AZWQY","CAABX","BBDDA","ACACA"], ["A","BB","A"] -> 11
	      ["HAH","HBH","HHH","HAH","HBH"], ["C","B","B","B","B","H"] -> 4

	■ 풀이 방침 : 테두리를 빈 칸으로 두르고, 지게차는 바깥에서 BFS
	  창고를 1 ~ N, 1 ~ M 에 놓고 바깥 한 줄(0행, N+1행, 0열, M+1열)을 빈 칸(0)으로 둔다.
	  그러면 "창고 바깥" = (0, 0)에서 빈 칸만 밟고 갈 수 있는 곳이 된다.

	  지게차 (forklift)
	    1) (0, 0)에서 빈 칸만 따라 BFS
	    2) BFS 중에 만난 이웃이 요청한 종류면 "꺼낼 목록"에 표시
	    3) BFS가 끝난 뒤 표시한 칸을 한꺼번에 비운다
	  크레인 (crane)
	    창고 전체를 훑어 요청한 종류를 전부 비운다.

	■ 지게차는 "다 찾고 나서 한꺼번에" 비워야 한다
	  BFS 도중에 바로 비우면, 그 칸이 빈 칸이 되어 BFS가 그 안으로 들어가 버린다.
	  그러면 이번 요청 순간에는 접근할 수 없던 안쪽 컨테이너까지 꺼내게 된다.
	  (예제 1의 세 번째 "A"에서 2행 3열 A는 이번에는 꺼내지면 안 된다)
	  그래서 pick[][]에 표시만 해 두고 BFS가 끝난 뒤에 비운다.

	■ 주의할 점
	  - 크레인으로 꺼낸 자리도 빈 칸이다. 그 빈 칸이 바깥과 이어지면 다음 지게차 BFS가 들어갈 수 있다.
	    반대로 안쪽에 갇힌 빈 칸은 바깥과 이어지지 않으면 BFS가 닿지 않는다. (빈 칸이라고 다 바깥이 아니다)
	  - BFS 큐는 테두리 포함 (N+2) x (M+2) 칸까지 들어갈 수 있다.
	  - visit, pick은 요청마다 비운다.
*/
#include <stdio.h>

#define MAX (50+5)
#define EMPTY (0)

int T;

int N, M;                 // 창고 세로, 가로
char MAP[MAX][MAX];       // 컨테이너 종류 ('A' ~ 'Z'), 빈 칸은 0. 테두리 포함

int Q;                    // 요청 수
char REQ[100 + 5][3];     // 요청 문자열 ("A" 또는 "AA")

int visit[MAX][MAX];      // 지게차 BFS 방문
int pick[MAX][MAX];       // 지게차가 이번에 꺼낼 칸 (remove는 stdio.h 함수 이름이라 피했다)

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];

// 상, 하, 좌, 우
int dr[] = { -1,1,0,0 };
int dc[] = { 0,0,-1,1 };

void input()
{
	scanf("%d %d", &N, &M);

	// 테두리까지 전부 빈 칸으로
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= M + 1; c++)
			MAP[r][c] = EMPTY;

	for (int r = 1; r <= N; r++)
	{
		char line[MAX];
		scanf("%s", line);

		for (int c = 1; c <= M; c++)
			MAP[r][c] = line[c - 1];
	}

	scanf("%d", &Q);
	for (int i = 0; i < Q; i++)
		scanf("%s", REQ[i]);
}

// 지게차 : 바깥과 이어진 target 컨테이너를 꺼낸다
void forklift(char target)
{
	int rp, wp;

	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= M + 1; c++)
			visit[r][c] = pick[r][c] = 0;

	rp = wp = 0;

	// 바깥 (0, 0)에서 시작
	queue[wp].r = 0;
	queue[wp++].c = 0;
	visit[0][0] = 1;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr < 0 || nc < 0 || nr > N + 1 || nc > M + 1) continue;
			if (visit[nr][nc] == 1) continue;

			if (MAP[nr][nc] == EMPTY)            // 빈 칸이면 계속 퍼진다
			{
				visit[nr][nc] = 1;
				queue[wp].r = nr;
				queue[wp++].c = nc;
			}
			else if (MAP[nr][nc] == target)      // 바깥에 닿은 target -> 꺼낼 목록에 표시만
			{
				pick[nr][nc] = 1;
			}
		}
	}

	// BFS가 끝난 뒤 한꺼번에 꺼낸다
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (pick[r][c] == 1) MAP[r][c] = EMPTY;
}

// 크레인 : target 컨테이너를 전부 꺼낸다
void crane(char target)
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (MAP[r][c] == target) MAP[r][c] = EMPTY;
}

// 요청을 순서대로 처리하고 남은 컨테이너 수를 돌려준다
int simulate()
{
	for (int i = 0; i < Q; i++)
	{
		if (REQ[i][1] == '\0') forklift(REQ[i][0]);   // 한 글자 -> 지게차
		else crane(REQ[i][0]);                        // 두 글자 -> 크레인
	}

	int count = 0;
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (MAP[r][c] != EMPTY) count++;

	return count;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("%d\n", simulate());
	}

	return 0;
}
