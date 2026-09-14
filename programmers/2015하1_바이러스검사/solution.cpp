/*
	[코드트리] 2015 하반기 1번 - 바이러스 검사
	https://www.codetree.ai/training-field/frequent-problems/problems/virus-detector

	■ 문제 요약
	  식당이 N개 있고, i번 식당에는 검사해야 할 사람이 restaurant[i]명 있다.
	  검사관은 두 종류다.

	    팀장 : 한 명이 leader명을 검사할 수 있다. 모든 식당에 반드시 1명씩 배치해야 한다.
	    팀원 : 한 명이 member명을 검사할 수 있다. 필요한 만큼 배치할 수 있다.

	  모든 사람을 검사하는 데 필요한 검사관 수의 최솟값을 구한다.

	■ 풀이 방침
	  식당끼리는 서로 영향을 주지 않으므로 한 식당씩 따로 계산해서 더하면 된다.
	  각 식당의 계산은 두 단계다.

	    1) 팀장 1명은 무조건 배치된다 -> 검사관 +1, 남은 인원 -= leader
	    2) 그래도 사람이 남으면 팀원으로 채운다 -> ceil(남은 인원 / member)명

	  탐색이나 그리디 판단이 필요 없는, 식 하나로 끝나는 문제다.

	■ 핵심 : 정수 올림 나눗셈
	  팀원 수는 ceil(x / member)인데, 실수 나눗셈을 쓰면 오차가 생길 수 있다.
	  정수 연산만으로 올림을 하려면 아래 식을 쓴다.

	      ceil(x / m) == (x - 1) / m + 1        (단, x > 0)

	  x가 m의 배수일 때도 정확히 맞는다.
	  예) x = 10, m = 5 -> (9 / 5) + 1 = 1 + 1 = 2
	      x = 11, m = 5 -> (10 / 5) + 1 = 2 + 1 = 3

	  x > 0 조건이 필요하므로 if (restaurant[i] > 0) 검사를 먼저 한다.
	  (팀장 혼자 다 검사한 식당은 남은 인원이 0 이하가 되어 여기서 걸러진다)

	■ 주의할 점
	  식당 수와 각 식당의 인원이 모두 클 수 있어 합계가 int 범위를 넘길 수 있다.
	  그래서 sum은 long long으로 잡고 출력도 %lld를 쓴다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

#define MAX (1000000 + 50000)

typedef long long ll;

int T;

int N;                  // 식당 개수
int restaurant[MAX];    // 각 식당의 검사 대상 인원 수
int leader, member;     // 팀장 1명 / 팀원 1명이 검사할 수 있는 인원

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다.
//        전역 leader / member 와 이름이 겹치지 않게 인자는 뒤에 _ 를 붙였다.
void input(const std::vector<int>& restaurants, int leader_, int member_)
{
	N = (int)restaurants.size();   // [수정] scanf 대체

	// 각 식당의 인원 수
	for (int i = 0; i < N; i++)
		restaurant[i] = restaurants[i];   // [수정] scanf 대체

	// 팀장, 팀원의 검사 가능 인원
	leader = leader_;   // [수정] scanf 대체
	member = member_;   // [수정] scanf 대체
}

// [수정] main() -> solution().  답 계산이 main 안에 있던 유형이라 본문을 그대로 옮겨 왔다.
long long solution(std::vector<int> restaurants, int leader_, int member_)
{
	input(restaurants, leader_, member_);

	ll sum = 0;   // 필요한 총 검사관 수 (합이 커질 수 있어 long long)

	// 1단계 : 모든 식당에 팀장 1명씩
	for (int i = 0; i < N; i++)
		restaurant[i] -= leader;

	sum += N;

	// 2단계 : 팀장이 다 못 본 인원을 팀원으로 채우기
	for (int i = 0; i < N; i++)
	{
		if (restaurant[i] > 0)
		{
			// 필요한 팀원 수 = ceil(남은 인원 / member), 정수 올림 나눗셈
			sum += ((restaurant[i] - 1) / member) + 1;
		}
	}

	return sum;   // [수정] printf("%lld") -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n;
	scanf("%d", &n);                 // 원본 scanf 순서 그대로
	std::vector<int> restaurants(n);
	for (int i = 0; i < n; i++) scanf("%d", &restaurants[i]);
	int a, b;
	scanf("%d %d", &a, &b);

	long long ans = solution(restaurants, a, b);
#ifdef REPEAT_TEST
	long long ans2 = solution(restaurants, a, b);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%lld\n", ans);
	return 0;
}
#endif
