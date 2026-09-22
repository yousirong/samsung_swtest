/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2015_하반기1번_바이러스검사.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);                 // 원본 scanf 순서 그대로
	vector<int> restaurants(n);
	for (int i = 0; i < n; i++) scanf("%d", &restaurants[i]);
	int a, b;
	scanf("%d %d", &a, &b);

	long long ans = solution(restaurants, a, b);
#ifdef REPEAT_TEST
	long long ans2 = solution(restaurants, a, b);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%lld\n", ans);
	return 0;
}
