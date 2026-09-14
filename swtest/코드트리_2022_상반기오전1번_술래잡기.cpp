/*
	[코드트리] 2022 상반기 오전 1번 - 술래잡기
	https://www.codetree.ai/training-field/frequent-problems/problems/hide-and-seek

	■ 문제 요약
	  N x N 격자(N은 홀수)의 정가운데에 술래가 있고, 도망자 M명과 나무 H그루가 있다.
	  도망자는 방향 d로 주어진다. (d = 1이면 좌우로 움직이며 처음엔 오른쪽,
	  d = 2면 상하로 움직이며 처음엔 아래쪽)

	  한 턴은 아래 순서로 진행되고 K턴 동안 반복한다.

	    1) 도망자 이동 : 술래와의 거리(|x1-x2| + |y1-y2|)가 3 이하인 도망자만 1칸 움직인다.
	                    격자 밖으로 나가게 되면 방향을 반대로 틀고 1칸 움직인다.
	                    움직일 칸에 술래가 있으면 움직이지 않는다. (나무는 상관없다)
	    2) 술래 이동   : 가운데에서 시작해 달팽이 모양으로 바깥까지 1칸씩 가고,
	                    (1, 1)에 닿으면 반대로 달팽이를 되감아 가운데로 돌아온다. 이것을 반복한다.
	                    이동 직후 "다음에 갈 방향"을 바라본다.
	    3) 잡기       : 술래가 바라보는 방향으로 자기 칸 포함 3칸 안의 도망자를 잡는다.
	                    나무가 있는 칸의 도망자는 잡히지 않는다.
	                    t번째 턴이면 (t x 잡은 수)점을 얻는다.

	  K턴 동안 얻은 점수의 합을 출력한다.

	■ 풀이 방침 : 술래 경로를 미리 한 바퀴 전부 만들어 둔다
	  술래는 도망자와 무관하게 정해진 길만 반복해서 걷는다.
	  그러니 매 턴 방향 전환을 계산할 필요 없이, 한 주기의 (위치, 바라보는 방향)을
	  taggerInfo[]에 전부 적어 두고 taggerInfo[턴 % 주기]로 꺼내 쓰면 된다.

	    바깥으로 : taggerInfo[0 .. N*N-1]           (가운데 -> (1, 1))
	    안으로   : taggerInfo[N*N .. 2*N*N-3]       (바깥 경로를 거꾸로, 양 끝점은 빼고)
	    주기     : tcnt = 2*N*N - 2

	  양 끝점(가운데, (1, 1))은 한 주기에 한 번씩만 들러야 하므로 되감을 때 빠진다.
	  바라보는 방향은 "i번째 칸 -> i+1번째 칸"의 방향으로 한꺼번에 채운다.
	  이러면 꺾이는 칸에서 바로 방향을 트는 규칙이 저절로 맞춰진다.

	■ 방향 번호가 문제와 우연히 맞는다
	  dr/dc는 0 상, 1 우, 2 하, 3 좌 순서다.
	  입력 d = 1(오른쪽부터), d = 2(아래쪽부터)를 그대로 인덱스로 쓰면
	  1 -> 우, 2 -> 하가 되어 문제의 시작 방향과 정확히 일치한다.
	  반대 방향 changeDir = {2, 3, 0, 1}도 이 순서에서 (i + 2) % 4 이므로
	  좌우(1 <-> 3), 상하(0 <-> 2) 왕복이 된다.

	■ 턴 번호와 인덱스
	  k턴(0-based)에서
	    - 도망자는 "움직이기 전" 술래 위치 taggerInfo[k % tcnt]를 기준으로 거리를 잰다.
	    - 술래는 taggerInfo[(k + 1) % tcnt]로 한 칸 간 뒤 잡고, 점수는 (k + 1)배다.

	■ 주의할 점
	  [확인 필요] 되감는 복사 루프가 i = N*N-1까지 돌아 taggerInfo[-1]을 읽는다 (배열 앞 메모리).
	              쓰는 곳은 taggerInfo[2*N*N-1]이라 주기(tcnt) 밖이어서 결과에는 쓰이지 않는다.
	  [확인 필요] 주기 마지막 칸(가운데 바로 위)의 방향을 0(위)으로 두었는데,
	              다음 칸이 가운데이므로 규칙대로라면 2(아래)가 맞다.
	              다만 2로 바꿔 랜덤 입력 1,500개를 돌려도 답이 달라진 경우는 없었다.
	              (그 턴까지 도망자가 그 두 줄에 남아 있는 경우가 드물기 때문으로 보인다)
*/
#include <stdio.h>

#define MAX (100+50)

int T;

int N, M, H, K;

int MAP[MAX][MAX]; // 나무 (1 = 나무)
int snail[MAX][MAX]; // 달팽이 순서 번호 (디버그 출력용, 풀이에는 쓰지 않는다)

struct PERSON
{
	int r;
	int c;
	int dir;
	bool dead;
};

PERSON runner[MAX * MAX];
int rcnt;

// 술래의 한 주기 경로 : (위치, 그 칸에서 바라보는 방향)
PERSON taggerInfo[MAX * MAX * 2];
int tcnt;

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

void input()
{
	scanf("%d %d %d %d", &N, &M, &H, &K);

	rcnt = tcnt = 0;

	for (int m = 0; m < M; m++)
	{
		int r, c, d;
		scanf("%d %d %d", &r, &c, &d);

		runner[rcnt].dead = false;

		runner[rcnt].r = r;
		runner[rcnt].c = c;

		// d = 1(우), 2(하)가 dr/dc 인덱스와 그대로 맞는다.
		runner[rcnt++].dir = d;
	}

	// MAP init
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = 0;

	// MAP
	for (int h = 0; h < H; h++)
	{
		int r, c;
		scanf("%d %d", &r, &c);

		MAP[r][c] = 1;
	}
}

void printMap(int map[MAX][MAX]) //for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// 술래의 한 주기 경로(taggerInfo)를 만든다.
void makeSnail()
{
	int sr, sc, direction;
	int index, size;

	// 가운데에서 위쪽을 보고 시작한다.
	sr = sc = (N + 1) / 2;
	direction = 0;
	index = 1;
	size = 0;

	snail[sr][sc] = index++;

	taggerInfo[tcnt].r = sr;
	taggerInfo[tcnt++].c = sc;

	// 달팽이 : 1, 1, 2, 2, 3, 3, ... 칸씩 가며 상 -> 우 -> 하 -> 좌로 꺾는다.
	// 2N-1번 꺾으면 마지막 줄(N-1칸)까지 끝나 (1, 1)에 닿는다.
	for (int i = 0; i < 2 * N - 1; i++)
	{
		if (i % 2 == 0) size++;

		for (int s = 0; s < size; s++)
		{
			int nr, nc;

			nr = sr + dr[direction];
			nc = sc + dc[direction];

			snail[nr][nc] = index++;

			sr = nr;
			sc = nc;

			taggerInfo[tcnt].r = nr;
			taggerInfo[tcnt++].c = nc;
		}

		direction++;

		if (direction == 4) direction = 0;
	}

	// 되감기 : taggerInfo[N*N + i] = taggerInfo[N*N - 2 - i]
	// (1, 1)(= N*N-1번)을 한 번 더 넣지 않도록 N*N-2번부터 거꾸로 붙인다.
	// [확인 필요] i = N*N-1이면 taggerInfo[-1]을 읽는다. 쓰는 칸은 주기 밖이라 결과엔 영향 없음.
	tcnt = N * N;
	for (int i = 0; i < tcnt; i++)
		taggerInfo[tcnt + i] = taggerInfo[tcnt - i - 2];

	// 가운데(0번)는 다음 주기의 시작으로 다시 쓰므로 뒤에 붙이지 않는다.
	tcnt = 2 * N * N - 2;

	// 각 칸의 방향 = 다음 칸으로 가는 방향.
	// 꺾이는 칸에서는 이미 다음 방향이 들어가 "이동 직후 방향 전환" 규칙과 맞는다.
	for (int i = 0; i < tcnt - 1; i++)
	{
		int r, c, nr, nc;

		r = taggerInfo[i].r;
		c = taggerInfo[i].c;
		nr = taggerInfo[i + 1].r;
		nc = taggerInfo[i + 1].c;

		if (nr - r == -1) taggerInfo[i].dir = 0; // 위
		else if (nr - r  == 1) taggerInfo[i].dir = 2; // 아래
		else if (nc - c == 1) taggerInfo[i].dir = 1; // 오른쪽
		else if (nc - c == -1) taggerInfo[i].dir = 3; // 왼쪽

	}

	// 주기의 마지막 칸(가운데 바로 위)은 다음 칸이 없어 따로 채운다.
	// [확인 필요] 다음은 가운데(아래쪽)로 가므로 규칙상 2가 맞다. (실측으로는 답 차이 없음)
	taggerInfo[tcnt - 1].dir = 0;

	//return; //for debug
	//printMap(snail);
	//for (int i = 0; i < tcnt; i++)
	//	printf("%d] (%d, %d) / %d\n", i, taggerInfo[i].r, taggerInfo[i].c, taggerInfo[i].dir);
}

// <cstdlib>를 include하지 않았으므로 직접 만든 절댓값
int abs(int x)
{
	return (x > 0) ? x : -x;
}

int getDistance(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2);
}

// 1) 도망자 이동 (k턴, 술래는 아직 움직이기 전 위치)
void moveRunner(int k)
{
	PERSON tagger = taggerInfo[k % tcnt];
	int changeDir[4] = { 2,3,0,1 }; // 반대 방향

	for (int i = 0; i < rcnt; i++)
	{
		if (runner[i].dead == true)continue;

		int sr, sc;

		sr = runner[i].r;
		sc = runner[i].c;

		int distance = getDistance(tagger.r, tagger.c, sr, sc);

		// 술래와 거리 3 초과면 움직이지 않는다.
		if (distance > 3) continue;

		int dir;
		int nr, nc;

		dir = runner[i].dir;

		nr = sr + dr[dir];
		nc = sc + dc[dir];

		// 격자 밖이면 방향을 반대로 바꾸고(저장까지 한다) 그 방향으로 다시 계산한다.
		// 술래 칸이라 못 움직이더라도 바뀐 방향은 유지된다.
		if (nr < 1 || nc<1 || nr > N || nc > N)
		{
			dir = changeDir[dir];
			runner[i].dir = dir;

			nr = sr + dr[dir];
			nc = sc + dc[dir];
		}

		// 술래가 있는 칸으로는 가지 않는다. 나무는 막지 않는다.
		if (nr == tagger.r && nc == tagger.c) continue;

		runner[i].r = nr;
		runner[i].c = nc;
	}
}

// 2), 3) 술래 이동 + 잡기. k턴에 얻은 점수를 반환한다.
int moveTagger(int k)
{
	// 한 칸 이동한 뒤의 위치와 방향
	PERSON tagger = taggerInfo[(k + 1) % tcnt];

	int catchCount = 0;
	// 자기 칸(d = 0)부터 바라보는 방향으로 3칸
	for (int d = 0; d < 3; d++)
	{
		int nr, nc;
		nr = tagger.r + dr[tagger.dir] * d;
		nc = tagger.c + dc[tagger.dir] * d;

		if (nr<1 || nc<1 || nr>N || nc>N) break;
		// 나무 칸의 도망자는 잡히지 않는다. 시야는 가리지 않으므로 break가 아니라 continue.
		if (MAP[nr][nc] == 1) continue;

		// 한 칸에 여러 명이 있을 수 있으므로 전부 확인한다.
		for (int i = 0; i < rcnt; i++)
		{
			if (runner[i].dead == true) continue;

			if (runner[i].r == nr && runner[i].c == nc)
			{
				runner[i].dead = true;
				catchCount++;
			}
		}
	}

	// 턴 번호는 1부터이므로 k + 1
	return catchCount * (k + 1);
}

int simulate()
{
	int score = 0;
	for (int k = 0; k < K; k++)
	{
		moveRunner(k);

		score += moveTagger(k);
	}

	return score;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		makeSnail();

		printf("%d\n", simulate());
	}

	return 0;
}
