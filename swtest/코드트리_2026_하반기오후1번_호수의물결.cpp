/*
	[코드트리] 2026 하반기 오후 1번 - 호수의 물결
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/wave-of-lake/description

	■ 문제 요약
	  n x m 호수(5 ~ 30). 0행이 건너편, n-1행이 투석기가 있는 이쪽 기슭.
	  투석기 k개(최대 50). i번은 c_i열에서 시각 s_i부터 d_i 간격으로 돌을 p_i개 날린다.
	  돌 무게 a_i, n - h_i 행에 이르면 물에 떨어진다.
	  매 턴(시각 t = 1, 2, ...)
	    1) 날고 있는 돌이 모두 한 칸 위(건너편)로
	    2) 이번 시각에 날릴 돌을 (n-1, c_i)에 놓는다
	    3) 떨어질 행에 이른 돌은 떨어진다. 칸마다 물결 세기 += max(0, 3a - 5 x 맨해튼 거리)
	    4) 점수 = 모든 칸의 max(0, 물결 세기 - 혼탁도) 합 (떨어진 돌이 없으면 0)
	    5) 흙탕물 가라앉기 : 네 이웃이 모두 격자 안 + 흙탕물이면 -1, 아니면 -2 (0 아래로는 안 감, 동시에)
	    6) 물살 : 혼탁도가 모두 왼쪽으로 한 칸. 0열은 밖으로, m-1열은 0
	    7) 흙탕물 일기 : 물결 세기 > 0 인 칸을 상하좌우로 묶은 덩어리마다
	       (세기 합 / 칸 수, 내림)을 그 덩어리 모든 칸의 혼탁도에 더한다
	  t = 50 이거나, 날릴 돌도 날고 있는 돌도 없으면 그 턴을 마치고 끝.
	  테스트 케이스마다 "#번호 턴1점수 턴2점수 ..." 를 출력한다.

	  입출력 예
	      예제 1 -> #1 0 0 34 0 18

	■ 풀이 방침 : 7단계를 순서대로 그대로 구현한다
	  - 돌은 목록(stone[])으로 들고 다닌다. 행이 떨어질 행(targetRow)과 같아지면 떨어뜨린다.
	  - 물결 세기 wave[][]는 턴마다 새로 0에서 시작한다.
	  - 혼탁도 murk[][]는 턴을 넘어 남는다.

	■ 시간 맞추기
	  1단계(이동)가 2단계(발사)보다 먼저라, 이번 턴에 날린 돌은 이번 턴에 움직이지 않는다.
	  3단계(낙하)는 발사 뒤라, 거리 1인 돌은 날린 턴에 바로 떨어진다.
	  -> 시각 L에 날린 거리 h의 돌은 시각 L + h - 1에 떨어진다 (문제 설명과 같다).

	■ 5단계와 6단계는 "동시에" 바뀐다 -> 새 배열에 계산한 뒤 복사
	  가라앉기는 줄어들기 전 상태로 이웃을 봐야 하므로 next[][]에 계산하고 한꺼번에 옮긴다.
	  물살도 next[r][c] = murk[r][c + 1] 로 만든 뒤 옮긴다.

	■ 7단계 덩어리 = BFS (flood fill)
	  wave > 0 인 칸에서 BFS로 상하좌우 이어진 칸을 모아 합과 칸 수를 구하고, 평균을 다시 그 칸들에 더한다.
	  대각선만 닿은 칸은 다른 덩어리다.

	■ 주의할 점
	  - 4단계에서 쓰는 혼탁도는 "직전 턴이 끝났을 때" 값이다. 그래서 점수는 5 ~ 7단계 전에 계산한다.
	  - 7단계는 6단계(물살) 뒤라, 이번 턴 흙탕물은 흘러가지 않은 자리에 바로 더해진다.
	  - 떨어진 돌이 없으면 wave가 전부 0이라 점수도 0, 7단계도 아무 일이 없다.
	  - 같은 열에 투석기가 여러 개일 수 있다. 돌끼리는 서로 막지 않는다 (같은 칸에 있어도 된다).
	  - 대회는 최대 50턴이라 돌이 남아 있어도 50턴에서 끝난다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (30+5)                       // n, m 최대 30 + 여유
#define MAX_K (50+5)                     // 투석기 최대 50 + 여유
#define MAX_STONE (500+20)   // 투석기 50개 x 돌 10개
#define LAST_TURN (50)                   // 최대 턴

int T;                                   // 테스트 케이스 수
int N, M, K;                             // 행 수, 열 수, 투석기 수

struct CATAPULT                          // 투석기 정보
{
	int c, s, d, a, h, p;   // 열, 시작 시각, 간격, 무게, 거리, 돌 수
};

CATAPULT cat[MAX_K];                     // 투석기 목록

struct STONE                             // 돌 정보
{
	int r, c, a;            // 지금 위치, 무게
	int targetRow;          // 떨어질 행 (n - h)
	int alive;              // 1 = 날고 있음
};

STONE stone[MAX_STONE];                  // 돌 목록
int scnt;                                // 지금까지 날린 돌 수

int wave[MAX][MAX];         // 이번 턴 물결 세기
int murk[MAX][MAX];         // 혼탁도 (턴을 넘어 남는다)

int score[LAST_TURN + 5];                // 턴별 점수
int turnCount;                           // 진행한 턴 수

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐
int visit[MAX][MAX];                     // BFS 방문 표시

int dr[] = { -1,1,0,0 };                 // 상하좌우 행 변화량
int dc[] = { 0,0,-1,1 };                 // 상하좌우 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 호수 크기, 투석기 수 입력

	for (int i = 0; i < K; i++)          // 투석기별 정보 입력
		scanf("%d %d %d %d %d %d", &cat[i].c, &cat[i].s, &cat[i].d, &cat[i].a, &cat[i].h, &cat[i].p);

	for (int r = 0; r < N; r++)          // 혼탁도 초기화
		for (int c = 0; c < M; c++)
			murk[r][c] = 0;

	scnt = 0;                            // 돌 목록 초기화
	turnCount = 0;                       // 턴 수 초기화
}

int abs(int x)                           // 절댓값
{
	return x > 0 ? x : -x;
}

// 1단계 : 날고 있는 돌을 한 칸 위로
void moveStones()
{
	for (int i = 0; i < scnt; i++)
		if (stone[i].alive) stone[i].r--;   // 한 칸 위로
}

// 2단계 : 이번 시각에 날릴 돌을 이쪽 기슭에 놓는다
void launchStones(int t)
{
	for (int i = 0; i < K; i++)          // 투석기별
	{
		if (t < cat[i].s) continue;                  // 아직 시작 전
		if ((t - cat[i].s) % cat[i].d != 0) continue;   // 발사 간격이 아님
		if ((t - cat[i].s) / cat[i].d >= cat[i].p) continue;   // 이미 p개 다 날렸다

		stone[scnt].r = N - 1;           // 이쪽 기슭에서 시작
		stone[scnt].c = cat[i].c;        // 투석기 열
		stone[scnt].a = cat[i].a;        // 무게
		stone[scnt].targetRow = N - cat[i].h;   // 떨어질 행
		stone[scnt++].alive = 1;         // 날고 있음
	}
}

// 3단계 : 떨어질 행에 이른 돌을 떨어뜨리고 물결 세기를 더한다. 떨어진 돌 수를 돌려준다
int dropStones()
{
	int dropCount = 0;                   // 떨어진 돌 수

	for (int r = 0; r < N; r++)          // 물결 세기 초기화
		for (int c = 0; c < M; c++)
			wave[r][c] = 0;

	for (int i = 0; i < scnt; i++)       // 돌별
	{
		if (stone[i].alive == 0 || stone[i].r != stone[i].targetRow) continue;   // 떨어질 돌만

		stone[i].alive = 0;              // 떨어짐
		dropCount++;                     // 개수 증가

		for (int y = 0; y < N; y++)      // 모든 칸에
		{
			for (int x = 0; x < M; x++)
			{
				int power = 3 * stone[i].a - 5 * (abs(y - stone[i].r) + abs(x - stone[i].c));   // 3a - 5 x 거리
				if (power > 0) wave[y][x] += power;   // 양수만 더하기
			}
		}
	}

	return dropCount;                    // 반환
}

// 4단계 : 점수 (혼탁도는 직전 턴이 끝났을 때 값)
int getScore()
{
	int sum = 0;                         // 점수

	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < M; c++)
		{
			int v = wave[r][c] - murk[r][c];   // 물결 - 혼탁도
			if (v > 0) sum += v;         // 양수만 더하기
		}
	}
	return sum;                          // 반환
}

// 5단계 : 흙탕물 가라앉기 (줄어들기 전 상태로 이웃을 보므로 새 배열에 계산)
void settle()
{
	int next[MAX][MAX];                  // 새 혼탁도

	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < M; c++)
		{
			next[r][c] = murk[r][c];     // 기본은 그대로
			if (murk[r][c] == 0) continue;   // 흙탕물 없으면 제외

			int surrounded = 1;   // 네 이웃이 모두 격자 안 + 흙탕물
			for (int i = 0; i < 4; i++)
			{
				int nr = r + dr[i];      // 이웃 행
				int nc = c + dc[i];      // 이웃 열

				if (nr < 0 || nc < 0 || nr >= N || nc >= M || murk[nr][nc] == 0)   // 밖이거나 맑은 물
				{
					surrounded = 0;      // 둘러싸이지 않음
					break;
				}
			}

			next[r][c] -= surrounded ? 1 : 2;   // 둘러싸이면 -1, 아니면 -2
			if (next[r][c] < 0) next[r][c] = 0; // 0 아래로는 안 감
		}
	}

	for (int r = 0; r < N; r++)          // 한꺼번에 반영
		for (int c = 0; c < M; c++)
			murk[r][c] = next[r][c];
}

// 6단계 : 물살 (모두 왼쪽으로 한 칸)
void flow()
{
	for (int r = 0; r < N; r++)          // 행별
	{
		for (int c = 0; c < M - 1; c++)
			murk[r][c] = murk[r][c + 1];   // 왼쪽부터 덮어쓰면 아직 안 옮긴 오른쪽 값을 그대로 가져온다
		murk[r][M - 1] = 0;              // 마지막 열은 0
	}
}

// 7단계 : 물결 덩어리마다 평균을 혼탁도에 더한다
void stirUp()
{
	for (int r = 0; r < N; r++)          // 방문 초기화
		for (int c = 0; c < M; c++)
			visit[r][c] = 0;

	for (int sr = 0; sr < N; sr++)       // 모든 칸
	{
		for (int sc = 0; sc < M; sc++)
		{
			if (wave[sr][sc] <= 0 || visit[sr][sc] == 1) continue;   // 물결 없거나 방문함

			// BFS로 덩어리를 모은다
			int rp = 0, wp = 0;          // 큐 위치
			int sum = 0;                 // 세기 합

			queue[wp].r = sr;            // 시작 칸 넣기
			queue[wp++].c = sc;
			visit[sr][sc] = 1;           // 방문 표시

			while (rp < wp)              // 큐가 빌 때까지
			{
				RC out = queue[rp++];    // 하나 꺼내기
				sum += wave[out.r][out.c];   // 세기 누적

				for (int i = 0; i < 4; i++)  // 4방향
				{
					int nr = out.r + dr[i];  // 이웃 행
					int nc = out.c + dc[i];  // 이웃 열

					if (nr < 0 || nc < 0 || nr >= N || nc >= M) continue;   // 격자 밖
					if (wave[nr][nc] <= 0 || visit[nr][nc] == 1) continue;  // 물결 없거나 방문함

					visit[nr][nc] = 1;   // 방문 표시
					queue[wp].r = nr;    // 큐에 넣기
					queue[wp++].c = nc;
				}
			}

			// 큐에 모인 칸(0 ~ wp-1)이 곧 이 덩어리
			int avg = sum / wp;          // 평균 (내림)
			for (int i = 0; i < wp; i++)
				murk[queue[i].r][queue[i].c] += avg;   // 혼탁도에 더하기
		}
	}
}

// 날릴 돌이 남았거나 날고 있는 돌이 있으면 1
int isRemain(int t)
{
	for (int i = 0; i < scnt; i++)       // 날고 있는 돌
		if (stone[i].alive) return 1;

	for (int i = 0; i < K; i++)          // 날릴 돌
	{
		int last = cat[i].s + (cat[i].p - 1) * cat[i].d;   // 마지막 발사 시각
		if (last > t) return 1;          // 아직 남음
	}
	return 0;                            // 모두 끝남
}

void simulate()
{
	for (int t = 1; t <= LAST_TURN; t++) // 최대 50턴
	{
		moveStones();                          // 1
		launchStones(t);                       // 2
		int dropCount = dropStones();          // 3

		score[turnCount++] = (dropCount > 0) ? getScore() : 0;   // 4

		settle();                              // 5
		flow();                                // 6
		stirUp();                              // 7

		if (isRemain(t) == 0) break;     // 더 할 게 없으면 종료
	}
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수 입력
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		printf("#%d", tc);               // 케이스 번호 출력
		for (int i = 0; i < turnCount; i++)
			printf(" %d", score[i]);     // 턴별 점수 출력
		putchar('\n');                   // 줄바꿈
	}

	return 0;                            // 프로그램 정상 종료
}
