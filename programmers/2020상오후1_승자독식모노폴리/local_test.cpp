/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2020_상반기오후1번_승자독식모노폴리.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
    int n, m, k;
    scanf("%d %d %d", &n, &m, &k);   // 원본 scanf 순서 그대로

    vector<vector<int>> board(n, vector<int>(n));
    for (int r = 0; r < n; r++) for (int c = 0; c < n; c++) scanf("%d", &board[r][c]);

    vector<int> dirs(m);
    for (int i = 0; i < m; i++) scanf("%d", &dirs[i]);

    vector<vector<int>> priorities(m * 4, vector<int>(4));
    for (int i = 0; i < m * 4; i++)
        scanf("%d %d %d %d", &priorities[i][0], &priorities[i][1], &priorities[i][2], &priorities[i][3]);

    int ans = solution(k, board, dirs, priorities);
#ifdef REPEAT_TEST
    int ans2 = solution(k, board, dirs, priorities);
    if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
    printf("%d\n", ans);
    return 0;
}
