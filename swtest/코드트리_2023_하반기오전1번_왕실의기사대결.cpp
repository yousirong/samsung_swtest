/*
	[코드트리] 2023 하반기 오전 1번 - 왕실의 기사 대결
	https://www.codetree.ai/training-field/frequent-problems/problems/royal-knight-duel

	■ 문제 요약
	  L x L 체스판의 각 칸은 0(빈칸), 1(함정), 2(벽)이다.
	  기사 N명은 (좌상단 행, 좌상단 열, 높이 h, 너비 w, 체력 k)로 주어지는 직사각형이다.

	  명령 Q개가 (기사 번호 i, 방향 d)로 주어진다. 방향은 0 위, 1 오른쪽, 2 아래, 3 왼쪽이다.

	    - 이미 사라진 기사(체력이 0 이하)를 부르면 아무 일도 없다.
	    - i번 기사를 d 방향으로 한 칸 밀면, 그 방향에 닿는 다른 기사도 연쇄적으로 밀린다.
	    - 밀리는 기사 중 하나라도 벽(또는 판 밖)에 닿으면 아무도 움직이지 않는다.
	    - 움직인 뒤, 명령을 받은 기사를 뺀 나머지 밀린 기사들은
	      자기 영역에 놓인 함정 수만큼 체력이 깎인다.
	    - 체력이 0 이하가 된 기사는 판에서 사라진다.

	  명령을 모두 처리한 뒤, 살아 있는 기사들이 받은 피해량의 합을 출력한다.

	■ 풀이 방침
	  - 기사는 칸이 아니라 직사각형이므로, 매 명령마다 tmpMAP에 "이 칸은 몇 번 기사"인지 찍는다.
	    사라진 기사는 찍지 않으므로 자동으로 통행 대상에서 빠진다(setKnight).
	  - 연쇄 밀림은 BFS로 찾는다. 밀리는 기사의 칸들을 큐에 넣고,
	    각 칸의 d 방향 이웃을 보며
	        벽이면        -> 즉시 return (아무도 움직이지 않는다)
	        다른 기사면   -> 그 기사의 칸을 전부 큐에 넣는다
	    끝까지 벽을 만나지 않았다면 큐에 담긴 기사들이 모두 한 칸 움직인다.
	  - 판 테두리를 WALL로 채워 두었기 때문에 "판 밖으로 밀림"도 벽 검사 하나로 끝난다.

	■ 입력 값을 바로 의미 있는 값으로 바꿔 둔다
	  입력의 1(함정)을 TRAP(-1), 2(벽)을 WALL(-2)로 바꿔 저장한다.
	  덕분에 "기사 번호(양수)"와 "지형(음수)"이 섞이지 않고, 빈칸은 0 하나로 통일된다.

	■ 피해량 계산
	  체력이 깎이는 것은 "명령을 받은 기사를 뺀" 밀린 기사들뿐이다 (n == index는 건너뛴다).
	  움직인 뒤의 영역을 기준으로 함정 수를 센다.
	  답은 살아 있는 기사만 모아 (처음 체력 - 지금 체력)을 더한 값이다.
	  그래서 처음 체력을 init_k에 따로 보관해 둔다.

	■ 주의할 점
	  - BFS의 return은 "밀 수 없음"을 뜻한다. 이미 큐에 담아 둔 것은 버려지고 아무도 움직이지 않는다.
	  - 사라진 기사는 좌표를 그대로 들고 있지만 setKnight에서 빠지므로 다른 기사를 막지 않는다.
	  - 함정은 여러 번 밟을 수 있다. 밀릴 때마다 새 영역의 함정 수만큼 다시 깎인다.
*/
#include <stdio.h>

#define MAX_L (40+5)
#define MAX_N (30+5)

#define EMPTY (0)	
#define TRAP (-1)
#define	WALL (-2)

int T;

int L, N, Q;
int MAP[MAX_L][MAX_L];
int tmpMAP[MAX_L][MAX_L];

struct COMMAND
{
	int index;
	int direction;
};

COMMAND command[100 + 10];

struct RCI
{
	int r;
	int c;
	int index;
};

RCI queue[MAX_L * MAX_L];

struct KNIGHT
{
	int r;
	int c;
	int h;
	int w;
	int k; // 기사의 체력
	int init_k; // 기사의 원래 체력
};

KNIGHT knight[MAX_N];

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

void input()
{
	scanf("%d %d %d", &L, &N, &Q);

	for (int r = 0; r <= L+1; r++)
		for (int c = 0; c <= L+1; c++)
			MAP[r][c] = WALL;

	for (int r = 1; r <= L; r++)
	{
		for (int c = 1; c <= L; c++)
		{
			scanf("%d", &MAP[r][c]);

			if (MAP[r][c] == 1) MAP[r][c] = TRAP;
			// 입력의 1, 2를 음수로 바꿔 두면 기사 번호(양수)와 섞이지 않는다.
			else if (MAP[r][c] == 2) MAP[r][c] = WALL;
		}
	}

	for (int n = 1; n <= N; n++)
	{
		scanf("%d %d %d %d %d", &knight[n].r, &knight[n].c, &knight[n].h, &knight[n].w, &knight[n].k);
		
		// 최초 체력 저장
		knight[n].init_k = knight[n].k;
	}

	for (int q = 0; q < Q; q++)
		scanf("%d %d", &command[q].index, &command[q].direction);
}

void printMap(int map[MAX_L][MAX_L]) // for debug
{
	for (int r = 1; r <= L; r++)
	{
		for (int c = 1; c <= L; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printStatus()
{
	for (int n = 1; n <= N; n++)
	{
		KNIGHT tmp = knight[n];
		printf("%d] (%d, %d) %d, %d, : %d\n", n, tmp.r, tmp.c, tmp.h, tmp.w, tmp.k);
	}
	putchar('\n');
}

void setKnight()
{
	for (int r = 1; r <= L; r++)
		for (int c = 1; c <= L; c++)
			tmpMAP[r][c] = 0;

	for (int n = 1; n <= N; n++)
	{
		if (knight[n].k <= 0) continue;
		// 사라진 기사는 찍지 않는다. 그래서 다른 기사를 막지도 않는다.

		int kr, kc, kh, kw;
		kr = knight[n].r;
		kc = knight[n].c;
		kh = knight[n].h;
		kw = knight[n].w;

		for (int r = kr; r < (kr + kh); r++)
			for (int c = kc; c < (kc + kw); c++)
				tmpMAP[r][c] = n;	
	}
}

void BFS(int index, int direction)
{
	if (knight[index].k <= 0) return;

	int rp, wp;
	bool visit[MAX_L][MAX_L] = { 0 };
	int kr, kc, kh, kw;

	kr = knight[index].r;
	kc = knight[index].c;
	kh = knight[index].h;
	kw = knight[index].w;

	rp = wp = 0;

	for (int r = kr; r < (kr + kh); r++)
	{
		for (int c = kc; c < (kc + kw); c++)
		{
			queue[wp].r = r;
			queue[wp].c = c;
			queue[wp++].index = index;

			visit[r][c] = true;
		}
	}

	while (rp < wp)
	{
		RCI out = queue[rp++];

		int nr, nc;

		nr = out.r + dr[direction];
		nc = out.c + dc[direction];

		if (MAP[nr][nc] == WALL) return;
		// 벽(또는 판 밖)에 닿았다. 이 명령은 아무도 움직이지 않는다.

		if (visit[nr][nc] == true || tmpMAP[nr][nc] == EMPTY) continue;
		// 빈칸이면 더 밀 대상이 없고, 이미 담은 칸은 건너뛴다.

		int another = tmpMAP[nr][nc];
		int kr, kc, kh, kw;

		kr = knight[another].r;
		kc = knight[another].c;
		kh = knight[another].h;
		kw = knight[another].w;

		for (int r = kr; r < (kr + kh); r++)
		{
			for (int c = kc; c < (kc + kw); c++)
			{
				queue[wp].r = r;
				queue[wp].c = c;
				queue[wp++].index = another;

				visit[r][c] = true;
			}
		}
	}

	bool check[MAX_N] = { 0 };
	for (int i = 0; i < wp; i++)
	{
		int knightIndex = queue[i].index;

		check[knightIndex] = true;
	}

	for (int n = 1; n <= N; n++)
	{
		if (check[n] == false) continue;

		knight[n].r = knight[n].r + dr[direction];
		knight[n].c = knight[n].c + dc[direction];

		if (n == index) continue;
		// 명령을 받은 기사는 함정 피해를 입지 않는다.

		int kr, kc, kh, kw;

		kr = knight[n].r;
		kc = knight[n].c;
		kh = knight[n].h;
		kw = knight[n].w;

		for (int r = kr; r < (kr + kh); r++)
			for (int c = kc; c < (kc + kw); c++)
				if (MAP[r][c] == TRAP) knight[n].k--;
	}
	//printMap(tmpMAP);


}

void simulate()
{
	for (int q = 0; q < Q; q++)
	{
		setKnight();
		BFS(command[q].index, command[q].direction);
		//printMap(MAP);
		//printStatus();
	}

}

int getAnswer()
{
	int sum = 0;
	for (int n = 1; n <= N; n++)
	{
		if (knight[n].k > 0)sum += (knight[n].init_k - knight[n].k);
	}
	return sum;
}

int main()
{
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
