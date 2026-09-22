/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2020_상반기오전1번_2차원테트리스.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
    int k;
    scanf("%d", &k);                 // 원본 scanf 순서 그대로
    vector<vector<int>> blocks(k, vector<int>(3));
    for (int i = 0; i < k; i++)
        scanf("%d %d %d", &blocks[i][0], &blocks[i][1], &blocks[i][2]);

    vector<int> ans = solution(blocks);
#ifdef REPEAT_TEST
    vector<int> ans2 = solution(blocks);
    if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
    printf("%d\n%d\n", ans[0], ans[1]);   // 원본은 점수와 남은 블록 수를 각각 다른 줄에
    return 0;
}
