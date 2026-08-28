/*
	[코드트리] 2018 상반기 오전 2번 - 디버깅
	https://www.codetree.ai/training-field/frequent-problems/problems/debugging
	(= 백준 15684 사다리 조작, 이쪽이 테스트 케이스가 더 강하다)
	https://www.acmicpc.net/problem/15684

	■ 문제 요약
	  세로선이 C개, 가로선을 놓을 수 있는 가로 줄이 R개인 사다리가 있다.
	  이미 놓여 있는 가로선 M개가 주어진다.

	  가로선을 최대 3개까지 더 놓아서, i번 세로선에서 출발하면 i번으로 도착하도록
	  (= 사다리가 아무 것도 바꾸지 않도록) 만들려 한다.
	  필요한 가로선의 최소 개수를 출력하고, 3개로도 불가능하면 -1을 출력한다.

	  가로선은 서로 인접해서 놓을 수 없다. 즉 같은 줄에서 바로 옆 칸에는 놓을 수 없다.

	■ 자료 구조
	    MAP[r][c] == 1  <=>  r번 줄에서 c번 세로선과 c+1번 세로선을 잇는 가로선이 있다

	■ 판정 : check()
	  실제로 사람을 내려보낼 필요 없이, 각 세로선 자리에 놓인 번호를 위에서 아래로 훑으며
	  가로선이 있을 때마다 이웃과 맞바꾸면 된다.

	    start[c] : 지금 c번 세로선 자리에 있는 번호 (처음에는 start[c] = c)
	    r번 줄에서 MAP[r][c] == 1 이면 start[c] 와 start[c+1] 을 swap

	  다 내려간 뒤 모든 c에 대해 start[c] == c 이면 성공이다.

	■ 탐색 : 개수를 늘려 가며 조합 시도
	  최소 개수를 구해야 하므로, 0개 -> 1개 -> 2개 -> 3개 순으로 시도하고
	  처음 성공하는 개수를 그대로 답으로 돌려준다.

	    1) 먼저 아무것도 안 놓고 check()
	    2) setup = 1, 2, 3 에 대해 "후보 위치 중 setup개를 고르는 조합"을 DFS로 전부 시도

	  DFS는 조합이므로 다음 시작 인덱스를 i + 1로 넘기고,
	  MAP에 직접 1을 썼다가 돌아올 때 0으로 되돌리는 백트래킹을 쓴다.

	■ 인접 금지 조건을 두 군데서 처리한다
	  1) getEmptyLadder() : 처음부터 "기존 가로선과 붙어 있는 자리"는 후보에서 제외한다.
	     기존 가로선은 없어지지 않으므로 이런 자리는 영원히 못 쓴다.
	  2) DFS 안 : 이번 탐색에서 방금 놓은 가로선 때문에 새로 못 쓰게 되는 자리가 생기므로,
	     실제로 놓기 직전에 MAP[r][c-1], MAP[r][c+1]을 다시 확인한다.

	  c가 1일 때 MAP[r][0]을 읽지만 전역 배열이라 0이므로 안전하다.

	■ 참고 (더 빠르게 하려면)
	  지금은 PASS가 참이 된 뒤에도 DFS가 남은 조합을 계속 훑는다.
	  DFS 맨 앞에 if (PASS == true) return; 을 넣으면 성공하는 순간 바로 빠져나올 수 있다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX_H (30 + 5)
#define MAX_N (10 + 5)

int T;

int C, M, R;                // C: 세로선 수, M: 기존 가로선 수, R: 가로 줄 수
int MAP[MAX_H][MAX_N];      // MAP[r][c] == 1 이면 c와 c+1을 잇는 가로선이 있다

int num_of_cases[MAX_H * MAX_N]; // 예전 구현의 흔적. 지금은 쓰지 않는다.

// 가로선을 새로 놓을 수 있는 후보 자리
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC ladder[MAX_H * MAX_N];
int lcnt;                   // 후보 자리 개수

bool PASS;                  // 지금 시도 중인 개수로 성공한 경우가 있었는지

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &C, &M, &R);

	// 사다리 초기화
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
		{
			MAP[r][c] = 0;
		}
	}

	// 기존 가로선 : (r, c)는 c번과 c+1번 세로선을 잇는다
	for (int i = 0; i < M; i++)
	{
		int r, c;
		scanf("%d %d", &r, &c);

		MAP[r][c] = 1;
	}
}

// ---------------------------
// 디버그용 사다리 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 배열 복사 (예전 구현에서 쓰던 함수. 지금 흐름에서는 호출하지 않는다)
// ---------------------------
void copyMap(int copy[MAX_H][MAX_N], int original[MAX_H][MAX_N])
{
	for (int i = 1; i <= R; i++)
	{
		for (int k = 1; k <= C; k++)
		{
			copy[i][k] = original[i][k];
		}
	}
}

// ---------------------------
// 디버그용 출력 (지금은 쓰지 않는다)
// ---------------------------
void printCases()
{
	for (int i = 0; i < lcnt; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 지금 MAP 상태에서 모든 세로선이 제자리로 돌아오는지 검사
//
// 사람을 하나씩 내려보내는 대신, 번호판을 통째로 들고 한 번만 내려가면 된다.
// 가로선을 만날 때마다 이웃 자리와 번호를 맞바꾸는 것이 곧 "옆 줄로 건너가기"다.
//
// (별도의 임시 배열에 가로선을 설치해 검사하던 예전 방식 대신,
//  지금은 DFS가 MAP에 직접 놓고 되돌리므로 이 함수는 MAP을 그대로 읽기만 한다)
// ---------------------------
bool check()
{
	int start[MAX_N] = { 0 };

	// 처음에는 c번 자리에 c번 번호가 있다
	for (int c = 1; c <= C; c++)
		start[c] = c;

	// 위에서 아래로 한 줄씩 내려가며 가로선이 있으면 맞바꾼다
	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C; c++)
		{
			if (MAP[r][c] == 0)
				continue;

			int tmp = start[c];
			start[c] = start[c + 1];
			start[c + 1] = tmp;
		}
	}

	// 전부 제자리로 돌아왔는가
	for (int c = 1; c <= C; c++)
	{
		if (start[c] != c)
			return false;
	}

	return true;
}

// ---------------------------
// DFS : 후보 자리 중 setup개를 고르는 모든 조합을 시도
//
//   depth : 지금까지 놓은 개수
//   start : 이번에 고를 수 있는 최소 후보 인덱스 (조합이므로 뒤로만 간다)
//   setup : 이번 라운드에서 놓기로 한 총 개수
// ---------------------------
void DFS(int depth, int start, int setup)
{
	// 정해진 개수를 다 놓았으면 현재 사다리를 검사
	if (depth == setup)
	{
		// printCases();

		if (check() == true)
			PASS = true;

		return;
	}

	for (int i = start; i < lcnt; i++)
	{
		int r = ladder[i].r;
		int c = ladder[i].c;

		// 이번 탐색에서 방금 놓은 가로선 때문에 붙어 버리는 자리는 건너뛴다.
		// (후보를 만들 때는 기존 가로선만 고려했으므로 여기서 다시 봐야 한다)
		if (MAP[r][c - 1] == 1 || MAP[r][c + 1] == 1)
			continue;

		// 가로선 설치
		MAP[r][c] = 1;

		DFS(depth + 1, i + 1, setup);

		// 되돌리기 (백트래킹)
		MAP[r][c] = 0;
	}
}

// ---------------------------
// 최소 개수 찾기
//
// 0개부터 시작해 하나씩 늘려 가며 처음 성공하는 개수를 답으로 삼는다.
// 최대 3개까지 해 보고 안 되면 -1.
// ---------------------------
int simulate()
{
	// 아무것도 안 놓아도 이미 조건을 만족하는 경우
	if (check() == true)
		return 0;

	for (int setup = 1; setup <= 3; setup++)
	{
		DFS(0, 0, setup);

		// 개수를 늘려 가며 처음 성공한 지점이 곧 최솟값이다
		if (PASS == true)
			return setup;
	}

	return -1;
}

// ---------------------------
// 가로선을 새로 놓을 수 있는 후보 자리 수집
//
// 조건 : 그 자리가 비어 있고, 좌우 이웃 자리에도 가로선이 없어야 한다.
//        (기존 가로선은 사라지지 않으므로 여기서 걸러진 자리는 영원히 쓸 수 없다)
//
// 가로선 (r, c)는 c와 c+1을 잇는 것이므로 c는 C-1까지만 유효하다.
// ---------------------------
void getEmptyLadder()
{
	lcnt = 0;

	for (int r = 1; r <= R; r++)
	{
		for (int c = 1; c <= C - 1; c++)
		{
			if ((MAP[r][c] == 0 && MAP[r][c + 1] == 0)
				&& (MAP[r][c] == 0 && MAP[r][c - 1] == 0))
			{
				ladder[lcnt].r = r;
				ladder[lcnt++].c = c;
			}
		}
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// 놓을 수 있는 자리를 먼저 목록으로 만들어 둔다
		getEmptyLadder();

		PASS = false;

		printf("%d\n", simulate());
	}

	return 0;
}
