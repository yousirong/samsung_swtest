/*
	[프로그래머스] 2025 프로그래머스 코드챌린지 2차 예선 - 서버 증설 횟수
	https://school.programmers.co.kr/learn/courses/30/lessons/389479

	■ 문제 요약
	  0 ~ 23시 시간대별 이용자 수 players[24]가 주어진다.
	  이용자가 n x m 명 이상 (n + 1) x m 명 미만이면 증설 서버가 최소 n대 돌아가야 한다.
	  -> 필요한 서버 수 = players[i] / m (정수 나눗셈)
	  한 번 증설한 서버는 k시간 동안 돌고 반납된다 (i시에 늘리면 i ~ i+k-1 시간대에 운영).
	  하루 동안 필요한 최소 증설 횟수를 구한다.

	  제한 : 0 <= players[i] <= 1,000,  1 <= m <= 1,000,  1 <= k <= 24

	  입출력 예
	      예제 1 (m 3, k 5) -> 7,  예제 2 (m 5, k 1) -> 11,  예제 3 (m 1, k 1) -> 12

	■ 풀이 방침 : 0시부터 순서대로 시뮬레이션, "모자랄 때 모자란 만큼만" 증설
	  i시마다
	    1) i시에 반납되는 서버를 뺀다 (i - k 시에 늘린 서버들)
	    2) 필요한 수 need = players[i] / m
	    3) 지금 돌아가는 수 running < need 이면 need - running 대를 증설한다
	       -> 증설 횟수에 더하고, running에 더하고, i + k 시에 반납된다고 적어 둔다

	■ 왜 "모자랄 때 모자란 만큼"이 최소인가
	  미리 늘려 두면 반납도 그만큼 빨리 된다. 같은 서버라면 늦게 늘릴수록 뒤쪽 시간을 더 오래 덮는다.
	  그래서 정말 필요한 순간에, 필요한 만큼만 늘리는 것이 항상 가장 적게 늘리는 방법이다.

	■ 반납 시각 관리 : expire[] 배열
	  i시에 x대를 늘리면 expire[i + k] += x 로 적어 둔다.
	  (i + k)시가 되면 running -= expire[i + k].
	  i + k는 최대 23 + 24 = 47 이라 배열을 넉넉히 잡는다.
	  큐를 쓰지 않고 "시각별 반납 대수" 배열 하나로 끝난다.

	■ 주의할 점
	  - 표 예시에서 k = 5, 2시에 늘린 서버는 2 ~ 6시 칸에서 돌고 7시 칸에서 0이 된다.
	    -> 반납 시각은 i + k (i + k - 1 이 아니다).
	  - 필요한 서버 수는 이용자 / m 을 내림한 값이다. m - 1명까지는 서버가 필요 없다.
	  - expire[]와 running은 케이스마다 비운다.
*/
#include <stdio.h>

#define HOUR (24)
#define MAX_TIME (24 + 24 + 5)   // 반납 시각은 최대 23 + 24

int T;

int players[HOUR];   // 시간대별 이용자 수
int M, K;            // 서버 한 대가 감당하는 이용자 수, 서버 운영 시간

int expire[MAX_TIME];   // expire[t] : t시에 반납되는 서버 수

int answer;

void input()
{
	for (int i = 0; i < HOUR; i++)
		scanf("%d", &players[i]);

	scanf("%d %d", &M, &K);

	for (int t = 0; t < MAX_TIME; t++)
		expire[t] = 0;

	answer = 0;
}

void simulate()
{
	int running = 0;   // 지금 돌아가는 증설 서버 수

	for (int i = 0; i < HOUR; i++)
	{
		// 1) i시에 반납되는 서버를 뺀다
		running -= expire[i];

		// 2) 이 시간대에 필요한 서버 수
		int need = players[i] / M;

		// 3) 모자라면 모자란 만큼만 늘린다
		if (running < need)
		{
			int add = need - running;

			answer += add;
			running += add;
			expire[i + K] += add;   // k시간 뒤에 반납
		}
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();

		printf("%d\n", answer);
	}

	return 0;
}
