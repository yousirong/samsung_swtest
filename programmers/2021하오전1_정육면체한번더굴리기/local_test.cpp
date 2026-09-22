/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2021_하반기오전1번_정육면제한번더굴리기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, x;
	scanf("%d %d", &n, &x);          // 원본 scanf 순서 그대로

	vector<vector<int>> board(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	int ans = solution(x, board);
#ifdef REPEAT_TEST
	int ans2 = solution(x, board);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
