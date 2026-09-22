/*
	[BOJ] 2161 - 카드1
	원본 : swtest/20_BOJ_2161_카드1.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.

	https://www.acmicpc.net/problem/2161

	■ 문제 요약
	  1번부터 N번까지의 카드가 위에서부터 순서대로 쌓여 있다.
	  카드가 한 장 남을 때까지 아래를 반복한다.

	    1) 맨 위 카드를 바닥에 버린다
	    2) 그다음 맨 위 카드를 제일 아래로 옮긴다

	  버려지는 카드의 순서를 차례로 출력하고, 마지막에 남은 카드도 이어서 출력한다.

	■ 풀이 방침
	  "맨 위에서 빼고 맨 아래로 넣는다"가 정확히 큐의 pop / push다.
	  그러므로 요령 없이 큐로 그대로 시뮬레이션하면 된다.

	    rp : 맨 위 카드의 위치
	    wp : 제일 아래(다음에 넣을) 위치

	  한 번의 반복이 pop 두 번 + push 한 번이므로,
	  카드가 한 장 줄어든다. 따라서 N-1번 반복하면 한 장만 남는다.

	■ 배열 크기
	  선형 큐라 pop한 앞자리를 재사용하지 않는다.
	  총 쓰기 횟수는 최초 N번 + 아래로 보내는 N-1번 = 2N-1번이므로
	  배열은 2N 이상이면 충분하다. (그래서 MAX * 2로 잡았다)
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

#define MAX (1000 + 100)

int N;
int queue[MAX * 2];   // 선형 큐. 최대 2N-1칸까지 쓰이므로 2배로 잡는다
int rp, wp;           // rp: 맨 위 카드 위치, wp: 다음에 넣을 위치

// [수정] main() -> solution().
// 원본은 버리는 카드를 "%d " 로, 마지막 남은 카드를 "%d\n" 으로 출력했다.
// 여기서는 버린 순서 + 마지막 카드를 한 목록에 담아 반환한다. (길이 N)
std::vector<int> solution(int n)
{
	std::vector<int> answer;

	N = n;   // [수정] scanf("%d", &N) 대체

	rp = wp = 0;   // [추가] 재호출 대비 초기화이기도 하다

	// 초기 상태 : 위에서부터 1, 2, 3, ..., N
	for (int i = 1; i <= N; i++)
		queue[wp++] = i;

	// 카드가 한 장 남을 때까지 = N-1번 반복
	for (int i = 0; i < N - 1; i++)
	{
		// 1) 맨 위 카드를 버린다 (버리는 순서가 곧 정답)
		answer.push_back(queue[rp++]);   // [수정] printf -> 목록에 담기

		// 2) 그다음 카드를 빼서(rp++) 제일 아래에 넣는다(wp++)
		queue[wp++] = queue[rp++];
	}

	// 마지막에 남은 카드.
	// 마지막으로 쓰인 자리가 wp-1이고 그 자리에 남은 한 장이 있다.
	// (N == 1이면 반복이 없으므로 처음 넣은 카드 1이 그대로 들어간다)
	answer.push_back(queue[wp - 1]);

	return answer;
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// 원본 출력 형식(버린 카드는 "%d ", 마지막은 "%d\n")을 그대로 흉내 낸다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n;
	scanf("%d", &n);

	std::vector<int> ans = solution(n);

#ifdef REPEAT_TEST
	if (solution(n) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i + 1 < ans.size(); i++) printf("%d ", ans[i]);
	printf("%d\n", ans[ans.size() - 1]);

	return 0;
}
#endif
