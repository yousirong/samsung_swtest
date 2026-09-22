/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/40_BOJ_2234_성곽.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int m, n;
	scanf("%d %d", &m, &n);

	vector<vector<int>> castle(n, vector<int>(m));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < m; c++)
			scanf("%d", &castle[r][c]);

	vector<int> ans = solution(castle);

#ifdef REPEAT_TEST
	if (solution(castle) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	printf("%d\n%d\n%d\n", ans[0], ans[1], ans[2]);

	return 0;
}
