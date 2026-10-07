/*
	[프로그래머스] 2025 카카오 하반기 1차 - 리프 노드 수 최대화
	https://school.programmers.co.kr/learn/courses/30/lessons/468372

	[제출용] main()이 없는 함수형이다. 이 파일을 그대로 프로그래머스에 붙여 넣는다.
	(로컬에서 돌리려면 main()을 따로 만들어 solution(dist_limit, split_limit)을 부른다)

	■ 문제 요약
	  루트는 자식이 1개, 나머지 노드는 자식이 0개(리프) 또는 2개 / 3개(분배 노드)인 트리를 만든다.
	    - 분배 노드는 최대 dist_limit개
	    - 같은 깊이의 분배 노드는 자식 수가 모두 같다 (깊이마다 2 또는 3 하나로 정해진다)
	    - 리프의 분배도 = 부모 ~ 루트 경로의 자식 수 곱. 모든 리프의 분배도 <= split_limit
	  만들 수 있는 리프 수의 최댓값을 구한다.

	  제한 : 0 <= dist_limit <= 10^9,  1 <= split_limit <= 10^9

	  입출력 예
	      (3, 6) -> 6,  (0, 10) -> 1,  (3, 100) -> 7,  (5, 16) -> 9

	■ 관찰 1 : 리프 수 = 1 + (2갈래 분배 노드 수) + 2 x (3갈래 분배 노드 수)
	  처음엔 루트의 자식 하나가 리프다 (리프 1개).
	  리프 하나를 f갈래 분배 노드로 바꾸면 리프 1개가 사라지고 자식 f개가 생긴다 -> f - 1개 늘어난다.
	  그래서 2갈래 노드는 +1, 3갈래 노드는 +2. 같은 예산이면 3갈래 노드를 많이 놓을수록 좋다.

	■ 관찰 2 : 층의 모양은 "2층 a개 위, 3층 b개 아래" 로만 보면 된다
	  가장 깊은 리프의 분배도는 2^a x 3^b 다 -> 2^a x 3^b <= split_limit 인 (a, b)만 가능하다.
	  a <= 29, b <= 18 이라 몇백 쌍뿐이다.
	  3층은 아래에 둘수록 위 층들이 노드 수를 불려 줘서 3갈래 노드를 놓을 자리가 많아진다.
	  (작은 입력 11,500개를 완전탐색과 비교해 이 가정이 맞는 것을 확인했다)

	■ 관찰 3 : (a, b)를 정했으면 "첫 3층에 몇 개(t) 놓을까"만 고르면 된다
	  첫 3층에 t개를 놓으려면 위 2층에 부모가 있어야 한다.
	  가장 적게 쓰면 바로 위 층부터 ceil(t / 2), ceil(t / 4), ... , 1 개다.  -> getParentCost(t)
	  3층들은 위에서부터 꽉 채우면 t, 3t, 9t, ... 최대 t x G3 개를 담는다. (G3 = 1 + 3 + ... + 3^(b-1))

	      3갈래 노드 = min(남은 예산, t x G3)                   (+2짜리를 먼저)
	      2갈래 노드 = min(dist_limit - 3갈래 노드, 2^a - 1)     (남는 예산으로 2층을 더 채운다, +1짜리)
	      리프 수   = 1 + 2갈래 노드 + 2 x 3갈래 노드

	■ t는 이분 탐색으로 고른다
	  - 3층 자리(t x G3)가 남은 예산보다 작은 동안은 t를 늘릴수록 이득이다.
	  - 예산이 3층에 다 들어가기 시작하면, t를 더 늘려 봐야 부모 비용만 늘어 손해다.
	  -> "t x G3 >= dist_limit - getParentCost(t)" 인 가장 작은 t를 찾고, t와 t - 1을 둘 다 계산한다.
	     (t는 1 ~ 2^a, 최대 10^9 이라 하나씩 볼 수는 없다)

	■ 2층을 다 채우는 욕심(greedy)은 틀린다
	  dist 12, split 24 (2, 2, 2, 3) :
	      2층을 꽉 채우면 1 + 2 + 4 = 7개, 첫 3층에 남은 5개 -> 리프 1 + 7 + 2 x 5 = 18
	      2층 마지막을 3개만 채우면 6개, 첫 3층에 남은 6개 -> 리프 1 + 6 + 2 x 6 = 19

	■ 주의할 점
	  - 값이 10^9 근처라 곱하면 int를 넘는다. 계산은 전부 long long, 반환할 때만 int.
	    (1LL << A 는 2^A를 long long으로 계산한다. 1 << A 로 쓰면 int라 A >= 31에서 넘친다)
	  - dist_limit == 0 이면 분배 노드를 못 놓는다 -> 답 1.
	  - b == 0 (2층만 쓰는 경우)도 따로 본다. 리프 = 1 + min(dist_limit, 2^a - 1).
	  - #include <string.h>는 C 문자열 함수 헤더이고, 템플릿의 <string>(C++ string)과는 다르다.
	    이 코드는 둘 다 쓰지 않으므로 어느 쪽이든 상관없다.
*/
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

int T;

long long DIST, SPLIT;   // 분배 노드 최대 개수, 분배도 최댓값

int A, B;                // 2층 개수, 3층 개수
long long G3;            // 첫 3층에 1개를 두면 3층 전체에 들어가는 최대 노드 수

long long answer;

void input(int dist_limit, int split_limit)
{
	DIST = dist_limit;
	SPLIT = split_limit;

	answer = 1;   // 분배 노드가 없으면 리프는 루트의 자식 1개
}

// 첫 3층에 t개를 놓기 위해 위의 2층들에 필요한 최소 분배 노드 수
long long getParentCost(long long t)
{
	long long sum = 0;

	for (int i = 0; i < A; i++)
	{
		t = (t + 1) / 2;   // 부모 하나가 자식 2개를 받친다 -> ceil(t / 2)
		sum += t;
	}
	return sum;
}

// 첫 3층에 t개를 놓을 때의 리프 수 (불가능하면 -1)
long long getLeaf(long long t)
{
	long long parent = getParentCost(t);
	if (parent > DIST) return -1;

	long long three = t * G3;                          // 3층에 들어갈 수 있는 자리
	if (three > DIST - parent) three = DIST - parent;  // 예산만큼만

	long long two = DIST - three;                      // 남는 예산은 2층에
	long long cap2 = (1LL << A) - 1;                   // 2층 A개를 꽉 채운 노드 수
	if (two > cap2) two = cap2;

	return 1 + two + 2 * three;
}

void solve()
{
	for (A = 0; (1LL << A) <= SPLIT; A++)
	{
		long long p2 = 1LL << A;

		//3층없이 2층만 쓰는 경우
		long long only2 = 1 + (DIST < p2 - 1 ? DIST : p2 - 1);
		if (answer < only2) answer = only2;

		if (DIST == 0)continue;

		G3 = 1;
		long long p = p2 * 3;   // 분배도 = 2^A x 3^B
		for (B = 1; p <= SPLIT; B++)
		{
			// t x G3 >= DIST - getParentCost(t) 인 가장 작은 t (1 ~ 2^A)
			long long lo = 1, hi = p2;
			while (lo < hi)
			{
				long long mid = (lo + hi) / 2;

				if (mid * G3 >= DIST - getParentCost(mid)) hi = mid;
				else lo = mid + 1;
			}

			for (long long t = lo - 1; t <= lo; t++)
			{
				if (t < 1) continue;

				long long leaf = getLeaf(t);
				if (answer < leaf) answer = leaf;
			}

			p *= 3;
			G3 = G3 * 3 + 1;   // 3층이 하나 늘면 1 + 3 x (이전 G3)
		}
	}
}

int solution(int dist_limit, int split_limit)
{
	input(dist_limit, split_limit);

	solve();

	return (int)answer;
}