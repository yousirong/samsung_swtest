/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2020_하반기오후2번_회전하는빙하.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
    int n, q;
    scanf("%d %d", &n, &q);      // 원본 scanf 순서 그대로

    int side = 1 << n;           // 격자 한 변은 입력 n이 아니라 2^n 이다
    vector<vector<int>> board(side, vector<int>(side));
    for (int r = 0; r < side; r++)
        for (int c = 0; c < side; c++)
            scanf("%d", &board[r][c]);

    vector<int> levels(q);
    for (int i = 0; i < q; i++)
        scanf("%d", &levels[i]);

    vector<int> ans = solution(n, board, levels);

#ifdef REPEAT_TEST
    vector<int> ans2 = solution(n, board, levels);   // 같은 인자로 한 번 더
    if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

    printf("%d\n%d\n", ans[0], ans[1]);   // 원본 출력 형식 그대로 (두 줄)
    return 0;
}
