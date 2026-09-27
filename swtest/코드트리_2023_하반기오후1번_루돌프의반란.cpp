/*
	[코드트리] 2023 하반기 오후 1번 - 루돌프의 반란
	https://www.codetree.ai/training-field/frequent-problems/problems/rudolph-rebellion

	■ 문제 요약
	  N x N 격자에서 루돌프 1마리와 산타 P명이 M턴 동안 움직인다.
	  루돌프의 돌진력은 C, 산타의 돌진력은 D다.

	  한 턴은 아래 순서다.

	    1) 루돌프가 움직인다.
	       - 탈락하지 않은 산타 중 가장 가까운 산타를 고른다.
	         거리는 (행 차이)^2 + (열 차이)^2 이고, 같으면 행이 큰 산타, 행도 같으면 열이 큰 산타.
	       - 그 산타에게 가장 가까워지는 8방향 중 하나로 한 칸 간다.
	    2) 산타가 1번부터 순서대로 움직인다.
	       - 탈락했으면 건너뛴다. 기절 중이면 이번 턴은 쉬고 기절이 풀린다.
	       - 루돌프와 가까워지는 방향으로만, 상하좌우(상 우 하 좌 순서)로 한 칸 간다.
	         다른 산타가 있는 칸으로는 못 간다. 갈 곳이 없으면 그대로 있는다.
	    3) 충돌
	       - 루돌프가 산타를 받으면 그 산타는 C점을 얻고 루돌프가 온 방향으로 C칸 밀린다.
	       - 산타가 루돌프를 받으면 그 산타는 D점을 얻고 자기가 온 방향의 반대로 D칸 밀린다.
	       - 밀려난 칸에 다른 산타가 있으면 그 산타도 같은 방향으로 한 칸씩 연쇄로 밀린다.
	       - 밀려서 격자 밖으로 나가면 탈락한다.
	       - 충돌한 산타는 기절한다 (루돌프에게 받히면 2턴, 자기가 받으면 1턴 쉬는 셈).
	    4) 턴이 끝나면 살아 있는 산타마다 1점씩 더한다. 모두 탈락하면 즉시 끝난다.

	  M턴 뒤 산타 1번부터 P번까지의 점수를 출력한다.

	■ 풀이 방침
	  - 거리는 제곱합으로만 비교한다. 루트를 씌우지 않아도 대소 관계는 같다.
	  - "누가 어디 있는가"는 움직이기 직전에 setMap으로 새로 찍는다.
	    산타는 자기 번호, 루돌프는 RUDOLPH(99)로 찍어 두고 MAP 하나로 판단한다.
	    (산타 번호는 최대 30이라 99와 겹치지 않는다)
	  - 가장 가까운 산타 고르기는 r, c를 큰 값부터 훑으며 "더 작을 때만" 갱신한다.
	    그러면 거리가 같을 때 자연히 행 -> 열이 큰 산타가 남는다.
	  - 연쇄 밀림(interaction)은 밀릴 산타들을 먼저 줄로 모은 뒤 한꺼번에 옮긴다.
	    빈칸을 만나면 줄이 끝나고, 옮기다 격자 밖으로 나가는 산타는 탈락 처리한다.

	■ 기절을 카운터 하나로 처리하는 방법
	  stun에 남은 턴 수를 넣고, 산타 차례에 0이 아니면 하나 줄이고 건너뛴다.
	      루돌프에게 받힌 산타 : 아직 이번 턴에 움직이지 않았으므로 2를 넣는다 (이번 턴 + 다음 턴 쉼)
	      자기가 루돌프를 받은 산타 : 이미 이번 턴을 썼으므로 1을 넣는다 (다음 턴만 쉼)

	■ 주의할 점
	  - moveSanta에서 루돌프 칸은 "갈 수 있는 칸"이다. 그 칸으로 가면 거리가 0이 되므로
	    발견하자마자 break로 확정한다 (다른 방향이 더 좋을 수 없다).
	  - MAP에는 루돌프가 99로 찍혀 있다. 연쇄 밀림은 항상 루돌프에서 멀어지는 쪽으로만
	    이어지므로 99가 산타 번호로 잘못 섞이지 않는다.
	  [확인 필요] 루돌프가 갈 방향이 여러 개일 때(거리가 같을 때) 코드는 ← ↖ ↑ ↗ → ↘ ↓ ↙ 중
	              앞선 것을 고른다. 문제에 우선순위가 따로 적혀 있는지 확인이 필요하다.
*/
#include <stdio.h>

#define MAX_N (50 + 5)
#define MAX_P (30 + 5)
#define INF (0x7fff0000)

#define RUDOLPH (99)

int T;

int N, M, P, C, D;
int MAP[MAX_N][MAX_N];

struct RC
{
	int r;
	int c;
};

RC rudolph;

struct SANTA
{
	int r;
	int c;
	int stun;
	bool dead;
	int score;
};

SANTA santa[MAX_P];

// 3-7. 상우하좌 우선순위
// ↑, →, ↓, ←
int dr4[] = { -1, 0, 1, 0 };
int dc4[] = { 0, 1, 0, -1 };

// ←, ↖, ↑, ↗, →, ↘, ↓, ↙
int dr8[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
int dc8[] = { -1, -1, 0, 1, 1, 1, 0, -1 };

void input()
{
	scanf("%d %d %d %d %d", &N, &M, &P, &C, &D);

	scanf("%d %d", &rudolph.r, &rudolph.c);

	for (int p = 1; p <= P; p++)
	{
		int index, r, c;

		scanf("%d %d %d", &index, &r, &c);

		santa[index].r = r;
		santa[index].c = c;
		santa[index].stun = 0;
		santa[index].dead = false;
	}
}

void printStatus() // for debug
{
	printf("rudolph %d, %d\n", rudolph.r, rudolph.c);

	for (int p = 1; p <= P; p++)
	{
		SANTA s = santa[p];
		printf("%d] (%d, %d) %d / %d / %d\n", p, s.r, s.c, s.stun, s.dead, s.score);
	}
	putchar('\n');

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

int getDistance(int r1, int c1, int r2, int c2)
{
	return (r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2);
	// 제곱합으로 비교한다. 루트를 씌워도 대소 관계는 같으므로 필요 없다.
}

bool check()
{
	for (int p = 1; p <= P; p++)
		if (santa[p].dead == false) return false;

	return true;
}

void scoreUp()
{
	for (int p = 1; p <= P; p++)
	{
		if (santa[p].dead == true) continue;

		santa[p].score++;
	}
}

void setMap()
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = 0;

	for (int p = 1; p <= P; p++)
	{
		SANTA s = santa[p];

		if (s.dead == true) continue;

		MAP[s.r][s.c] = p;
	}

	MAP[rudolph.r][rudolph.c] = RUDOLPH;
	// 산타 번호(최대 30)와 겹치지 않는 99로 찍는다.
}

int getNearSantaIndex()
{
	int santaIndex, minDistance;

	santaIndex = 0;
	minDistance = INF;

	for (int r = N; r >= 1; r--)
	// 큰 좌표부터 훑고 "더 작을 때만" 갱신 -> 거리가 같으면 행 -> 열이 큰 산타가 남는다.
	{
		for (int c = N; c >= 1; c--)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == RUDOLPH) continue;

			int distance = getDistance(rudolph.r, rudolph.c, r, c);

			if (distance < minDistance)
			{
				minDistance = distance;
				santaIndex = MAP[r][c];
			}
		}
	}

	return santaIndex;
}

int getRudolphDirection(int santaIndex)
{
	int direction = -1;
	int minDistance = INF;

	SANTA s = santa[santaIndex];

	for (int i = 0; i < 8; i++)
	{
		int nr, nc;

		nr = rudolph.r + dr8[i];
		nc = rudolph.c + dc8[i];

		int distance = getDistance(nr, nc, s.r, s.c);

		if (distance < minDistance)
		{
			minDistance = distance;
			direction = i;
		}
	}

	return direction;
}

void interaction(int santaIndex, int direction, bool isRudolph)
{
	SANTA startSanta = santa[santaIndex];

	// 5-2 연쇄적으로 밀려날 산타들
	int candidate[MAX_P] = { 0 };
	int count = 0;

	candidate[count++] = santaIndex;
	// 먼저 밀려날 산타들을 줄로 모은다. 빈칸을 만나면 줄이 끝난다.

	int sr, sc;

	sr = startSanta.r;
	sc = startSanta.c;

	while (1)
	{
		int nr, nc;

		if (isRudolph == true)
		{
			nr = sr + dr8[direction];
			nc = sc + dc8[direction];
		}
		else
		{
			nr = sr + dr4[direction];
			nc = sc + dc4[direction];
		}

		if (nr < 1 || nc < 1 || nr > N || nc > N) break;
		if (MAP[nr][nc] == 0) break;

		candidate[count++] = MAP[nr][nc];

		sr = nr;
		sc = nc;
	}

	for (int i = 0; i < count; i++)
	{
		int index = candidate[i];
		SANTA s = santa[index];

		int snr, snc;

		snr = s.r;
		snc = s.c;

		if (isRudolph == true)
		{
			snr = snr + dr8[direction];
			snc = snc + dc8[direction];
		}
		else
		{
			snr = snr + dr4[direction];
			snc = snc + dc4[direction];
		}

		if (snr < 1 || snc < 1 || snr > N || snc > N)
		{
			santa[index].dead = true;
			// 밀려서 격자 밖으로 나가면 탈락

			continue;
		}

		santa[index].r = snr;
		santa[index].c = snc;
	}
}

void moveRudolph()
{
	setMap();

	// 2-1, 2-2 우선순위가 가장 높은 가까운 산타의 번호
	int nearSantaIndex = getNearSantaIndex();

	// 2-3 우선순위가 높은 산타를 향해 돌진하는 방향
	int rudolfDirection = getRudolphDirection(nearSantaIndex);

	int nr, nc;

	nr = rudolph.r + dr8[rudolfDirection];
	nc = rudolph.c + dc8[rudolfDirection];

	rudolph.r = nr;
	rudolph.c = nc;

	// 4-2 루돌프가 움직여서 충돌이 일어난 경우
	if (MAP[nr][nc] != 0)
	{
		// 4-2 산타의 이동
		int crashSantaIndex = MAP[nr][nc];
		SANTA crashSanta = santa[crashSantaIndex];

		int snr, snc;

		snr = crashSanta.r + (dr8[rudolfDirection]) * C;
		snc = crashSanta.c + (dc8[rudolfDirection]) * C;

		// 4-5 게임판 밖인 경우 탈락
		if (snr < 1 || snc < 1 || snr > N || snc > N)
			santa[crashSantaIndex].dead = true;
		else if (MAP[snr][snc] != 0)
		{
			int interSantaIndex = MAP[snr][snc];

			// 5-2 상호작용
			interaction(interSantaIndex, rudolfDirection, true);
		}

		santa[crashSantaIndex].r = snr;
		santa[crashSantaIndex].c = snc;

		// 6-1 기절
		// 아직 이번 턴에 움직이지 않았으므로 2 (이번 턴 + 다음 턴 쉼)
		santa[crashSantaIndex].stun = 2;

		// 4-2 점수 획득
		santa[crashSantaIndex].score += C;
	}
}

void moveSanta(int santaIndex)
{
	setMap();

	SANTA s = santa[santaIndex];
	int distance = getDistance(rudolph.r, rudolph.c, s.r, s.c);
	int direction = -1;
	int minDistance = INF;

	// 3-7 상우하좌 우선순위대로 확인
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;

		nr = s.r + dr4[i];
		nc = s.c + dc4[i];

		int nextDistance = getDistance(rudolph.r, rudolph.c, nr, nc);

		// 3-6 루돌프와 멀어지는 방향
		if (distance < nextDistance) continue;

		// 루돌프 확인
		if (MAP[nr][nc] == RUDOLPH)
		{
			minDistance = 1;
			direction = i;

			break;
		}

		if (MAP[nr][nc] != 0) continue;

		if (nextDistance < minDistance)
		{
			minDistance = nextDistance;
			direction = i;
		}
	}

	// 3-5 움직일 수 있는 칸이 없는 경우
	if (direction == -1) return;

	santa[santaIndex].r = s.r + dr4[direction];
	santa[santaIndex].c = s.c + dc4[direction];

	// 4-3 산타의 다음 좌표가 루돌프인 경우
	if (MAP[santa[santaIndex].r][santa[santaIndex].c] == RUDOLPH)
	{
		santa[santaIndex].stun = 1;
		santa[santaIndex].score += D;

		//    ↑, →, ↓, ← (0, 1, 2, 3)
		// => ↓, ←, ↑, → (2, 3, 0, 1)
		int changeDir[4] = { 2, 3, 0, 1 };
		int snr, snc;

		direction = changeDir[direction];
		snr = santa[santaIndex].r + (dr4[direction]) * D;
		snc = santa[santaIndex].c + (dc4[direction]) * D;

		if (snr < 1 || snc < 1 || snr > N || snc > N)
		{
			santa[santaIndex].dead = true;

			return;
		}

		santa[santaIndex].r = snr;
		santa[santaIndex].c = snc;

		// 5-2 상호작용
		if (MAP[snr][snc] != 0)
		{
			int interSantaIndex = MAP[snr][snc];

			if (interSantaIndex != santaIndex)
				interaction(interSantaIndex, direction, false);
		}
	}
}

void moveAllSanta()
{
	// 3-1 산타는 1번부터 순서대로 이동
	for (int p = 1; p <= P; p++)
	{
		SANTA s = santa[p];

		// 3-2 탈락한 산타
		if (s.dead == true) continue;

		// 3-3 기절한 산타
		// 6-1, 6-2 기절 처리
		if (s.stun != 0)
		{
			santa[p].stun--;

			continue;
		}

		moveSanta(p);
	}
}

void simulate()
{
	for (int m = 0; m < M; m++)
	{
		// 2. 루돌프의 움직임
		moveRudolph();

		// 3. 산타의 움직임
		moveAllSanta();

		// 7-2 모든 산타가 탈락한 경우, 게임 종료
		if (check() == true) break;

		// 7-3 탈락하지 않은 산타 점수 추가
		scoreUp();
	}

	for (int p = 1; p <= P; p++)
		printf("%d ", santa[p].score);
}

int main()
{
	// scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();
	}

	return 0;
}
