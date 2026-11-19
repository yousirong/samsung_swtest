/*
	[코드트리] 2026 하반기 오전 1번 - 수하물 컨베이어 벨트
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/baggage-conveyor-belt/description

	■ 문제 요약
	  N x N 격자(2 <= N <= 8). 0 벽, 1 ~ 4 레일(1 아래, 2 왼쪽, 3 위, 4 오른쪽), 9 적재 장소.
	  어느 레일에서 출발해도 화살표를 따라가면 반드시 적재 장소에 닿는다 (순환이 없다).
	  수하물 B개(최대 30). i번은 무게 w_i, t_i턴에 (r_i, c_i)로 투입될 예정이다.
	  매 턴
	    1단계 이동 : 벨트 위 모든 수하물이 동시에 화살표 방향으로 한 칸 가려고 한다.
	       - 같은 칸으로 여럿이 가려 하면 가장 무거운 것(같으면 번호가 작은 것)만 갈 수 있다
	       - 가려는 칸에 수하물이 있고 그 수하물이 이번 턴에 못 움직이면 나도 못 움직인다
	         (앞 수하물이 움직이면 비운 자리로 들어갈 수 있다)
	    2단계 적재 / 파손 : 적재 장소에 도착한 수하물은 적재 (그 턴 번호 기록).
	       남은 수하물 중 3턴 연속 못 움직인 것은 파손 표시 (그래도 계속 움직이고 적재된다).
	    3단계 투입 : 예정 턴이 됐거나 지난 미투입 수하물이 투입 칸에 올라가려 한다.
	       칸이 차 있으면 실패, 같은 칸에 여럿이면 예정 턴이 가장 이른 것만. 실패하면 다음 턴에 다시.
	  수하물마다 적재 턴을 출력한다. 파손된 것은 -(적재 턴).

	  입출력 예
	      예제 1 -> 5 6 7 -8 -9 10
	      예제 2 -> 7 3 10 6 13 -12 8 11 9 6

	■ 풀이 방침 : 턴마다 moveBags(1, 2단계) -> enterBags(3단계)
	  occupied[r][c] = 그 칸에 있는 수하물 번호 (0 = 빈 칸) 로 칸 -> 수하물을 바로 찾는다.
	  수하물 상태는 entered(벨트에 올라감), delivered(적재됨), broken(파손) 세 플래그로 둔다.
	  벨트 위 = entered && !delivered.
	  loaded(적재된 수)가 B가 되면 끝.

	■ 1단계의 핵심 : "누가 움직이나"를 원래 자리 기준으로 다 정하고, 그다음 한꺼번에 옮긴다
	  1) 목적지 경쟁 : 수하물마다 가려는 칸(nextR, nextC)을 구하고,
	     칸마다 그 칸을 노리는 수하물 중 우선순위가 가장 높은 하나(winner)만 남긴다.
	  2) canMove(i) : i가 이번 턴에 움직일 수 있나 (재귀 + 메모 moveState)
	       - 가려는 칸의 winner가 내가 아니면 -> 못 움직임
	       - 가려는 칸에 수하물 front가 있고 front가 못 움직이면 -> 못 움직임
	       - 나머지(빈 칸, 또는 front가 비켜 줌) -> 움직임
	     화살표에 순환이 없으므로 이 재귀는 반드시 끝난다 (결국 적재 장소나 빈 칸에 닿는다).
	  3) occupied를 통째로 비운 뒤, 수하물마다 새 위치(못 움직이면 제자리)를 다시 적는다.
	     하나씩 옮기며 덮어쓰지 않으므로 순서 문제가 없다.

	■ 2단계도 moveBags 안에서 바로 처리
	  - 움직였으면 stopped = 0, 못 움직였으면 stopped++ (3 이상이면 broken)
	  - 새 위치가 적재 장소(9)면 delivered, answer = broken ? -turn : turn, loaded++
	    -> 적재된 수하물은 occupied에 적지 않는다 (적재 장소에는 수하물이 머무르지 않는다).
	  적재 장소에 있다는 건 이번 턴에 움직였다는 뜻이라, stopped를 먼저 계산해도 결과가 같다.

	■ 3단계 투입 : 칸마다 후보 하나 (candidate[r][c])
	  예정 턴이 지났고 아직 안 들어간 수하물 중, 투입 칸이 비어 있는 것만 후보.
	  같은 칸이면 예정 턴이 가장 이른 것 하나 (투입 예정 (t, r, c)가 같은 수하물은 없다).
	  후보를 다 고른 뒤 한꺼번에 올린다.

	■ 주의할 점
	  - 이번 턴에 투입된 수하물은 다음 턴부터 움직인다 -> 투입은 이동 / 적재 뒤(3단계)에 한다.
	  - 파손은 표시만 한다. 무게, 이동 규칙은 그대로다.
	  - 같은 적재 장소로 동시에 둘이 가려 하면 winner 규칙이 그대로 적용된다.
	  - 출력은 한 줄에 공백으로 구분한다.
*/
#include <stdio.h>                       // scanf, printf

#define MAX (8 + 5)                      // N 최대 8 + 여유
#define MAX_B (30 + 5)                   // B 최대 30 + 여유

int T;                                   // 테스트 케이스 수
int N, B, loaded;                        // 격자 크기, 수하물 수, 적재된 수하물 수
int MAP[MAX][MAX];                       // 0 벽, 1 ~ 4 레일, 9 적재 장소
int occupied[MAX][MAX];                  // 그 칸의 수하물 번호 (0 = 빈 칸)
int winner[MAX][MAX];                    // 이번 턴 그 칸으로 갈 수 있는 수하물 (0 = 없음)
int candidate[MAX][MAX];                 // 이번 턴 그 칸에 투입될 수하물 (0 = 없음)

// 0, 아래, 왼쪽, 위, 오른쪽 (레일 번호 1 ~ 4 그대로)
int dr[] = { 0, 1, 0, -1, 0 };           // 레일 번호별 행 변화량
int dc[] = { 0, 0, -1, 0, 1 };           // 레일 번호별 열 변화량

struct BAG                               // 수하물 정보
{
	int t;              // 투입 예정 턴
	int r;              // 지금 행 (투입 전에는 투입 칸)
	int c;              // 지금 열
	int w;              // 무게
	int stopped;        // 연속으로 이동하지 못한 턴
	int answer;         // 적재 턴 (파손되면 음수)
	bool entered;       // 벨트에 올라갔나
	bool delivered;     // 적재됐나
	bool broken;        // 파손됐나
};

BAG bag[MAX_B];                          // 수하물 배열 (1번부터)
int nextR[MAX_B], nextC[MAX_B];          // 이번 턴에 가려는 칸
int moveState[MAX_B];                    // canMove 메모 - 0: 미확인, 1: 이동 불가, 2: 이동 가능

void input()
{
	scanf("%d", &N);                     // 격자 크기

	for (int r = 1; r <= N; r++)         // 격자 입력
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	scanf("%d", &B);                     // 수하물 수
	for (int i = 1; i <= B; i++)         // 수하물별 예정 턴, 칸, 무게
	{
		scanf("%d %d %d %d", &bag[i].t, &bag[i].r, &bag[i].c, &bag[i].w);
		bag[i].stopped = bag[i].answer = 0;
		bag[i].entered = bag[i].delivered = bag[i].broken = false;
	}

	loaded = 0;                          // 적재 수 초기화
	for (int r = 1; r <= N; r++)         // 빈 벨트
		for (int c = 1; c <= N; c++)
			occupied[r][c] = 0;
}

// a가 b보다 우선인가 (b == 0 이면 경쟁자 없음 -> a가 우선)
bool isPriority(int a, int b)
{
	if (b == 0) return true;                                // 경쟁자 없음
	if (bag[a].w != bag[b].w) return bag[a].w > bag[b].w;   // 무거운 쪽
	return a < b;                                           // 번호가 작은 쪽
}

// index번 수하물이 이번 턴에 움직일 수 있나 (재귀 + 메모)
bool canMove(int index)
{
	if (moveState[index] != 0) return moveState[index] == 2;   // 이미 계산했으면 그대로

	int r = nextR[index];                // 가려는 칸
	int c = nextC[index];

	// 같은 칸을 노린 수하물 중 우선순위에서 밀리면 이동할 수 없다.
	if (winner[r][c] != index)
	{
		moveState[index] = 1;
		return false;
	}

	// 앞 수하물이 비켜야 현재 수하물도 그 칸에 들어갈 수 있다.
	int front = occupied[r][c];          // 가려는 칸에 지금 있는 수하물 (원래 자리 기준)
	if (front != 0 && canMove(front) == false)
	{
		moveState[index] = 1;
		return false;
	}

	moveState[index] = 2;                // 빈 칸이거나 앞 수하물이 비켜 줌
	return true;
}

// 1단계 이동 + 2단계 적재 / 파손
void moveBags(int turn)
{
	for (int r = 1; r <= N; r++)         // winner 초기화
		for (int c = 1; c <= N; c++)
			winner[r][c] = 0;

	// 현재 위치를 바꾸기 전에 각 목적지의 이동 후보를 확정한다.
	for (int i = 1; i <= B; i++)
	{
		moveState[i] = 0;                // 메모 초기화
		if (bag[i].entered == false || bag[i].delivered == true) continue;   // 벨트 위만

		int dir = MAP[bag[i].r][bag[i].c];   // 지금 칸의 레일 방향
		nextR[i] = bag[i].r + dr[dir];       // 가려는 칸
		nextC[i] = bag[i].c + dc[dir];

		int &other = winner[nextR[i]][nextC[i]];   // 그 칸의 현재 winner
		if (isPriority(i, other) == true) other = i;   // 내가 더 우선이면 교체
	}

	// 원래 점유 상태를 기준으로 모든 수하물의 이동 여부를 결정한다.
	for (int i = 1; i <= B; i++)
		if (bag[i].entered == true && bag[i].delivered == false)
			canMove(i);

	for (int r = 1; r <= N; r++)         // 점유 상태를 비우고 새로 적는다
		for (int c = 1; c <= N; c++)
			occupied[r][c] = 0;

	// 이동 결과를 한꺼번에 반영하고, 적재 또는 연속 정지에 따른 파손을 처리한다.
	for (int i = 1; i <= B; i++)
	{
		if (bag[i].entered == false || bag[i].delivered == true) continue;   // 벨트 위만

		if (moveState[i] == 2)           // 움직임
		{
			bag[i].r = nextR[i];
			bag[i].c = nextC[i];
			bag[i].stopped = 0;          // 정지 기록 초기화
		}
		else                             // 못 움직임
		{
			bag[i].stopped++;
			if (bag[i].stopped >= 3) bag[i].broken = true;   // 3턴 연속 정지 -> 파손
		}

		if (MAP[bag[i].r][bag[i].c] == 9)    // 적재 장소 도착
		{
			bag[i].delivered = true;
			bag[i].answer = bag[i].broken ? -turn : turn;   // 파손이면 음수
			loaded++;
		}
		else occupied[bag[i].r][bag[i].c] = i;   // 벨트에 남으면 새 위치 기록
	}
}

// 3단계 투입
void enterBags(int turn)
{
	for (int r = 1; r <= N; r++)         // 후보 초기화
		for (int c = 1; c <= N; c++)
			candidate[r][c] = 0;

	// 아직 투입되지 않은 수하물 중 칸마다 예정 턴이 가장 이른 하나를 고른다.
	for (int i = 1; i <= B; i++)
	{
		if (bag[i].entered == true || bag[i].t > turn) continue;   // 이미 들어갔거나 아직 예정 전

		int r = bag[i].r;                // 투입 칸
		int c = bag[i].c;
		if (occupied[r][c] != 0) continue;   // 칸이 차 있으면 실패 (다음 턴에 다시)

		int &other = candidate[r][c];
		if (other == 0 || bag[i].t < bag[other].t) other = i;   // 예정 턴이 더 이르면 교체
	}

	for (int r = 1; r <= N; r++)         // 후보를 한꺼번에 올린다
		for (int c = 1; c <= N; c++)
		{
			int index = candidate[r][c];
			if (index == 0) continue;
			bag[index].entered = true;
			occupied[r][c] = index;
		}
}

void simulate()
{
	// 투입은 이동과 적재가 끝난 뒤에 하므로 새 수하물은 다음 턴부터 이동한다.
	for (int turn = 1; loaded < B; turn++)   // 모두 적재될 때까지
	{
		// 1단계: 현재 벨트 위 수하물을 동시에 이동
		moveBags(turn);

		// 2단계: 적재와 파손은 moveBags에서 처리

		// 3단계: 아직 투입되지 않은 수하물 투입
		enterBags(turn);
	}

	for (int i = 1; i <= B; i++)         // 결과 출력
		printf("%d%c", bag[i].answer, i == B ? '\n' : ' ');
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)
	{
		input();                         // 입력
		simulate();                      // 시뮬레이션 + 출력
	}

	return 0;
}
