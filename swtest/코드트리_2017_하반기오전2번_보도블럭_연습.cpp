/*
	[코드트리] 2017 하반기 오전 2번 - 보도블럭 (연습 버전)

	■ 요약
	  가로줄은 MAP, 세로줄은 전치 행렬 TMAP으로 같은 checkRow()에 넘긴다.
	  한 줄은 정방향으로 내리막을 처리하고, 뒤집어서 한 번 더 내리막(= 원래 오르막)을 처리한다.
	  자세한 설명은 코드트리_2017_하반기오전2번_보도블럭.cpp 참고.

	■ 주의 (연습 코드의 버그)
	  1) 1단계(정방향) 루프에 "높이 차가 2 이상이면 실패" 검사가 빠져 있다.
	     오르막 2칸 차이는 3단계에서 걸리지만, 그 사이 내리막 처리 결과가 달라질 수 있다.
	  2) 3단계 겹침 검사 for문의 조건이 c <= c + L 로 되어 있어 항상 참이다.
	     k <= c + L 이어야 한다. 지금은 visit_inverse가 1인 칸을 만날 때까지 배열 밖까지 읽는다.
*/
#include <stdio.h>            // scanf, printf 사용

#define MAX (100+20)          // N 최대 100 + 여유

int T;                        // 테스트 케이스 수
int N, L;                     // 지도 크기, 경사로 길이
int MAP[MAX][MAX];            // 원본 지도
int TMAP[MAX][MAX];           // 전치 행렬

void input()
{
	scanf("%d %d", &N, &L);              // 지도 크기, 경사로 길이 입력

	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
		{
			scanf("%d", &MAP[r][c]);     // 높이 입력
			TMAP[c][r] = MAP[r][c];      // 전치 행렬 채우기
		}
	}
}

int abs(int x)
{
	return (x > 0) ? x : -x;             // 절댓값
}

bool isFlat(int arr[MAX], int start, int end)
{
	int value = arr[start];              // 기준 높이
	for (int i = start + 1; i <= end; i++)   // 구간의 나머지 칸
		if (value != arr[i]) return false;   // 다르면 평평하지 않다

	return true;                         // 모두 같은 높이
}


int checkRow(int arr[MAX])
{
	bool visit[MAX] = { 0 };             // 정방향 경사로 표시
	int inverse[MAX] = { 0 };            // 뒤집은 배열
	bool visit_inverse[MAX] = { 0 };     // 뒤집은 경사로 표시

	for (int c = 1; c <= N - 1; c++)     // 1단계 : 정방향
	{
		if (arr[c] == arr[c + 1])continue;   // 같은 높이면 넘어간다
		                                     // ※ 버그: 여기서 높이 차 2 이상 검사가 빠져 있다

		if (arr[c] > arr[c + 1])         // 내리막이면
		{
			if (c + L > N) return 0;     // 경사로가 지도 밖

			if (isFlat(arr, c + 1, c + L) == false) return 0;   // 평평하지 않으면 실패

			for (int k = c + 1; k <= c + L; k++)
				visit[k] = true;         // 경사로 표시
		}
	}

	for (int c = 1; c <= N; c++)         // 2단계 : 뒤집기
	{
		inverse[c] = arr[N + 1 - c];             // 높이 뒤집기
		visit_inverse[c] = visit[N + 1 - c];     // 경사로 표시 뒤집기
	}

	for (int c = 1; c <= N-1; c++)       // 3단계 : 뒤집은 배열의 내리막
	{
		if (inverse[c] == inverse[c + 1]) continue;   // 같은 높이면 넘어간다

		if (abs(inverse[c] - inverse[c + 1]) > 1)	return 0;   // 높이 차 2 이상

		if (inverse[c] > inverse[c + 1]) // 내리막이면
		{
			if (c + L > N) return 0;     // 경사로가 지도 밖

			if (isFlat(inverse, c + 1, c + L) == false) return 0;   // 평평하지 않으면 실패

			for (int k = c + 1; c <= c + L; k++)                    // ※ 버그: 조건은 k <= c + L 이어야 한다
				if (visit_inverse[k] == true) return 0;             // 정방향 경사로와 겹치면 실패
		}
	}
	return 1;                            // 지나갈 수 있는 길
}

int checkAllRow()
{
	int sum = 0;                         // 길 수
	for (int r = 1; r <= N; r++)         // 줄 번호마다
	{
		sum += checkRow(MAP[r]);         // 가로줄
		sum += checkRow(TMAP[r]);        // 세로줄
	}

	return sum;                          // 길 수 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개

	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		printf("%d\n", checkAllRow());   // 결과 출력
	}
	return 0;                            // 프로그램 정상 종료
}
