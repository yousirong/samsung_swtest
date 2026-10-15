/*
	[SWEA] 5658 - [모의 SW 역량테스트] 보물상자 비밀번호
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRUN9KfZ8DFAUo

	■ 문제 요약
	  16진수 숫자 N개(N은 4의 배수, 8 ~ 28)가 사각형 보물상자 네 변에 N/4개씩 적혀 있다.
	  뚜껑을 시계 방향으로 한 칸씩 돌릴 수 있고, 변마다 적힌 N/4자리를 16진수 수 하나로 읽는다.
	  돌리면서 만들 수 있는 모든 수 중 K번째로 큰 수를 10진수로 구한다 (같은 수는 한 번만 센다).
	  테스트 케이스가 T개.

	■ 풀이 방침 : 원형 문자열의 모든 시작점에서 N/4자리를 잘라 본다
	  돌리는 것 = 시작점이 한 칸씩 밀리는 것이다. 시작점 i = 0 ~ N-1 에서 N/4자리를 읽으면
	  모든 회전의 모든 변이 한 번씩 나온다.
	  원형을 다루기 쉽게 앞쪽 N/4자리를 문자열 뒤에 한 번 더 붙인다 (number[N + i] = number[i]).
	  그러면 number + i 에서 N/4자리를 그대로 읽으면 된다.

	  1) N개의 수를 makeDecimal로 10진수로 바꾼다
	  2) 내림차순 정렬 (선택 정렬)
	  3) 중복을 뺀 목록(uniqueList)을 만든다. 정렬돼 있으니 바로 앞과 같은지만 보면 된다
	  4) uniqueList[K - 1]이 답

	■ 주의할 점
	  - 16진수 글자는 '0' ~ '9', 'A' ~ 'F' (대문자). 'A' 이상이면 - 'A' + 10.
	  - N/4는 최대 7자리 -> 16^7 = 2억 6천만 정도라 int에 들어간다.
	  - number 버퍼는 N + N/4 + 1(끝 문자) = 최대 36칸이 필요하다. MAX = 38이라 충분하다.
	  - 같은 수는 한 번만 센다. 그래서 정렬 뒤 중복을 빼고 K번째를 고른다.
*/
#include <stdio.h>

#define MAX (28+10)

int T;
int N, K;            // 숫자 개수, 몇 번째로 큰 수
int decimal[MAX];    // 만들 수 있는 수들 (10진수)
int dcnt;


// ptr에서 size자리를 16진수로 읽어 10진수로 바꾼다
int makeDecimal(char* ptr, int size)
{
	int sum, mul;

	sum = 0; mul = 1;

	// 맨 끝자리부터 16^0, 16^1, ...
	for (int i = size - 1; i >= 0; i--)
	{
		if (ptr[i] >= 'A') sum += (ptr[i] - 'A' + 10) * mul;
		else sum += (ptr[i] - '0') * mul;

		mul *= 16;
	}
	return sum;
}
int main()
{
	scanf("%d", &T);
	for (int tc = 1; tc <= T; tc++)
	{
		dcnt = 0;

		char number[MAX];

		scanf("%d %d %s", &N, &K, number);

		// 원형 처리 : 앞쪽 N/4자리를 뒤에 한 번 더 붙인다
		for (int i = 0; i < N / 4; i++) number[N + i] = number[i];
		number[N + N / 4] = 0;

		// 모든 시작점에서 N/4자리 수
		for (int i = 0; i < N; i++) decimal[dcnt++] = makeDecimal(number + i, N / 4);

		// 내림차순 정렬
		for (int i = 0; i < dcnt - 1; i++)
		{
			for (int k = i + 1; k < dcnt; k++)
			{
				if (decimal[i] < decimal[k])
				{
					int tmp = decimal[i];
					decimal[i] = decimal[k];
					decimal[k] = tmp;
				}
			}
		}

		// 중복 제거 (정렬돼 있으므로 바로 앞과 같은지만 본다)
		int uniqueList[MAX] = { 0 };
		int ucnt = 0;

		uniqueList[ucnt++] = decimal[0];

		for (int i = 1; i < dcnt; i++)
		{
			if (decimal[i] == uniqueList[ucnt - 1])continue;
			uniqueList[ucnt++] = decimal[i];
		}
		printf("#%d %d\n", tc, uniqueList[K - 1]);   // K번째로 큰 수
	}
	return 0;
}