/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2020_상반기오전2번_술래잡기체스.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
    vector<vector<int>> fishes(16, vector<int>(2));
    for (int i = 0; i < 16; i++)
        scanf("%d %d", &fishes[i][0], &fishes[i][1]);   // 원본 scanf 순서 그대로 (격자 순서 16쌍)

    int ans = solution(fishes);
#ifdef REPEAT_TEST
    int ans2 = solution(fishes);
    if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
    printf("%d\n", ans);
    return 0;
}
