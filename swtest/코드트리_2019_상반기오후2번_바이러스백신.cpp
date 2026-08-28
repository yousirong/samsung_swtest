/*
	[코드트리] 2019 상반기 오후 2번 - 바이러스 백신
	https://www.codetree.ai/training-field/frequent-problems/problems/vaccine-for-virus

	■ 문제 요약
	  N x N 격자가 주어진다.

	    0 : 빈 칸 (바이러스가 퍼져야 하는 칸)
	    1 : 벽
	    2 : 병원 후보 자리

	  병원 후보 중 정확히 M개를 골라 활성화하면, 그 자리에서 바이러스가 동시에 퍼진다.
	  1초마다 상하좌우로 한 칸씩 번지고 벽은 통과하지 못한다.
	  모든 빈 칸에 바이러스가 퍼지는 최소 시간을 출력하고, 불가능하면 -1을 출력한다.

	  (백준 17142 "연구소 3"과 같은 문제다)

	■ 풀이 방침 : 병원 조합 완전탐색 + 멀티 소스 BFS
	  병원 후보는 최대 10개라 M개를 고르는 조합이 많아야 10C5 = 252가지다.
	  조합마다 BFS를 한 번씩 돌려도 부담이 없다.

	    1) 후보 중 M개를 고른다 (DFS 조합)
	    2) 고른 자리를 전부 큐에 넣고 동시에 퍼뜨린다 (멀티 소스 BFS)
	    3) 남은 0이 없으면 걸린 시간을 구해 최솟값을 갱신한다

	■ 값 인코딩
	  BFS 결과를 같은 격자에 시간으로 덮어써야 하는데, 입력값 0/1/2와 섞이면 곤란하다.
	  그래서 입력을 읽은 뒤 아래처럼 바꿔 둔다.

	     0  : 아직 퍼지지 않은 빈 칸 (그대로)
	    -1  : 벽              (원래 1)
	    -2  : 비활성 병원 후보 (원래 2)

	  이렇게 하면 BFS 중에 나오는 양수는 전부 "퍼진 시각"이라는 뜻이 되어 구분이 깔끔하다.
	  활성 병원은 시작 시각을 1로 둔다. 0을 "아직 안 퍼짐"으로 쓰기 때문에 1부터 세야 하고,
	  그래서 마지막에 최댓값에서 1을 빼야 실제 경과 시간이 된다.

	■ 이 문제의 핵심 함정 : 고르지 않은 병원 칸
	  활성화하지 않은 병원 후보 칸(-2)은 지나갈 수는 있지만,
	  거기까지 퍼지는 데 걸린 시간을 정답에 반영하면 안 된다.
	  마지막 빈 칸이 채워진 순간 이미 상황이 끝나는데,
	  그 뒤에 병원 칸으로 더 번져 나간 시간까지 세면 답이 부풀려지기 때문이다.

	  이 코드는 "아직 채워야 할 빈 칸이 남아 있을 때만" -2 칸으로 넘어가는 방식으로 해결한다.

	      else if (tmpMAP[nr][nc] == -2)
	          if (checkVirus() == true)  -> 그때만 퍼뜨린다

	  BFS는 시각 순서대로 진행되므로, 시각 t의 칸을 펼치는 시점에 0이 남아 있다면
	  그 0은 반드시 t+1 이후에 채워진다. 즉 이때 기록되는 병원 칸의 값 t+1은
	  마지막 빈 칸의 시각보다 결코 커질 수 없어서, 최댓값을 부풀리지 않는다.

	  checkVirus()가 격자를 통째로 훑지만, -2 칸은 최대 10개뿐이라 호출 횟수가 적어 문제없다.

	■ 정답 계산
	  BFS가 끝난 뒤 0이 하나라도 남아 있으면 그 조합으로는 전부 퍼뜨릴 수 없다는 뜻이라 INF.
	  아니면 격자의 최댓값에서 1을 뺀 값이 걸린 시간이다.
*/

#include <stdio.h>
#include <stdbool.h>

#define MAX_N (50 + 5)
#define MAX_M (10 + 3)

#define INF (0x7fff0000)

// 입력에서의 의미
#define VIRUS (0)      // 빈 칸
#define WALL (1)       // 벽
#define HOSPITAL (2)   // 병원 후보

int T;

int N, M;
int MAP[MAX_N][MAX_N];      // 전처리를 마친 원본 (건드리지 않는다)
int tmpMAP[MAX_N][MAX_N];   // 조합마다 새로 복사해 쓰는 작업용 격자

int num_of_cases[MAX_M];    // 고른 병원 후보의 인덱스들

int minAnswer;              // 최소 시간

// 좌표 한 쌍
struct RC
{
	int r;
	int c;
};

typedef struct RC RC;

RC queue[MAX_N * MAX_N];    // BFS용 큐

RC hospital[MAX_N];         // 병원 후보 좌표 목록
int hcnt;                   // 병원 후보 개수

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력 + 값 인코딩 전처리
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

	// 벽은 -1, 병원 후보는 -2로 바꾼다.
	// BFS가 격자에 "퍼진 시각"(양수)을 덮어쓸 것이므로,
	// 원래 값과 시간이 섞이지 않게 음수 쪽으로 밀어 두는 것이다.
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
				// 좌표는 따로 목록으로 모아 조합 탐색에 쓴다
				hospital[hcnt].r = r;
				hospital[hcnt++].c = c;

				MAP[r][c] = -2;
			}
		}
	}
}

// ---------------------------
// 디버그용 작업 격자 출력
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
// 디버그용: 고른 병원 인덱스 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < M; i++)
		printf("%d ", num_of_cases[i]);
	putchar('\n');
}

// ---------------------------
// original -> copy 로 격자 복사
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
// 아직 퍼지지 않은 빈 칸(0)이 남아 있는가
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
// 고른 병원들을 시작점으로 하는 멀티 소스 BFS
// ---------------------------
void BFS()
{
	// 매번 원본에서 새로 시작
	copyMAP(tmpMAP, MAP);

	// 고른 후보를 활성 병원으로 바꾼다. 시작 시각은 1.
	for (int i = 0; i < M; i++)
	{
		int hr, hc;
		int index = num_of_cases[i];

		hr = hospital[index].r;
		hc = hospital[index].c;

		tmpMAP[hr][hc] = 1;
	}

	int rp, wp;
	rp = wp = 0;

	// 값이 1인 칸(= 활성 병원)을 전부 큐에 넣어 동시에 출발시킨다
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

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr, nc;

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// 격자 밖
			if (nr < 1 || nc < 1 || nr > N || nc > N)
				continue;

			// 아직 안 퍼진 빈 칸이면 그냥 퍼진다.
			// 시각을 써 넣는 것이 곧 방문 표시라 visit 배열이 필요 없다.
			if (tmpMAP[nr][nc] == 0)
			{
				queue[wp].r = nr;
				queue[wp++].c = nc;

				tmpMAP[nr][nc] = tmpMAP[out.r][out.c] + 1;
			}
			// 활성화하지 않은 병원 칸은 "아직 채울 빈 칸이 남아 있을 때만" 지나간다.
			// 그래야 마지막 빈 칸이 채워진 뒤의 확산이 최댓값을 부풀리지 않는다.
			else if (tmpMAP[nr][nc] == -2)
			{
				if (checkVirus() == true)
				{
					queue[wp].r = nr;
					queue[wp++].c = nc;

					tmpMAP[nr][nc] = tmpMAP[out.r][out.c] + 1;
				}
			}
			// 벽(-1)이나 이미 시각이 적힌 칸(양수)은 여기서 자동으로 걸러진다
		}
	}
}

// ---------------------------
// 이번 조합에 걸린 시간
//
//   0이 남아 있으면 실패 -> INF
//   아니면 최댓값 - 1 (시작 시각을 1로 뒀으므로)
// ---------------------------
int getAnswer()
{
	int max = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			// 끝까지 퍼지지 못한 칸이 있다
			if (tmpMAP[r][c] == 0)
				return INF;

			if (max < tmpMAP[r][c])
				max = tmpMAP[r][c];
		}
	}

	return max - 1;
}

// ---------------------------
// DFS : 병원 후보 중 M개를 고르는 조합 생성
//
//   depth : 지금까지 고른 개수
//   start : 이번에 고를 수 있는 최소 인덱스
// ---------------------------
void DFS(int depth, int start)
{
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

		// 조합이므로 다음은 i보다 뒤에서만 고른다
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		minAnswer = INF;

		DFS(0, 0);

		// 어떤 조합으로도 전부 퍼뜨리지 못했다
		if (minAnswer == INF)
			printf("-1\n");
		else
			printf("%d\n", minAnswer);
	}

	return 0;
}
