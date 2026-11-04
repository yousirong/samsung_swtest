/*
	[코드트리] 2023 상반기 오후 1번 - 메이즈 러너 (연습)
	풀이 설명은 코드트리_2023_상반기오후1번_메이즈러너.cpp 참고.

	■ 본 코드와 다른 점 / 버그 (주석으로만 표시, 코드는 그대로)
	  [버그1] move() : 네 방향 모두 MAP[nextR[UP]][nextC[UP]]로 벽을 검사한다.
	          DOWN/LEFT/RIGHT는 각각 nextR[DOWN]..., nextR[LEFT]..., nextR[RIGHT]...여야 한다.
	  [버그2] rotateCheck() : return destinationCheck && destinationCheck;
	          -> playerCheck && destinationCheck 여야 한다. (지금은 참가자가 없어도 통과한다)
	  [버그3] main()에서 printStatus()를 부른다. 디버그 출력이 정답 출력 앞에 섞여 오답이 된다.
	  [참고] getSqureInfo / rotateRC가 못 찾았을 때 -1을 돌려주게 바뀌어 있다.
	         그 값으로 rotate를 부르면 배열 밖을 건드리므로 디버그 용도로만 봐야 한다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (10+5)                       // N 최대 10 + 여유

#define PLAYER_NUMBER (10)               // 임시 격자 사람 번호 시작값
#define DESTINATION (-1)                 // 임시 격자 출구 표시

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
	bool escape; //  Player 탈출 확인
};

RCE player[MAX];                         // 참가자들
RCE destination;                         // 출구

struct RCS                               // 회전할 정사각형
{
	int r;                               // 좌상단 행
	int c;                               // 좌상단 열
	int size; // 회전을 위한 크기
};

// 상, 우, 하, 좌
int dr[] = { -1, 0,1,0 };                // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 미로 크기, 참가자 수, 시간 입력

	for (int r = 1; r <= N; r++)         // 모든 칸
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);     // 미로 입력

	for (int m = 0; m < M; m++)          // 참가자별
	{
		scanf("%d %d", &player[m].r, &player[m].c);   // 좌표 입력
		player[m].escape = false;        // 아직 탈출 안 함
	}

	scanf("%d %d", &destination.r, &destination.c);   // 출구 입력
}

void printMap(int map[MAX][MAX])         // for debug
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
	int tmpMAP[MAX][MAX] = { 0 };        // 임시 격자
	for (int r = 1; r <= N; r++)         // 미로 복사
		for (int c = 1; c <= N; c++)
			tmpMAP[r][c] = MAP[r][c];

	for (int m = 0; m < M; m++)          // 참가자별
	{
		RCE p = player[m];               // 참가자 정보

		printf("%d : %d, %d (exit : %d)\n", m, p.r, p.c, p.escape);   // 상태 출력
		if (p.escape == false)           // 탈출 안 했으면
			tmpMAP[p.r][p.c] = m + PLAYER_NUMBER;   // 격자에 표시
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
	for(int m = 0; m < M; m++)
		if (player[m].escape == false)return false;   // 남은 사람 있으면 false

	return true;                         // 모두 탈출
}

int checkRow(RCE player)                 // 출구와의 행 차이
{
	int dr = destination.r;              // 출구 행
	int pr = player.r;                   // 참가자 행

	return dr - pr;                      // 음수 위, 양수 아래
}

int checkColumn(RCE player)              // 출구와의 열 차이
{
	int dc = destination.c;              // 출구 열
	int pc = player.c;                   // 참가자 열

	return dc - pc;                      // 음수 왼쪽, 양수 오른쪽
}

int move()                               // 참가자 이동
{
	int step = 0;                        // 이동 거리 합
	for (int m = 0; m < M; m++)          // 참가자별
	{
		if (player[m].escape == true) continue;   // 탈출한 사람 제외

		int nextR[4] = { 0 };            // 방향별 다음 행
		int nextC[4] = { 0 };            // 방향별 다음 열

		for (int i = 0; i < 4; i++)      // 4방향 다음 칸
		{
			nextR[i] = player[m].r + dr[i];
			nextC[i] = player[m].c + dc[i];
		}

		int upDown = checkRow(player[m]);        // 행 차이
		int leftRight = checkColumn(player[m]);  // 열 차이

		int direction = -1;              // 이동 방향

		if (upDown < 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = UP;
		else if (upDown > 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = DOWN;     // ※ 버그1 : DOWN 칸을 봐야 함
		else if (leftRight < 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = LEFT;  // ※ 버그1 : LEFT 칸을 봐야 함
		else if (leftRight > 0 && MAP[nextR[UP]][nextC[UP]] == 0) direction = RIGHT; // ※ 버그1 : RIGHT 칸을 봐야 함

		if (direction == -1) continue;   // 갈 곳이 없으면 제자리

		player[m].r = player[m].r + dr[direction];   // 한 칸 이동
		player[m].c = player[m].c + dc[direction];

		if (player[m].r == destination.r && player[m].c == destination.c)   // 출구 도착
			player[m].escape = true;     // 탈출

		step++;                          // 이동 거리 증가

	}

	return step;                         // 이동 거리 반환
}

bool rotateCheck(int map[MAX][MAX], int sr, int sc, int size)   // 정사각형 안에 참가자와 출구가 있는가
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
	return destinationCheck && destinationCheck;   // ※ 버그2 : playerCheck && destinationCheck 여야 함
}


RCS getSqureInfo()                       // 회전할 정사각형 찾기
{
	RCS ret = { 0 };                     // 결과
	for (int size = 2; size <= N; size++)            // 작은 크기부터
	{
		for (int r = 1; r <= N - size + 1; r++)      // 좌상단 행
		{
			for (int c = 1; c <= N - size + 1; c++)  // 좌상단 열
			{
				int tmpMAP[MAX][MAX] = { 0 };         // 임시 격자
				copyMap(tmpMAP, MAP);                 // 미로 복사

				for (int m = 0; m < M; m++)           // 남은 참가자 표시
				{
					if (player[m].escape == true) continue;

					tmpMAP[player[m].r][player[m].c] = m + PLAYER_NUMBER;

				}
				tmpMAP[destination.r][destination.c] = DESTINATION;   // 출구 표시

				if (rotateCheck(tmpMAP, r, c, size) == true)          // 조건 만족
				{
					ret.r = r;           // 좌상단 행
					ret.c = c;           // 좌상단 열
					ret.size = size;     // 크기

					return ret;          // 반환
				}
			}
		}
	}

	//for debug
	ret.r = ret.c = ret.size = -1;       // 못 찾음 표시

	return ret;                          // 반환
}

void rotate(int map[MAX][MAX], int sr, int sc, int size)   // 시계 방향 90도 회전
{
	int tmpMAP[MAX][MAX] = { 0 };        // 회전 전 복사본

	for (int r = 0; r < size; r++)       // 정사각형 부분 복사
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = map[sr + r][sc + c];

	for (int r = 0; r < size; r++)       // 회전해서 다시 쓰기
		for (int c = 0; c < size; c++)
			map[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
}

RCE rotateRC(RCE rce, RCS squareInfo)    // 좌표 하나 회전
{
	RCE ret = { 0 };                     // 결과
	int tmpMAP[MAX][MAX] = { 0 };        // 빈 격자

	tmpMAP[rce.r][rce.c] = 1;            // 좌표 하나만 표시

	rotate(tmpMAP, squareInfo.r, squareInfo.c, squareInfo.size);   // 같은 방식으로 회전

	for (int r = 1; r <= N; r++)         // 옮겨간 자리 찾기
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

	//for debug
	ret.r = ret.c = -1;                  // 못 찾음 표시

	return ret;                          // 반환
}

void durabilityDown(RCS squareInfo)      // 회전 구역 내구도 감소
{
	int sr = squareInfo.r;               // 시작 행
	int er = squareInfo.r + squareInfo.size;   // 끝 행 (미포함)
	int sc = squareInfo.c;               // 시작 열
	int ec = squareInfo.c + squareInfo.size;   // 끝 열 (미포함)

	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0) continue;   // 빈칸 제외

			if (sr <= r && r < er && sc <= c && c < ec)   // 회전 구역 안이면
				MAP[r][c]--;             // 내구도 감소
		}
	}
}

void rotateMaze()                        // 미로 회전 전체
{
	RCS squareInfo = getSqureInfo();     // 회전할 정사각형

	rotate(MAP, squareInfo.r, squareInfo.c, squareInfo.size);   // 미로 회전

	destination = rotateRC(destination, squareInfo);            // 출구 회전

	for (int m = 0; m < M; m++)          // 남은 참가자 회전
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
		int step = move();               // 이동

		sum += step;                     // 누적

		if (check() == true) break;      // 모두 탈출하면 종료

		rotateMaze();                    // 회전

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

		printStatus();                   // ※ 버그3 : 디버그 출력이 정답에 섞임 (지워야 함)

		simulate();                      // 시뮬레이션 + 출력

	}
	return 0;                            // 프로그램 정상 종료
}
