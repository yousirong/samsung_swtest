/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2022_상반기오전1번_술래잡기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, h, k;
	scanf("%d %d %d %d", &n, &m, &h, &k);

	vector<vector<int>> runners(m, vector<int>(3));
	for (int i = 0; i < m; i++) scanf("%d %d %d", &runners[i][0], &runners[i][1], &runners[i][2]);

	vector<vector<int>> trees(h, vector<int>(2));
	for (int i = 0; i < h; i++) scanf("%d %d", &trees[i][0], &trees[i][1]);

	int ans = solution(n, k, runners, trees);

#ifdef REPEAT_TEST
	int ans2 = solution(n, k, runners, trees);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);

	return 0;
}
