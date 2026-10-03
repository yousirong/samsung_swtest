/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2024_하반기오후1번_메두사와전사들.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m;
	vector<int> home(2), park(2);
	scanf("%d %d %d %d %d %d", &n, &m, &home[0], &home[1], &park[0], &park[1]);   // 원본 scanf 순서 그대로

	vector<vector<int>> warriors(m, vector<int>(2));
	for (int i = 0; i < m; i++) scanf("%d %d", &warriors[i][0], &warriors[i][1]);

	vector<vector<int>> board(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	vector<vector<int>> ans = solution(board, home, park, warriors);

#ifdef REPEAT_TEST
	if (solution(board, home, park, warriors) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++)   // 원본 출력 형식 그대로
	{
		for (size_t k = 0; k < ans[i].size(); k++)
			printf(k + 1 < ans[i].size() ? "%d " : "%d", ans[i][k]);
		putchar('\n');
	}

	return 0;
}
