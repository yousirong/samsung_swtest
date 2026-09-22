/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/13_BOJ_16926_16927_배열돌리기1_2.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, r;
	scanf("%d %d %d", &n, &m, &r);

	vector<vector<int>> board(n, vector<int>(m));
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			scanf("%d", &board[i][j]);

	vector<vector<int>> ans = solution(r, board);

#ifdef REPEAT_TEST
	if (solution(r, board) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
			printf("%d ", ans[i][j]);   // 원본 printMap과 같은 형식
		putchar('\n');
	}

	return 0;
}
