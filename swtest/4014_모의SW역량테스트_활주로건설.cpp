/*
	[SWEA] 4014 - [모의 SW 역량테스트] 활주로 건설
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWIeW7FakkUDFAVH

	■ 문제 요약
	  N x N 지형(6 <= N <= 20)의 각 칸에 높이(1 ~ 6)가 있다. 가로줄 N개, 세로줄 N개 각각에
	  활주로를 놓을 수 있는지 센다.
	    - 한 줄의 높이가 모두 같으면 가능
	    - 높이 차가 1인 곳에는 길이 L(2 ~ 4), 높이 1인 경사로를 낮은 쪽에 놓을 수 있다
	      경사로 밑은 L칸 모두 같은 높이여야 하고, 경사로끼리 겹치면 안 되며, 지도 밖으로 나가면 안 된다
	    - 높이 차가 2 이상이면 불가능
	  가능한 줄의 개수를 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 한 줄을 1차원 배열로 보고 check(), 세로줄은 전치 행렬(TMAP)로 같은 함수 재사용
	  입력받을 때 TMAP[c][r] = MAP[r][c] 로 뒤집어 두면 세로줄도 TMAP의 한 행이다.
	  main()에서 MAP[i], TMAP[i]를 check()에 넘긴다.

	■ check() : "내려가는 곳"만 처리하는 함수를 정방향 / 역방향 두 번 쓴다
	  1) 정방향 : arr[i] > arr[i+1] (한 칸 내려감)이면 i+1 ~ i+L에 경사로.
	              그 L칸이 평평해야 하고, 놓은 자리를 visit[]에 표시한다.
	  2) 역방향 : 배열을 뒤집은 inverse[]에서 같은 일을 한다.
	              뒤집으면 "올라가는 곳"이 "내려가는 곳"이 되므로 같은 코드로 오르막을 처리한다.
	              이때 경사로 자리가 정방향에서 놓은 자리(visit_inverse)와 겹치면 실패.
	  어느 쪽이든 높이 차가 2 이상이면 바로 실패.

	■ 주의할 점
	  - 같은 방향의 경사로끼리는 겹칠 수 없다. 두 번째 내리막이 앞 경사로 안쪽에서 시작하면
	    그 구간이 평평하지 않아 isFlat에서 이미 걸린다. 그래서 겹침 검사는 정방향 vs 역방향만 한다.
	  - [버그] 경사로가 지도 밖으로 나가는지 검사가 i + L == N 하나뿐이다.
	           i + L > N 인 경우(예: i = N - 2, L = 4)를 놓쳐서 isFlat이 arr[N] 이후를 읽는다.
	           그 칸은 이번 케이스의 지도 밖인데, MAP / TMAP이 전역이라 앞 케이스의 값이 남아 있을 수 있다.
	           남은 값이 우연히 같은 높이면 경사로를 놓을 수 있다고 잘못 센다.
	           (재현 : 앞 케이스 N = 9 전부 1, 다음 케이스 N = 6, L = 4, 첫 줄 2 2 2 2 2 1
	                   -> 혼자 돌리면 0, 앞 케이스 뒤에 돌리면 1)
	           -> if (i + L >= N) return 0;   (경사로 끝 i + L 이 N - 1 을 넘으면 안 된다)
	           처음 케이스는 지도 밖이 0이고 높이는 1 이상이라 우연히 맞는다.
	  - abs(a, b)는 두 수의 차이를 돌려주는 직접 만든 함수다 (표준 abs(x)와 인자 수가 달라 겹치지 않는다).
*/
#include <stdio.h>

#define MAX (100+20)

int T;
int N, L;              // 지도 크기, 경사로 길이
int MAP[MAX][MAX];     // 가로줄 = MAP의 행
int TMAP[MAX][MAX];    // 세로줄 = TMAP의 행 (전치 행렬)

void input()
{
	scanf("%d %d", &N, &L);

	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < N; c++)
		{
			scanf("%d", &MAP[r][c]);
			TMAP[c][r] = MAP[r][c];   // 뒤집어 두면 세로줄도 한 행으로 볼 수 있다
		}
	}
}

// 두 수의 차이
int abs(int a, int b)
{
	return (a > b) ? a - b : b - a;
}

// arr[start] ~ arr[end]가 모두 같은 높이인가
int isFlat(int* arr, int start, int end)
{
	int value = arr[start];
	for (int i = start + 1; i <= end; i++)
		if (value != arr[i]) return 0;

	return 1;
}

// 한 줄(arr[0] ~ arr[N-1])에 활주로를 놓을 수 있으면 1
int check(int* arr)
{
	int inverse[MAX] = { 0 };         // arr를 뒤집은 것
	int visit[MAX] = { 0 };           // 정방향에서 경사로를 놓은 자리
	int visit_inverse[MAX] = { 0 };   // visit를 뒤집은 것 (역방향 좌표로 맞춤)

	for (int i = 0; i < N; i++) inverse[i] = arr[N - i - 1];

	// 1) 정방향 : 내리막에 경사로
	for (int i = 0; i < N - 1; i++)
	{
		if (arr[i] == arr[i + 1]) continue;
		if (abs(arr[i], arr[i + 1]) > 1) return 0;   // 높이 차 2 이상

		if (arr[i] > arr[i + 1])
		{
			if (i + L == N) return 0;                  // 경사로가 지도 밖 ([버그] i + L > N 도 막아야 한다)
			if (isFlat(arr, i + 1, i + L) == 0) return 0;

			for (int k = i + 1; k <= i + L; k++) visit[k] = 1;
		}
	}

	for (int i = 0; i < N; i++) visit_inverse[i] = visit[N - i - 1];

	// 2) 역방향 : 뒤집은 배열의 내리막 = 원래 배열의 오르막
	for (int i = 0; i < N - 1; i++)
	{
		if (inverse[i] == inverse[i + 1]) continue;
		if (abs(inverse[i], inverse[i + 1]) > 1) return 0;
		if (inverse[i] > inverse[i + 1])
		{
			if (i + L == N) return 0;
			if (isFlat(inverse, i + 1, i + L) == 0) return 0;
			for (int k = i + 1; k <= i + L; k++)
				if (visit_inverse[k]) return 0;        // 정방향 경사로와 겹침
		}
	}
	return 1;
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		int sum;

		input();

		sum = 0;
		for (int i = 0; i < N; i++)
		{
			sum += check(MAP[i]);    // i번 가로줄
			sum += check(TMAP[i]);   // i번 세로줄

		}

		printf("#%d %d\n", tc, sum);
	}
	return 0;
}