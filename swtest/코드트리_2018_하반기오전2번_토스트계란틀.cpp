// 코드트리 2018 하반기 오전 2번 - 토스트 계란틀
//
// 문제 요약
// N x N 격자에 각 칸의 인구(혹은 계란 수)가 주어진다.
// 인접한 두 칸의 차이가 [L, R] 범위 안에 있으면
// 국경을 열어서 하나의 연합으로 묶을 수 있다.
//
// 하나의 연합이 만들어지면
// 그 연합에 속한 모든 칸의 값을
// "연합 총합 / 연합 칸 수" 로 동일하게 갱신한다.
//
// 이 과정을 더 이상 어떤 이동(연합 형성)도 일어나지 않을 때까지 반복할 때
// 총 몇 번의 이동이 발생하는지를 구하는 문제이다.
//
// 핵심 아이디어
// 1. 하루(한 번의 이동)마다 전체 격자를 훑으면서
//    아직 방문하지 않은 칸에 대해 BFS를 수행한다.
// 2. BFS로 하나의 연합을 찾고,
//    그 연합의 칸들을 모두 평균값으로 갱신한다.
// 3. 연합 크기가 2 이상인 경우가 하나라도 있으면
//    그 날은 인구 이동이 발생한 것이다.
// 4. 더 이상 이동이 없을 때까지 반복한다.

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)

int T;

int N, L, R;               // N: 격자 크기, L~R: 연합 형성 조건
int MAP[MAX][MAX];         // 현재 격자 상태
bool visit[MAX][MAX];      // BFS 방문 체크

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];       // BFS용 큐

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &L, &R);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 디버그용 맵 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// 절댓값 함수
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;
}

// ---------------------------
// BFS로 하나의 연합을 찾고 처리
//
// 시작 좌표: (r, c)
//
// 반환값:
//   연합에 포함된 칸 수
//
// 동작:
// 1. (r, c)에서 출발하여
//    인접 칸과의 차이가 [L, R]인 칸들을 BFS로 묶는다.
// 2. 연합의 전체 합(sum)을 구한다.
// 3. BFS가 끝난 뒤, 연합에 포함된 모든 칸의 값을
//    sum / 연합크기 로 갱신한다.
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;
	int sum;

	rp = wp = 0;

	// 시작 칸 큐에 넣기
	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	// 연합 총합 초기값 = 시작 칸 값
	sum = MAP[r][c];

	// BFS 시작
	while (rp < wp)
	{
		RC out = queue[rp++];

		// 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 범위 밖이면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// 현재 칸과 인접 칸의 차이 계산
			int diff = abs(MAP[out.r][out.c] - MAP[nr][nc]);

			// 이미 방문했거나,
			// 차이가 [L, R] 범위를 벗어나면 연합에 포함 불가
			if (visit[nr][nc] == true || (L <= diff && diff <= R) == false)
				continue;

			// 연합에 포함
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;
			sum += MAP[nr][nc];
		}
	}

	// BFS가 끝난 뒤,
	// queue[0 .. wp-1] 에는 이번 연합에 포함된 모든 칸이 들어 있음
	// 이 칸들을 모두 평균값으로 갱신
	for (int i = 0; i < wp; i++)
	{
		RC tmp = queue[i];
		MAP[tmp.r][tmp.c] = sum / wp;
	}

	// 연합 크기 반환
	return wp;
}

// ---------------------------
// 하루(1회 이동)를 시뮬레이션
//
// 반환값:
//   true  -> 이번 날에 실제 이동(연합 크기 2 이상)이 있었다.
//   false -> 아무 이동도 없었다.
//
// 동작:
// 1. visit 배열 초기화
// 2. 전체 격자를 훑으면서 아직 방문하지 않은 칸에 대해 BFS
// 3. BFS 결과가 1칸짜리가 아니면 이동 발생으로 간주
// ---------------------------
bool simulate()
{
	// 방문 배열 초기화
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			visit[r][c] = false;
		}
	}

	bool result = false; // 이번 날에 이동이 있었는지

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (visit[r][c] == true)
				continue;

			// 하나의 연합 찾기
			int moveEggCount = BFS(r, c);

			// 연합 크기가 2 이상이면 실제 이동이 발생한 것
			if (moveEggCount != 1)
				result = true;
		}
	}

	return result;
}

int main()
{
	// 원래 테스트케이스 형식일 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int answer = 0; // 총 이동 횟수(일 수)

		// 더 이상 이동이 없을 때까지 반복
		while (1)
		{
			bool result = simulate();

			// 이번 날에 이동이 없었다면 종료
			if (result == false)
				break;

			answer++;
		}

		printf("%d\n", answer);
	}

	return 0;
}