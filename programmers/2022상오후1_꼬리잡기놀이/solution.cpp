/*
	[코드트리] 2022 상반기 오후 1번 - 꼬리잡기놀이
	원본 : swtest/코드트리_2022_상반기오후1번_꼬리잡기놀이.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.

	https://www.codetree.ai/training-field/frequent-problems/problems/tail-catch-play

	■ 문제 요약
	  N x N 격자에 M개의 팀이 있다. 각 팀은 하나의 이동선(닫힌 고리) 위에 있고,
	  입력 값은 0 빈칸, 1 머리사람, 2 머리/꼬리가 아닌 사람, 3 꼬리사람, 4 이동선이다.
	  이동선끼리는 겹치지 않고, 이동선의 각 칸은 이동선 칸 두 개와 맞닿아 있다.

	  K라운드 동안 매 라운드 아래를 진행한다.

	    1) 이동 : 모든 팀이 머리사람을 따라 이동선 위로 한 칸씩 움직인다.
	    2) 공 던지기 : 라운드에 따라 정해진 줄로 공을 던진다.
	           1 ~ N 라운드     : i번째 행을 왼쪽 -> 오른쪽
	           N+1 ~ 2N 라운드  : i번째 열을 아래 -> 위
	           2N+1 ~ 3N 라운드 : 아래 행부터 오른쪽 -> 왼쪽
	           3N+1 ~ 4N 라운드 : 오른쪽 열부터 위 -> 아래
	           4N 라운드가 지나면 처음부터 반복한다.
	    3) 점수 : 공을 처음 맞은 사람이 팀에서 머리부터 k번째면 k^2점을 얻고,
	             그 팀은 머리와 꼬리가 바뀐다 (진행 방향이 반대가 된다).

	  K라운드 동안 얻은 점수의 합을 출력한다.

	■ 풀이 방침 : 이동선을 "순서가 있는 좌표 목록"으로 만든다
	  팀마다 머리 -> 몸통 -> 꼬리 -> 이동선(4) 순서로 좌표를 한 줄로 늘어놓는다.

	      position[팀][0]   = 머리 칸
	      position[팀][1..] = 몸통, ..., 꼬리, 4, 4, ... (고리를 한 바퀴 돈 순서)

	  이러면 이동은 "좌표를 한 칸씩 밀기"가 되고, 역할(value, order)은 자리(인덱스)에
	  붙은 채로 남는다. 한 칸 민 뒤 MAP에 다시 적어 주기만 하면 된다.

	    정방향 : 모든 자리가 앞 자리의 좌표로, 0번(머리)은 맨 끝(머리 바로 뒤쪽 4) 좌표로
	    역방향 : 모든 자리가 뒤 자리의 좌표로, 맨 끝은 0번 좌표로

	  머리/꼬리가 바뀌어도 라벨(1, 3)은 그대로 두고 방향만 뒤집는다(checkDirection).
	  대신 점수를 셀 때 "머리부터 몇 번째"를 뒤집어 계산한다.
	      뒤집힌 팀의 k = (사람 수) - order + 1

	■ 좌표 목록 만들기 (BFS)
	  머리에서 먼저 몸통(2) 칸 하나만 큐에 넣는다. 그다음부터는 이동선 칸이
	  각자 이동선 이웃을 두 개씩만 가지므로, 방문 안 한 이웃은 항상 하나뿐이라
	  BFS가 고리를 한 방향으로만 따라가며 순서대로 번호를 매긴다.
	  머리 옆의 다른 이웃(4 또는 꼬리)은 큐에 넣지 않았으니 반대쪽으로 새지 않는다.

	■ 공이 가는 줄 계산
	  ballDir = ((round - 1) % 4N) / N   : 0 →, 1 ↑, 2 ←, 3 ↓
	  line    = ((round - 1) % N) + 1   : 그 방향에서 몇 번째 줄인가
	  ← 방향은 아래 행부터(N + 1 - line), ↓ 방향은 오른쪽 열부터(N + 1 - line) 센다.

	■ 주의할 점
	  - 팀은 최소 3명(머리, 몸통, 꼬리)이라고 가정한다. 몸통이 없으면 첫 BFS 단계에서
	    큐가 비어 좌표 목록이 머리 하나로 끝난다.
	  - position, MAP은 input()/simulate()에서 매번 새로 채우지만 posIndex 등은 M개만 지운다.
	    한 번 실행하는 데는 문제가 없다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

#define MAX_N (20 + 5)
#define MAX_M (5 + 2)

#define HEAD (1)
#define BODY (2)
#define TAIL (3)
#define ROOP (4) // 이동선 (사람 없음)

// 공을 던지는 방향
#define RIGHT (0)
#define UP (1)
#define LEFT (2)
#define DOWN (3)

int T;

int N, M, K;

struct MAP_INFO
{
	int value; // 입력 값
	int order; // k번째 사람
	int loop; // loop의 번호
};

MAP_INFO MAP[MAX_N][MAX_N];
bool visit[MAX_N][MAX_N];

struct RCON
{
	int r;
	int c;
	int value;
	int order; // k번째 사람
};

RCON queue[MAX_N * MAX_N];

RCON position[MAX_M][MAX_N * MAX_N]; // 루프 : 1, 2, 2, 3, 4, 4, 4, 4, 4, ... 의 좌표가 생성
int posIndex[MAX_M]; // 1 ~ 4의 수 (이동선 전체 길이)
int peopleIndex[MAX_M]; // 1 ~ 3의 수 (팀의 사람 수)

bool checkDirection[MAX_M]; // 방향이 바뀌었는지 여부

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

// [수정] scanf 대신 인자로 받는다
void input(const std::vector<std::vector<int>>& board, int k)
{
	N = (int)board.size();   // [수정] scanf("%d %d %d", &N, &M, &K) 대체
	K = k;

	// 팀 수 M은 머리사람(1)의 개수와 같으므로 격자에서 세어 구한다.
	M = 0;
	for (int r = 0; r < N; r++)
		for (int c = 0; c < N; c++)
			if (board[r][c] == HEAD) M++;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			MAP[r][c].value = board[r - 1][c - 1];   // [수정] scanf 대체
			MAP[r][c].loop = MAP[r][c].order = -1;
		}
	}

	for (int i = 0; i < M; i++)
		posIndex[i] = peopleIndex[i] = checkDirection[i] = 0;
}

void printMap(int map[MAX_N][MAX_N]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printMapValue(MAP_INFO map[MAX_N][MAX_N]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c].value);
		putchar('\n');
	}
	putchar('\n');
}

void printPosition(int loop) // for debug
{
	int index = posIndex[loop];

	for (int i = 0; i < index; i++)
	{
		RCON p = position[loop][i];

		printf("%d] (%d, %d) %d, %d\n", i, p.r, p.c, p.order, p.value);
	}
	putchar('\n');
}

// 머리 (r, c)에서 시작해 팀 loop의 이동선을 머리 -> 몸통 -> 꼬리 -> 4 순서로 적는다.
void BFS(int r, int c, int loop)
{
	int rp, wp, order;

	order = 1;

	MAP[r][c].loop = loop;
	MAP[r][c].order = order;

	position[loop][posIndex[loop]].r = r;
	position[loop][posIndex[loop]].c = c;
	position[loop][posIndex[loop]].order = order++;
	position[loop][posIndex[loop]++].value = HEAD;

	rp = wp = 0;

	visit[r][c] = true;

	// 1 -> 2 먼저 찾기
	// 몸통 쪽 이웃 하나만 큐에 넣어야 BFS가 몸통 방향으로만 고리를 따라간다.
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;

		nr = r + dr[i];
		nc = c + dc[i];

		if (nr < 1 || nc < 1 || nr > N || nc > N) continue;

		if (MAP[nr][nc].value == BODY)
		{
			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;

			MAP[nr][nc].loop = loop;
			MAP[nr][nc].order = order;

			position[loop][posIndex[loop]].r = nr;
			position[loop][posIndex[loop]].c = nc;
			position[loop][posIndex[loop]].order = order++;
			position[loop][posIndex[loop]++].value = BODY;

			break;
		}
	}

	// 1, 2 count
	peopleIndex[loop] = 2;

	// 이동선 칸은 이동선 이웃이 딱 두 개라 방문 안 한 이웃은 하나뿐이다.
	// 그래서 BFS지만 실제로는 고리를 한 줄로 따라가는 셈이다.
	while (rp < wp)
	{
		RCON out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr < 1 || nc < 1 || nr > N || nc > N) continue;
			if (MAP[nr][nc].value == 0 || visit[nr][nc] == true) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;

			MAP[nr][nc].loop = loop;
			MAP[nr][nc].order = order;

			position[loop][posIndex[loop]].r = nr;
			position[loop][posIndex[loop]].c = nc;
			position[loop][posIndex[loop]].order = order++;
			position[loop][posIndex[loop]++].value = MAP[nr][nc].value;

			// 사람(몸통, 꼬리)이면 팀 인원에 더한다.
			if (MAP[nr][nc].value <= TAIL) peopleIndex[loop]++;
		}
	}
}

// 팀 loop를 한 칸 움직인다. 역할(value, order)은 자리에 두고 좌표만 민다.
void move(int loop)
{
	RCON tmp;
	int index = posIndex[loop];
	bool check = checkDirection[loop];

	if (check == false) // 1 -> 4 -> .. -> 2 -> 1로 이동
	{
		// 각 자리가 앞 자리의 좌표로 가고, 머리는 고리의 맨 끝(머리 바로 뒤쪽) 좌표로 간다.
		tmp = position[loop][index - 1];

		for (int i = index - 2; i >= 0; i--)
		{
			int r, c;

			r = position[loop][i].r;
			c = position[loop][i].c;

			position[loop][i + 1].r = r;
			position[loop][i + 1].c = c;
		}

		position[loop][0].r = tmp.r;
		position[loop][0].c = tmp.c;
	}
	else // 1 -> 2 -> 2 -> ... -> 3 -> 4 -> 1로 이동
	{
		// 방향이 뒤집힌 팀 : 반대로 민다. 꼬리 자리(실제 머리)가 4 쪽으로 나아간다.
		tmp = position[loop][0];

		for (int i = 1; i < index; i++)
		{
			int r, c;

			r = position[loop][i].r;
			c = position[loop][i].c;

			position[loop][i - 1].r = r;
			position[loop][i - 1].c = c;
		}

		position[loop][index - 1].r = tmp.r;
		position[loop][index - 1].c = tmp.c;
	}

	// 바뀐 좌표에 역할을 다시 적는다. (고리의 모든 칸을 덮어쓰므로 지울 필요가 없다)
	for (int i = 0; i < index; i++)
	{
		int r = position[loop][i].r;
		int c = position[loop][i].c;
		int order = position[loop][i].order;
		int value = position[loop][i].value;

		MAP[r][c].order = order;
		MAP[r][c].value = value;
	}
}

// round번째 공을 던져 얻는 점수
int getScore(int round)
{
	// 사람(1, 2, 3)만 공을 맞는다. 0 빈칸, 4 이동선은 통과
	bool hit[5] = { false, true, true, true, false };

	// → 1 ~ N ==> 0번 방향
	// ↑ N + 1 ~ 2 * N ==> 1번 방향
	// ← 2 * N + 1 ~ 3 * N ==> 2번 방향
	// ↓ 3 * N + 1 ~ 4 * N ==> 3번 방향

	// if N == 7
	// → 1 ~ 7  ==> 0번 방향
	// ↑ 8 ~ 14 ==> 1번 방향
	// ← 15 ~ 21 ==> 2번 방향
	// ↓ 22 ~ 28 ==> 3번 방향

	int ballDir = (((round - 1) % (4 * N))) / N;
	int line = ((round - 1) % N) + 1;

	// 네 방향 모두 같은 틀이다 : 줄을 따라가다 처음 만난 사람의 순서로 점수를 내고 방향을 뒤집는다.
	if (ballDir == RIGHT) // →   line번째 행, 왼쪽부터
	{
		for (int c = 1; c <= N; c++)
		{
			if (hit[MAP[line][c].value] == true)
			{
				int order = MAP[line][c].order;
				int loop = MAP[line][c].loop;

				// 방향이 뒤집힌 팀은 라벨이 그대로라 머리부터의 순서를 뒤집어 센다.
				if (checkDirection[loop] == true)
					order = peopleIndex[loop] - order + 1;

				checkDirection[loop] = !checkDirection[loop];

				return order * order;
			}
		}
	}
	else if (ballDir == UP) // ↑   line번째 열, 아래부터
	{
		for (int r = N; r >= 1; r--)
		{
			if (hit[MAP[r][line].value] == true)
			{
				int order = MAP[r][line].order;
				int loop = MAP[r][line].loop;

				if (checkDirection[loop] == true)
					order = peopleIndex[loop] - order + 1;

				checkDirection[loop] = !checkDirection[loop];

				return order * order;
			}
		}
	}
	else if (ballDir == LEFT) // ←   아래에서 line번째 행, 오른쪽부터
	{
		for (int c = N; c >= 1; c--)
		{
			if (hit[MAP[N + 1 - line][c].value] == true)
			{
				int order = MAP[N + 1 - line][c].order;
				int loop = MAP[N + 1 - line][c].loop;

				if (checkDirection[loop] == true)
					order = peopleIndex[loop] - order + 1;

				checkDirection[loop] = !checkDirection[loop];

				return order * order;
			}
		}
	}
	else if (ballDir == DOWN) // ↓   오른쪽에서 line번째 열, 위부터
	{
		for (int r = 1; r <= N; r++)
		{
			if (hit[MAP[r][N + 1 - line].value] == true)
			{
				int order = MAP[r][N + 1 - line].order;
				int loop = MAP[r][N + 1 - line].loop;

				if (checkDirection[loop] == true)
					order = peopleIndex[loop] - order + 1;

				checkDirection[loop] = !checkDirection[loop];

				return order * order;
			}
		}
	}

	// 아무도 맞지 않았다.
	return 0;
}

int simulate()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			visit[r][c] = false;

	// 머리(1)를 찾을 때마다 한 팀의 이동선을 만든다.
	int loop = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c].value != HEAD || visit[r][c] == true) continue;

			BFS(r, c, loop);

			//printPosition(loop);

			loop++;
		}
	}

	int score = 0;
	for (int k = 1; k <= K; k++)
	{
		// 1) 모든 팀 이동 -> 2), 3) 공 던지기와 점수
		for (int m = 0; m < M; m++) move(m);

		score += getScore(k);
	}

	return score;
}

// [수정] main() -> solution()
int solution(std::vector<std::vector<int>> board, int k)
{
	input(board, k);

	return simulate();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n, m, k;
	scanf("%d %d %d", &n, &m, &k);   // 원본과 같은 형식 (m은 solution에서 격자로부터 다시 구한다)

	std::vector<std::vector<int>> board(n, std::vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	int ans = solution(board, k);

#ifdef REPEAT_TEST
	int ans2 = solution(board, k);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);

	return 0;
}
#endif
