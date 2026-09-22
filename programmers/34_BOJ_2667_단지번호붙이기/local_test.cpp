/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/34_BOJ_2667_단지번호붙이기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);

	vector<vector<int>> board(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%1d", &board[r][c]);   // 원본과 같은 형식(붙어 있는 입력)

	vector<int> ans = solution(board);

#ifdef REPEAT_TEST
	if (solution(board) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	printf("%d\n", (int)ans.size());
	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);

	return 0;
}
