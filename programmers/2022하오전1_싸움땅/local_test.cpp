/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2022_하반기오전1번_싸움땅.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, k;
	scanf("%d %d %d", &n, &m, &k);

	vector<vector<int>> guns(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &guns[r][c]);

	vector<vector<int>> players(m, vector<int>(4));
	for (int i = 0; i < m; i++)
		scanf("%d %d %d %d", &players[i][0], &players[i][1], &players[i][2], &players[i][3]);

	vector<int> ans = solution(k, guns, players);

#ifdef REPEAT_TEST
	if (solution(k, guns, players) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d ", ans[i]);
	putchar('\n');

	return 0;
}
