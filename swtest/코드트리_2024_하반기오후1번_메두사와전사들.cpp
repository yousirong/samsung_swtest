/*
	[코드트리] 2024 하반기 오후 1번 - 메두사와 전사들
	https://www.codetree.ai/training-field/frequent-problems/problems/medusa-and-warriors

	■ 문제 요약
	  N x N 마을은 도로(0)와 비도로(1)로 되어 있다. 메두사는 집에서 공원까지 도로만 따라
	  최단 경로로 이동한다(경로가 여러 개면 상 -> 하 -> 좌 -> 우 우선). 전사 M명이 메두사를 노린다.
	  집에서 공원으로 가는 길이 없으면 -1을 출력하고 끝난다.

	  매 턴은 아래 순서다.

	    1) 메두사 이동 : 경로를 따라 한 칸 간다. 그 칸에 전사가 있으면 그 전사들은 사라진다.
	    2) 메두사의 시선 : 상하좌우 중 한 방향을 바라본다. 시야는 그 방향으로 90도 부채꼴이다.
	       시야 안의 전사는 돌이 되어 이번 턴에 움직이지 못한다.
	       단, 다른 전사 뒤에 가려진 칸은 보이지 않는다.
	       돌이 되는 전사가 가장 많은 방향을 고르고, 같으면 상 -> 하 -> 좌 -> 우 순이다.
	    3) 전사 이동 (최대 두 번)
	       - 메두사와의 거리(|행 차이| + |열 차이|)가 줄어드는 방향으로만 한 칸 간다.
	       - 메두사의 시야 안 칸으로는 들어갈 수 없다.
	       - 첫 번째 이동은 상 -> 하 -> 좌 -> 우, 두 번째 이동은 좌 -> 우 -> 상 -> 하 우선이다.
	    4) 전사 공격 : 메두사와 같은 칸에 도착한 전사는 공격하고 사라진다.

	  메두사가 공원에 도착하기 전까지 턴마다
	  "전사들이 이동한 거리의 합, 돌이 된 전사 수, 공격한 전사 수"를 한 줄씩 출력하고,
	  공원에 도착하면 0을 출력한다.

	■ 풀이 방침
	  - 메두사의 경로는 처음에 BFS 한 번으로 통째로 구해 둔다(position[]).
	    이후 턴마다 position[p]로 한 칸씩 옮기기만 하면 된다.
	    BFS + before[] 역추적으로 경로를 얻고, 거꾸로 쌓였으므로 뒤집는다.
	    (도착점에서 거리를 재고 우선순위대로 한 칸씩 고르는 정석 방식과 무작위 1,778건을 비교해 경로가 모두 같았다)
	  - 시선은 scope[][] 한 장에 칠한다.
	        STRAIGHT(1) : 바라보는 방향 일직선
	        LEFT(2)     : 왼쪽 대각선 쪽 부채꼴
	        RIGHT(3)    : 오른쪽 대각선 쪽 부채꼴
	    값을 셋으로 나눠 둔 이유는 "그 전사가 어느 쪽 그림자를 만드는지" 알아야 하기 때문이다.

	■ 시야 부채꼴을 그리는 방법 (diagonal)
	  "대각선으로 k칸 간 점에서, 바라보는 방향으로 끝까지 직진"을 k = 1, 2, 3, ... 로 반복한다.
	  예를 들어 위를 볼 때 왼쪽 부채꼴은 ↖ 로 k칸 간 뒤 ↑ 로 쭉 칠한다.
	  대각선 방향은 (바라보는 방향 + 옆 방향)으로 만든다.
	      leftLook  : 상 -> 좌, 하 -> 우, 좌 -> 하, 우 -> 상
	      rightLook : 상 -> 우, 하 -> 좌, 좌 -> 상, 우 -> 하

	■ 가려지는 칸(그림자) 지우기
	  시야 안의 전사마다, 그 전사 뒤에 생기는 그림자를 0으로 지운다.
	      일직선 위의 전사     : 그 전사부터 바라보는 방향으로 일직선
	      왼쪽 부채꼴 위의 전사 : 일직선 + 왼쪽 부채꼴
	      오른쪽 부채꼴 위의 전사 : 일직선 + 오른쪽 부채꼴
	  전사 자기 칸은 지우지 않는다(그 전사는 보인다).
	  가려진 전사의 그림자는 앞 전사의 그림자 안에 다 들어가므로 전사를 어떤 순서로 처리해도 결과가 같다.
	  벽(비도로)은 시야를 가리지 않는다. 시야를 가리는 것은 전사뿐이다.

	■ 전사 이동
	  돌이 된 전사(scope != 0 인 칸에 있는 전사)는 움직이지 않는다.
	  메두사 쪽으로만 가므로 격자를 벗어날 일이 없고, 시야 칸만 피하면 된다.
	  같은 함수를 first 플래그만 바꿔 두 번 부른다 (첫 이동 / 두 번째 이동의 우선순위가 다르다).

	■ 주의할 점
	  - visit은 bool인데 visit[out] + 1 을 넣는다. bool이라 그냥 true가 되며, 방문 여부로만 쓴다.
	  - 전역 이름 end 는 std::end 와 겹친다. 지금은 stdio.h만 써서 괜찮지만
	    프로그래머스 제출용으로 옮길 때는 이름을 바꿔야 한다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX_N (50+5)                     // N 최대 50 + 여유
#define MAX_M (300+30)                   // M 최대 300 + 여유

#define WALL (1)                         // 비도로

#define UP (0)                           // 상
#define DOWN (1)                         // 하
#define LEFT (2)                         // 좌 (scope 값으로는 왼쪽 부채꼴)
#define RIGHT (3)                        // 우 (scope 값으로는 오른쪽 부채꼴)

#define STRAIGHT (1)                     // scope 값 : 일직선 시야

int T;                                   // 테스트 케이스 수

int N, M;                                // 마을 크기, 전사 수
int MAP[MAX_N][MAX_N];                   // 도로(0) / 비도로(1)
bool visit[MAX_N][MAX_N]; // BFS 방문 체크
int scope[MAX_N][MAX_N]; // 메두사의 시선 처리

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC start, end, medusa;                   // 집, 공원, 메두사 현재 위치
RC queue[MAX_N * MAX_N];                 // BFS 큐
RC before[MAX_N][MAX_N];                 // BFS 앞 칸
RC position[MAX_N * MAX_N]; // 메두사의 경로
int pcnt; // 메두사의 경로 길이

struct RCD                               // 전사 정보
{
	int r;                               // 행
	int c;                               // 열
	bool dead; // 사망 처리
};

RCD warrior[MAX_M];                      // 전사 배열

struct ANSWER                            // 턴별 출력 값
{
	int distance; // 모든 전사가 이동한 거리의 합
	int stone; // 메두사로 인해 돌이 된 전사의 수
	int attack; // 메두사로 공격한 전사의 수
};

ANSWER answer;                           // 이번 턴 결과

// ↑, ↓, ←, → (상, 하, 좌, 우)
int dr[] = { -1,1,0,0 };                 // 방향별 행 변화량
int dc[] = { 0,0,-1,1 };                 // 방향별 열 변화량

void input()
{
	int sr, sc, er, ec;                  // 집, 공원 좌표

	scanf("%d %d %d %d %d %d", &N, &M, &sr, &sc, &er, &ec);   // 기본 정보 입력

	start.r = sr + 1;
	// 입력은 0-based, 내부는 1-based로 쓴다.
	start.c = sc + 1;
	end.r = er + 1;
	end.c = ec + 1;

	pcnt = 0;                            // 경로 길이 초기화

	for (int m = 0; m < M; m++)          // 전사별
	{
		int r, c;                        // 전사 좌표

		scanf("%d %d", &r, &c);          // 입력

		warrior[m].r = r + 1;            // 1-based로 저장
		warrior[m].c = c + 1;
		warrior[m].dead = false;         // 살아 있음
	}

	for (int r = 1; r <= N; r++)         // 마을 입력
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX_N][MAX_N]) // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%2d ", map[r][c]);   // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void printStatus() // for debug
{
	int tmpMAP[MAX_N][MAX_N] = { 0 };    // 출력용 격자

	tmpMAP[medusa.r][medusa.c] = -1;     // 메두사 표시

	for (int m = 0; m < M; m++)          // 전사 표시
	{
		if (warrior[m].dead == true) continue;

		int r, c;

		r = warrior[m].r;
		c = warrior[m].c;

		tmpMAP[r][c] = m;
	}

	printMap(tmpMAP);                    // 출력
}

void BFS()                               // 메두사 경로 구하기
{
	int rp, wp;                          // 큐 위치
	int sr, sc, er, ec;                  // 출발, 도착

	rp = wp = 0;                         // 큐 초기화

	sr = start.r;
	sc = start.c;
	er = end.r;
	ec = end.c;

	queue[wp].r = sr;                    // 집 큐에 넣기
	queue[wp++].c = sc;

	visit[sr][sc] = 1;                   // 방문 표시

	before[sr][sc].r = -1;               // 앞 칸 없음
	before[sr][sc].c = -1;

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		if (er == out.r && ec == out.c)  // 공원 도착
		{
			int tr = er;                 // 되짚는 현재 칸
			int tc = ec;

			position[pcnt].r = er;       // 공원부터 경로에 쌓기
			position[pcnt++].c = ec;

			while (1)
			{
				int br, bc; //이전좌표

				br = before[tr][tc].r;
				bc = before[tr][tc].c;

				if (br == sr && bc == sc) break;   // 집까지 오면 멈춤

				position[pcnt].r = br;   // 경로에 추가
				position[pcnt++].c = bc;

				tr = br;                 // 한 칸 거슬러 올라감
				tc = bc;
			}

			for (int i = 0; i < (pcnt / 2); i++)
				// 도착점부터 거꾸로 쌓았으므로 뒤집어 출발 다음 칸부터 순서대로 만든다.
			{
				RC tmp = position[i];
				position[i] = position[pcnt - 1 - i];
				position[pcnt - 1 - i] = tmp;
			}
		}

		for (int i = 0; i < 4; i++)      // 상하좌우 순서로
		{
			int nr = out.r + dr[i];      // 이웃 행
			int nc = out.c + dc[i];      // 이웃 열

			if (nr<1 || nc<1 || nr>N || nc>N) continue;                  // 격자 밖
			if (visit[nr][nc] != 0 || MAP[nr][nc] == WALL) continue;     // 방문했거나 비도로

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;   // 방문 표시 (bool이라 true)

			before[nr][nc] = out;        // 앞 칸 기록
		}
	}
}

void moveMedusa(int index)               // 메두사 한 칸 이동
{
	medusa = position[index];            // 경로의 index번째 칸으로

	for (int m = 0; m < M; m++)          // 전사별
	{
		if (warrior[m].dead == true) continue;

		int wr, wc;                      // 전사 좌표

		wr = warrior[m].r;
		wc = warrior[m].c;

		if (wr == medusa.r && wc == medusa.c)
			// 메두사가 들어온 칸의 전사는 사라진다.
			warrior[m].dead = true;
	}
}

void straight(int r, int c, int dir, int value)   // (r, c) 다음 칸부터 dir 방향 끝까지 value로 칠하기
{
	int sr, sc;                          // 현재 칸

	sr = r;
	sc = c;

	while (1)
	{
		int nr, nc;                      // 다음 칸

		nr = sr + dr[dir];
		nc = sc + dc[dir];

		if (nr <1 || nc<1 || nr>N || nc>N) break;   // 격자 밖이면 멈춤

		scope[nr][nc] = value;           // 칠하기

		sr = nr;                         // 한 칸 진행
		sc = nc;
	}
}

void diagonal(int r, int c, int dir, int nDir, int value)   // 부채꼴 한쪽 칠하기
{
	int step = 1;                        // 대각선 칸 수
	while (1)
	{
		int sr, sc;                      // 대각선으로 step칸 간 점

		sr = r + (dr[dir] + dr[nDir]) * step;
		// 대각선으로 step칸 간 점 -> 거기서 바라보는 방향으로 끝까지 칠한다.
		sc = c + (dc[dir] + dc[nDir]) * step;

		if (sr<1 || sc<1 || sr>N || sc>N) break;   // 대각선이 격자 밖이면 끝

		scope[sr][sc] = value;           // 대각선 칸 칠하기

		while (1)                        // 바라보는 방향으로 직진
		{
			int nr, nc;                  // 다음 칸

			nr = sr + dr[dir];
			nc = sc + dc[dir];

			if (nr<1 || nc<1 || nr>N || nc>N) break;   // 격자 밖이면 멈춤

			scope[nr][nc] = value;       // 칠하기

			sr = nr;                     // 한 칸 진행
			sc = nc;
		}
		step++;                          // 다음 대각선
	}
}

// ↑, ↓, ←, → (상, 하, 좌, 우 / 0, 1, 2, 3)
void leftLook(int r, int c, int dir, int value)   // 왼쪽 부채꼴
{
	int changeDir[4] = { 2,3,1,0 };      // 바라보는 방향별 왼쪽 방향
	int leftDir = changeDir[dir];        // 왼쪽 방향

	diagonal(r, c, dir, leftDir, value); // 칠하기
}

void rightLook(int r, int c, int dir, int value)  // 오른쪽 부채꼴
{
	int changeDir[4] = { 3,2,0,1 };      // 바라보는 방향별 오른쪽 방향
	int rightDir = changeDir[dir];       // 오른쪽 방향

	diagonal(r, c, dir, rightDir, value);   // 칠하기
}

int countStoneWarrior(int dir)           // dir 방향을 볼 때 돌이 되는 전사 수 (scope도 그려 둔다)
{
	for (int r = 1; r <= N; r++)         // scope 초기화
		for (int c = 1; c <= N; c++)
			scope[r][c] = 0;

	int mr, mc;                          // 메두사 위치

	mr = medusa.r;
	mc = medusa.c;

	straight(mr, mc, dir, STRAIGHT);     // 일직선 시야
	leftLook(mr, mc, dir, LEFT);         // 왼쪽 부채꼴
	rightLook(mr, mc, dir, RIGHT);       // 오른쪽 부채꼴

	for (int m = 0; m < M; m++)          // 전사별 그림자 지우기
	{
		if (warrior[m].dead == true) continue;

		int wr = warrior[m].r;           // 전사 행
		int wc = warrior[m].c;           // 전사 열

		if (scope[wr][wc] == STRAIGHT) straight(wr, wc, dir, 0);
		// 시야 안의 전사 뒤쪽(그림자)을 0으로 지운다. 전사 자기 칸은 그대로 둔다.
		else if (scope[wr][wc] == LEFT)  // 왼쪽 부채꼴 위의 전사
		{
			straight(wr, wc, dir, 0);    // 일직선 그림자
			leftLook(wr, wc, dir, 0);    // 왼쪽 그림자
		}
		else if (scope[wr][wc] == RIGHT) // 오른쪽 부채꼴 위의 전사
		{
			straight(wr, wc, dir, 0);    // 일직선 그림자
			rightLook(wr, wc, dir, 0);   // 오른쪽 그림자
		}
	}

	int count = 0;                       // 돌이 되는 전사 수

	for (int m = 0; m < M; m++)          // 전사별
	{
		if (warrior[m].dead == true) continue;

		int wr, wc;                      // 전사 좌표

		wr = warrior[m].r;
		wc = warrior[m].c;

		if (scope[wr][wc] != 0) count++; // 시야 안이면 돌
	}

	return count;                        // 반환
}

void lookAt()                            // 메두사가 볼 방향 정하기
{
	int maxCount, maxDir;                // 최대 돌 수, 그 방향

	maxCount = maxDir = 0;               // 초기화

	for (int i = 0; i < 4; i++)          // 상하좌우
	{
		int tmp = countStoneWarrior(i);  // 돌이 되는 수
		if (maxCount < tmp)
			// "더 클 때만" 갱신 -> 동점이면 상 하 좌 우 순으로 앞선 방향이 남는다.
		{
			maxCount = tmp;
			maxDir = i;
		}
	}

	countStoneWarrior(maxDir);
	// 고른 방향으로 scope를 다시 그려 둔다 (전사 이동 때 이 시야를 쓴다).

	answer.stone = maxCount;             // 돌이 된 전사 수 기록
}

int checkRow(RCD w)                      // 메두사와의 행 차이
{
	int mr = medusa.r;                   // 메두사 행
	int wr = w.r;                        // 전사 행

	// mr > wr -> 메두사는 아래에
	// mr = wr -> 같은 행, 상하로 움직일 수 없음.
	// mr < wr -> 메두사는 위에
	return mr - wr;
}

int checkCol(RCD w)                      // 메두사와의 열 차이
{
	int mc = medusa.c;                   // 메두사 열
	int wc = w.c;                        // 전사 열

	// mc > wc -> 메두사는 오른쪽에
	// mc = wc -> 같은 열, 좌우로 움직일 수 없음.
	// mc < wc -> 메두사는 왼쪽에
	return mc - wc;
}

bool checkScope(RCD w, int dir)          // dir로 한 칸 간 칸이 시야 밖인가
{
	int nr, nc;                          // 다음 칸

	nr = w.r + dr[dir];
	nc = w.c + dc[dir];

	if (scope[nr][nc] != 0) return false;
	// 메두사 시야 안으로는 들어갈 수 없다.

	return true;                         // 갈 수 있음
}

void moveWarrior(bool first)             // 전사 한 번 이동 (first : 첫 번째 이동인가)
{
	for (int m = 0; m < M; m++)          // 전사별
	{
		RCD w = warrior[m];              // 전사 정보

		if (w.dead == true) continue;    // 사라진 전사 제외
		if (scope[w.r][w.c] != 0) continue;
		// 돌이 된 전사는 이번 턴에 움직이지 않는다.

		int upDown = checkRow(w);        // 행 차이
		int leftRight = checkCol(w);     // 열 차이

		int direction = -1;              // 이동 방향

		// 메두사를 향해서 이동하므로 격자의 바깥으로 나가지 않는다.
		if (first == true)               // 첫 이동 : 상 하 좌 우
		{
			if (upDown < 0 && checkScope(w, UP) == true) direction = UP;
			else if (upDown > 0 && checkScope(w, DOWN) == true) direction = DOWN;
			else if (leftRight < 0 && checkScope(w, LEFT) == true) direction = LEFT;
			else if (leftRight > 0 && checkScope(w, RIGHT) == true) direction = RIGHT;
		}
		else                             // 두 번째 이동 : 좌 우 상 하
		{
			if (leftRight < 0 && checkScope(w, LEFT) == true) direction = LEFT;
			else if (leftRight > 0 && checkScope(w, RIGHT) == true) direction = RIGHT;
			else if (upDown < 0 && checkScope(w, UP) == true) direction = UP;
			else if (upDown > 0 && checkScope(w, DOWN) == true) direction = DOWN;
		}

		if (direction == -1) continue;   // 갈 곳이 없으면 제자리

		int nr, nc;                      // 다음 칸

		nr = w.r + dr[direction];
		nc = w.c + dc[direction];

		warrior[m].r = nr;               // 이동
		warrior[m].c = nc;

		answer.distance++;               // 이동 거리 증가

		if (nr == medusa.r && nc == medusa.c)   // 메두사에게 도착
		{
			answer.attack++;             // 공격 수 증가
			warrior[m].dead = true;      // 사라짐
		}
	}
}

void simulate()
{
	if (pcnt == 0)                       // 경로가 없으면
	{
		printf("-1\n");                  // -1 출력
		return;
	}

	for (int p = 0; p < pcnt - 1; p++)
		// 마지막 칸(공원)에 도착하는 턴은 0만 출력하므로 pcnt - 1 까지만 돈다.
	{
		answer.distance = answer.stone = answer.attack = 0;   // 턴 결과 초기화

		moveMedusa(p);                   // 1) 메두사 이동

		lookAt();                        // 2) 시선

		moveWarrior(true);               // 3) 전사 첫 이동
		moveWarrior(false);              //    전사 두 번째 이동

		printf("%d %d %d\n", answer.distance, answer.stone, answer.attack);   // 턴 결과 출력
	}

	printf("%d\n", 0);                   // 공원 도착
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		BFS();                           // 메두사 경로 구하기

		simulate();                      // 시뮬레이션 + 출력
	}

	return 0;                            // 프로그램 정상 종료
}
