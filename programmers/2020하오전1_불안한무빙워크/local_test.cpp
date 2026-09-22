/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2020_하반기오전1번_불안한무빙워크.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
    int n, k;
    scanf("%d %d", &n, &k);          // 원본 scanf 순서 그대로
    vector<int> stability(2 * n);
    for (int i = 0; i < 2 * n; i++) scanf("%d", &stability[i]);

    int ans = solution(k, stability);
#ifdef REPEAT_TEST
    int ans2 = solution(k, stability);
    if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
    printf("%d\n", ans);
    return 0;
}
