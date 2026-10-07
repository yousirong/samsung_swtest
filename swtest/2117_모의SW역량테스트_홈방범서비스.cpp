/*
	[SWEA] 2117 - [모의 SW 역량테스트] 홈 방범 서비스
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5V61LqAf8DFAWu

	■ 문제 요약
	  N x N 도시(5 <= N <= 20)에 집(1)이 있다. 서비스 영역은 크기 K의 마름모다
	  (중심에서 맨해튼 거리 K - 1 이하인 칸). 운영 비용은 K * K + (K - 1) * (K - 1).
	  집 하나가 내는 비용은 M (1 <= M <= 10).
	  손해를 보지 않으면서(집 수 x M - 운영 비용 >= 0) 서비스할 수 있는 집의 최대 개수를 구한다.
	  테스트 케이스가 T개.

	■ 풀이 방침 : 마름모 모양을 미리 만들어 두고, K를 큰 것부터 줄이며 모든 위치에 대 본다
	  1) main() 시작에서 K = 1 ~ 21 의 마름모를 AREA[K]에 0/1로 그려 둔다.
	     AREA[K]는 (2K-1) x (2K-1) 상자이고, 가운데 (K, K)가 중심이다.  비용도 cost[K]에 저장.
	  2) K = N + 1 (도시 전체를 덮는 크기)부터 시작한다.
	     모든 위치에 마름모를 대 보고(scan), 손해가 아닌 곳 중 집이 가장 많은 수를 maxAreaNum에 남긴다.
	  3) 하나라도 찾았으면 멈추고, 못 찾았으면 K를 1 줄여 다시 한다.

	■ 왜 "손해 없는 가장 큰 K"에서 멈춰도 되나
	  K에서 손해 없는 배치 중 최대 집 수를 h라 하자. 더 작은 K'에서 집 h' > h 개를 덮는 배치가 있다면,
	  같은 중심에 K를 대면 h'개 이상을 덮고, h' x M > h x M >= cost[K] 라서 K에서도 손해가 아니다.
	  그러면 h가 최대라는 데 모순이다. 그래서 더 작은 K는 볼 필요가 없다.

	■ 마름모 그리기 (makeArea)
	  위쪽 절반(r = 1 ~ K)      : r번째 줄은 가운데 열 K를 중심으로 좌우 r - 1 칸
	  아래쪽 절반(r = K+1 ~ 2K-1) : 한 줄 내려갈 때마다 좌우로 한 칸씩 줄어든다
	  주석 처리된 두 번째 makeArea는 같은 모양을 "|dr| + |dc| <= K - 1" 로 그린 것이다.

	■ 주의할 점
	  - scan(sr, sc)는 상자의 왼쪽 위를 (sr + 1, sc + 1)에 놓는다. 중심은 (sr + K, sc + K).
	    main()에서 sr = r - K - 1 이므로 중심은 0 ~ N + K 까지 움직인다. 도시 밖 중심도 보지만
	    도시 밖 중심은 안쪽으로 한 칸 옮긴 배치보다 덮는 집이 많을 수 없어 결과에는 영향이 없다.
	  - 도시 밖 칸은 scan에서 범위 검사로 건너뛴다.
	  - K는 input()에서 케이스마다 N + 1로 되돌린다.
	  - [확인 필요] 집이 하나도 없으면 maxAreaNum이 계속 0이라 K가 0, -1 ... 로 내려가며
	    AREA[-1]을 읽는다. 문제에서 집이 1개 이상 주어진다면 K = 1 (비용 1, 집 1개면 M - 1 >= 0)에서 반드시 멈춘다.
	  - [확인 필요] AREA는 [22][MAX * MAX][MAX * MAX] int 라 약 34MB다.
	    실제로 쓰는 범위는 [22][2 * 21][2 * 21] 정도라 [22][2 * MAX][2 * MAX] 면 충분하다.
*/
#include <stdio.h>

#define MAX (20+5)

int T;

int N, M;                              // 도시 크기, 집 하나가 내는 비용
int MAP[MAX][MAX];                     // 1 = 집
int AREA[22][MAX * MAX][MAX * MAX];    // AREA[K] : 크기 K 마름모 모양 (1 = 서비스 영역)

int K;                                 // 지금 보는 서비스 영역 크기
int cost[22];                          // cost[K] : 운영 비용

void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	K = N + 1;   // 도시 전체를 덮는 크기부터 시작
}

// 크기 k 마름모를 AREA[k]의 (2k-1) x (2k-1) 상자에 그린다. 중심은 (k, k)
void makeArea(int k)
{
	cost[k] = k * k + (k - 1) * (k - 1);

	// 위쪽 절반 : r번째 줄은 좌우로 r - 1칸
	for (int r = 1; r <= k; r++)
		for (int c = k + 1 - r; c <= k + r - 1; c++)
			AREA[k][r][c] = 1;

	// 아래쪽 절반 : 한 줄 내려갈 때마다 좌우로 한 칸씩 줄어든다
	for (int r = k + 1; r <= 2 * k - 1; r++)
		for (int c = r - k + 1; c <= 3 * k - r - 1; c++)
			AREA[k][r][c] = 1;
}
/*
void makeArea(int k)
{
	cost[k] = k * k + (k - 1) * (k - 1);

	for (int r = 1; r <= 2 * k - 1; r++)
		for (int c = 1; c <= 2 * k - 1; c++)
		{
			int dr = r - k; if (dr < 0) dr = -dr;
			int dc = c - k; if (dc < 0) dc = -dc;
			if (dr + dc <= k - 1) AREA[k][r][c] = 1;
		}
}
*/


// 상자의 왼쪽 위를 (sr + 1, sc + 1)에 놓았을 때 서비스 영역 안의 집 수
int scan(int sr, int sc)
{
	int sum = 0;
	for (int r = 1; r <= 2 * K - 1; r++)
	{
		for (int c = 1; c <= 2 * K - 1; c++)
		{
			if (sr + r<1 || sc + c<1 || sr + r>N || sc + c>N) continue;   // 도시 밖
			sum += MAP[sr + r][sc + c] * AREA[K][r][c];
		}
	}
	return sum;
}

int main()
{
	for (int i = 1; i <= 21; i++)makeArea(i);   // 마름모는 케이스와 상관없이 한 번만 만든다

	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		int areaNum, maxAreaNum;

		areaNum = maxAreaNum = 0;

		// 손해 없는 배치를 하나라도 찾을 때까지 K를 줄인다
		while (maxAreaNum <= 0)
		{
			for (int r = 1; r <= N + K + 1; r++)
			{
				for (int c = 1; c <= N + K + 1; c++)
				{
					areaNum = scan(r - K - 1, c - K - 1);   // 중심 = (r - 1, c - 1)

					// 손해가 아니면 (집 수 x M >= 운영 비용) 최댓값 후보
					if (areaNum * M - cost[K] >= 0)
					{
						if (maxAreaNum < areaNum) maxAreaNum = areaNum;
					}
				}
			}
			K--;
		}
		printf("#%d %d\n", tc, maxAreaNum);
	}

	return 0;
}