/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2023_하반기오전1번_왕실의기사대결.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int l, n, q;
	scanf("%d %d %d", &l, &n, &q);   // 원본 scanf 순서 그대로

	vector<vector<int>> board(l, vector<int>(l));
	for (int r = 0; r < l; r++)
		for (int c = 0; c < l; c++)
			scanf("%d", &board[r][c]);

	vector<vector<int>> knights(n, vector<int>(5));
	for (int i = 0; i < n; i++)
		scanf("%d %d %d %d %d", &knights[i][0], &knights[i][1], &knights[i][2], &knights[i][3], &knights[i][4]);

	vector<vector<int>> commands(q, vector<int>(2));
	for (int i = 0; i < q; i++)
		scanf("%d %d", &commands[i][0], &commands[i][1]);

	int ans = solution(board, knights, commands);

#ifdef REPEAT_TEST
	int ans2 = solution(board, knights, commands);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로

	return 0;
}
