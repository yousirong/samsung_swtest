// 코드트리 2018 하반기 오후 2번 - 전투로봇
//
// 문제 요약
// N x N 격자 안에 전투로봇과 몬스터들이 있다.
// 로봇은 자신의 레벨보다 작은 몬스터만 제거할 수 있고,
// 같은 레벨의 몬스터가 있는 칸은 지나갈 수만 있으며 제거는 못 한다.
// 더 큰 레벨의 몬스터가 있는 칸은 지나갈 수 없다.
//
// 로봇의 초기 상태
// - 시작 위치: 입력에서 9로 표시된 칸
// - 초기 레벨: 2
// - 제거한 몬스터 수(attack): 0
//
// 레벨업 규칙
// - 현재 레벨과 같은 수의 몬스터를 제거하면 레벨이 1 증가
// - 레벨업하면 제거 횟수는 다시 0으로 초기화
//
// 목표
// 로봇이 이동하면서 제거할 수 있는 몬스터를 모두 제거할 때까지의
// 총 이동 시간을 구하는 문제
//
// 핵심 아이디어
// 1. 현재 로봇이 제거 가능한 몬스터가 존재하는지 확인
// 2. BFS로 모든 칸까지의 최단 거리를 구함
// 3. 제거 가능한 몬스터 중
//    - 가장 가까운 몬스터
//    - 거리가 같다면 가장 위쪽
//    - 그것도 같다면 가장 왼쪽
//   을 선택
// 4. 그 몬스터를 제거하고 로봇 상태를 갱신
// 5. 더 이상 제거할 수 없을 때까지 반복
//
// 즉,
// "한 번 먹을 때마다 BFS"를 반복하는 시뮬레이션 문제이다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (20 + 5)
#define INF (0x7fff0000)

#define ROBOT_POSITION (9)

int T;

int N;
int MAP[MAX][MAX];     // 격자 정보
int visit[MAX][MAX];   // BFS 거리 저장용

// 전투로봇 상태
struct ROBOT
{
	int r;
	int c;
	int attack;   // 지금까지 제거한 몬스터 수
	int level;    // 현재 로봇 레벨
};

typedef struct ROBOT ROBOT;

ROBOT battleRobot;

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];   // BFS용 큐

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d", &N);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);

			// 로봇 시작 위치 찾기
			if (MAP[r][c] == ROBOT_POSITION)
			{
				// 로봇이 있던 칸은 실제로는 빈칸처럼 취급
				MAP[r][c] = 0;

				battleRobot.r = r;
				battleRobot.c = c;
				battleRobot.attack = 0;
				battleRobot.level = 2;
			}
		}
	}
}

// ---------------------------
// 디버그용 출력 함수
// 현재 visit 배열 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 현재 로봇이 제거할 수 있는 몬스터가 하나라도 있는지 확인
//
// 조건:
// - MAP[r][c] != 0   : 몬스터가 있어야 함
// - MAP[r][c] < level: 로봇 레벨보다 작아야 제거 가능
// ---------------------------
bool monsterExists()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] != 0 && MAP[r][c] < battleRobot.level)
				return true;
		}
	}

	return false;
}

// ---------------------------
// BFS로 현재 로봇 위치에서 각 칸까지의 최단 거리 계산
//
// visit[r][c] 의미:
// - 0 : 방문하지 않음 / 도달 불가능
// - 1 : 시작 위치
// - 2 이상 : 시작 위치로부터의 거리 + 1
//
// 주의:
// - 로봇보다 큰 몬스터 칸은 지나갈 수 없음
// - 같은 레벨 몬스터 칸은 지나갈 수는 있음
// - 작은 몬스터 칸도 당연히 지나갈 수 있음
// ---------------------------
void BFS()
{
	// 방문 배열 초기화
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			visit[r][c] = 0;
		}
	}

	int rp, wp;
	rp = wp = 0;

	// 시작 위치 큐 삽입
	queue[wp].r = battleRobot.r;
	queue[wp++].c = battleRobot.c;

	visit[battleRobot.r][battleRobot.c] = 1;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 범위 밖이면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// 로봇보다 큰 몬스터가 있는 칸은 못 지나감
			if (battleRobot.level < MAP[nr][nc])
				continue;

			// 이미 방문한 칸이면 무시
			if (visit[nr][nc] != 0)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;
		}
	}
}

// ---------------------------
// 로봇이 몬스터 하나를 제거하는 1회 시뮬레이션
//
// 반환값:
// - 이번에 몬스터를 제거했다면 걸린 이동 시간
// - 더 이상 제거할 수 없으면 0
//
// 처리 순서:
// 1. 제거 가능한 몬스터가 아예 없으면 0 반환
// 2. BFS 수행
// 3. 먹을 몬스터 선택
//    우선순위:
//    - 가장 가까운 몬스터
//    - 가장 위쪽
//    - 가장 왼쪽
// 4. 해당 몬스터 제거, 로봇 이동, 레벨 갱신
// 5. 이동 시간 반환
// ---------------------------
int simulate()
{
	// 제거 가능한 몬스터 자체가 없으면 종료
	if (monsterExists() == false)
		return 0;

	// 현재 위치에서 거리 계산
	BFS();

	int tmpR, tmpC;
	int minTime = INF;

	// ---------------------------
	// 먹을 몬스터 선택
	//
	// r을 작은 순서(위쪽), c를 작은 순서(왼쪽)로 탐색하므로
	// 같은 거리라면 자동으로 "가장 위, 가장 왼쪽" 우선순위가 적용됨
	// ---------------------------
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 빈칸이거나 도달 못한 칸이면 제외
			if (MAP[r][c] == 0 || visit[r][c] == 0)
				continue;

			// 같은 레벨 이상은 제거 불가
			if (battleRobot.level <= MAP[r][c])
				continue;

			// 더 가까운 몬스터 발견 시 갱신
			if (visit[r][c] < minTime)
			{
				minTime = visit[r][c];
				tmpR = r;
				tmpC = c;
			}
		}
	}

	// printMap();

	// BFS는 갔지만 실제로 제거 가능한 몬스터가 하나도 없는 경우
	if (minTime == INF)
		return 0;

	// ---------------------------
	// 몬스터 제거
	// ---------------------------

	// 몬스터를 제거하면 그 칸은 빈칸
	MAP[tmpR][tmpC] = 0;

	// 로봇 이동
	battleRobot.r = tmpR;
	battleRobot.c = tmpC;

	// 제거 횟수 증가
	battleRobot.attack++;

	// 현재 레벨만큼 제거하면 레벨업
	if (battleRobot.attack == battleRobot.level)
	{
		battleRobot.level++;
		battleRobot.attack = 0;
	}

	// visit는 시작 위치를 1로 잡았으므로 실제 이동 시간은 minTime - 1
	return minTime - 1;
}

int main()
{
	// 원래 테스트케이스 형식이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int time = 0;   // 총 이동 시간

		while (1)
		{
			int result = simulate();

			// 더 이상 제거할 수 없으면 종료
			if (result == 0)
				break;

			time += result;
		}

		printf("%d\n", time);
	}

	return 0;
}