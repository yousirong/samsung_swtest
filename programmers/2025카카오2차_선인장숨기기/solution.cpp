/*
	[프로그래머스] 2025 카카오 하반기 2차 - 선인장 숨기기

	[프로그래머스 제출용]  원본 : swtest/프로그래머스_2025_하반기2차_선인장숨기기.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://school.programmers.co.kr/learn/courses/30/lessons/468379

	■ 문제 요약
	  m행 n열 격자(사막)에 세로 h, 가로 w 크기의 선인장 구역을 놓는다. 회전은 못 한다.
	  빗방울은 주어진 순서대로 칸에 떨어진다(같은 칸에 두 번 떨어지지 않는다).
	  선인장 구역에 포함된 칸에 빗방울이 처음 떨어진 순번이 "비를 맞는 순간"이다.

	  비를 가장 늦게 맞는 위치를 고른다. 아예 맞지 않는 위치가 있으면 그쪽이 우선이다.
	  후보가 여럿이면 행이 가장 작은 것, 그다음 열이 가장 작은 것.
	  고른 구역의 왼쪽 위 좌표를 [행, 열]로 반환한다.

	  제한 : 1 <= m, n <= 500,000 이고 m x n <= 500,000, 1 <= h <= m, 1 <= w <= n

	  입출력 예
	      4 5 2 2 / (0,0) (3,1) (1,3) (2,4) (1,1) (2,2) (2,3) (0,4)  -> [2, 2]
	      3 3 1 1 / (0,0) (0,1) (0,2) (1,0)                          -> [1, 1]
	      4 6 3 4 / (1,2)                                            -> [0, 0]
	      4 6 1 2 / 홀수 열만 순서대로 12칸                           -> [3, 4]
	      2 2 2 2 / 네 칸 모두                                        -> [0, 0]
	      4 4 3 1 / (2,0) (1,3) (3,2) (0,1)                          -> [0, 2]

	■ 문제를 다시 쓰면 : "모든 h x w 창의 최솟값 중 최댓값"
	  칸마다 "그 칸에 비가 떨어진 순번"을 적어 둔다(한 번도 안 떨어지면 INF).

	      dropTime[r][c] = 순번 (1부터) 또는 INF

	  선인장 구역을 (r, c)에 놓으면, 비를 맞는 순간은 그 구역 h x w 칸의 dropTime 중 최솟값이다.
	  가장 늦게 맞으려면 그 최솟값이 가장 큰 자리를 찾으면 된다.
	  "아예 안 맞는 자리"는 최솟값이 INF인 자리라서 따로 처리할 필요가 없다.
	  행 우선으로 훑으며 "더 클 때만" 갱신하면 동점일 때 행 -> 열이 작은 자리가 남는다.

	■ 최솟값을 빠르게 구하는 방법 : 창을 밀면서 단조 덱 쓰기
	  구역마다 h x w 칸을 다 보면 (m x n) x (h x w) 라서 너무 느리다.
	  대신 두 번에 나눠 민다.

	      1) 각 행에서 가로 w 창의 최솟값  -> rowMin[r][c]
	      2) rowMin 을 세로 h 창으로 다시 밀기 -> windowMin[r][c]

	  각 단계가 "1차원 슬라이딩 윈도 최솟값"이고, 단조 덱을 쓰면 칸마다 상수 시간이다.
	  전체는 O(m x n)이고 m x n <= 500,000 이라 넉넉하다.

	  단조 덱의 규칙은 두 줄뿐이다.
	      - 새로 들어온 값보다 크거나 같은 값은 뒤에서 버린다 (앞으로 최솟값이 될 수 없다)
	      - 창을 벗어난 인덱스는 앞에서 버린다
	  그러면 덱의 맨 앞이 언제나 그 창의 최솟값이다.

	■ 2차원 배열을 쓰지 못하는 이유
	  m, n이 각각 500,000까지 갈 수 있어서 [500000][500000] 같은 배열은 만들 수 없다.
	  대신 m x n <= 500,000 이라는 조건을 이용해 한 줄로 펴서 쓴다.

	      at(r, c) = r * N + c

	  rowMin, windowMin도 같은 방식으로 같은 배열 모양에 담는다.
	  실제로 쓰는 범위만 다르다 (rowMin은 열 0 ~ N-W, windowMin은 행 0 ~ M-H).

	■ 주의할 점
	  - dropTime의 순번은 1부터 시작한다. 0을 쓰면 "비가 안 온 칸(INF)"과 헷갈리지 않지만,
	    답을 고를 때 best의 초기값을 -1로 둬야 순번 0과 구분된다. 여기서는 1부터 써서 그 문제가 없다.
	  - 덱에는 값이 아니라 인덱스를 담는다. 창을 벗어났는지 판단해야 하기 때문이다.
	  - 창 크기가 1이어도(h = 1 또는 w = 1) 같은 코드가 그대로 동작한다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX_CELL (500000 + 10)
#define INF (0x7fffffff)

int T;

int M, N, H, W;   // 격자 세로, 가로 / 선인장 세로, 가로
int K;            // 빗방울 개수

int dropTime[MAX_CELL];    // 그 칸에 떨어진 빗방울 순번 (1부터, 안 떨어지면 INF)
int rowMin[MAX_CELL];      // 가로 W 창의 최솟값
int windowMin[MAX_CELL];   // 가로 W + 세로 H 창의 최솟값 (= 그 자리에 놓았을 때 비를 맞는 순간)

int deq[MAX_CELL];         // 단조 덱 (값이 아니라 인덱스를 담는다)
int head, tail;

int answerR, answerC;

// 2차원 좌표를 한 줄로 펴서 쓴다 (m, n이 각각 50만까지 갈 수 있어 2차원 배열을 못 쓴다)
int at(int r, int c)
{
	return r * N + c;
}

// [수정] scanf 대신 인자로 받는다
// drops[i] = {행, 열}  (i+1번째로 떨어진 빗방울)
void input(int m, int n, int h, int w, const vector<vector<int>>& drops)
{
	M = m;   // [수정] scanf("%d %d %d %d", &M, &N, &H, &W) 대체
	N = n;
	H = h;
	W = w;

	for (int i = 0; i < M * N; i++)
		dropTime[i] = INF;

	K = (int)drops.size();   // [수정] scanf("%d", &K) 대체

	for (int k = 1; k <= K; k++)
	{
		int r = drops[k - 1][0];   // [수정] scanf 대체
		int c = drops[k - 1][1];

		dropTime[at(r, c)] = k;   // 순번은 1부터
	}
}

void printMap(int map[MAX_CELL]) // for debug
{
	for (int r = 0; r < M; r++)
	{
		for (int c = 0; c < N; c++)
		{
			if (map[at(r, c)] == INF) printf("  . ");
			else printf("%3d ", map[at(r, c)]);
		}
		putchar('\n');
	}
	putchar('\n');
}

// 1) 각 행에서 가로 W 창의 최솟값을 구한다
void slideRow()
{
	for (int r = 0; r < M; r++)
	{
		head = tail = 0;   // 행이 바뀌면 덱을 비운다

		for (int c = 0; c < N; c++)
		{
			int value = dropTime[at(r, c)];

			// 새 값보다 크거나 같은 값은 앞으로 최솟값이 될 수 없다
			while (head < tail && dropTime[at(r, deq[tail - 1])] >= value) tail--;

			deq[tail++] = c;

			// 창을 벗어난 인덱스는 앞에서 버린다
			if (deq[head] <= c - W) head++;

			// 창이 다 채워졌으면 맨 앞이 그 창의 최솟값이다
			if (c >= W - 1) rowMin[at(r, c - W + 1)] = dropTime[at(r, deq[head])];
		}
	}
}

// 2) rowMin을 세로 H 창으로 다시 밀어 h x w 창의 최솟값을 만든다
void slideColumn()
{
	for (int c = 0; c + W <= N; c++)
	{
		head = tail = 0;   // 열이 바뀌면 덱을 비운다

		for (int r = 0; r < M; r++)
		{
			int value = rowMin[at(r, c)];

			while (head < tail && rowMin[at(deq[tail - 1], c)] >= value) tail--;

			deq[tail++] = r;

			if (deq[head] <= r - H) head++;

			if (r >= H - 1) windowMin[at(r - H + 1, c)] = rowMin[at(deq[head], c)];
		}
	}
}

// 비를 가장 늦게 맞는 자리를 고른다 (행 우선으로 훑고 "더 클 때만" 갱신)
void getAnswer()
{
	int best = -1;

	answerR = answerC = 0;

	for (int r = 0; r + H <= M; r++)
	{
		for (int c = 0; c + W <= N; c++)
		{
			if (best < windowMin[at(r, c)])
			{
				best = windowMin[at(r, c)];

				answerR = r;
				answerC = c;
			}
		}
	}
}

// [수정] main() -> solution(). 고른 구역의 왼쪽 위 좌표를 [행, 열]로 반환한다.
vector<int> solution(int m, int n, int h, int w, vector<vector<int>> drops)
{
	input(m, n, h, w, drops);

	slideRow();

	slideColumn();

	getAnswer();

	int out[2];   // [수정] printf -> 목록으로 반환
	out[0] = answerR;
	out[1] = answerC;

	return vector<int>(out, out + 2);
}
