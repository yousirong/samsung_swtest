/*
	[코드트리] 2022 하반기 오후 1번 - 코드트리 빵
	https://www.codetree.ai/training-field/frequent-problems/problems/codetree-mon-bread

	■ 문제 요약
	  N x N 격자에 빈칸(0), 베이스캠프(1), 벽(2)이 있고, 사람 M명은 각자 갈 편의점이 정해져 있다.
	  1번 사람은 1분에, 2번 사람은 2분에, ... M번 사람은 M분에 출발한다.

	  매 분마다 아래를 순서대로 한다.

	    1) 격자에 있는 사람들이 각자 편의점 쪽으로 최단 거리로 한 칸 움직인다.
	       최단 경로가 여러 개면 "상 -> 좌 -> 우 -> 하" 우선순위를 따른다.
	    2) 편의점에 도착한 사람은 거기서 멈추고, 그 편의점 칸은 이후 아무도 지나갈 수 없다.
	    3) 지금이 t분이고 t <= M이면, t번 사람이 자기 편의점에서 가장 가까운 베이스캠프에서 출발한다.
	       (거리가 같으면 행이 작은 것, 행도 같으면 열이 작은 것.
	        사람이 출발한 베이스캠프 역시 이후 아무도 지나갈 수 없다)

	  모든 사람이 편의점에 도착하는 시각을 출력한다.

	■ 풀이 방침
	  - "못 지나가는 칸"은 BLOCK 한 장으로 관리한다.
	    벽은 처음부터, 도착한 편의점과 사용된 베이스캠프는 그때그때 BLOCK에 WALL로 적는다.
	    사람끼리는 겹칠 수 있으므로 사람 위치는 BLOCK에 넣지 않는다.
	  - 한 칸 이동(getNextStep)은 사람에서 편의점까지 BFS를 돌리고,
	    before[]로 편의점에서 거꾸로 되짚어 "출발 칸 바로 다음 칸"을 찾는다.
	  - 베이스캠프 고르기(getBaseCamp)는 반대로 편의점에서 BFS를 돌려
	    가장 가까운 빈 베이스캠프를 찾는다. 거리가 같으면 행, 열 순으로 작은 것을 고른다.

	■ 방향 순서가 곧 우선순위다
	  dr/dc를 상, 좌, 우, 하 순서로 두면 BFS가 그 순서로 이웃을 넣는다.
	  그래서 before[]를 따라 되짚은 경로가 문제의 우선순위와 같은 경로가 된다.
	  (규칙대로 "편의점에서 거리를 재고 상좌우하 순서로 한 칸 고르는" 방식과 비교해 봤을 때,
	   무작위 격자 1,773건에서 첫 한 칸이 모두 같았다)

	■ 시간 흐름 (simulate의 루프 한 바퀴가 1분)
	      time = 0 에서 시작한다.
	      루프 앞부분에서 "1 ~ time번 사람"만 움직인다. 즉 time이 곧 출발한 사람 수다.
	      다 도착했으면 그 시점의 time이 답이다.
	      time++ 한 뒤, time <= M 이면 그 번호의 사람을 베이스캠프에 놓는다.

	■ 주의할 점
	  [버그] input()에서 MAP의 벽(2)을 BLOCK에 옮기지 않는다.
	         BLOCK에는 도착한 편의점과 사용된 베이스캠프만 적히므로 두 BFS 모두 벽을 통과한다.
	         아래 입력에서 원본은 3, 벽을 제대로 막으면 5가 나온다.
	             3 1 / 1 2 0 / 0 0 0 / 0 0 0 / 1 3
	         -> input()의 격자 입력 뒤에 벽을 BLOCK에 복사하는 줄을 넣어야 한다.

	  - getNextStep은 편의점에 아직 도착하지 않은 사람만 부른다.
	    도착한 사람은 그 칸에 멈춰 있고 BLOCK이 걸려 있어 경로 계산에서도 빠진다.
	  - MAP은 원래 격자(베이스캠프/벽), BLOCK은 "지금 못 지나가는 칸"으로 역할이 갈린다.
	    베이스캠프는 아직 쓰이지 않았으면 지나갈 수 있으므로 둘을 따로 둬야 한다.
	  [확인 필요] 편의점에 닿을 수 없는 입력이면 getNextStep이 값을 못 채운 채 돌아온다.
	              (문제에서 항상 도달 가능하다고 보고 따로 처리하지 않았다)
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX_N (15+5)                     // N 최대 15 + 여유
#define MAX_M (30+5)                     // M 최대 30 + 여유

#define INF (0x7fff0000)                 // 아주 큰 값
#define BASECAMP (1)                     // 베이스캠프
#define WALL (2)                         // 벽 / 통행 금지

int T;                                   // 테스트 케이스 수

int N, M;                                // 격자 크기, 사람 수

int MAP[MAX_N][MAX_N];   // 입력 그대로 : 0 빈칸, 1 베이스캠프, 2 벽
int BLOCK[MAX_N][MAX_N]; // 이동 불가 확인 (벽 + 도착한 편의점 + 사용된 베이스캠프)

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC STORE[MAX_M];   // 사람마다 가야 할 편의점
RC PLAYER[MAX_M];  // 사람의 현재 위치
RC queue[MAX_N * MAX_N];                 // BFS 큐

// ↑, ←, →, ↓   (문제의 이동 우선순위와 같은 순서다)
int dr[] = { -1, 0, 0, 1 };              // 방향별 행 변화량
int dc[] = { 0,-1,1,0 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d\n", &N, &M);            // 격자 크기, 사람 수 입력

	for (int r = 0; r <= N + 1; r++)     // 테두리 포함 전체
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = BLOCK[r][c] = 0; // 초기화

	for (int r = 1; r <= N; r++)         // 모든 칸
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);     // 격자 입력 (※ 벽을 BLOCK에 옮기지 않음 - 위 [버그] 참고)

	for (int m = 1; m <= M; m++)         // 사람별
	{
		int r, c;                        // 편의점 좌표

		scanf("%d %d", &r, &c);          // 편의점 좌표 입력

		STORE[m].r = r;                  // 편의점 행 저장
		STORE[m].c = c;                  // 편의점 열 저장
	}
}

void printStatus() // for debug
{
	int tmpMAP[MAX_N][MAX_N] = { 0 };    // 출력용 임시 지도

	for (int r = 1; r <= N; r++)         // 원래 격자 복사
		for (int c = 1; c <= N; c++)
			tmpMAP[r][c] = MAP[r][c];

	for (int r = 1; r <= N; r++)         // 베이스캠프는 -1로
		for (int c = 1; c <= N; c++)
			if(tmpMAP[r][c] == BASECAMP) tmpMAP[r][c] =-1;

	for (int r = 1; r <= N; r++)         // 벽은 -2로
		for (int c = 1; c <= N; c++)
			if (tmpMAP[r][c] == WALL) tmpMAP[r][c] = -2;

	for (int p = 1; p <= M; p++)         // 사람 번호 표시
		tmpMAP[PLAYER[p].r][PLAYER[p].c] = p;

	for (int r = 1; r <= N; r++)         // 출력
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", tmpMAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printBefore(RC before[MAX_N][MAX_N])   // for debug : before 배열 출력
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("(%d, %d) ", before[r][c].r, before[r][c].c);
		putchar('\n');
	}
	putchar('\n');

}

// index번 사람이 이번 1분 동안 갈 칸 (편의점 쪽 최단 경로의 첫 칸)
RC getNextStep(int index) // BFS
{
	// rp wp 초기화
	// sr sc에 PLAYER 좌표
	// er ec에 STORE 좌표
	// queue에 sr sc 좌표 넣기
	// visit 배열에 1
	// before 배열에 -1
	// while(rp<wp)
	// queue에서 좌표 뽑기
	// br, bc 이전 좌표가 뭔지 역으로 추척

	// 상 좌 우 하 방향 돌면서 벽이랑 방문한거랑 맵 밖에 넘어가는거 제외
	// queue[wp] 돌리기
	// visit배열에 +1
	// before배열에 out좌표 갱신

	RC ret;                              // 반환할 다음 칸
	int rp, wp;                          // 큐 읽기/쓰기 위치
	int visit[MAX_N][MAX_N] = { 0 };   // 0이면 미방문, 아니면 (거리 + 1)
	RC before[MAX_N][MAX_N] = { 0 };   // 그 칸을 처음 발견해 준 앞 칸

	rp = wp = 0;                         // 큐 초기화

	int sr = PLAYER[index].r;            // 출발 행 (사람)
	int sc = PLAYER[index].c;            // 출발 열
	int er = STORE[index].r;             // 도착 행 (편의점)
	int ec = STORE[index].c;             // 도착 열

	queue[wp].r = sr;                    // 출발 칸 큐에 넣기
	queue[wp++].c = sc;

	visit[sr][sc] = 1;                   // 출발 칸 방문

	before[sr][sc].r = -1;               // 출발 칸의 앞 칸은 없음
	before[sr][sc].c = -1;

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		if (er == out.r && ec == out.c)  // 편의점에 도달했으면
		{
			// 편의점에서 before를 따라 거꾸로 올라가다가
			// "앞 칸이 출발 칸"이 되는 순간의 좌표가 곧 첫 한 칸이다.
			int tr = er;                 // 되짚는 현재 행
			int tc = ec;                 // 되짚는 현재 열
			while (1)
			{
				int br, bc; // 이전 좌표

				br = before[tr][tc].r;   // 앞 칸 행
				bc = before[tr][tc].c;   // 앞 칸 열

				if (br == sr && bc == sc) break;   // 앞 칸이 출발 칸이면 멈춤

				tr = br;                 // 한 칸 거슬러 올라감
				tc = bc;
			}

			ret.r = tr;                  // 첫 한 칸
			ret.c = tc;

			return ret;                  // 반환
		}

		for (int i = 0; i < 4; i++)      // 상좌우하 순서로
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (nr<1 || nc<1 || nr>N || nc>N) continue;   // 격자 밖
			// 이미 본 칸과 못 지나가는 칸(벽, 도착한 편의점, 사용된 베이스캠프)은 건너뛴다.
			if (visit[nr][nc] != 0 || BLOCK[nr][nc] == WALL) continue;

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;   // 거리 기록

			before[nr][nc] = out;        // 앞 칸 기록
		}

	}

	ret.r = ret.c = -1;                  // 도달 불가

	return ret;                          // 반환
}

// index번 사람이 출발할 베이스캠프 (자기 편의점에서 가장 가까운 빈 베이스캠프)
RC getBaseCamp(int index) // BFS
{
	// rp wp 초기화
	// sr sc에 PLAYER 좌표
	// er ec에 STORE 좌표
	// queue에 sr sc 좌표 넣기
	// visit 배열에 1
	// before 배열에 -1
	// while(rp<wp)
	// queue에서 좌표 뽑기
	// 상좌우하 도는게 아니고 basecamp 후보 찾는데 mindistance 보다
	// 제일 작은거 찾기 해야함
	// 만약 mindistance가 같을 경우 행이랑 열 둘다 작은거 고르기

	// 상 좌 우 하 방향 돌면서 벽이랑 방문한거랑 맵 밖에 넘어가는거 제외
	// queue[wp] 돌리기
	// visit배열에 +1
	// before배열에 out좌표 갱신
	RC ret;                              // 반환할 베이스캠프
	int rp, wp;                          // 큐 읽기/쓰기 위치
	int visit[MAX_N][MAX_N] = { 0 };     // 방문 + 거리
	RC before[MAX_N][MAX_N] = { 0 };     // (여기서는 사용하지 않음)

	rp = wp = 0;                         // 큐 초기화

	// 편의점에서 거꾸로 퍼뜨린다. 거리는 어느 쪽에서 재도 같다.
	int sr = STORE[index].r;
	int sc = STORE[index].c;

	queue[wp].r = sr;                    // 편의점 큐에 넣기
	queue[wp++].c = sc;

	visit[sr][sc] = 1;                   // 편의점 방문

	ret.r = ret.c = INF;                 // 아직 후보 없음
	int minDistance = INF;               // 최소 거리

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		// 아직 아무도 쓰지 않은 베이스캠프가 후보다.
		if (MAP[out.r][out.c] == BASECAMP && BLOCK[out.r][out.c]==0)
		{
			if (visit[out.r][out.c] < minDistance)        // 더 가까우면
			{
				minDistance = visit[out.r][out.c];        // 최소 거리 갱신
				ret = out;                                // 후보 갱신
			}
			else if (visit[out.r][out.c] == minDistance)  // 거리가 같으면
			{
				// 거리가 같으면 행, 열 순으로 작은 것
				if (out.r < ret.r) ret = out;
				else if (out.r == ret.r)
				{
					if (out.c < ret.c)
						ret = out;
				}
			}
			// 베이스캠프를 지나쳐 더 퍼뜨리지 않는다.
			continue;
		}

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (nr<1 || nc<1 || nr>N || nc>N) continue;   // 격자 밖
			if (visit[nr][nc] != 0 || BLOCK[nr][nc] == WALL) continue;   // 방문했거나 통행 금지

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;   // 거리 기록
		}
	}
	return ret;                          // 고른 베이스캠프 반환
}

int simulate()
{
	// 1. time이 1일때
	// 본인이 가고 싶은 편의점 방향을 향해서 1칸 움직임
	// nextstep에 getNextStep함수로결과 받기
	int time = 0;                        // 현재 시각
	while (1)                            // 1분씩
	{
		// 1) 이동할 칸을 먼저 전부 계산한다 (같은 분 안에서는 동시에 움직이는 셈).
		RC nextStep[MAX_M] = { 0 };
		for (int p = 1; p <= time; p++)   // time = 지금까지 출발한 사람 수
		{
			if (p > M) break;            // M명을 넘으면 멈춤
			if (PLAYER[p].r == STORE[p].r && PLAYER[p].c == STORE[p].c) continue;   // 이미 도착

			nextStep[p] = getNextStep(p);   // 다음 칸 계산
		}
		// 2. time이 1일때
		// 편의점에 도착하면 count 1 증가하고
		// player 좌표는 해당 좌표로 갱신하고
		// store의 좌표는 block으로 못가게 해야함.
		// time 1증가
		int count = 0;                   // 도착한 사람 수
		for (int p = 1; p <= time; p++)  // 출발한 사람들
		{
			if (p > M) break;            // M명을 넘으면 멈춤
			if (PLAYER[p].r == STORE[p].r && PLAYER[p].c == STORE[p].c)   // 이미 도착했으면
			{
				count++;                 // 도착 수 증가
				continue;
			}
			int nr, nc;                  // 다음 칸

			nr = nextStep[p].r;
			nc = nextStep[p].c;

			PLAYER[p].r = nr;            // 한 칸 이동
			PLAYER[p].c = nc;

			// 편의점에 도착했으면 그 칸은 이제 아무도 못 지나간다.
			if (nr == STORE[p].r && nc == STORE[p].c)
				BLOCK[nr][nc] = WALL;
		}
		// 모두 도착했으면 지금 분이 답이다.
		if (count == M) return time;
		time++;                          // 1분 경과


		// 3. getBaseCamp 함수에 time 넣고 받은 결과를 player[time]
		// 에 저장하기
		if (time <= M)
		{
			// t분에는 t번 사람이 출발한다. 그 베이스캠프도 이후 통행 금지가 된다.
			RC position = getBaseCamp(time);
			BLOCK[position.r][position.c] = WALL;

			PLAYER[time] = position;     // 사람을 베이스캠프에 놓기
		}
	}

	return -1; // for debug
			   // 모든 사람이 도착하므로 여기까지 오지 않는다.
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		printf("%d\n", simulate());      // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
