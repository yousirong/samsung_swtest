/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/15_BOJ_20327_배열돌리기6.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, r;
	scanf("%d %d", &n, &r);   // 원본과 같은 형식 (n은 지수, 배열 한 변은 2^n)

	int s = (1 << n);
	vector<vector<int>> board(s, vector<int>(s));
	for (int i = 0; i < s; i++)
		for (int j = 0; j < s; j++)
			scanf("%d", &board[i][j]);

	vector<vector<int>> commands(r, vector<int>(2));
	for (int i = 0; i < r; i++) scanf("%d %d", &commands[i][0], &commands[i][1]);

	vector<vector<int>> ans = solution(board, commands);

#ifdef REPEAT_TEST
	if (solution(board, commands) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (int i = 0; i < s; i++)
	{
		for (int j = 0; j < s; j++)
			printf("%d ", ans[i][j]);
		putchar('\n');
	}

	return 0;
}
