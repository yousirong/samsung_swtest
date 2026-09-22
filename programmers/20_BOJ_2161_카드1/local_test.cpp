/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/20_BOJ_2161_카드1.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);

	vector<int> ans = solution(n);

#ifdef REPEAT_TEST
	if (solution(n) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i + 1 < ans.size(); i++) printf("%d ", ans[i]);
	printf("%d\n", ans[ans.size() - 1]);

	return 0;
}
