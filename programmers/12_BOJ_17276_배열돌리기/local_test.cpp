/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/12_BOJ_17276_배열돌리기.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int tcnt;
	scanf("%d", &tcnt);   // 원본처럼 테스트 케이스 수부터 읽는다

	for (int tc = 1; tc <= tcnt; tc++)
	{
		int n, d;
		scanf("%d %d", &n, &d);

		vector<vector<int>> board(n, vector<int>(n));
		for (int r = 0; r < n; r++)
			for (int c = 0; c < n; c++)
				scanf("%d", &board[r][c]);

		vector<vector<int>> ans = solution(d, board);

#ifdef REPEAT_TEST
		if (solution(d, board) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

		for (int r = 0; r < n; r++)
		{
			for (int c = 0; c < n; c++)
				printf("%d ", ans[r][c]);   // 원본 printMap과 같은 형식 (끝에 공백 포함)
			putchar('\n');
		}
	}

	return 0;
}
