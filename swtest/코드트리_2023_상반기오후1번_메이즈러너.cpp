/*
	[코드트리] 2023 상반기 오후 1번 - 메이즈 러너
	https://www.codetree.ai/training-field/frequent-problems/problems/maze-runner

	■ 문제 요약
	  N x N 미로에 참가자 M명과 출구 하나가 있다. 각 칸은 0(빈칸) 또는 1~9(벽의 내구도)다.
	  K초 동안 아래를 반복한다.

	    1) 모든 참가자가 동시에 한 칸 움직인다.
	       출구까지의 최단 거리(|행 차이| + |열 차이|)가 줄어드는 방향으로만 가고,
	       상하로 줄일 수 있으면 상하, 아니면 좌우로 간다. 벽이면 못 가고 제자리에 있는다.
	       움직인 참가자 수만큼 이동 거리가 쌓인다.
	    2) 출구에 도착한 참가자는 즉시 탈출한다(이후 움직이지도, 회전에 포함되지도 않는다).
	    3) 모두 탈출하지 못했으면 미로를 회전한다.
	       "탈출하지 못한 참가자 한 명 이상과 출구를 함께 포함하는 가장 작은 정사각형"을 고른다.
	       여러 개면 좌상단 행이 작은 것, 그다음 열이 작은 것.
	       그 정사각형을 시계 방향 90도로 돌리고, 회전한 구역의 벽은 내구도가 1 줄어든다.

	  K초가 지나거나 모두 탈출하면 끝난다.
	  출력은 두 줄이다. 첫 줄은 참가자들의 총 이동 거리, 둘째 줄은 마지막 출구 좌표다.

	■ 풀이 방침
	  - 이동은 "상하 먼저, 좌우 나중"이라는 우선순위만 지키면 된다.
	    출구 쪽으로만 가므로 격자를 벗어날 일이 없어 경계 검사가 필요 없다.
	        checkRow    : 출구가 위(음수) / 같은 행(0) / 아래(양수)
	        checkColumn : 출구가 왼쪽(음수) / 같은 열(0) / 오른쪽(양수)
	  - 회전할 정사각형은 크기 2부터 키워 가며 처음 찾은 것을 쓴다.
	    "가장 작은 것 -> 행이 작은 것 -> 열이 작은 것"이 곧 for문 순서(size, r, c)라 별도 비교가 없다.
	  - 참가자와 출구가 그 안에 있는지는 임시 격자에 사람과 출구를 찍어 두고 확인한다
	    (사람 = m + PLAYER_NUMBER, 출구 = DESTINATION).
	    벽 내구도(1~9)와 섞이지 않도록 사람 번호를 10부터 쓴 것이다.

	■ 좌표도 같이 회전시키는 방법
	  rotateRC는 "그 좌표에만 1을 찍은 빈 격자"를 만들어 같은 회전을 돌린 뒤 1을 다시 찾는다.
	  회전 공식을 좌표용으로 따로 쓰지 않아도 되고, 회전 구역 밖의 좌표는 그대로 남는다.
	  (map[sr+r][sc+c] = tmp[size-1-c][r] 이 시계 방향 90도다)

	■ 주의할 점
	  [확인 필요] getSqureInfo가 조건을 만족하는 정사각형을 못 찾으면 {0, 0, 0}을 돌려준다.
	              그 뒤 rotateRC가 (0, 0)을 반환해 출구와 참가자 좌표가 (0, 0)이 된다.
	              문제 조건상 남은 참가자와 출구를 포함하는 정사각형은 항상 있으므로 생기지 않는다.
	  - 내구도 감소는 회전한 구역 안의 "벽(0이 아닌 칸)"만 대상이다. 참가자와 출구는 MAP에 없다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (10+5)                       // N 최대 10 + 여유
#define PLAYER_NUMBER (10)               // 임시 격자에서 사람 번호 시작값 (벽 1~9와 구분)
#define DESTINATION (-1)                 // 임시 격자에서 출구 표시

#define UP (0)                           // 상
#define RIGHT (1)                        // 우
#define DOWN (2)                         // 하
#define LEFT (3)                         // 좌

int T;                                   // 테스트 케이스 수

int N, M, K;                             // 미로 크기, 참가자 수, 시간
int MAP[MAX][MAX];                       // 0 빈칸, 1~9 벽 내구도

struct RCE                               // 좌표 + 탈출 여부
{
	int r;                               // 행
	int c;                               // 열
	bool escape; // player 탈출 확인
};

RCE player[MAX];                         // 참가자들
RCE destination;                         // 출구

struct RCS                               // 회전할 정사각형 정보
{
	int r;                               // 좌상단 행
	int c;                               // 좌상단 열
	int size; // 회전을 위한 크기
};

// ↑, →, ↓, ←
int dr[] = { -1,0,1,0 };                 // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 미로 크기, 참가자 수, 시간 입력

	for (int r = 1; r <= N; r++)         // 모든 칸
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);     // 미로 입력

	for (int m = 0; m < M; m++)          // 참가자별
	{
		scanf("%d %d", &player[m].r, &player[m].c);   // 참가자 좌표 입력
		player[m].escape = false;        // 아직 탈출 안 함
	}

	scanf("%d %d", &destination.r, &destination.c);   // 출구 좌표 입력
}

void printMap(int map[MAX][MAX]) // for debug
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
	int tmpMAP[MAX][MAX] = { 0 };        // 출력용 임시 격자
	for (int r = 1; r <= N; r++)         // 미로 복사
		for (int c = 1; c <= N; c++)
			tmpMAP[r][c] = MAP[r][c];

	for (int m = 0; m < M; m++)          // 참가자별
	{
		RCE p = player[m];               // 참가자 정보

		printf("%d : %d, %d (exit : %d)\n", m, p.r, p.c, p.escape);   // 상태 출력
		if (p.escape == false)           // 탈출 안 했으면
			tmpMAP[p.r][p.c] = m + PLAYER_NUMBER;   // 격자에 표시

		putchar('\n');                   // 빈 줄

		tmpMAP[destination.r][destination.c] = DESTINATION;   // 출구 표시

		printMap(tmpMAP);                // 격자 출력
	}
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])   // 격자 복사
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			copy[r][c] = original[r][c];
}

bool check()                             // 모두 탈출했는가
{
	for (int m = 0; m < M; m++)
		if (player[m].escape == false) return false;   // 한 명이라도 남았으면 false

	return true;                         // 모두 탈출
}

int checkRow(RCE player)                 // 출구와의 행 차이
{
	int dr = destination.r;              // 출구 행
	int pr = player.r;                   // 참가자 행
	// dr > pr -> 출구는 아래에
	// dr = pr -> 같은 행, 상하로 움직일 수 없음.
	// dr < pr -> 출구는 위에
	return dr - pr;
}

int checkColumn(RCE player)              // 출구와의 열 차이
{
	int dc = destination.c;              // 출구 열
	int pc = player.c;                   // 참가자 열
	// dc > pc -> 출구는 오른쪽에
	// dc = pc -> 같은 열, 좌우로 움직일 수 없음.
	// dc < pc -> 출구는 왼쪽에
	return dc - pc;
}

int move()                               // 모든 참가자 한 칸 이동, 움직인 사람 수 반환
{
	int step = 0;                        // 이번 이동 거리 합
	for (int m = 0; m < M; m++)          // 참가자별
	{
		if (player[m].escape == true) continue;   // 탈출한 사람 제외

		int nextR[4] = { 0 };            // 방향별 다음 행
		int nextC[4] = { 0 };            // 방향별 다음 열

		for (int i = 0; i < 4; i++)      // 4방향 다음 칸 계산
		{
			nextR[i] = player[m].r + dr[i];
			nextC[i] = player[m].c + dc[i];
		}

		int upDown = checkRow(player[m]);        // 행 차이
		int leftRight = checkColumn(player[m]);  // 열 차이

		int direction = -1;              // 이동 방향 (없으면 -1)

		// 항상 출구를 향해 움직이기 때문에 격자를 벗어나지 않음.
			// 상하로 줄일 수 있으면 상하, 아니면 좌우. (벽이면 그 방향은 못 쓴다)
		if (upDown < 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = UP;
		else if (upDown > 0 && MAP[nextR[DOWN]][nextC[DOWN]] == 0)direction = DOWN;
		else if (leftRight < 0 && MAP[nextR[LEFT]][nextC[LEFT]] == 0)direction = LEFT;
		else if (leftRight > 0 && MAP[nextR[RIGHT]][nextC[RIGHT]] == 0)direction = RIGHT;

		if (direction == -1) continue;
			// 네 방향 모두 막혔으면 제자리 (이동 거리도 늘지 않는다)

		player[m].r = player[m].r + dr[direction];   // 한 칸 이동
		player[m].c = player[m].c + dc[direction];

		if (player[m].r == destination.r && player[m].c == destination.c)   // 출구 도착
			player[m].escape = true;     // 탈출

		step++;                          // 이동 거리 1 증가
	}
	return step;                         // 이동 거리 반환
}


bool rotateCheck(int map[MAX][MAX], int sr, int sc, int size)   // 정사각형 안에 참가자와 출구가 다 있는가
{
	bool destinationCheck, playerCheck;  // 출구 / 참가자 포함 여부

	destinationCheck = playerCheck = false;   // 초기화

	for (int r = sr; r < sr + size; r++)      // 정사각형 안
	{
		for (int c = sc; c < sc + size; c++)
		{
			if (map[r][c] >= PLAYER_NUMBER) playerCheck = true;          // 참가자 있음
			else if (map[r][c] == DESTINATION) destinationCheck = true;  // 출구 있음
		}
	}
	return playerCheck && destinationCheck;   // 둘 다 있어야 true
}

RCS getSqureInfo()                       // 회전할 정사각형 찾기
{
	RCS ret = { 0 };                     // 결과
	for (int size = 2; size <= N; size++)
		// 크기 2부터 키우고, 같은 크기면 행 -> 열 순서로 본다. 처음 찾은 것이 곧 정답이다.
	{
		for (int r = 1; r <= N - size + 1; r++)       // 좌상단 행
		{
			for (int c = 1; c <= N - size + 1; c++)   // 좌상단 열
			{
				int tmpMAP[MAX][MAX] = { 0 };          // 임시 격자

				copyMap(tmpMAP, MAP);                  // 미로 복사

				for (int m = 0; m < M; m++)            // 남은 참가자 표시
				{
					if (player[m].escape == true) continue;

					tmpMAP[player[m].r][player[m].c] = m + PLAYER_NUMBER;
				}

				tmpMAP[destination.r][destination.c] = DESTINATION;   // 출구 표시
				if (rotateCheck(tmpMAP, r, c, size) == true)          // 조건 만족하면
				{
					ret.r = r;           // 좌상단 행
					ret.c = c;           // 좌상단 열
					ret.size = size;     // 크기

					return ret;          // 바로 반환
				}
			}
		}
	}

	// for debug
	//ret.r = ret.c = ret.size = 1;
	return ret;                          // 못 찾음 (0, 0, 0)
}


void rotate(int map[MAX][MAX], int sr, int sc, int size)   // 정사각형 시계 방향 90도 회전
{
	int tmpMAP[MAX][MAX] = { 0 };        // 회전 전 복사본

	for (int r = 0; r < size; r++)       // 정사각형 부분 복사
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = map[sr + r][sc + c];

	for (int r = 0; r < size; r++)       // 회전해서 다시 쓰기
		for (int c = 0; c < size; c++)
			map[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
	// (r, c) <- (size-1-c, r) 가 시계 방향 90도다.

}

RCE rotateRC(RCE rce, RCS squareInfo)    // 좌표 하나를 같은 방식으로 회전
{
	RCE ret = { 0 };                     // 결과 (escape는 false)
	int tmpMAP[MAX][MAX] = { 0 };        // 빈 격자

	tmpMAP[rce.r][rce.c] = 1;
	// 좌표 하나만 찍은 빈 격자를 같은 방식으로 돌려서 옮겨진 자리를 찾는다.

	rotate(tmpMAP, squareInfo.r, squareInfo.c, squareInfo.size);   // 회전

	for (int r = 1; r <= N; r++)         // 1이 옮겨간 자리 찾기
	{
		for (int c = 1; c <= N; c++)
		{
			if (tmpMAP[r][c] == 1)
			{
				ret.r = r;               // 새 행
				ret.c = c;               // 새 열

				return ret;              // 반환
			}
		}
	}

	// for debug
	//ret.r = ret.c = -1;
	return ret;                          // 못 찾음 (0, 0)
}

void durabilityDown(RCS squareInfo)      // 회전 구역 벽 내구도 1 감소
{
	int sr = squareInfo.r;               // 시작 행
	int sc = squareInfo.c;               // 시작 열
	int er = squareInfo.r + squareInfo.size;   // 끝 행 (미포함)
	int ec = squareInfo.c + squareInfo.size;   // 끝 열 (미포함)

	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0) continue;
			// 내구도가 0인 칸(빈칸)은 더 깎을 것이 없다.

			if (sr <= r && r < er && sc <= c && c < ec)   // 회전 구역 안이면
				MAP[r][c]--;             // 내구도 감소
		}
	}
}

void rotateMaze()                        // 미로 회전 전체 처리
{
	RCS squareInfo = getSqureInfo();     // 회전할 정사각형

	rotate(MAP, squareInfo.r, squareInfo.c, squareInfo.size);   // 미로 회전

	destination = rotateRC(destination, squareInfo);            // 출구 좌표 회전

	for (int m = 0; m < M; m++)          // 남은 참가자 좌표 회전
	{
		if (player[m].escape == true) continue;

		player[m] = rotateRC(player[m], squareInfo);
	}

	durabilityDown(squareInfo);          // 내구도 감소
}

void simulate()
{
	int sum = 0;                         // 총 이동 거리
	for (int k = 0; k < K; k++)          // K초
	{
		int step = move();               // 1) 이동
		sum += step;                     // 이동 거리 누적

		if (check() == true) break;      // 2) 모두 탈출했으면 종료

		rotateMaze();                    // 3) 미로 회전
		//printStatus();

	}

	printf("%d\n", sum);                 // 총 이동 거리 출력
	printf("%d %d\n", destination.r, destination.c);   // 출구 좌표 출력
}

int main()
{
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		//printStatus();

		simulate();                      // 시뮬레이션 + 출력

	}

	return 0;                            // 프로그램 정상 종료
}
