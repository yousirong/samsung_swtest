/*
	[BOJ] 2234 - 성곽
	https://www.acmicpc.net/problem/2234

	■ 문제 요약
	  N x M 격자의 각 칸에 0 ~ 15 사이의 수가 주어진다.
	  이 수는 그 칸의 서/북/동/남 벽이 있는지를 비트로 표현한 것이다.

	    1 = 서쪽 벽, 2 = 북쪽 벽, 4 = 동쪽 벽, 8 = 남쪽 벽 (합으로 표현)

	  아래 세 가지를 구한다.
	    1) 방(서로 오갈 수 있는 칸들의 덩어리)의 개수
	    2) 가장 넓은 방의 넓이
	    3) 벽 하나를 제거해서 만들 수 있는 가장 넓은 방의 넓이

	■ 풀이 방침
	  (1)(2)는 34번(단지번호붙이기)과 같은 연결 요소 문제인데,
	  "이웃 칸으로 갈 수 있는가"의 판정만 벽 정보로 바뀐다.

	    - 아직 방문하지 않은 칸에서 BFS를 시작한다
	    - 방마다 고유 번호(mark)를 붙이고 visit에 그 번호를 적는다
	    - BFS가 돌려주는 칸 수를 answers[mark]에 저장한다

	  (3)은 벽을 실제로 하나씩 제거해 보며 다시 BFS를 돌릴 필요가 없다.
	  벽 하나를 없애 두 방이 합쳐진다는 것은 곧 "서로 인접한 서로 다른 두 방"이라는 뜻이므로,
	  모든 칸에서 4방향 이웃을 보며 방 번호가 다르면 두 방의 넓이를 더해 최댓값만 갱신하면 된다.
	  (방 번호가 다르다 = 그 사이에 벽이 있다. 벽이 없었다면 같은 방이었을 테니까)

	■ 벽 판정 : isOpen()
	  두 칸 사이를 오가려면 양쪽 벽이 모두 없어야 한다.
	  예를 들어 서쪽으로 가려면 내 서쪽 벽도, 이웃의 동쪽 벽도 없어야 한다.
	  값에서 비트를 뽑아 써도 되지만, 여기서는 0~15의 모든 경우를 WALL 표로 미리 펼쳐 두었다.

	■ 주의할 점
	  1) 입력 순서가 M(가로) N(세로)이다.
	  2) 격자 바깥을 15(사방이 벽)로 채워 두면 BFS가 절대 밖으로 나가지 않아
	     좌표 범위 검사를 생략할 수 있다.
	  3) (3)에서 이웃이 격자 바깥이면 visit이 0이고 answers[0]은 한 번도 쓰이지 않아 0이다.
	     그래서 "자기 방 넓이 + 0"이 되어 최댓값을 부풀리지 않는다.
	     방이 하나뿐인 입력에서도 이 값 덕분에 답이 그 방의 넓이로 맞게 나온다.
*/

#include <stdio.h>

#define MAX (50 + 5)

// 방향 번호 (dr/dc 배열의 인덱스와 일치시켜 둔다)
#define LEFT  (0)   // 서
#define UP    (1)   // 북
#define RIGHT (2)   // 동
#define DOWN  (3)   // 남

int N, M;                 // N: 행 개수, M: 열 개수
int MAP[MAX][MAX];        // 각 칸의 벽 정보 (0 ~ 15)
int visit[MAX][MAX];      // 그 칸이 몇 번 방에 속하는지 (0이면 아직 미방문)

int answers[MAX * MAX];   // answers[방 번호] = 그 방의 넓이

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];      // BFS용 큐

// 방향 배열: 서(0), 북(1), 동(2), 남(3)
int dr[] = { 0, -1, 0, 1 };
int dc[] = { -1, 0, 1, 0 };

// WALL[칸의 값][방향] = 그 방향에 벽이 있으면 1
//
// 칸의 값은 서(1) 북(2) 동(4) 남(8)의 합이다.
// 비트 연산(value & 1, value & 2, ...)으로 뽑아 써도 되지만,
// 여기서는 0 ~ 15 열여섯 가지를 전부 표로 펼쳐 두어 헷갈릴 여지를 없앴다.
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

// room1에서 direction 방향의 이웃 room2로 이동할 수 있는지
//
// 두 칸 사이의 벽은 "내 쪽 벽"과 "상대 쪽 벽" 두 번 표현되므로,
// 둘 중 하나라도 있으면 막힌 것으로 본다.
// 방향은 서로 반대쪽끼리 짝지어 검사한다 (서<->동, 북<->남).
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

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d", &M, &N);   // 가로(M) 세로(N) 순서에 주의

	// 격자 바깥을 15(사방이 벽)로 채운다.
	// 어느 방향에서 접근해도 벽에 막히므로 BFS가 밖으로 새지 않는다.
	for (int r = 0; r <= N + 1; r++)
	{
		for (int c = 0; c <= M + 1; c++)
		{
			MAP[r][c] = 15;
		}
	}

	// 실제 성곽 정보
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 디버그용 출력 (벽 정보 / 방 번호)
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", visit[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// BFS : (r, c)가 속한 방 전체에 mark를 칠하고 넓이를 반환
// ---------------------------
int BFS(int r, int c, int mark)
{
	int rp, wp;

	rp = wp = 0;

	queue[wp].r = r;
	queue[wp++].c = c;

	// visit에 방 번호를 적는 것이 곧 방문 표시다 (0이 아니면 방문한 것)
	visit[r][c] = mark;

	while (rp < wp)
	{
		RC out = queue[rp++];

		// 서, 북, 동, 남
		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 벽에 막혀 있거나 이미 다른(혹은 같은) 방으로 표시된 칸이면 건너뛴다.
			// 격자 바깥은 값이 15라 isOpen이 반드시 0을 돌려주므로 여기서 걸린다.
			if (isOpen(MAP[out.r][out.c], MAP[nr][nc], i) == 0 ||
				visit[nr][nc] != 0)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = mark;
		}
	}

	// 큐에 들어간 칸 수 = 이 방의 넓이 (중복 삽입이 없으므로 wp가 정확한 개수)
	return wp;
}

int main()
{
	int answerCount;   // 다음에 쓸 answers 인덱스 (= 방 개수 + 1)
	int mark;          // 다음에 붙일 방 번호
	int maxAnswer;     // 가장 넓은 방의 넓이
	int maxAreaSum;    // 벽 하나를 없앴을 때의 최대 넓이

	input();

	// ---------------------------
	// 1. 방을 모두 찾아 번호를 붙이고 넓이를 기록
	// ---------------------------
	answerCount = 1;   // answers[0]은 "방이 아님(격자 바깥)"용으로 비워 둔다
	mark = 1;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			// 아직 어느 방에도 속하지 않은 칸 = 새 방의 시작점
			if (visit[r][c] == 0)
			{
				answers[answerCount++] = BFS(r, c, mark++);
			}
		}
	}

	// ---------------------------
	// 2. 가장 넓은 방
	// ---------------------------
	maxAnswer = 0;
	for (int i = 1; i <= answerCount; i++)
	{
		if (maxAnswer < answers[i])
			maxAnswer = answers[i];
	}

	// ---------------------------
	// 3. 벽 하나를 없애 두 방을 합쳤을 때의 최대 넓이
	// ---------------------------
	//
	// 인접한 두 칸의 방 번호가 다르다는 것은 그 사이에 벽이 있다는 뜻이다.
	// 그 벽을 없애면 두 방이 하나로 합쳐지므로 넓이는 두 방의 합이 된다.
	// 실제로 벽을 지우고 BFS를 다시 돌릴 필요가 전혀 없다.
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

				// 같은 방이면 그 사이엔 벽이 없다는 뜻이므로 볼 필요가 없다
				if (visit[r][c] == visit[nr][nc])
					continue;

				// 두 방을 합친 넓이.
				// 이웃이 격자 바깥이면 visit이 0이고 answers[0]이 0이라
				// "자기 방 넓이"만 후보로 들어간다 (외벽은 없앨 수 없으므로 이 값이 상한이 된다)
				int tmpArea = answers[visit[r][c]] + answers[visit[nr][nc]];

				if (maxAreaSum < tmpArea)
					maxAreaSum = tmpArea;
			}
		}
	}

	// 1) 방의 개수  2) 가장 넓은 방  3) 벽 하나 없앤 뒤 가장 넓은 방
	printf("%d\n%d\n%d\n", answerCount - 1, maxAnswer, maxAreaSum);

	// printMap(); // 디버그용

	return 0;
}
