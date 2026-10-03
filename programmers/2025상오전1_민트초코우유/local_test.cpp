/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2025_상반기오전1번_민트초코우유.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, t;
	scanf("%d %d", &n, &t);   // 원본 scanf 순서 그대로

	vector<string> foods(n, string(n, ' '));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf(" %c", &foods[r][c]);

	vector<vector<int>> believes(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &believes[r][c]);

	vector<vector<int>> ans = solution(foods, believes, t);

#ifdef REPEAT_TEST
	if (solution(foods, believes, t) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t d = 0; d < ans.size(); d++)   // 원본 출력 형식 그대로
	{
		for (int i = 0; i < 7; i++) printf("%d ", ans[d][i]);
		putchar('\n');
	}

	return 0;
}
