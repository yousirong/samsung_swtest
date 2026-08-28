/*
	[코드트리] 2018 하반기 오전 2번 - 토스트 계란틀
	https://www.codetree.ai/training-field/frequent-problems/problems/toast-eggmold

	■ 문제 요약
	  N x N 격자의 각 칸에 값이 하나씩 들어 있고, 두 정수 L과 R이 주어진다.

	  하루 동안 아래 일이 한꺼번에 일어난다.
	    - 인접한(상하좌우) 두 칸의 값 차이가 L 이상 R 이하이면 두 칸 사이의 경계가 열린다
	    - 열린 경계로 이어진 칸들이 하나의 "연합"이 된다
	    - 연합에 속한 모든 칸의 값이 (연합 전체 합 / 연합 칸 수, 소수점 버림)로 바뀐다

	  더 이상 아무 연합도 생기지 않을 때까지 며칠이 걸리는지 출력한다.

	  (백준 16234 "인구 이동"과 같은 문제다)

	■ 풀이 방침
	  하루를 이렇게 처리한다.

	    1) visit을 전부 비운다
	    2) 격자를 훑다가 아직 방문하지 않은 칸을 만나면 그 칸에서 BFS를 시작한다
	    3) BFS는 조건을 만족하는 이웃을 계속 묶어 하나의 연합을 만들고,
	       끝나면 그 연합의 칸들을 평균값으로 덮어쓴 뒤 연합 크기를 돌려준다
	    4) 크기가 2 이상인 연합이 하루에 하나라도 있었으면 그날은 이동이 있었던 것

	  이동이 있었던 날만 세면서, 이동이 없는 날이 나오면 멈춘다.

	■ "하루는 동시에 일어난다"인데 BFS가 값을 바로 덮어써도 되는 이유
	  얼핏 보면 앞의 연합이 값을 바꾼 뒤 뒤의 연합이 그 바뀐 값을 보게 될 것 같지만, 그렇지 않다.

	    - 평균 갱신은 BFS의 while 루프가 끝난 "뒤"에 한다.
	      그래서 한 연합을 넓히는 동안 읽는 값은 전부 갱신 전 원본이다.
	    - 이미 다른 연합에 들어간 칸은 visit이 true라 아예 후보에서 걸러진다.
	      따라서 갱신된 값을 비교에 쓸 일이 없다.

	  결국 각 연합은 언제나 "그날 아침 상태"만 보고 만들어지므로 동시 처리와 결과가 같다.

	■ 구현 포인트
	  BFS가 끝난 시점에 queue[0 .. wp-1]에는 그 연합의 모든 칸이 들어 있다.
	  방문 표시를 큐에 넣을 때 하므로 중복이 없고, wp가 곧 연합의 크기다.
	  덕분에 연합 목록을 따로 만들 필요 없이 큐를 그대로 다시 훑어 평균을 적용할 수 있다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX (100 + 10)

int T;

int N, L, R;               // N: 격자 크기, 연합 조건은 차이가 L 이상 R 이하
int MAP[MAX][MAX];         // 현재 격자 상태
bool visit[MAX][MAX];      // 이번 날 이미 어떤 연합에 들어갔는지

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX * MAX];       // BFS용 큐 (그날의 한 연합을 담는다)

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
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
// 디버그용 격자 출력
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
// 절댓값 (stdlib.h 없이 직접 구현)
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;
}

// ---------------------------
// BFS : (r, c)가 속한 연합을 만들고 평균값으로 갱신
//
//   반환값 : 연합에 속한 칸의 개수
// ---------------------------
int BFS(int r, int c)
{
	int rp, wp;
	int sum;

	rp = wp = 0;

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	// 연합 전체 합. 시작 칸의 값부터 담아 둔다.
	sum = MAP[r][c];

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			// 격자 밖
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// 두 칸의 값 차이 (여기서 읽는 값은 전부 갱신 전 원본이다)
			int diff = abs(MAP[out.r][out.c] - MAP[nr][nc]);

			// 이미 어떤 연합에 들어갔거나, 차이가 L ~ R 범위를 벗어나면 묶을 수 없다
			if (visit[nr][nc] == true || (L <= diff && diff <= R) == false)
				continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;   // 넣는 즉시 표시 (중복 삽입 방지)
			sum += MAP[nr][nc];
		}
	}

	// 연합이 확정된 뒤에 한꺼번에 평균으로 덮어쓴다.
	// queue[0 .. wp-1]이 이번 연합의 전체 칸 목록이고, wp가 그 크기다.
	// (정수 나눗셈이라 소수점은 자동으로 버려진다)
	for (int i = 0; i < wp; i++)
	{
		RC tmp = queue[i];
		MAP[tmp.r][tmp.c] = sum / wp;
	}

	return wp;
}

// ---------------------------
// 하루 처리
//
//   반환값 : 이번 날 실제로 이동이 있었으면 true
// ---------------------------
bool simulate()
{
	// 날마다 방문 기록을 새로 시작한다
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			visit[r][c] = false;
		}
	}

	bool result = false;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 이미 어떤 연합에 속한 칸은 새 연합의 시작점이 될 수 없다
			if (visit[r][c] == true)
				continue;

			int moveEggCount = BFS(r, c);

			// 혼자인 연합(크기 1)은 아무 값도 바뀌지 않으므로 이동으로 치지 않는다
			if (moveEggCount != 1)
				result = true;
		}
	}

	return result;
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int answer = 0; // 이동이 일어난 날 수

		while (1)
		{
			bool result = simulate();

			// 이동이 없는 날이 오면 그대로 끝난다 (그날은 세지 않는다)
			if (result == false)
				break;

			answer++;
		}

		printf("%d\n", answer);
	}

	return 0;
}
