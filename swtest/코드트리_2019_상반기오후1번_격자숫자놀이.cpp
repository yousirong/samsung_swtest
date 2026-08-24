// 코드트리 2019 상반기 오후 2번 - 바이러스 백신
// https://www.codetree.ai/training-field/frequent-problems/problems/vaccine-for-virus
// 문제 요약
// N x N 격자에
// - 바이러스가 있는 칸
// - 벽
// - 병원 후보 위치
// 가 주어진다.
//
// 이 중 병원 후보들 중 정확히 M개를 활성화해서
// 바이러스를 모두 퍼뜨리는 최소 시간을 구하는 문제이다.
//
// 칸 의미 (입력 기준)
// 0 : 바이러스가 퍼져야 하는 빈 칸
// 1 : 벽
// 2 : 병원 후보
//
// 목표
// 병원 후보들 중 M개를 선택하고,
// 그 위치에서 동시에 BFS를 시작했을 때
// 모든 빈 칸(0)을 채우는 데 걸리는 최소 시간을 구한다.
// 불가능하면 -1 출력.
//
// 핵심 아이디어
// 1. 병원 후보들 중 M개를 고르는 모든 조합을 DFS로 생성한다.
// 2. 선택된 병원들을 시작점으로 BFS를 돌린다.
// 3. 모든 빈 칸이 퍼졌는지 확인하고, 걸린 시간을 계산한다.
// 4. 그 중 최소 시간을 정답으로 갱신한다.
//
// 즉,
// "병원 M개 고르는 조합 DFS" + "멀티 소스 BFS"
// 구조의 완전탐색 문제이다.

#include <stdio.h>

#define MAX_N (50 + 5)
#define MAX_M (10 + 3)

#define INF (0x7fff0000)

// 내부 처리용 값
#define VIRUS (0)     // 아직 퍼지지 않은 빈 칸
#define WALL  (1)     // 입력에서는 1이지만, 전처리 후 -1로 바꿔 쓴다
#define HOSPITAL (2)  // 입력에서는 2이지만, 전처리 후 -2로 바꿔 쓴다

int T;

int N, M;
int MAP[MAX_N][MAX_N];      // 원본 맵
int tmpMAP[MAX_N][MAX_N];   // BFS 시뮬레이션용 복사 맵

int num_of_cases[MAX_M];    // DFS로 선택한 병원 후보 인덱스

int minAnswer;              // 정답 최소 시간

// 좌표 구조체
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX_N * MAX_N];    // BFS용 큐

RC hospital[MAX_N];         // 병원 후보 좌표 목록
int hcnt;                   // 병원 후보 수

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 전처리
	// - 벽(1)은 -1로 바꿔서 "절대 못 가는 칸"으로 사용
	// - 병원 후보(2)는 좌표를 따로 저장하고, 맵에서는 -2로 바꾼다
	//   이렇게 하면 BFS에서 일반 빈 칸(0)과 구분하기 쉽다.
	hcnt = 0;
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == WALL)
			{
				MAP[r][c] = -1;
			}
			else if (MAP[r][c] == HOSPITAL)
			{
				hospital[hcnt].r = r;
				hospital[hcnt++].c = c;

				MAP[r][c] = -2;
			}
		}
	}
}

// ---------------------------
// 디버그용 tmpMAP 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", tmpMAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 디버그용: 현재 선택한 병원 조합 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// 배열 복사 함수
// tmpMAP <- MAP
// ---------------------------
void copyMAP(int copy[MAX_N][MAX_N], int original[MAX_N][MAX_N])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			copy[r][c] = original[r][c];
		}
	}
}

// ---------------------------
// 아직 퍼지지 않은 빈 칸(0)이 남아 있는지 확인
//
// true  -> 아직 남아 있음
// false -> 모두 퍼졌음
// ---------------------------
bool checkVirus()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (tmpMAP[r][c] == VIRUS)
				return true;
		}
	}

	return false;
}

// ---------------------------
// 선택된 병원들(num_of_cases[])을 시작점으로
// 멀티 소스 BFS 수행
//
// 처리 방식
// 1. tmpMAP를 원본 MAP으로 초기화
// 2. 선택된 M개의 병원 위치를 시작점(값 1)로 둔다
// 3. 큐에 전부 넣고 동시에 BFS 시작
// 4. 빈 칸(0)은 무조건 퍼뜨린다
// 5. 비활성 병원 칸(-2)은
//    아직 퍼져야 할 바이러스 칸이 남아 있을 때만 지나간다
// ---------------------------
void BFS()
{
	// 시뮬레이션용 맵 초기화
	copyMAP(tmpMAP, MAP);

	// 선택된 병원들을 BFS 시작점으로 설정
	for (int i = 0; i < M; i++)
	{
		int hr, hc;
		int index = num_of_cases[i];

		hr = hospital[index].r;
		hc = hospital[index].c;

		// 시작 시간 1로 표시
		tmpMAP[hr][hc] = 1;
	}

	int rp, wp;
	rp = wp = 0;

	// 시작점(활성 병원)들을 큐에 모두 넣는다.
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (tmpMAP[r][c] == 1)
			{
				queue[wp].r = r;
				queue[wp++].c = c;
			}
		}
	}

	// BFS 시작
	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 범위 밖이면 무시
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// ---------------------------
			// 일반 빈 칸(0)으로 퍼지는 경우
			// ---------------------------
			if (tmpMAP[nr][nc] == 0)
			{
				queue[wp].r = nr;
				queue[wp++].c = nc;

				tmpMAP[nr][nc] = tmpMAP[out.r][out.c] + 1;
			}
			// ---------------------------
			// 비활성 병원 칸(-2)으로 이동하는 경우
			//
			// 원본 코드 의도:
			// 아직 퍼져야 할 빈 칸이 남아 있다면
			// 병원 후보 칸도 통과 가능하게 처리
			// ---------------------------
			else if (tmpMAP[nr][nc] == -2)
			{
				if (checkVirus() == true)
				{
					queue[wp].r = nr;
					queue[wp++].c = nc;

					tmpMAP[nr][nc] = tmpMAP[out.r][out.c] + 1;
				}
			}
		}
	}
}

// ---------------------------
// BFS 결과에서 걸린 시간을 계산
//
// 규칙:
// - 아직 0이 남아 있으면 모든 바이러스를 못 퍼뜨린 것
//   -> INF 반환
// - 아니면 tmpMAP의 최댓값 - 1 이 실제 걸린 시간
//
// 왜 -1?
// 시작 병원들을 1로 두고 시작했기 때문
// ---------------------------
int getAnswer()
{
	int max = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 아직 빈 칸이 남아 있으면 실패
			if (tmpMAP[r][c] == 0)
				return INF;

			// 가장 늦게 퍼진 시간 찾기
			if (max < tmpMAP[r][c])
				max = tmpMAP[r][c];
		}
	}

	return max - 1;
}

// ---------------------------
// DFS로 병원 후보 중 M개를 고르는 조합 생성
//
// depth : 현재 몇 개 골랐는지
// start : 다음 후보를 탐색할 시작 인덱스
//
// 조합이므로 DFS(depth+1, i+1) 사용
// ---------------------------
void DFS(int depth, int start)
{
	// 병원 M개를 다 골랐으면 BFS 시뮬레이션
	if (depth == M)
	{
		// printCases();

		BFS();

		int tmp = getAnswer();
		if (tmp < minAnswer)
			minAnswer = tmp;

		return;
	}

	for (int i = start; i < hcnt; i++)
	{
		num_of_cases[depth] = i;

		DFS(depth + 1, i + 1);
	}
}

int main()
{
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		// 병원 후보 중 M개를 고르는 모든 조합 탐색
		DFS(0, 0);

		if (minAnswer == INF)
			printf("-1\n");
		else
			printf("%d\n", minAnswer);
	}

	return 0;
}