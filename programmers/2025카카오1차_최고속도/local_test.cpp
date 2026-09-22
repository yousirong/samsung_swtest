/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기1차_최고속도.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);

	vector<vector<int>> city(n, vector<int>(2));
	for (int i = 0; i < n; i++) scanf("%d %d", &city[i][0], &city[i][1]);

	int m;
	scanf("%d", &m);

	vector<vector<int>> road(m, vector<int>(5));
	for (int i = 0; i < m; i++)
		scanf("%d %d %d %d %d", &road[i][0], &road[i][1], &road[i][2], &road[i][3], &road[i][4]);

	vector<int> ans = solution(city, road);

#ifdef REPEAT_TEST
	if (solution(city, road) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d ", ans[i]);
	putchar('\n');

	return 0;
}
