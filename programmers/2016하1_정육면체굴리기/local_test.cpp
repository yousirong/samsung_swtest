/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2016_하반기1번_정육면체굴리기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, r0, c0, k;
	scanf("%d %d %d %d %d", &n, &m, &r0, &c0, &k);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(n, vector<int>(m));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < m; c++)
			scanf("%d", &board[r][c]);

	vector<int> commands(k);
	for (int i = 0; i < k; i++) scanf("%d", &commands[i]);

	vector<int> ans = solution(r0, c0, board, commands);
#ifdef REPEAT_TEST
	vector<int> ans2 = solution(r0, c0, board, commands);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	for (size_t i = 0; i < ans.size(); i++)
		printf("%d\n", ans[i]);      // 원본은 이동에 성공할 때마다 윗면을 한 줄씩
	return 0;
}
