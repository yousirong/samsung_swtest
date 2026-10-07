/*
	[SWEA] 2112 - [모의 SW 역량테스트] 보호 필름
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5V1SYKAaUDFAWu

	■ 문제 요약
	  두께 D, 가로 W인 보호 필름이 있다. 셀마다 특성 A(0) 또는 B(1)를 가진다.
	  (3 <= D <= 13, 1 <= W <= 20, 1 <= K <= D)
	  모든 세로 열에서 같은 특성이 K개 이상 연속으로 있어야 성능검사를 통과한다.
	  약품을 투입하면 막(가로 한 줄) 전체가 A 또는 B로 바뀐다.
	  통과하기 위한 약품 투입 횟수의 최솟값을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 막마다 "그대로 / A 투입 / B 투입" 세 갈래 DFS
	  막은 최대 13개, 막마다 선택지 3개 -> 3^13 = 약 160만 가지. 가지치기와 함께 전부 해 본다.
	  끝까지 정했으면(L > D) 모든 열을 검사하고, 통과하면 투입 횟수로 최솟값을 갱신한다.

	■ 구현 포인트 : 막을 실제로 바꾸지 않고 "어느 판을 볼지"만 고른다
	  MAP[0] = 입력 원본, MAP[1] = 전부 A(0), MAP[2] = 전부 B(1) 세 장을 준비해 두고,
	  list[r] = 0 / 1 / 2 로 r번 막을 어느 판에서 읽을지 정한다.
	  그러면 투입할 때 줄을 복사하고 되돌리는 작업이 필요 없다.
	      MAP[1]은 전역이라 처음부터 0(A)이고, MAP[2]는 main()에서 1(B)로 채운다.

	■ 가지치기
	  투입 횟수 cnt는 줄어들지 않는다. 이미 찾은 답(MINANS) 이상이면 더 볼 필요가 없다.

	■ 주의할 점
	  - check(col)은 위에서부터 연속 길이를 세다가 K에 닿는 순간 통과다.
	    K == 1이면 어떤 열도 바로 통과한다 (답 0).
	  - MINANS를 전역 선언에서는 0x7fff0000, main()에서는 0x7fff000(0이 하나 적음)으로 둔다.
	    둘 다 답(최대 K)보다 훨씬 커서 결과에는 영향이 없다.
	  - 답은 K를 넘지 않는다 (아무 K개 막에 같은 약품을 넣으면 반드시 통과한다).
	    cnt >= K 가지치기를 더하면 더 빨라지지만, 지금 코드도 MINANS 가지치기로 충분하다.
*/
#include <stdio.h>

#define MAX (25+5)
int MINANS = 0x7fff0000;   // 최소 투입 횟수

int T;
int D, W, K;               // 두께(막 수), 가로 크기, 합격 기준
int MAP[3][MAX][MAX];      // [0] 원본, [1] 전부 A(0), [2] 전부 B(1)

// 1 = A, 2 = B
int list[MAX];             // list[r] : r번 막을 MAP의 몇 번 판에서 읽을지 (0 = 투입 안 함)

void input()
{
	scanf("%d %d %d", &D, &W, &K);

	for (int r = 1; r <= D; r++)
		for (int c = 1; c <= W; c++)
			scanf("%d", &MAP[0][r][c]);
}

// col 열에 같은 특성이 K개 이상 연속인가
int check(int col)
{
	int cnt, tmp;   // cnt : 지금 연속 길이, tmp : 지금 이어지는 특성

	cnt = 1;
	tmp = MAP[list[1]][1][col];
	for (int r = 2; r <= D; r++)
	{
		if (cnt == K) return 1;
		if (tmp == MAP[list[r]][r][col]) cnt++;
		else
		{
			tmp = MAP[list[r]][r][col];
			cnt = 1;
		}
	}
	if (cnt == K) return 1;   // 마지막 막까지 와서 K에 닿은 경우
	return 0;
}

// 모든 열이 통과하는가
int allCheck()
{
	for (int c = 1; c <= W; c++)
		if (check(c) == 0) return 0;
	return 1;
}

// L : 이번에 정할 막 번호, cnt : 지금까지 약품 투입 횟수
void DFS(int L, int cnt)
{
	if (MINANS <= cnt) return;   // 가지치기

	// 모든 막을 정했다 -> 검사
	if (L > D)
	{
		if (allCheck())
		{
			if (cnt < MINANS) MINANS = cnt;
		}
		return;
	}

	list[L] = 0;               // 투입 안 함
	DFS(L + 1, cnt);

	list[L] = 1;               // A 투입
	DFS(L + 1, cnt+1);

	list[L] = 2;               // B 투입
	DFS(L + 1, cnt+1);
}

int main()
{
	// MAP[2]를 전부 B(1)로 채운다. MAP[1]은 전역 0 그대로 = 전부 A
	for (int r = 1; r <= MAX - 1; r++)
		for (int c = 1; c <= MAX - 1; c++) MAP[2][r][c] = 1;

	scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		MINANS = 0x7fff000;   // 케이스마다 초기화 (전역 초기값과 자릿수가 다르지만 둘 다 충분히 크다)

		DFS(1, 0);

		printf("#%d %d\n", tc, MINANS);
	}
	return 0;
}
