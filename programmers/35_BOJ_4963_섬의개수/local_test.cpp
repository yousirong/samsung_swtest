/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/35_BOJ_4963_섬의개수.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	while (1)
	{
		int w, h;
		scanf("%d %d", &w, &h);

		if (w == 0 && h == 0) break;

		vector<vector<int>> board(h, vector<int>(w));
		for (int r = 0; r < h; r++)
			for (int c = 0; c < w; c++)
				scanf("%d", &board[r][c]);

		int ans = solution(board);

#ifdef REPEAT_TEST
		int ans2 = solution(board);
		if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

		printf("%d\n", ans);
	}

	return 0;
}
