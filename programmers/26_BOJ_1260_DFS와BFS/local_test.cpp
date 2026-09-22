/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/26_BOJ_1260_DFS와BFS.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, v;
	scanf("%d %d %d", &n, &m, &v);

	vector<vector<int>> edges(m, vector<int>(2));
	for (int i = 0; i < m; i++) scanf("%d %d", &edges[i][0], &edges[i][1]);

	vector<vector<int>> ans = solution(n, v, edges);

#ifdef REPEAT_TEST
	if (solution(n, v, edges) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++)
	{
		for (size_t k = 0; k < ans[i].size(); k++) printf("%d ", ans[i][k]);
		putchar('\n');
	}

	return 0;
}
