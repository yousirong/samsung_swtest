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

	■ 풀이 방침 : 돌을 움직이지 않고 "떨어지는 턴"을 미리 계산한다
	  돌끼리는 서로 막지 않고, 매 턴 정확히 한 칸씩 움직인다.
	  그래서 돌 하나가 언제 어디에 떨어지는지는 입력만 보고 바로 알 수 있다.
	  -> 1, 2단계(이동, 발사)는 시뮬레이션하지 않고, 입력 때 landing[턴][]에 떨어질 돌을 넣어 둔다.
	  매 턴은 makeWave(3) -> getScore(4) -> settleMud(5) -> flowLeft(6) -> makeMud(7) 순서.

	■ 낙하 턴 계산 : t = s + j x d + h - 1  (j = 0 ~ p-1)
	  j번째 돌은 시각 s + j x d 에 날린다.
	  2단계(발사)가 1단계(이동)보다 뒤라, 날린 턴에는 움직이지 않고 n-1행에 있다.
	  3단계(낙하)는 발사 뒤라, 거리 h = 1이면 날린 턴에 바로 떨어진다.
	  그 뒤 한 턴에 한 칸씩 올라가므로 n - h 행에는 h - 1턴 뒤에 닿는다.
	  떨어지는 칸은 (n - h, c).

	■ 끝나는 턴 = min(50, 마지막 낙하 턴)
	  마지막 돌이 떨어진 턴이 끝나면 날릴 돌도 날고 있는 돌도 없다.
	  그래서 lastTurn(모든 돌 중 가장 늦은 낙하 턴)까지만 돌리면 된다. 단 50턴을 넘으면 50에서 끝.
	  50턴 뒤에 떨어질 돌은 landing[]에 넣지 않는다(배열 크기도 50턴까지만).
	  한 투석기는 한 턴에 돌을 하나만 날리므로, 한 턴에 떨어지는 돌은 최대 k개다.

	■ 5단계와 6단계는 "동시에" 바뀐다 -> settled[][]를 거쳐 옮긴다
	  settleMud : 줄어들기 전 mud[][]로 이웃을 보고, 결과는 settled[][]에 쓴다.
	              네 이웃이 모두 격자 안 = 1 <= r <= n-2 이고 1 <= c <= m-2 (경계 칸은 무조건 -2).
	              흙탕물이 없는 칸(0)도 -2 후 0으로 잘라 그대로 0이 된다.
	  flowLeft  : mud[r][c] = settled[r][c + 1], 마지막 열은 0.
	              settled에서 읽어 mud에 쓰므로 덮어쓰기 순서를 신경 쓸 필요가 없다.

	■ 7단계 덩어리 = BFS (flood fill)
	  wave > 0 인 칸에서 BFS로 상하좌우 이어진 칸을 모아 합과 칸 수(wp)를 구하고,
	  큐에 모인 칸(0 ~ wp-1)에 평균을 더한다. 대각선만 닿은 칸은 다른 덩어리다.

	■ 주의할 점
	  - 4단계에서 쓰는 혼탁도는 "직전 턴이 끝났을 때" 값이다. 그래서 점수는 5 ~ 7단계 전에 계산한다.
	  - 7단계는 6단계(물살) 뒤라, 이번 턴 흙탕물은 흘러가지 않은 자리에 바로 더해진다.
	  - 떨어진 돌이 없으면 wave가 전부 0이라 점수도 0, 7단계도 아무 일이 없다 (따로 처리 안 해도 된다).
	  - 같은 열에 투석기가 여러 개일 수 있다. 같은 턴 같은 칸에 떨어져도 물결은 그냥 더한다.
	  - 테스트 케이스마다 landingCount[], mud[][], lastTurn 을 초기화한다.
*/
#include <stdio.h>                       // scanf, printf, putchar
#include <stdlib.h>                      // abs

#define MAX (30 + 5)                     // n, m 최대 30 + 여유
#define MAX_K (50 + 5)                   // 한 턴에 떨어지는 돌 최대 k(50) + 여유
#define MAX_T (50 + 5)                   // 최대 50턴 + 여유

int T;                                   // 테스트 케이스 수
int N, M, K, lastTurn;                   // 행 수, 열 수, 투석기 수, 가장 늦은 낙하 턴
int wave[MAX][MAX];                      // 이번 턴 물결 세기 (턴마다 0부터)
int mud[MAX][MAX];                       // 혼탁도 (턴을 넘어 남는다)
int settled[MAX][MAX];                   // 가라앉은 뒤 혼탁도 (5단계 결과, 6단계 입력)
bool visit[MAX][MAX];                    // BFS 방문 표시

int dr[] = { -1, 1, 0, 0 };              // 상하좌우 행 변화량
int dc[] = { 0, 0, -1, 1 };              // 상하좌우 열 변화량

struct STONE                             // 떨어지는 돌
{
	int r;                               // 떨어지는 행 (n - h)
	int c;                               // 떨어지는 열
	int weight;                          // 무게 a
};

STONE landing[MAX_T][MAX_K];             // landing[t][] = t턴에 떨어지는 돌들
int landingCount[MAX_T];                 // t턴에 떨어지는 돌 수

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐 (덩어리 칸 목록으로도 쓴다)

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 호수 크기, 투석기 수

	lastTurn = 0;                        // 초기화
	for (int t = 1; t <= 50; t++) landingCount[t] = 0;
	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			mud[r][c] = 0;

	for (int i = 1; i <= K; i++)         // 투석기별
	{
		int c, s, d, a, h, p;            // 열, 시작 시각, 간격, 무게, 거리, 돌 수
		scanf("%d %d %d %d %d %d", &c, &s, &d, &a, &h, &p);

		for (int j = 0; j < p; j++)      // j번째 돌
		{
			// 발사한 턴에 이쪽 기슭 칸을 지나므로 거리 h는 h - 1턴 뒤에 낙하한다.
			int t = s + j * d + h - 1;   // 발사 시각 + 물에 떨어질 때까지 걸리는 턴
			if (t > lastTurn) lastTurn = t;   // 가장 늦은 낙하 턴 갱신 (50 넘어도 기록)
			if (t > 50) continue;        // 50턴 뒤는 대회가 끝나서 필요 없다

			STONE &stone = landing[t][landingCount[t]++];   // t턴 목록에 추가
			stone.r = N - h;             // 떨어지는 행
			stone.c = c;                 // 떨어지는 열
			stone.weight = a;            // 무게
		}
	}
}

// 3단계 : 이 턴에 떨어진 모든 돌의 물결 세기를 칸별로 더한다.
void makeWave(int turn)
{
	for (int r = 0; r < N; r++)          // 물결은 턴마다 새로
		for (int c = 0; c < M; c++)
			wave[r][c] = 0;

	for (int i = 0; i < landingCount[turn]; i++)   // 이 턴에 떨어진 돌별
	{
		STONE stone = landing[turn][i];
		for (int r = 0; r < N; r++)      // 모든 칸에
			for (int c = 0; c < M; c++)
			{
				int distance = abs(r - stone.r) + abs(c - stone.c);   // 맨해튼 거리
				int strength = 3 * stone.weight - 5 * distance;       // 3a - 5 x 거리
				if (strength > 0) wave[r][c] += strength;             // 양수만 더한다
			}
	}
}

// 4단계 : 점수. 이 시점의 mud는 직전 턴 종료 시 혼탁도이다.
int getScore()
{
	int score = 0;
	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			if (wave[r][c] > mud[r][c])  // 물결 - 혼탁도 가 양수인 칸만
				score += wave[r][c] - mud[r][c];

	return score;
}

// 5단계 : 흙탕물 가라앉기. 이웃의 흙탕물 여부는 감소 전 상태(mud)로 동시에 판정한다.
void settleMud()
{
	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
		{
			int decrease = 2;            // 기본 -2
			if (r > 0 && r + 1 < N && c > 0 && c + 1 < M &&     // 네 이웃이 모두 격자 안
				mud[r - 1][c] > 0 && mud[r + 1][c] > 0 &&       // 위, 아래 흙탕물
				mud[r][c - 1] > 0 && mud[r][c + 1] > 0)         // 왼쪽, 오른쪽 흙탕물
				decrease = 1;            // 둘러싸이면 -1

			settled[r][c] = mud[r][c] - decrease;   // 결과는 settled에 (mud는 그대로 둔다)
			if (settled[r][c] < 0) settled[r][c] = 0;   // 0 아래로는 안 감
		}
}

// 6단계 : 물살. 가라앉은 흙탕물(settled)을 한 칸 왼쪽으로 옮기고 0열의 것은 버린다.
void flowLeft()
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c + 1 < M; c++)
			mud[r][c] = settled[r][c + 1];   // 오른쪽 칸 값을 가져온다
		mud[r][M - 1] = 0;               // 마지막 열은 맑은 물
	}
}

// 7단계 : 흙탕물 일기. 물결이 양수인 칸을 BFS로 묶고, 덩어리별 평균 세기를 더한다.
// 점수 계산, 가라앉기, 물살이 모두 끝난 뒤에 실행한다.
void makeMud()
{
	for (int r = 0; r < N; r++)          // 방문 초기화
		for (int c = 0; c < M; c++)
			visit[r][c] = false;

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
		{
			if (wave[r][c] == 0 || visit[r][c] == true) continue;   // 물결 없거나 이미 묶음

			int rp = 0, wp = 0;          // 큐 읽기/쓰기 위치
			int sum = 0;                 // 덩어리 세기 합
			queue[wp].r = r;             // 시작 칸
			queue[wp++].c = c;
			visit[r][c] = true;

			while (rp < wp)
			{
				RC out = queue[rp++];
				sum += wave[out.r][out.c];   // 꺼낼 때 세기 누적

				for (int dir = 0; dir < 4; dir++)
				{
					int nr = out.r + dr[dir];
					int nc = out.c + dc[dir];
					if (nr < 0 || nc < 0 || nr >= N || nc >= M) continue;   // 격자 밖
					if (wave[nr][nc] == 0 || visit[nr][nc] == true) continue;   // 물결 없거나 방문

					visit[nr][nc] = true;
					queue[wp].r = nr;
					queue[wp++].c = nc;
				}
			}

			int average = sum / wp;      // wp = 덩어리 칸 수, 평균 (내림)
			for (int i = 0; i < wp; i++) // 큐에 모인 칸 = 이 덩어리
				mud[queue[i].r][queue[i].c] += average;
		}
}

void simulate(int tc)
{
	printf("#%d", tc);                   // 케이스 번호

	// 마지막 낙하 턴과 50번째 턴 중 더 이른 때에 대회가 끝난다.
	for (int turn = 1; turn <= 50 && turn <= lastTurn; turn++)
	{
		// 돌의 낙하 시각을 미리 계산했으므로 이동과 발사(1, 2단계)는 따로 시뮬레이션하지 않는다.
		makeWave(turn);                  // 3
		printf(" %d", getScore());       // 4 (바로 출력)
		settleMud();                     // 5
		flowLeft();                      // 6
		makeMud();                       // 7
	}

	putchar('\n');
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수
	for (int tc = 1; tc <= T; tc++)
	{
		input();                         // 입력 + 낙하 턴 계산
		simulate(tc);                    // 시뮬레이션 + 출력
	}

	return 0;
}
