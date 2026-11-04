/*
	[코드트리] 2026 하반기 오후 1번 - 호수의 물결 (연습)
	풀이 설명은 코드트리_2026_하반기오후1번_호수의물결.cpp 참고.

	■ 본 코드와 다른 점 / 버그 (주석으로만 표시, 코드는 그대로)
	  [버그1] input()의 scanf : 형식 문자열에 %d가 5개뿐인데 인자는 6개이고,
	          & 대신 % 가 쓰인 곳(% cat[i].c, % cat[i].d ...)이 있어 컴파일 에러가 난다.
	          -> scanf("%d %d %d %d %d %d", &cat[i].c, &cat[i].s, &cat[i].d, &cat[i].a, &cat[i].h, &cat[i].p);
	  [버그2] dropStones() : for (int y = 0; y < M; y++) -> 행은 y < N 이어야 한다.
	          N != M 이면 물결이 일부 행에 안 퍼지거나 배열 밖까지 쓴다.
	  [버그3] settle() : nr >= N || nr >= N -> 두 번째는 nc >= M 이어야 한다.
	          지금은 오른쪽 경계 밖(nc == M)을 검사하지 않아, 마지막 열이 "둘러싸였다"고 잘못 판단될 수 있다.
*/

#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (30+5)                       // n, m 최대 30 + 여유
#define MAX_K (50+5)                     // 투석기 최대 50 + 여유
#define MAX_STONE (500+20)               // 돌 최대 500 + 여유
#define LAST_TURN (50)                   // 최대 턴

int T;                                   // 테스트 케이스 수
int N, M, K;                             // 행 수, 열 수, 투석기 수

struct CATAPULT                          // 투석기 정보
{
	int c; // 열
	int s; // 시작 시각
	int d; // 간격
	int a; // 무게
	int h; // 거리
	int p; // 돌 수
};

CATAPULT cat[MAX_K];                     // 투석기 목록

struct STONE                             // 돌 정보
{
	int r, c, a; // 지금위치
	int targetRow; // 떨어질 행
	int alive; // 1 = 날고 있음
};

STONE stone[MAX_STONE];                  // 돌 목록
int scnt;                                // 날린 돌 수

int wave[MAX][MAX]; // 이번턴 물결 세기
int murk[MAX][MAX]; //혼탁도

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

	for (int i = 0; i < K; i++)          // 투석기 정보 입력
		scanf("%d %d %d %d %d", % cat[i].c, &cat[i].s, % cat[i].d, % cat[i].a, % cat[i].h, % cat[i].p);   // ※ 버그1 : %d 6개, & 로 고쳐야 함

	for (int r = 0; r < N; r++)          // 혼탁도 초기화
		for (int c = 0; c < M; c++)
			murk[r][c] = 0;

	scnt = 0;                            // 돌 목록 초기화
	turnCount = 0;	                     // 턴 수 초기화
}

int abs(int x)                           // 절댓값
{
	return x > 0 ? x : -x;
}

void moveStones()                        // 1단계 : 돌 한 칸 위로
{
	for (int i = 0; i < scnt; i++)
		if (stone[i].alive) stone[i].r--;
}

void launchStones(int t)                 // 2단계 : 돌 발사
{
	for (int i = 0; i < K; i++)          // 투석기별
	{
		if (t < cat[i].s) continue;                  // 시작 전
		if ((t - cat[i].s) % cat[i].d != 0) continue;   // 간격이 아님
		if ((t - cat[i].s) / cat[i].d >= cat[i].p) continue;   // 다 날림

		stone[scnt].r = N - 1;           // 이쪽 기슭
		stone[scnt].c = cat[i].c;        // 투석기 열
		stone[scnt].a = cat[i].a;        // 무게
		stone[scnt].targetRow = N - cat[i].h;   // 떨어질 행
		stone[scnt++].alive = 1;         // 날고 있음
	}
}

int dropStones()                         // 3단계 : 낙하 + 물결
{
	int dropCount = 0;                   // 떨어진 돌 수

	for (int r = 0; r < N; r++)          // 물결 초기화
		for (int c = 0; c < M; c++)
			wave[r][c] = 0;

	for (int i = 0; i < scnt; i++)       // 돌별
	{
		if (stone[i].alive == 0 || stone[i].r != stone[i].targetRow)continue;   // 떨어질 돌만

		stone[i].alive = 0;              // 떨어짐
		dropCount++;                     // 개수 증가

		for (int y = 0; y < M; y++)      // ※ 버그2 : y < N 이어야 함
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

int getScore()                           // 4단계 : 점수
{
	int sum = 0;                         // 점수

	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < M; c++)
		{
			int v = wave[r][c] - murk[r][c];   // 물결 - 혼탁도
			if (v > 0) sum += v;         // 양수만
		}
	}
	return sum;                          // 반환
}


void settle()                            // 5단계 : 가라앉기
{
	int next[MAX][MAX];                  // 새 혼탁도

	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < M; c++)
		{
			next[r][c] = murk[r][c];     // 기본은 그대로
			if (murk[r][c] == 0) continue;   // 흙탕물 없으면 제외

			int surrounded = 1;          // 둘러싸였는가
			for (int i = 0; i < 4; i++)  // 4방향
			{
				int nr = r + dr[i];      // 이웃 행
				int nc = c + dc[i];      // 이웃 열

				if (nr < 0 || nc < 0 || nr >= N || nr >= N || murk[nr][nc] == 0)   // ※ 버그3 : 두 번째는 nc >= M
				{
					surrounded = 0;      // 둘러싸이지 않음
					break;
				}
			}

			next[r][c] -= surrounded ? 1 : 2;   // -1 또는 -2
			if (next[r][c] < 0) next[r][c] = 0; // 0 아래로는 안 감
		}
	}

	for (int r = 0; r < N; r++)          // 한꺼번에 반영
		for (int c = 0; c < M; c++)
			murk[r][c] = next[r][c];
}

void flow()                              // 6단계 : 물살 (왼쪽으로)
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M - 1; c++)
			murk[r][c] = murk[r][c + 1]; // 오른쪽 값을 당겨 오기
		murk[r][M - 1] = 0;              // 마지막 열은 0
	}
}

void stirUp()                            // 7단계 : 흙탕물 일기
{
	for (int r = 0; r < N; r++)          // 방문 초기화
		for (int c = 0; c < M; c++)
			visit[r][c] = 0;

	for (int sr = 0; sr < N; sr++)       // 모든 칸
	{
		for (int sc = 0; sc < M; sc++)
		{
			if (wave[sr][sc] <= 0 || visit[sr][sc] == 1) continue;   // 물결 없거나 방문함

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

					if (nr < 0 || nc < 0 || nr >= N || nc >= M)continue;    // 격자 밖
					if (wave[nr][nc] <= 0 || visit[nr][nc] == 1) continue;  // 물결 없거나 방문함

					visit[nr][nc] = 1;   // 방문 표시
					queue[wp].r = nr;    // 큐에 넣기
					queue[wp++].c = nc;
				}
			}

			int avg = sum / wp;          // 평균 (내림)
			for (int i = 0; i < wp; i++)
				murk[queue[i].r][queue[i].c] += avg;   // 혼탁도에 더하기
		}
	}
}

int isRemain(int t)                      // 날릴 / 날고 있는 돌이 남았는가
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
		moveStones(); //1
		launchStones(t); //2
		int dropCount = dropStones(); //3
		score[turnCount++] = (dropCount > 0) ? getScore() : 0; //4

		settle(); //5
		flow(); //6
		stirUp(); //7

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
		printf("#%d", tc);               // 케이스 번호
		for (int i = 0; i < turnCount; i++)
			printf(" %d", score[i]);     // 턴별 점수
		putchar('\n');                   // 줄바꿈
	}
	return 0;                            // 프로그램 정상 종료
}
