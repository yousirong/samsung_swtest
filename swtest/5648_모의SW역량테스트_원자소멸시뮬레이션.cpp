/*
	[SWEA] 5648 - [모의 SW 역량테스트] 원자 소멸 시뮬레이션
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRFInKex8DFAUo

	■ 문제 요약
	  원자 N개(최대 1,000)가 (x, y) (-1,000 ~ 1,000)에서 상(0) 하(1) 좌(2) 우(3) 중 한 방향으로
	  1초에 1씩 움직인다. 원자끼리 만나면(칸 위에서든, 지나치는 도중 0.5초 지점에서든) 모두 소멸하며
	  각자의 에너지를 방출한다. 방출된 에너지 총합을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 좌표를 2배로 늘려 "0.5초 충돌"도 칸 위 충돌로 만든다
	  마주 보고 오는 두 원자가 한 칸 차이면 0.5초에 칸 사이에서 만난다.
	  좌표를 2배로 하면 그 지점도 정수 칸이 된다. 대신 0.5초 단위(한 칸씩)로 움직인다.
	  (x, y) -> (2y + 2000, 2x + 2000) 으로 0 ~ 4000 범위의 MAP에 놓는다 (음수 좌표를 없애는 OFFSET).

	■ MAP에 "그 칸 에너지 합"을 쌓아 충돌을 찾는다
	  매 단계 살아 있는 원자를 하나씩 보면서
	    1) 내 에너지 != MAP[내 칸] 이면 다른 원자와 겹친 것 -> 그 칸 합을 sum에 더하고 칸을 비운다
	       (같은 칸의 다른 원자들은 MAP이 0이 되어 있어 0을 더하고 사라진다)
	    2) 아니면 한 칸 움직여 MAP[다음 칸]에 내 에너지를 더한다.
	       다음 칸이 비어 있었으면 살아 있는 목록에 남긴다.
	       이미 누가 있었으면 목록에 남기지 않는다 -> 먼저 와 있던 원자가 다음 단계 1)에서 처리한다.
	  살아 있는 원자가 없으면 끝.

	■ 주의할 점
	  - 에너지는 1 이상이라 둘 이상 겹친 칸의 합은 어느 원자의 에너지와도 같을 수 없다.
	  - 충돌 지점은 항상 처음 범위 안(두 배 좌표 0 ~ 4000)에 있다. 그래서 범위를 벗어난 원자는 버려도 된다.
	  - [버그] 범위를 벗어나 버리는 원자(nr, nc가 0 ~ 4000 밖)의 MAP 칸을 비우지 않고 continue 한다.
	           그 칸(테두리)에 에너지가 남아, 나중에 두 원자가 바로 그 칸에서 만나도
	           "이미 누가 있다"로 보고 둘 다 목록에서 빠진다 -> 충돌 에너지를 못 센다.
	           (재현 : (999,0) 우 5, (0,0) 우 1, (1000,1000) 하 1 -> 정답 2, 이 코드 0)
	           남은 값은 전역 MAP이라 다음 케이스에도 남는다.
	           -> continue 전에 MAP[liveAtom[i].r][liveAtom[i].c] = 0; 을 해 주면 된다.
	  - MAP은 int [4100][4100] = 약 67MB다. 전역이라 스택 문제는 없다.
*/
#include <stdio.h>

#define MAX (4000+100)
#define OFFSET (2000)

int T;
int N;
int MAP[MAX][MAX];   // 칸에 있는 원자 에너지 합 (2배 좌표)

struct ATOM
{
	int r;
	int c;
	int dir;
	int energy;
};

ATOM atom[1000 + 100];

// 상(y+1), 하(y-1), 좌(x-1), 우(x+1)  ->  r = y, c = x 로 놓았으므로 상이 r + 1
int dr[] = { 1,-1,0,0 };
int dc[] = { 0,0,-1,1 };

void input()
{
	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		int r, c, dir, energy;
		scanf("%d %d %d %d", &c, &r, &dir, &energy);   // (x, y) = (열, 행)

		// 2배 + OFFSET : 0.5초 충돌도 정수 칸으로, 음수 좌표는 0 이상으로
		r = r * 2 + OFFSET;
		c = c * 2 + OFFSET;

		atom[i].r = r;
		atom[i].c = c;
		atom[i].dir = dir;
		atom[i].energy = energy;

		MAP[r][c] = energy;
	}
}

int simulate()
{
	ATOM liveAtom[1000 + 10] = { 0 };   // 살아 있는 원자
	int lcnt, tcnt, sum;                // lcnt : 이번 단계 수, tcnt : 다음 단계로 남는 수

	lcnt = sum = 0;

	for (int i = 0; i < N; i++) liveAtom[lcnt++] = atom[i];

	while (1)
	{
		tcnt = 0;
		for (int i = 0; i < lcnt; i++)
		{
			// 1) 내 칸에 다른 원자도 있다 -> 충돌. 칸의 합을 더하고 비운다
			if (liveAtom[i].energy != MAP[liveAtom[i].r][liveAtom[i].c])
			{
				sum += MAP[liveAtom[i].r][liveAtom[i].c];
				MAP[liveAtom[i].r][liveAtom[i].c] = 0;

				continue;
			}

			// 2) 혼자다 -> 한 칸 움직인다
			int nr, nc;

			nr = liveAtom[i].r + dr[liveAtom[i].dir];
			nc = liveAtom[i].c + dc[liveAtom[i].dir];

			if (nr < 0 || nc < 0 || nr>4000 || nc>4000)continue;   // 범위 밖 -> 버림 ([버그] 내 칸을 안 비운다)

			MAP[liveAtom[i].r][liveAtom[i].c] = 0;

			liveAtom[i].r = nr;
			liveAtom[i].c = nc;

			// 다음 칸이 비어 있었을 때만 목록에 남긴다 (먼저 온 원자가 충돌을 처리한다)
			if (MAP[nr][nc] == 0) liveAtom[tcnt++] = liveAtom[i];

			MAP[nr][nc] += liveAtom[i].energy;
		}
		lcnt = tcnt;

		if (lcnt == 0) break;
	}
	return sum;
}

int main()
{
	scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("#%d %d\n", tc, simulate());
	}
	return 0;
}