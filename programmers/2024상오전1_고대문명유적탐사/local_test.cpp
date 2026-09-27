/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2024_상반기오전1번_고대문명유적탐사.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int k, m;
	scanf("%d %d", &k, &m);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(5, vector<int>(5));
	for (int r = 0; r < 5; r++)
		for (int c = 0; c < 5; c++)
			scanf("%d", &board[r][c]);

	vector<int> pieces(m);
	for (int i = 0; i < m; i++) scanf("%d", &pieces[i]);

	vector<int> ans = solution(k, board, pieces);

#ifdef REPEAT_TEST
	if (solution(k, board, pieces) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d ", ans[i]);   // 원본 출력 형식 그대로

	return 0;
}
