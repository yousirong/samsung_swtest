/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2025_하반기오후1번_ai로봇청소기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt
	             (swtest 원본은 버그 4개 때문에 그대로는 죽거나 결과가 다르다.
	              solution.cpp 의 [버그수정] 4곳을 똑같이 고친 사본을 orig 로 쓴다)

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, k, l;
	scanf("%d %d %d", &n, &k, &l);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &board[r][c]);

	vector<vector<int>> cleaners(k, vector<int>(2));
	for (int i = 0; i < k; i++) scanf("%d %d", &cleaners[i][0], &cleaners[i][1]);

	vector<int> ans = solution(board, cleaners, l);

#ifdef REPEAT_TEST
	if (solution(board, cleaners, l) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본 출력 형식 그대로

	return 0;
}
