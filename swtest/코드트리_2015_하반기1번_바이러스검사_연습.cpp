/*
	[코드트리] 2015 하반기 1번 - 바이러스 검사 (연습 버전)

	■ 요약
	  식당마다 팀장 1명을 먼저 넣고, 남은 인원은 팀원으로 올림 나눗셈해서 채운다.
	  자세한 설명은 코드트리_2015_하반기1번_바이러스검사.cpp 참고.
*/
#include <stdio.h>                  // scanf, printf 사용

#define MAX (1000000 + 50000)       // 식당 최대 100만 개 + 여유

typedef long long ll;               // long long을 ll로 줄여 쓰기

int T;                              // 테스트 케이스 수

int N;                              // 식당 개수
int restaurant[MAX];                // 각 식당의 검사 대상 인원 수
int leader, member;                 // 팀장 / 팀원 1명이 검사할 수 있는 인원

void input()
{
	scanf("%d", &N);                     // 식당 개수 입력

	for (int i = 0; i < N; i++) scanf("%d", &restaurant[i]);   // 식당별 인원 입력

	scanf("%d %d", &leader, &member);    // 팀장, 팀원 검사 가능 인원 입력
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		ll sum = 0;                      // 총 검사관 수 (long long)

		for (int i = 0; i < N; i++) restaurant[i] -= leader;   // 팀장이 검사한 만큼 뺀다

		sum += N;                        // 식당마다 팀장 1명

		for (int i = 0; i < N; i++)      // 식당마다
		{
			if (restaurant[i] > 0)       // 남은 인원이 있으면
			{
				sum += ((restaurant[i] - 1) / member) + 1;   // 올림 나눗셈으로 팀원 수 더하기
			}

		}
			printf("%lld\n", sum);       // 총 검사관 수 출력
	}

	return 0;                            // 프로그램 정상 종료
}
