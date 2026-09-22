/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/16_BOJ_1913_달팽이.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, k;
	scanf("%d %d", &n, &k);

	vector<vector<int>> ans = solution(n, k);

#ifdef REPEAT_TEST
	if (solution(n, k) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (int r = 0; r < n; r++)
	{
		for (int c = 0; c < n; c++)
			printf("%d ", ans[r][c]);
		putchar('\n');
	}

	// 격자에서 K를 찾아 좌표를 출력 (1-based)
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			if (ans[r][c] == k) printf("%d %d", r + 1, c + 1);

	return 0;
}
