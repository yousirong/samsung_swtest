/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2017_상반기오후1번_자율주행자동차.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, r0, c0, d0;
	scanf("%d %d %d %d %d", &n, &m, &r0, &c0, &d0);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(n, vector<int>(m));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < m; c++)
			scanf("%d", &board[r][c]);

	int ans = solution(r0, c0, d0, board);
#ifdef REPEAT_TEST
	int ans2 = solution(r0, c0, d0, board);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
