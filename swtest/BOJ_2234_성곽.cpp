// BOJ 2234 - 성곽
//
// 문제에서 구해야 하는 것
// 1. 방의 개수
// 2. 가장 넓은 방의 넓이
// 3. 벽 하나를 제거해서 만들 수 있는 가장 넓은 방의 넓이
//
// 핵심 아이디어
// 1. BFS로 각 방(서로 연결된 칸들의 집합)을 찾는다.
// 2. 각 방에 고유 번호(mark)를 붙이고, 그 넓이를 저장한다.
// 3. 모든 칸을 보면서 인접한 다른 방과 합칠 수 있는 경우
//    두 방 넓이의 합의 최댓값을 구한다.

#include <stdio.h>

#define MAX (50 + 5)

// 방향 번호
#define LEFT  (0)   // 서
#define UP    (1)   // 북
#define RIGHT (2)   // 동
#define DOWN  (3)   // 남

int N, M;                 // N: 행 개수, M: 열 개수
int MAP[MAX][MAX];        // 각 칸의 벽 정보
int visit[MAX][MAX];      // 각 칸이 몇 번 방(mark)에 속하는지 저장

int answers[MAX * MAX];   // answers[방 번호] = 그 방의 넓이

// 좌표를 저장하는 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];      // BFS용 큐

// 방향 배열: 서, 북, 동, 남
int dr[] = { 0, -1, 0, 1 };
int dc[] = { -1, 0, 1, 0 };

// WALL[벽정보값][방향]
// 해당 방향에 벽이 있으면 1, 없으면 0
//
// 문제에서 각 칸의 값은 0~15 사이이며,
// 이 값은 서/북/동/남 벽 존재 여부를 비트로 나타낸 것이다.
//
// 예)
// 1  = 서쪽 벽만 있음
// 2  = 북쪽 벽만 있음
// 4  = 동쪽 벽만 있음
// 8  = 남쪽 벽만 있음
// 15 = 4방향 모두 벽
//
// 이 코드는 미리 모든 경우를 표로 만들어서 사용한다.
int WALL[16 + 3][5] = {
	{0,0,0,0},   // 0
	{1,0,0,0},   // 1  : 서
	{0,1,0,0},   // 2  : 북
	{1,1,0,0},   // 3  : 서, 북
	{0,0,1,0},   // 4  : 동
	{1,0,1,0},   // 5  : 서, 동
	{0,1,1,0},   // 6  : 북, 동
	{1,1,1,0},   // 7  : 서, 북, 동
	{0,0,0,1},   // 8  : 남
	{1,0,0,1},   // 9  : 서, 남
	{0,1,0,1},   // 10 : 북, 남
	{1,1,0,1},   // 11 : 서, 북, 남
	{0,0,1,1},   // 12 : 동, 남
	{1,0,1,1},   // 13 : 서, 동, 남
	{0,1,1,1},   // 14 : 북, 동, 남
	{1,1,1,1},   // 15 : 서, 북, 동, 남
};

// 두 칸(room1, room2)이 direction 방향으로 서로 이동 가능한지 확인
//
// room1 = 현재 칸의 벽 정보 값
// room2 = 이웃 칸의 벽 정보 값
// direction = 어느 방향으로 이동하는지
//
// 예를 들어 현재 칸에서 LEFT로 가려면
// 현재 칸의 LEFT 벽도 없어야 하고
// 이웃 칸의 RIGHT 벽도 없어야 한다.
//
// 둘 중 하나라도 벽이 있으면 이동 불가
int isOpen(int room1, int room2, int direction)
{
	if (direction == LEFT)
		return !(WALL[room1][LEFT] || WALL[room2][RIGHT]);

	if (direction == UP)
		return !(WALL[room1][UP] || WALL[room2][DOWN]);

	if (direction == RIGHT)
		return !(WALL[room1][RIGHT] || WALL[room2][LEFT]);

	if (direction == DOWN)
		return !(WALL[room1][DOWN] || WALL[room2][UP]);

	return -1;
}

// 입력 함수
void input()
{
	scanf("%d %d", &M, &N);

	// 바깥을 전부 15(사방이 벽)로 초기화
	// 이렇게 해두면 경계 밖으로 나가는 경우를 자동으로 막을 수 있다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
		{
			MAP[r][c] = 15;
		}
	}

	// 실제 성곽 정보 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// 디버그용 출력 함수
void printMap()
{
	// 원래 MAP 출력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');

	// 방문한 방 번호 출력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// BFS로 하나의 방을 탐색
//
// r, c  : 시작 좌표
// mark  : 이 방에 붙일 번호
//
// 반환값:
//   이 방의 넓이(칸 개수)
int BFS(int r, int c, int mark)
{
	int rp, wp;

	rp = wp = 0;

	// 시작 칸 큐에 삽입
	queue[wp].r = r;
	queue[wp++].c = c;

	// 시작 칸을 mark번 방으로 표시
	visit[r][c] = mark;

	// 큐가 빌 때까지 BFS
	while (rp < wp)
	{
		RC out = queue[rp++];

		// 4방향 탐색
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 현재 칸과 이웃 칸 사이가 벽으로 막혀 있거나
			// 이미 방문한 칸이면 건너뜀
			if (isOpen(MAP[out.r][out.c], MAP[nr][nc], i) == 0 ||
				visit[nr][nc] != 0)
				continue;

			// 이동 가능한 칸이면 큐에 넣고 같은 방 번호 표시
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = mark;
		}
	}

	// 큐에 들어간 칸 수 = 방의 넓이
	return wp;
}

int main()
{
	int answerCount;   // answers에 저장된 방 개수 + 1
	int mark;          // 방 번호
	int maxAnswer;     // 가장 넓은 방의 넓이
	int maxAreaSum;    // 벽 하나 제거 후 만들 수 있는 최대 넓이

	input();

	// ---------------------------
	// 1. BFS로 각 방 구하기
	// ---------------------------
	answerCount = 1;   // answers[1]부터 사용
	mark = 1;          // 방 번호도 1부터 시작

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 아직 방문하지 않은 칸이면 새로운 방 시작
			if (visit[r][c] == 0)
			{
				answers[answerCount++] = BFS(r, c, mark++);
			}
		}
	}

	// ---------------------------
	// 2. 가장 넓은 방 찾기
	// ---------------------------
	maxAnswer = 0;
	for (int i = 1; i <= answerCount; i++)
	{
		if (maxAnswer < answers[i])
			maxAnswer = answers[i];
	}

	// ---------------------------
	// 3. 벽 하나 제거해서 만들 수 있는 최대 넓이 찾기
	// ---------------------------
	//
	// 모든 칸에서 4방향 이웃을 보며
	// 서로 다른 방이라면 두 방을 합친 넓이를 계산
	maxAreaSum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				// 같은 방이면 합칠 필요 없음
				if (visit[r][c] == visit[nr][nc])
					continue;

				// 서로 다른 방이면 두 방 넓이 합 계산
				int tmpArea = answers[visit[r][c]] + answers[visit[nr][nc]];

				if (maxAreaSum < tmpArea)
					maxAreaSum = tmpArea;
			}
		}
	}

	// 출력
	// 1) 방의 개수
	// 2) 가장 넓은 방의 넓이
	// 3) 벽 하나 제거 후 가장 넓은 방의 넓이
	printf("%d\n%d\n%d\n", answerCount - 1, maxAnswer, maxAreaSum);

	// printMap(); // 디버그용

	return 0;
}