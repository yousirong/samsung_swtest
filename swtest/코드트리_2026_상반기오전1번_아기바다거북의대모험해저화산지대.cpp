/*
	[코드트리] 2026 상반기 오전 1번 - 아기 바다거북의 대모험: 해저 화산 지대
	https://www.codetree.ai/training-field/frequent-problems   ("아기 바다거북" 검색)
	[확인 필요] 문제 개별 주소(slug)를 찾지 못해 기출 목록 주소를 달아 두었다.

	■ 문제 요약 (코드에서 읽어 낸 규칙이다. 세부 조건은 문제 본문과 대조가 필요하다)
	  N x N 바다(0은 바다, 1은 산호)에 아기 바다거북 M마리와 해저 화산 K개가 있다.
	  거북들은 오른쪽 아래 끝 (N-1, N-1)까지 가야 한다. 최대 100턴 동안 아래를 반복한다.

	    1) 거북 이동 : 1번 거북부터 차례로, 목적지 쪽 최단 경로로 한 칸 간다.
	       산호와 다른 거북이 있는 칸은 지나갈 수 없다. 길이 막혀 있으면 그 턴은 제자리다.
	       목적지에 닿으면 탈출한다(격자에서 빠진다).
	    2) 압력 증가 : 모든 화산의 압력이 10씩 오른다.
	    3) 분출 : 압력이 임계치 P 이상인 화산이 분출한다.
	       자기 칸에 P만큼, 상하좌우 네 방향으로 P/2, P/4, ... 의 열기를 퍼뜨린다.
	       (산호에 막히거나 값이 0이 되면 그 방향은 멈춘다)
	       받은 열기까지 더해 임계치를 넘는 화산은 연쇄로 분출한다. 더 없을 때까지 반복한다.
	    4) 화석화 : 이번 턴에 받은 열기가 20 이상인 칸의 거북은 화석이 된다.
	    5) 분출한 화산은 압력이 0으로 돌아간다.

	  거북마다 목적지에 도착한 턴 번호를 출력한다. 도착하지 못했거나 화석이 되었으면 -1.

	■ 풀이 방침
	  - 거북 이동은 BFS + before[] 역추적으로 "목적지까지 최단 경로의 첫 칸"을 구한다.
	    방향 순서를 우 -> 하 -> 좌 -> 상 으로 두어 같은 거리일 때의 우선순위를 맞춘다.
	  - 열기는 heat[][] 한 장에 이번 턴 동안 쌓는다. 화산은 압력(current)과 열기(heat)를 더해 판단한다.
	  - 연쇄 분출은 "이번 바퀴에 새로 터진 화산이 없을 때까지" while 로 반복한다.
	    volcano[k].check 로 이번 턴에 이미 터진 화산을 표시해 두 번 터지지 않게 한다.

	■ 상태 값 정리
	      turtle[m].check : 0 = 아직 진행 중, -1 = 끝(도착 또는 화석)
	      turtle[m].count : 지금까지 지난 턴 수. 화석이 되면 -1로 덮는다.
	  그래서 마지막 출력은
	      check == 0 (끝까지 도착 못 함)  -> -1
	      그 외                           -> count (도착 턴, 화석이면 -1)
	  로 한 줄에 정리된다.

	■ 주의할 점
	  - 이 문제는 좌표를 0부터 쓴다. 목적지는 (N-1, N-1)이다.
	  - 턴 수(count)는 움직이지 못한 턴에도 1씩 늘어난다.
	  - P[][]와 volcano[].check는 input()에서 초기화하지 않는다. 한 번만 실행하면 문제없지만
	    여러 번 돌리면 이전 값이 남는다.
	  - 분출 코드가 "처음 분출"과 "연쇄 분출"에 똑같이 두 번 적혀 있다. 하나를 고치면 둘 다 고쳐야 한다.
*/
#include <stdio.h>	                     // scanf, printf, putchar 사용

#define MAX (20+5)                       // N 최대 20 + 여유
#define CORAL (1)                        // 산호

int T;                                   // 테스트 케이스 수
int N, M, K;                             // 바다 크기, 거북 수, 화산 수

int MAP[MAX][MAX];                       // 바다(0) / 산호(1)
int P[MAX][MAX]; // 분출 임계치 P
int current[MAX][MAX]; // 현재 압력

// 우하좌상
int dr[] = { 0,1,0,-1 };                 // 방향별 행 변화량
int dc[] = { 1,0,-1,0 };                 // 방향별 열 변화량

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐
int rp, wp;                              // (전역 큐 위치, getNextStep에서는 지역 변수를 씀)

struct RCC                               // 좌표 + 상태
{
	int r;                               // 행
	int c;                               // 열
	int check;                           // 화산 : 이번 턴 분출 여부 / 거북 : 0 진행 중, -1 끝
	int count; // 거북이 턴 번호
};

RCC volcano[1000 + 10];                  // 화산 목록
RCC turtle[10 + 5];                      // 거북 목록 (1번부터)
int turtleMAP[MAX][MAX];                 // 칸별 거북 번호

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 바다 크기, 거북 수, 화산 수 입력

	for (int r = 0; r < N; r++)          // 바다 입력
		for (int c = 0; c < N; c++)
			scanf("%d", &MAP[r][c]);

	for (int r = 0; r < N; r++)          // 거북 지도 초기화
		for (int c = 0; c < N; c++)
			turtleMAP[r][c] = 0;

	for (int m = 1; m <= M; m++) // ID가 1번부터 시작
	{
		int r, c;                        // 거북 좌표

		scanf("%d %d", &r, &c);          // 입력

		turtle[m].r = r;                 // 행
		turtle[m].c = c;                 // 열
		turtle[m].check = 0;             // 진행 중
		turtle[m].count = 0;             // 턴 수 0

		turtleMAP[r][c] = m;             // 지도에 표시
	}

	for (int r = 0; r < N; r++)          // 압력 초기화
		for (int c = 0; c < N; c++)
			current[r][c] = 0;

	for (int k = 0; k < K; k++)          // 화산별
	{
		int r, c, p;                     // 좌표, 임계치

		scanf("%d %d %d", &r, &c, &p);   // 입력

		P[r][c] = p;                     // 임계치 저장
		volcano[k].r = r;                // 좌표 저장
		volcano[k].c = c;
	}
}

void printMap(int map[MAX][MAX])         // for debug
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

RC getNextStep(int index)	// BFS
{
	RC ret;                              // 결과 (다음 칸)
	int rp, wp;                          // 큐 위치
	int visit[MAX][MAX] = { 0 };         // 방문 + 거리
	RC before[MAX][MAX] = { 0 };         // 앞 칸

	rp = wp = 0;                         // 큐 초기화

	int sr = turtle[index].r;            // 출발 행
	int sc = turtle[index].c;            // 출발 열
	int er = N - 1;                      // 목적지 행
	int ec = N - 1;                      // 목적지 열

	queue[wp].r = sr;                    // 출발 칸 넣기
	queue[wp++].c = sc;

	visit[sr][sc] = 1;                   // 방문 표시

	before[sr][sc].r = -1;               // 앞 칸 없음
	before[sr][sc].c = -1;

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		if (er == out.r && ec == out.c)  // 목적지 도달
		{
			int tr = er;                 // 되짚는 현재 칸
			int tc = ec;
			while (1)
			{
				int br, bc; // 이전 좌표

				br = before[tr][tc].r;
				bc = before[tr][tc].c;

				if (br == sr && bc == sc) break;   // 앞 칸이 출발 칸이면 멈춤

				tr = br;                 // 한 칸 거슬러 올라감
				tc = bc;
			}
			ret.r = tr;                  // 첫 한 칸
			ret.c = tc;

			return ret;                  // 반환
		}

		for (int i = 0; i < 4; i++)      // 우하좌상 순서로
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) continue;   // 격자 밖
			if (visit[nr][nc] != 0 || MAP[nr][nc] != 0 || turtleMAP[nr][nc] != 0)continue;
			// 산호와 다른 거북이 있는 칸은 지나갈 수 없다.

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;   // 거리 기록

			before[nr][nc] = out;        // 앞 칸 기록
		}
	}

	ret.r = ret.c = -1;                  // 길 없음

	return ret; // for debug
}

void move()                              // 거북 이동
{
	for (int m = 1; m <= M; m++)         // 1번부터
	{
		if (turtle[m].check == -1) continue;   // 끝난 거북 제외

		turtle[m].count++;
		// 움직이지 못해도 한 턴이 지난다.

		RC next = getNextStep(m);        // 다음 칸

		if (next.r == -1) continue;      // 길이 막혔으면 제자리

		int r, c, nr, nc;                // 현재, 다음 칸

		r = turtle[m].r;
		c = turtle[m].c;
		nr = next.r;
		nc = next.c;

		turtleMAP[r][c] = 0;             // 옛 칸 비우기

		turtle[m].r = nr;                // 이동
		turtle[m].c = nc;

		turtleMAP[nr][nc] = m;           // 새 칸 표시

		if (nr == N - 1 && nc == N - 1)  // 목적지 도착
		{
			turtle[m].check = -1;        // 끝
			turtleMAP[nr][nc] = 0;
			// 도착한 거북은 격자에서 빠진다 (다른 거북의 길을 막지 않는다).
		}
	}
}

void increase()                          // 압력 증가
{
	for (int k = 0; k < K; k++)          // 화산별
	{
		int r, c;                        // 화산 좌표

		r = volcano[k].r;
		c = volcano[k].c;

		current[r][c] += 10;             // 압력 +10
	}
}

void explode()                           // 분출 + 연쇄 + 화석화
{
	int heat[MAX][MAX] = { 0 };          // 이번 턴 열기

	// 열기 전파
	for (int k = 0; k < K; k++)          // 화산별
	{
		int r, c;                        // 화산 좌표

		r = volcano[k].r;
		c = volcano[k].c;

		if (current[r][c] >= P[r][c])    // 임계치 이상이면 분출
		{
			volcano[k].check = 1;        // 분출 표시
			heat[r][c] += P[r][c];       // 자기 칸 열기

			for (int i = 0; i < 4; i++)  // 4방향
			{
				int nr, nc;              // 퍼지는 칸

				nr = r + dr[i];
				nc = c + dc[i];

				int p = P[r][c] / 2;     // 첫 칸 열기

				while (1)
				{
					if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;   // 격자 밖
					if (MAP[nr][nc] == CORAL || p == 0) break;         // 산호거나 0

					heat[nr][nc] += p;
					// 멀어질수록 절반씩 줄어든다. 산호에 막히거나 0이 되면 멈춘다.

					p = p / 2;           // 절반으로
					nr += dr[i];         // 한 칸 더
					nc += dc[i];
				}
			}
		}
	}

	// 연쇄 반응
	while (1)
	{
		bool explodeCheck = false;       // 이번 바퀴에 새로 터졌는가

		for (int k = 0; k < K; k++)      // 화산별
		{
			if (volcano[k].check == 1) continue;   // 이미 터진 화산 제외

			int r, c;                    // 화산 좌표

			r = volcano[k].r;
			c = volcano[k].c;

			if (current[r][c] + heat[r][c] >= P[r][c])
				// 압력 + 이번 턴에 받은 열기가 임계치를 넘으면 연쇄로 터진다.
			{
				explodeCheck = true;     // 새로 터짐

				volcano[k].check = 1;    // 분출 표시
				heat[r][c] += P[r][c];   // 자기 칸 열기

				for (int i = 0; i < 4; i++)   // 4방향
				{
					int nr, nc;          // 퍼지는 칸

					nr = r + dr[i];
					nc = c + dc[i];

					int p = P[r][c] / 2; // 첫 칸 열기

					while (1)
					{
						if (nr<0 || nc<0 || nr>N - 1 || nc>N - 1) break;   // 격자 밖
						if (MAP[nr][nc] == CORAL || p == 0)break;          // 산호거나 0

						heat[nr][nc] += p;   // 열기 누적

						p = p / 2;       // 절반으로
						nr += dr[i];     // 한 칸 더
						nc += dc[i];
					}
				}
			}
		}
		if (explodeCheck == false) break;
		// 이번 바퀴에 새로 터진 화산이 없으면 연쇄가 끝난다.
	}

	// 바다거북의 위기 (화석화)
	for (int m = 1; m <= M; m++)
	{
		if (turtle[m].check == -1)continue;   // 끝난 거북 제외

		int r, c;                        // 거북 좌표

		r = turtle[m].r;
		c = turtle[m].c;

		if (heat[r][c] >= 20)
			// 열기 20 이상이면 화석이 된다. count를 -1로 덮어 출력 때 -1이 나오게 한다.
		{
			turtle[m].check = -1;
			turtle[m].count = -1;
		}
	}

}

void reset()                             // 분출한 화산 압력 초기화
{
	for (int k = 0; k < K; k++)          // 화산별
	{
		int r, c;                        // 화산 좌표

		r = volcano[k].r;
		c = volcano[k].c;

		if (volcano[k].check == 1) current[r][c] = 0;   // 분출했으면 압력 0

		volcano[k].check = 0;            // 분출 표시 해제
	}
}

void simulate()
{
	for (int i = 0; i < 100; i++)        // 최대 100턴
	{
		move();                          // 거북 이동
		increase();                      // 압력 증가
		explode();                       // 분출
		reset();                         // 압력 초기화
	}

	for (int m = 1; m <= M; m++)         // 거북별 결과 출력
	{
		//도착실패
		if (turtle[m].check == 0) printf("-1\n");
		else printf("%d\n", turtle[m].count);   // 도착 턴 (화석이면 -1)
	}
}


int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션 + 출력
	}
	return 0;                            // 프로그램 정상 종료
}
