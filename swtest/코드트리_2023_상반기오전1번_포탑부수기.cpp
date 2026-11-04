/*
	[코드트리] 2023 상반기 오전 1번 - 포탑 부수기
	https://www.codetree.ai/training-field/frequent-problems/problems/destroy-the-turret

	■ 문제 요약
	  N x M 격자의 각 칸에 포탑이 있고 공격력이 적혀 있다. 공격력이 0이면 부서진 포탑이다.
	  K번의 턴 동안 아래를 반복한다.

	    1) 공격자 선정 : 부서지지 않은 포탑 중 가장 약한 하나
	       (공격력이 가장 낮은 것 -> 가장 최근에 공격한 것 -> 행+열이 큰 것 -> 열이 큰 것)
	       공격자는 자기 공격력을 N + M 만큼 올린다.
	    2) 공격 대상 : 자신을 뺀 포탑 중 가장 강한 하나
	       (공격력이 가장 높은 것 -> 공격한 지 오래된 것 -> 행+열이 작은 것 -> 열이 작은 것)
	    3) 공격
	       - 레이저 : 상하좌우로만 가고 부서진 포탑은 지날 수 없다. 격자의 끝은 반대편과 이어져 있다.
	                 최단 경로로 쏘며, 여러 개면 "우 -> 하 -> 좌 -> 상" 우선순위를 따른다.
	                 대상은 공격력만큼, 경로 중간의 포탑은 그 절반(몫)만큼 깎인다.
	       - 포탄 : 레이저 경로가 없으면 포탄을 쏜다. 대상은 공격력만큼,
	               대상의 주변 8칸은 그 절반만큼 깎인다. 공격자는 영향을 받지 않는다.
	    4) 포탑 부서짐 : 공격력이 0 이하가 되면 부서진다.
	    5) 포탑 정비 : 이번 턴에 공격과 무관했던(값이 그대로인) 포탑은 공격력이 1 오른다.

	  K턴이 끝난 뒤 남은 포탑 중 가장 큰 공격력을 출력한다.

	■ 풀이 방침
	  - "가장 약한/가장 강한"의 기준이 네 단계나 되므로 비교를 isWeak() 하나로 몰아넣었다.
	    isWeak(a, b)는 "a가 b보다 공격자에 가까운가"다. 대상 고르기는 그 반대를 쓰면 된다.
	        공격자 : isWeak(tmp, ret) == true  일 때 갱신
	        대상   : isWeak(tmp, ret) == false 일 때 갱신
	    두 기준이 정확히 뒤집힌 관계라 비교 함수를 하나만 써도 된다.
	  - 비교의 시작값(ret)은 (0, 0)을 빌려 쓴다. MAP[0][0]에
	        공격자를 고를 때는 INF (누구보다 세다 = 약하지 않다)
	        대상을 고를 때는 0   (누구보다 약하다)
	    를 넣어 첫 후보가 무조건 이기게 만든다.
	    getStrongestTower가 (0, 0)을 그대로 돌려주면 "남은 포탑이 공격자 하나뿐"이라는 뜻이다.
	  - 격자 끝이 반대편과 이어져 있으므로 좌표는 늘 (((x + d + N) - 1) % N) + 1 로 감싼다.
	  - 레이저 경로는 BFS + before[]로 되짚는다. 방향 배열을 우 하 좌 상 순서로 두면
	    BFS가 그 순서로 퍼져서 문제의 우선순위와 같은 경로가 잡힌다.
	  - 정비 대상은 "턴 시작 때 값(tmpMAP)과 지금 값이 같은가"로 가린다.
	    공격자는 N+M이 더해져 있고 맞은 포탑은 깎여 있으므로 자동으로 걸러진다.

	■ 주의할 점
	  [확인 필요] 정비를 "값이 그대로인가"로 판단한다. 공격력이 1인 포탑이 쏘면 절반 피해가 0이라,
	              맞았는데도 값이 그대로여서 정비 대상이 된다.
	              문제의 "공격에 관여하지 않은 포탑"과 어긋날 수 있다.
	  [참고] 부서진 포탑(0)은 레이저 경로로도 못 쓰고 포탄 피해도 받지 않는다. 정비도 되지 않는다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (10 + 5)                     // N, M 최대 10 + 여유
#define INF (0x7fff0000)                 // 아주 큰 값

#define BROKEN (0)                       // 부서진 포탑

int T;                                   // 테스트 케이스 수
int N, M, K;                             // 행 수, 열 수, 턴 수

int MAP[MAX][MAX];                       // 포탑 공격력
int tmpMAP[MAX][MAX];                    // 턴 시작 때 공격력 (정비 판단용)

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐

int attackTime[MAX][MAX];                // 마지막으로 공격한 턴

// →, ↓, ←, ↑
int dr4[] = { 0, 1, 0, -1 };             // 레이저 방향별 행 변화량
int dc4[] = { 1, 0, -1, 0 };             // 레이저 방향별 열 변화량

// ←, ↖, ↑, ↗, →, ↘, ↓, ↙
int dr8[] = { 0, -1, -1, -1, 0, 1, 1, 1 };   // 8방향 행 변화량
int dc8[] = { -1, -1, 0, 1, 1, 1, 0, -1 };   // 8방향 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 격자 크기, 턴 수 입력

	for (int r = 1; r <= N; r++)         // 모든 칸
		for (int c = 1; c <= M; c++)
			scanf("%d", &MAP[r][c]);     // 공격력 입력

	// 시점 0에서 모두 공격
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			attackTime[r][c] = 0;        // 공격 시점 초기화
}

void printMap() // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= M; c++)     // 모든 열
			printf("%d ", MAP[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}

	putchar('\n');                       // 빈 줄
}

// a가 더 약하면 true
bool isWeak(RC a, RC b)
{
	if (MAP[a.r][a.c] != MAP[b.r][b.c])          // 1순위 : 공격력
		return MAP[a.r][a.c] < MAP[b.r][b.c];    // 낮을수록 약함

	int timeA = attackTime[a.r][a.c];            // a의 마지막 공격 시점
	int timeB = attackTime[b.r][b.c];            // b의 마지막 공격 시점
	if (timeA != timeB) return timeA > timeB;    // 2순위 : 최근에 공격했을수록 약함

	int sumA = a.r + a.c;                        // a의 행+열
	int sumB = b.r + b.c;                        // b의 행+열
	if (sumA != sumB) return sumA > sumB;        // 3순위 : 행+열이 클수록 약함

	return a.c > b.c;                            // 4순위 : 열이 클수록 약함
}

RC getWeakestTower()                     // 공격자 선정
{
	RC ret = { 0 };                      // 시작값 (0, 0)

	MAP[0][0] = INF; // 최악의 값
	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] == BROKEN) continue;   // 부서진 포탑 제외

			RC tmp = { r, c };           // 후보
			if (isWeak(tmp, ret) == true)   // 더 약하면
				ret = tmp;               // 갱신
		}
	}

	return ret;                          // 공격자 반환
}

RC getStrongestTower(RC attacker)        // 공격 대상 선정
{
	RC ret = { 0 };                      // 시작값 (0, 0)

	MAP[0][0] = 0; // 최악의 값
	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= M; c++)
		{
			if (attacker.r == r && attacker.c == c) continue;   // 공격자 제외
			if (MAP[r][c] == BROKEN) continue;                 // 부서진 포탑 제외

			RC tmp = { r, c };           // 후보
			if (isWeak(tmp, ret) == false)   // 더 강하면
				ret = tmp;               // 갱신
		}
	}

	return ret;                          // 대상 반환 ((0,0)이면 대상 없음)
}

bool BFS(RC start, RC end)               // 레이저 공격 (경로가 있으면 공격하고 true)
{
	int rp, wp;                          // 큐 읽기/쓰기 위치
	int visit[MAX][MAX] = { 0 };         // 방문 + 거리
	RC before[MAX][MAX] = { 0 };         // 앞 칸

	rp = wp = 0;                         // 큐 초기화

	int sr = start.r;                    // 공격자 행
	int sc = start.c;                    // 공격자 열
	int er = end.r;                      // 대상 행
	int ec = end.c;                      // 대상 열

	queue[wp].r = sr;                    // 시작 칸 큐에 넣기
	queue[wp++].c = sc;

	visit[sr][sc] = 1;                   // 시작 칸 방문

	before[sr][sc].r = -1;               // 시작 칸의 앞 칸 없음
	before[sr][sc].c = -1;

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		if (out.r == end.r && out.c == end.c)   // 대상에 도달하면
		{
			int power = MAP[sr][sc];     // 공격력

			int tr = out.r;              // 되짚는 현재 행
			int tc = out.c;              // 되짚는 현재 열

			MAP[tr][tc] -= power;        // 대상은 공격력만큼 피해

			while (1)                    // 경로 되짚기
			{
				int br, bc;              // 앞 칸

				br = before[tr][tc].r;
				bc = before[tr][tc].c;

				if (br == sr && bc == sc) break;   // 공격자까지 오면 멈춤

				MAP[br][bc] -= (power / 2);        // 경로 중간은 절반 피해

				tr = br;                 // 한 칸 거슬러 올라감
				tc = bc;
			}

			return true;                 // 레이저 성공
		}

		for (int i = 0; i < 4; i++)      // 우하좌상 순서로
		{
			int nr, nc;                  // 이웃 칸

			nr = (((out.r + dr4[i] + N) - 1) % N) + 1;   // 행 (반대편과 연결)
			nc = (((out.c + dc4[i] + M) - 1) % M) + 1;   // 열 (반대편과 연결)

			if (MAP[nr][nc] == BROKEN || visit[nr][nc] != 0) continue;   // 부서졌거나 방문함

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = visit[out.r][out.c] + 1;   // 거리 기록

			before[nr][nc] = out;        // 앞 칸 기록
		}
	}

	return false;                        // 경로 없음
}

void attack(RC attacker, RC target, int time)
{
	attackTime[attacker.r][attacker.c] = time;   // 공격 시점 기록

	// laser
	if (BFS(attacker, target) == true) return;   // 레이저 성공하면 끝

	// 포탄
	int sr = attacker.r;                 // 공격자 행
	int sc = attacker.c;                 // 공격자 열
	int er = target.r;                   // 대상 행
	int ec = target.c;                   // 대상 열

	int power = MAP[sr][sc];             // 공격력

	MAP[er][ec] -= power;                // 대상 피해

	for (int i = 0; i < 8; i++)          // 주변 8칸
	{
		int nr, nc;                      // 주변 칸

		nr = (((er + dr8[i] + N) - 1) % N) + 1;   // 행 (반대편과 연결)
		nc = (((ec + dc8[i] + M) - 1) % M) + 1;   // 열 (반대편과 연결)

		if (MAP[nr][nc] == BROKEN) continue;      // 부서진 포탑 제외
		if (nr == sr && nc == sc) continue; // 공격자는 영향 x

		MAP[nr][nc] -= (power / 2);      // 절반 피해
	}
}

void setBrokenTower()                    // 0 이하가 된 포탑 부서짐
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (MAP[r][c] < 0) MAP[r][c] = BROKEN;   // 음수면 0으로
}

void maintainTower()                     // 포탑 정비
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] == BROKEN) continue;        // 부서진 포탑 제외
			if (tmpMAP[r][c] != MAP[r][c]) continue;  // 값이 바뀐(관여한) 포탑 제외

			MAP[r][c]++;                 // 공격력 1 증가
		}
	}
}

void simulate()
{
	for (int k = 1; k <= K; k++)         // K턴
	{
		// 0. 현재 상태 저장
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= M; c++)
				tmpMAP[r][c] = MAP[r][c];

		// 1. 공격자 선정
		RC attacker = getWeakestTower();

		// 2. 공격자의 공격
		// 2-1. target 탐색
		RC target = getStrongestTower(attacker);

		// 2-2 target이 없는 경우
		if (target.r == 0 && target.c == 0) break;

		// 2-3 target 공격
		MAP[attacker.r][attacker.c] += (N + M);   // 공격자 강화
		attack(attacker, target, k);              // 공격

		// 3. 포탑 부서짐
		setBrokenTower();

		// 4. 포탑 정비
		maintainTower();
	}
}

int getAnswer()                          // 가장 큰 공격력
{
	int max = 0;                         // 최대값
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (max < MAP[r][c]) max = MAP[r][c];   // 최대 갱신

	return max;                          // 반환
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		printf("%d\n", getAnswer());     // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
