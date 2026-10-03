/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2026_상반기오전1번_아기바다거북의대모험해저화산지대.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, k;
	scanf("%d %d %d", &n, &m, &k);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	vector<vector<int>> turtles(m, vector<int>(2));
	for (int i = 0; i < m; i++) scanf("%d %d", &turtles[i][0], &turtles[i][1]);

	vector<vector<int>> volcanoes(k, vector<int>(3));
	for (int i = 0; i < k; i++) scanf("%d %d %d", &volcanoes[i][0], &volcanoes[i][1], &volcanoes[i][2]);

	vector<int> ans = solution(board, turtles, volcanoes);

#ifdef REPEAT_TEST
	if (solution(board, turtles, volcanoes) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본 출력 형식 그대로

	return 0;
}
