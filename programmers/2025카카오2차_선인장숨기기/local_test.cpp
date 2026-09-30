/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_선인장숨기기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int m, n, h, w, k;
	scanf("%d %d %d %d", &m, &n, &h, &w);   // 원본 scanf 순서 그대로
	scanf("%d", &k);

	vector<vector<int>> drops(k, vector<int>(2));
	for (int i = 0; i < k; i++)
		scanf("%d %d", &drops[i][0], &drops[i][1]);

	vector<int> ans = solution(m, n, h, w, drops);

#ifdef REPEAT_TEST
	if (solution(m, n, h, w, drops) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	printf("%d %d\n", ans[0], ans[1]);   // 원본 출력 형식 그대로

	return 0;
}
