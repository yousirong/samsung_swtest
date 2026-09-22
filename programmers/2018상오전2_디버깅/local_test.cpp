/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2018_상반기오전2번_디버깅.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int c0, m, r0;
	scanf("%d %d %d", &c0, &m, &r0);   // 원본 scanf 순서 그대로 (세로선 수, 가로선 수, 가로 줄 수)
	vector<vector<int>> lines(m, vector<int>(2));
	for (int i = 0; i < m; i++) scanf("%d %d", &lines[i][0], &lines[i][1]);

	int ans = solution(c0, r0, lines);
#ifdef REPEAT_TEST
	int ans2 = solution(c0, r0, lines);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
