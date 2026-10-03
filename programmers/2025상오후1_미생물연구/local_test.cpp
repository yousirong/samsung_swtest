/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2025_상반기오후1번_미생물연구.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, q;
	scanf("%d %d", &n, &q);   // 원본 scanf 순서 그대로

	vector<vector<int>> queries(q, vector<int>(4));
	for (int i = 0; i < q; i++)
		scanf("%d %d %d %d", &queries[i][0], &queries[i][1], &queries[i][2], &queries[i][3]);

	vector<int> ans = solution(n, queries);

#ifdef REPEAT_TEST
	if (solution(n, queries) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본 출력 형식 그대로

	return 0;
}
