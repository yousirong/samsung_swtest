// [코드트리] 2026 하반기 오전 1번 - 수하물 컨베이어 벨트 (연습)
// 풀이 설명은 코드트리_2026_하반기오전1번_수하물컨베이어벨트.cpp 참고. 본 코드와 로직이 같다.
//
// NxN 격자 필요 define으로 숫자 넣고
// 0벽 1~4레일 (하좌상우 1234) 수하물 개수B 무게 w 스턴 t
// 1단계 이동 2단계 적재/ 파손여부 3단계 투입
// 풀이방침은 턴마다 3단계를 그대로 시뮬레이션
// bagMAP[r][c] = 그 칸에 있는 수하물 번호 0 은 빈칸
// 1단계 누가 움직이냐를 한꺼번에 정해서 한꺼번에 복사
// canMove(i) i가 이번턴에 움직일 수 있냐
// 가려는 칸의 winner가 내가 아니면 -> 못움직임
// 가려는 칸이 비어있으면 움직임
// 가려는 칸에 수하물 j 가 있으면 canMove(j)를 따름
// 움직이는 수하물을 bagMAP에서 지운뒤 덮어 씌우기

#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (8+2)                        // N 최대 8 + 테두리
#define MAX_BAG (30+5)                   // B 최대 30 + 여유

#define WALL (0)                         // 벽
#define LOAD (9)                         // 적재 장소

#define WAIT (0)                         // 아직 투입 안 됨
#define BELT (1)                         // 벨트 위
#define DONE (2)	                     // 적재됨

int T;                                   // 테스트 케이스 수
int N, B;                                // 격자 크기, 수하물 수
int MAP[MAX][MAX];                       // 0 벽, 1 ~ 4 레일, 9 적재 장소
int bagMAP[MAX][MAX];                    // 칸별 수하물 번호 (0 빈 칸)

struct BAG                               // 수하물 정보
{
	int t, r, c, w;       // 투입 예정 턴, 투입 칸, 무게
	int state;            // WAIT / BELT / DONE
	int stuck;            // 연속으로 못 움직인 턴 수
	int broken;           // 파손 여부
	int result;           // 적재 턴 (파손이면 음수)
};

BAG bag[MAX_BAG];                        // 수하물 배열 (1번부터)

// -, 하, 좌, 상, 우
int dr[] = { 0,1,0,-1,0 };               // 레일 번호별 행 변화량
int dc[] = { 0,0,-1,0,1 };               // 레일 번호별 열 변화량

int winner[MAX][MAX];                    // 칸별 이번 턴에 들어갈 수 있는 수하물
int moveState[MAX_BAG];                  // canMove 메모 (-1 모름, 0 못 움직임, 1 움직임)
int moved[MAX_BAG];                      // 이번 턴에 움직였나

void input()
{
	scanf("%d", &N);                     // 격자 크기 입력

	for (int r = 0; r <= N + 1; r++)     // 테두리 포함 초기화
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = WALL, bagMAP[r][c] = 0;   // 벽, 빈 칸

	for (int r = 1; r <= N; r++)         // 격자 입력
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	scanf("%d", &B);                     // 수하물 수 입력

	for (int i = 1; i <= B; i++)         // 수하물별
	{
		scanf("%d %d %d %d", &bag[i].t, &bag[i].r, &bag[i].c, &bag[i].w);   // 예정 턴, 칸, 무게 입력

		bag[i].state = WAIT;             // 미투입
		bag[i].stuck = 0;                // 정체 0
		bag[i].broken = 0;               // 파손 아님
		bag[i].result = 0;               // 결과 없음
	}
}

int isPriority(int a, int b)             // a가 b보다 우선인가
{
	if (bag[a].w != bag[b].w) return bag[a].w > bag[b].w;   // 무거운 쪽

	return a < b;                        // 번호가 작은 쪽
}

int canMove(int i)                       // i번이 이번 턴에 움직일 수 있나
{
	if (moveState[i] != -1) return moveState[i];   // 이미 계산했으면 그대로

	int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];   // 가려는 행
	int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];   // 가려는 열

	if (winner[nr][nc] != i) return moveState[i] = 0;  // 경쟁에서 짐
	if (bagMAP[nr][nc] == 0) return moveState[i] = 1;  // 빈 칸
	return moveState[i] = canMove(bagMAP[nr][nc]);     // 앞 수하물을 따름
}

// 1단계 이동
void moveBags()
{
	for (int r = 1; r <= N; r++)         // winner 초기화
		for (int c = 1; c <= N; c++)
			winner[r][c] = 0;

	for (int i = 1; i <= B; i++)         // 칸별 winner 정하기
	{
		moveState[i] = -1;               // 메모 초기화
		moved[i] = 0;                    // 이동 여부 초기화

		if (bag[i].state != BELT) continue;   // 벨트 위만

		int nr = bag[i].r + dr[MAP[bag[i].r][bag[i].c]];   // 가려는 행
		int nc = bag[i].c + dc[MAP[bag[i].r][bag[i].c]];   // 가려는 열

		if (winner[nr][nc] == 0 || isPriority(i, winner[nr][nc])) winner[nr][nc] = i;   // 더 우선이면 갱신
	}

	for (int i = 1; i <= B; i++)         // 누가 움직이는지 먼저 결정
		if (bag[i].state == BELT) moved[i] = canMove(i);

	for (int i = 1; i <= B; i++)         // 움직이는 수하물 전부 지우기
		if (moved[i]) bagMAP[bag[i].r][bag[i].c] = 0;

	for (int i = 1; i <= B; i++)         // 새 칸에 한꺼번에 놓기
	{
		if (moved[i] == 0) continue;     // 안 움직이면 제외

		int d = MAP[bag[i].r][bag[i].c]; // 레일 방향
		bag[i].r += dr[d];               // 한 칸 이동
		bag[i].c += dc[d];

		bagMAP[bag[i].r][bag[i].c] = i;  // 새 칸에 표시
	}
}

// 2단계
void loadAndCheck(int turn)
{
	for (int i = 1; i <= B; i++)         // 수하물별
	{
		if (bag[i].state != BELT) continue;   // 벨트 위만

		if (MAP[bag[i].r][bag[i].c] == LOAD)   // 적재 장소 도착
		{
			bag[i].state = DONE;         // 적재 완료
			bag[i].result = bag[i].broken ? -turn : turn;   // 파손이면 음수
			bagMAP[bag[i].r][bag[i].c] = 0;   // 칸 비우기
			continue;
		}

		if (moved[i]) bag[i].stuck = 0; // 움직였으면 정체 초기화
		else
		{
			bag[i].stuck++;              // 정체 +1
			if (bag[i].stuck >= 3) bag[i].broken = 1;   // 3턴 연속이면 파손
		}
	}
}

// 3단계
void insertBags(int turn)
{
	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= N; c++)
		{
			if (bagMAP[r][c] != 0) continue;   // 차 있으면 투입 불가

			int best = 0;                // 이 칸에 들어갈 수하물
			for (int i = 1; i <= B; i++)
			{
				if (bag[i].state != WAIT || bag[i].t > turn) continue;   // 미투입 + 예정 턴 지남
				if (bag[i].r != r || bag[i].c != c) continue;           // 이 칸 대상만

				if (best == 0 || bag[i].t < bag[best].t) best = i;      // 예정 턴이 더 이른 것
			}

			if (best == 0) continue;     // 대상 없음

			bag[best].state = BELT;      // 벨트에 올림
			bag[best].stuck = 0;         // 정체 0
			bagMAP[r][c] = best;         // 칸에 표시
		}
	}
}

int isFinished()                         // 모두 적재되었는가
{
	for (int i = 1; i <= B; i++)
		if (bag[i].state != DONE) return 0;   // 남은 것 있으면 0
	return 1;                            // 모두 적재
}

void simulate()
{
	for (int turn = 1; isFinished() == 0; turn++)   // 다 끝날 때까지
	{
		moveBags();                      // 1단계
		loadAndCheck(turn);              // 2단계
		insertBags(turn);                // 3단계
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		for (int i = 1; i <= B; i++)     // 수하물별
			printf("%d ", bag[i].result);   // 결과 출력
		putchar('\n');                   // 줄바꿈
	}
	return 0;                            // 프로그램 정상 종료
}
