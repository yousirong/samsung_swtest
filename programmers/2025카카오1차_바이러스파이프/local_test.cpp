/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기1차_바이러스파이프.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, infection;
	scanf("%d %d", &n, &infection);   // 원본 scanf 순서 그대로

	vector<vector<int>> edges(n - 1, vector<int>(3));
	for (int i = 0; i < n - 1; i++)
		scanf("%d %d %d", &edges[i][0], &edges[i][1], &edges[i][2]);

	int k;
	scanf("%d", &k);

	int ans = solution(n, infection, edges, k);

#ifdef REPEAT_TEST
	int ans2 = solution(n, infection, edges, k);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로

	return 0;
}
