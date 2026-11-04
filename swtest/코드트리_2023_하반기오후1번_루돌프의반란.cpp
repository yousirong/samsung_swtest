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
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX_N (50 + 5)                   // N 최대 50 + 여유
#define MAX_P (30 + 5)                   // P 최대 30 + 여유
#define INF (0x7fff0000)                 // 아주 큰 값

#define RUDOLPH (99)                     // MAP에서 루돌프 표시

int T;                                   // 테스트 케이스 수

int N, M, P, C, D;                       // 격자 크기, 턴 수, 산타 수, 루돌프 힘, 산타 힘
int MAP[MAX_N][MAX_N];                   // 산타 번호 / 루돌프 지도

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC rudolph;                              // 루돌프 위치

struct SANTA                             // 산타 정보
{
	int r;                               // 행
	int c;                               // 열
	int stun;                            // 남은 기절 턴 수
	bool dead;                           // 탈락 여부
	int score;                           // 점수
};

SANTA santa[MAX_P];                      // 산타 배열 (1번부터)

// 3-7. 상우하좌 우선순위
// ↑, →, ↓, ←
int dr4[] = { -1, 0, 1, 0 };             // 4방향 행 변화량
int dc4[] = { 0, 1, 0, -1 };             // 4방향 열 변화량

// ←, ↖, ↑, ↗, →, ↘, ↓, ↙
int dr8[] = { 0, -1, -1, -1, 0, 1, 1, 1 };   // 8방향 행 변화량
int dc8[] = { -1, -1, 0, 1, 1, 1, 0, -1 };   // 8방향 열 변화량

void input()
{
	scanf("%d %d %d %d %d", &N, &M, &P, &C, &D);   // 기본 정보 입력

	scanf("%d %d", &rudolph.r, &rudolph.c);         // 루돌프 위치 입력

	for (int p = 1; p <= P; p++)         // 산타 P명
	{
		int index, r, c;                 // 산타 번호, 위치

		scanf("%d %d %d", &index, &r, &c);   // 산타 정보 입력

		santa[index].r = r;              // 행 저장
		santa[index].c = c;              // 열 저장
		santa[index].stun = 0;           // 기절 없음
		santa[index].dead = false;       // 살아 있음
	}
}

void printStatus() // for debug
{
	printf("rudolph %d, %d\n", rudolph.r, rudolph.c);   // 루돌프 위치 출력

	for (int p = 1; p <= P; p++)         // 산타 상태 출력
	{
		SANTA s = santa[p];
		printf("%d] (%d, %d) %d / %d / %d\n", p, s.r, s.c, s.stun, s.dead, s.score);
	}
	putchar('\n');

	for (int r = 1; r <= N; r++)         // 지도 출력
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

int getDistance(int r1, int c1, int r2, int c2)   // 두 점 사이 거리 (제곱합)
{
	return (r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2);
	// 제곱합으로 비교한다. 루트를 씌워도 대소 관계는 같으므로 필요 없다.
}

bool check()                             // 모든 산타가 탈락했는가
{
	for (int p = 1; p <= P; p++)
		if (santa[p].dead == false) return false;   // 한 명이라도 살아 있으면 false

	return true;                         // 모두 탈락
}

void scoreUp()                           // 살아 있는 산타 1점씩
{
	for (int p = 1; p <= P; p++)
	{
		if (santa[p].dead == true) continue;   // 탈락한 산타 제외

		santa[p].score++;                // 1점 추가
	}
}

void setMap()                            // 현재 위치로 MAP 다시 찍기
{
	for (int r = 1; r <= N; r++)         // 초기화
		for (int c = 1; c <= N; c++)
			MAP[r][c] = 0;

	for (int p = 1; p <= P; p++)         // 산타별
	{
		SANTA s = santa[p];

		if (s.dead == true) continue;    // 탈락한 산타 제외

		MAP[s.r][s.c] = p;               // 산타 번호 찍기
	}

	MAP[rudolph.r][rudolph.c] = RUDOLPH;
	// 산타 번호(최대 30)와 겹치지 않는 99로 찍는다.
}

int getNearSantaIndex()                  // 루돌프가 노릴 산타 번호
{
	int santaIndex, minDistance;         // 결과 번호, 최소 거리

	santaIndex = 0;                      // 초기화
	minDistance = INF;

	for (int r = N; r >= 1; r--)
	// 큰 좌표부터 훑고 "더 작을 때만" 갱신 -> 거리가 같으면 행 -> 열이 큰 산타가 남는다.
	{
		for (int c = N; c >= 1; c--)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == RUDOLPH) continue;   // 산타가 아니면 제외

			int distance = getDistance(rudolph.r, rudolph.c, r, c);   // 루돌프와의 거리

			if (distance < minDistance)  // 더 가까우면
			{
				minDistance = distance;  // 최소 거리 갱신
				santaIndex = MAP[r][c];  // 산타 번호 갱신
			}
		}
	}

	return santaIndex;                   // 반환
}

int getRudolphDirection(int santaIndex)  // 루돌프가 갈 8방향
{
	int direction = -1;                  // 결과 방향
	int minDistance = INF;               // 최소 거리

	SANTA s = santa[santaIndex];         // 목표 산타

	for (int i = 0; i < 8; i++)          // 8방향
	{
		int nr, nc;                      // 다음 칸

		nr = rudolph.r + dr8[i];
		nc = rudolph.c + dc8[i];

		int distance = getDistance(nr, nc, s.r, s.c);   // 그 칸에서 산타까지 거리

		if (distance < minDistance)      // 더 가까우면
		{
			minDistance = distance;      // 갱신
			direction = i;
		}
	}

	return direction;                    // 반환
}

void interaction(int santaIndex, int direction, bool isRudolph)   // 연쇄 밀림
{
	SANTA startSanta = santa[santaIndex];   // 처음 밀리는 산타

	// 5-2 연쇄적으로 밀려날 산타들
	int candidate[MAX_P] = { 0 };
	int count = 0;                       // 후보 수

	candidate[count++] = santaIndex;
	// 먼저 밀려날 산타들을 줄로 모은다. 빈칸을 만나면 줄이 끝난다.

	int sr, sc;                          // 현재 확인 칸

	sr = startSanta.r;
	sc = startSanta.c;

	while (1)
	{
		int nr, nc;                      // 다음 칸

		if (isRudolph == true)           // 루돌프가 민 경우 8방향 사용
		{
			nr = sr + dr8[direction];
			nc = sc + dc8[direction];
		}
		else                             // 산타가 민 경우 4방향 사용
		{
			nr = sr + dr4[direction];
			nc = sc + dc4[direction];
		}

		if (nr < 1 || nc < 1 || nr > N || nc > N) break;   // 격자 밖이면 끝
		if (MAP[nr][nc] == 0) break;                       // 빈칸이면 끝

		candidate[count++] = MAP[nr][nc];   // 다음 산타 추가

		sr = nr;                         // 한 칸 진행
		sc = nc;
	}

	for (int i = 0; i < count; i++)      // 모은 산타를 한 칸씩 이동
	{
		int index = candidate[i];        // 산타 번호
		SANTA s = santa[index];          // 산타 정보

		int snr, snc;                    // 이동할 칸

		snr = s.r;
		snc = s.c;

		if (isRudolph == true)           // 8방향
		{
			snr = snr + dr8[direction];
			snc = snc + dc8[direction];
		}
		else                             // 4방향
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

		santa[index].r = snr;            // 위치 갱신
		santa[index].c = snc;
	}
}

void moveRudolph()
{
	setMap();                            // 지도 갱신

	// 2-1, 2-2 우선순위가 가장 높은 가까운 산타의 번호
	int nearSantaIndex = getNearSantaIndex();

	// 2-3 우선순위가 높은 산타를 향해 돌진하는 방향
	int rudolfDirection = getRudolphDirection(nearSantaIndex);

	int nr, nc;                          // 루돌프 다음 칸

	nr = rudolph.r + dr8[rudolfDirection];
	nc = rudolph.c + dc8[rudolfDirection];

	rudolph.r = nr;                      // 루돌프 이동
	rudolph.c = nc;

	// 4-2 루돌프가 움직여서 충돌이 일어난 경우
	if (MAP[nr][nc] != 0)
	{
		// 4-2 산타의 이동
		int crashSantaIndex = MAP[nr][nc];          // 부딪힌 산타
		SANTA crashSanta = santa[crashSantaIndex];

		int snr, snc;                    // 밀려날 칸

		snr = crashSanta.r + (dr8[rudolfDirection]) * C;   // C칸 밀림
		snc = crashSanta.c + (dc8[rudolfDirection]) * C;

		// 4-5 게임판 밖인 경우 탈락
		if (snr < 1 || snc < 1 || snr > N || snc > N)
			santa[crashSantaIndex].dead = true;
		else if (MAP[snr][snc] != 0)     // 밀려난 칸에 다른 산타가 있으면
		{
			int interSantaIndex = MAP[snr][snc];

			// 5-2 상호작용
			interaction(interSantaIndex, rudolfDirection, true);
		}

		santa[crashSantaIndex].r = snr;  // 부딪힌 산타 위치 갱신
		santa[crashSantaIndex].c = snc;

		// 6-1 기절
		// 아직 이번 턴에 움직이지 않았으므로 2 (이번 턴 + 다음 턴 쉼)
		santa[crashSantaIndex].stun = 2;

		// 4-2 점수 획득
		santa[crashSantaIndex].score += C;
	}
}

void moveSanta(int santaIndex)           // 산타 한 명 이동
{
	setMap();                            // 지도 갱신

	SANTA s = santa[santaIndex];         // 산타 정보
	int distance = getDistance(rudolph.r, rudolph.c, s.r, s.c);   // 현재 거리
	int direction = -1;                  // 이동 방향
	int minDistance = INF;               // 최소 거리

	// 3-7 상우하좌 우선순위대로 확인
	for (int i = 0; i < 4; i++)
	{
		int nr, nc;                      // 다음 칸

		nr = s.r + dr4[i];
		nc = s.c + dc4[i];

		int nextDistance = getDistance(rudolph.r, rudolph.c, nr, nc);   // 이동 후 거리

		// 3-6 루돌프와 멀어지는 방향
		if (distance < nextDistance) continue;

		// 루돌프 확인
		if (MAP[nr][nc] == RUDOLPH)
		{
			minDistance = 1;             // 확정
			direction = i;

			break;                       // 더 볼 필요 없음
		}

		if (MAP[nr][nc] != 0) continue;  // 다른 산타가 있으면 못 감

		if (nextDistance < minDistance)  // 더 가까워지면
		{
			minDistance = nextDistance;  // 갱신
			direction = i;
		}
	}

	// 3-5 움직일 수 있는 칸이 없는 경우
	if (direction == -1) return;

	santa[santaIndex].r = s.r + dr4[direction];   // 한 칸 이동
	santa[santaIndex].c = s.c + dc4[direction];

	// 4-3 산타의 다음 좌표가 루돌프인 경우
	if (MAP[santa[santaIndex].r][santa[santaIndex].c] == RUDOLPH)
	{
		santa[santaIndex].stun = 1;      // 다음 턴 기절
		santa[santaIndex].score += D;    // D점 획득

		//    ↑, →, ↓, ← (0, 1, 2, 3)
		// => ↓, ←, ↑, → (2, 3, 0, 1)
		int changeDir[4] = { 2, 3, 0, 1 };
		int snr, snc;                    // 밀려날 칸

		direction = changeDir[direction];   // 반대 방향
		snr = santa[santaIndex].r + (dr4[direction]) * D;   // D칸 밀림
		snc = santa[santaIndex].c + (dc4[direction]) * D;

		if (snr < 1 || snc < 1 || snr > N || snc > N)   // 격자 밖이면
		{
			santa[santaIndex].dead = true;   // 탈락

			return;
		}

		santa[santaIndex].r = snr;       // 위치 갱신
		santa[santaIndex].c = snc;

		// 5-2 상호작용
		if (MAP[snr][snc] != 0)          // 밀려난 칸에 누가 있으면
		{
			int interSantaIndex = MAP[snr][snc];

			if (interSantaIndex != santaIndex)   // 자기 자신(원래 자리)이 아니면
				interaction(interSantaIndex, direction, false);   // 연쇄 밀림
		}
	}
}

void moveAllSanta()
{
	// 3-1 산타는 1번부터 순서대로 이동
	for (int p = 1; p <= P; p++)
	{
		SANTA s = santa[p];              // 산타 정보

		// 3-2 탈락한 산타
		if (s.dead == true) continue;

		// 3-3 기절한 산타
		// 6-1, 6-2 기절 처리
		if (s.stun != 0)
		{
			santa[p].stun--;             // 기절 턴 감소

			continue;                    // 이번 턴 쉼
		}

		moveSanta(p);                    // 이동
	}
}

void simulate()
{
	for (int m = 0; m < M; m++)          // M턴
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

	for (int p = 1; p <= P; p++)         // 점수 출력
		printf("%d ", santa[p].score);
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션 + 출력
	}

	return 0;                            // 프로그램 정상 종료
}
