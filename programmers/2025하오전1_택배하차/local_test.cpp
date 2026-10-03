/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2025_하반기오전1번_택배하차.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m;
	scanf("%d %d", &n, &m);   // 원본 scanf 순서 그대로

	vector<vector<int>> boxes(m, vector<int>(4));
	for (int i = 0; i < m; i++)
		scanf("%d %d %d %d", &boxes[i][0], &boxes[i][1], &boxes[i][2], &boxes[i][3]);

	vector<int> ans = solution(n, boxes);

#ifdef REPEAT_TEST
	if (solution(n, boxes) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본 출력 형식 그대로

	return 0;
}
