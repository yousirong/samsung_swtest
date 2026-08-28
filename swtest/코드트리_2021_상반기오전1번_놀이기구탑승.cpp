/*
	[코드트리] 2021 상반기 오전 1번 - 놀이기구 탑승
	(백준 21608 "상어 초등학교"와 같은 문제다  https://www.acmicpc.net/problem/21608)

	■ 문제 요약
	  N x N 자리표에 학생 N*N명을 순서대로 앉힌다.
	  학생마다 좋아하는 친구 4명이 정해져 있고, 자리는 아래 우선순위로 고른다.

	    1) 인접한(상하좌우) 네 칸 중 "좋아하는 친구"가 가장 많은 빈 칸
	    2) 그런 칸이 여럿이면 인접한 빈 칸이 가장 많은 칸
	    3) 그래도 여럿이면 행 번호가 작은 칸
	    4) 그래도 여럿이면 열 번호가 작은 칸

	  모두 앉힌 뒤 만족도를 구한다.
	  학생마다 인접한 좋아하는 친구 수가 0/1/2/3/4명이면 각각 0/1/10/100/1000점이고,
	  그 합이 답이다.

	■ 풀이 방침
	  요령이 필요 없는 구현 문제다. 학생 한 명마다 모든 빈 칸을 훑어 보고
	  네 단계 우선순위로 가장 좋은 칸을 고르면 된다.
	  N이 20 이하라 (N*N명) x (N*N칸) x 4방향이면 충분히 빠르다.

	  이 문제의 요점은 "우선순위 비교를 어떻게 깔끔하게 쓰는가"이다.

	    getSeatInfo() : 어떤 칸의 (좋아하는 친구 수, 빈 칸 수, 행, 열)을 한 덩어리로 만든다
	    isPriority()  : 두 칸 정보를 받아 앞의 것이 더 좋은지 판정한다

	  비교 조건을 한 곳에 모아 두면, 4중 조건을 if로 겹겹이 쓰는 것보다 훨씬 읽기 쉽고
	  순서를 헷갈릴 위험도 줄어든다.

	      if (a.like  != b.like)  return a.like  > b.like;    // 많을수록 좋다
	      if (a.empty != b.empty) return a.empty > b.empty;   // 많을수록 좋다
	      if (a.r     != b.r)     return a.r     < b.r;       // 작을수록 좋다
	      return a.c < b.c;                                   // 작을수록 좋다

	  "값이 다르면 그 기준으로 결정, 같으면 다음 기준으로" 라는 형태라
	  우선순위가 몇 단계로 늘어나도 그대로 확장된다.

	■ 자료 구조
	    MAP[r][c]        : 그 자리에 앉은 학생 번호 (0이면 빈자리, 격자 바깥은 WALL)
	    student[i]       : i번째로 앉는 학생 번호 (입력 순서 = 앉는 순서)
	    love[a][b]       : a가 b를 좋아하는지

	  격자 바깥을 WALL로 채워 두면 인접 칸을 볼 때 범위 검사가 필요 없다.
	  빈자리(0)와 바깥(WALL)을 다른 값으로 둔 것이 중요하다.
	  둘을 같은 값으로 두면 가장자리 칸의 "빈 칸 수"가 부풀려진다.

	■ 초기값 잡기
	  wanted는 like = empty = 0, r = c = INF로 시작한다.
	  이러면 첫 후보가 무조건 wanted보다 우선순위가 높아(행 번호가 INF보다 작으므로)
	  "첫 후보를 특별 취급하는 코드" 없이 자연스럽게 갱신된다.

	■ 점수 계산
	  count가 0~4이므로 board[] 표로 바꾸면 if 사슬 없이 한 줄로 끝난다.
	      board[5] = { 0, 1, 10, 100, 1000 }
*/

#include <stdio.h>

#define MAX (20 +5)
#define INF (0x7fff0000)

#define WALL (-1)

int T;

int N;
int MAP[MAX][MAX];   // 자리표 (0: 빈자리, 양수: 학생 번호, WALL: 격자 바깥)

int student[MAX * MAX];              // 앉는 순서대로의 학생 번호
bool love[MAX * MAX][MAX * MAX];     // love[a][b] : a가 b를 좋아하는가

// 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0,1,0,-1 };

// 한 자리를 평가한 결과 (우선순위 비교에 필요한 값만 모아 둔다)
struct SEAT
{
	int like;  // 인접한 칸 중 좋아하는 친구 수
	int empty; // 인접한 빈 칸 수
	int r;
	int c;
};

void input()
{
	scanf("%d", &N);

	// 격자 안팎을 일단 전부 WALL로 채운 뒤, 실제 자리만 0(빈자리)으로 되돌린다.
	// 그러면 바깥 테두리만 WALL로 남아 인접 검사에서 범위 검사가 필요 없어진다.
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = WALL;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = 0;

	for (int r = 1; r <= N * N; r++)
		for (int c = 1; c <= N * N; c++)
			love[r][c] = false;

	// 입력 순서가 곧 앉는 순서다
	for (int i = 0; i < N * N; i++)
	{
		int num, one, two, three, four;

		scanf("%d %d %d %d %d", &num, &one, &two, &three, &four);

		student[i] = num;
		love[num][one] = true;
		love[num][two] = true;
		love[num][three] = true;
		love[num][four] = true;
	}
}


void printMap() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}


/*
	index번 학생이 (r, c)에 앉는다고 할 때 그 자리의 평가값

	  like  : 인접한 칸에 앉아 있는 사람 중 index가 좋아하는 친구 수
	  empty : 인접한 빈 칸 수
*/
SEAT getSeatInfo(int index, int r, int c)
{
	int like, empty;

	like = empty = 0;
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;

		nr = r + dr[i];
		nc = c + dc[i];

		// 격자 바깥은 빈 칸이 아니다. 이 검사를 빼면 가장자리의 empty가 부풀려진다.
		if (MAP[nr][nc] == WALL) continue;

		if (MAP[nr][nc] == 0) empty++;
		else if (love[index][MAP[nr][nc]] == true) like++;
	}

	return { like, empty, r, c };
}

/*
	a가 b보다 우선순위가 높으면 true

	우선순위를 위에서부터 한 줄씩 검사하고, 값이 같으면 다음 기준으로 넘어간다.
	앞의 두 기준은 클수록 좋고, 뒤의 두 기준은 작을수록 좋다는 점에 주의.
*/
bool isPriority(SEAT a, SEAT b)
{
	if (a.like != b.like) return a.like > b.like;
	if (a.empty != b.empty) return a.empty > b.empty;
	if (a.r != b.r) return a.r < b.r;

	return a.c < b.c;
}

void simulate()
{
	// 정해진 순서대로 한 명씩 앉힌다
	for (int i = 0; i < N * N; i++)
	{
		int index = student[i];

		SEAT wanted = { 0 };

		// 행/열을 INF로 두면 첫 후보가 무조건 이겨서 자연스럽게 초기화된다
		wanted.r = wanted.c = INF;

		for (int r = 1; r <= N; r++)
		{
			for (int c = 1; c <= N; c++)
			{
				if (MAP[r][c] != 0) continue;   // 이미 누가 앉은 자리

				SEAT tmp = getSeatInfo(index, r, c);

				if (isPriority(tmp, wanted) == true)
					wanted = tmp;
			}
		}

		MAP[wanted.r][wanted.c] = index;
	}
	//printMap();
}

/*
	최종 만족도

	  인접한 좋아하는 친구 수 -> 점수 : 0 -> 0, 1 -> 1, 2 -> 10, 3 -> 100, 4 -> 1000
	  경우가 다섯 가지뿐이라 표 하나로 바꾸면 if 사슬이 사라진다.
*/
int getAnswer()
{
	int board[5] = { 0,1,10,100,1000 };

	int score = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			int index, count;

			index = MAP[r][c];
			count = 0;
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (MAP[nr][nc] == WALL) continue;

				if (love[index][MAP[nr][nc]] == true) count++;
			}

			// 아래 if 사슬과 같은 동작이다
			//if (count == 1) score += 1;
			//else if (count == 2) score += 10;
			//else if (count == 3) score += 100;
			//else if (count == 4) score += 1000;

			score += board[count];
		}
	}

	return score;
}


int main()
{
	// 이 문제는 테스트 케이스가 하나다
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();

		printf("%d\n", getAnswer());
	}

	return 0;
}
