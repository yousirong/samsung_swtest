/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/41_BOJ_2194_유닛이동시키기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, a, b, k;
	scanf("%d %d %d %d %d", &n, &m, &a, &b, &k);

	vector<vector<int>> obstacles(k, vector<int>(2));
	for (int i = 0; i < k; i++) scanf("%d %d", &obstacles[i][0], &obstacles[i][1]);

	int s1, s2, e1, e2;
	scanf("%d %d %d %d", &s1, &s2, &e1, &e2);

	int ans = solution(n, m, a, b, obstacles, s1, s2, e1, e2);

#ifdef REPEAT_TEST
	int ans2 = solution(n, m, a, b, obstacles, s1, s2, e1, e2);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);

	return 0;
}
