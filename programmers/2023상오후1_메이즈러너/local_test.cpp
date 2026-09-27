/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2023_상반기오후1번_메이즈러너.cpp
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

	vector<vector<int>> players(m, vector<int>(2));
	for (int i = 0; i < m; i++)
		scanf("%d %d", &players[i][0], &players[i][1]);

	vector<int> exitPos(2);
	scanf("%d %d", &exitPos[0], &exitPos[1]);

	vector<int> ans = solution(k, board, players, exitPos);

#ifdef REPEAT_TEST
	if (solution(k, board, players, exitPos) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	printf("%d\n", ans[0]);              // 원본 출력 형식 그대로
	printf("%d %d\n", ans[1], ans[2]);

	return 0;
}
