/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_코드챌린지1차예선_비밀코드해독.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m;
	scanf("%d %d", &n, &m);   // 원본 scanf 순서 그대로

	vector<vector<int>> q(m, vector<int>(5));
	for (int i = 0; i < m; i++)
		for (int j = 0; j < 5; j++)
			scanf("%d", &q[i][j]);

	vector<int> ans(m);
	for (int i = 0; i < m; i++)
		scanf("%d", &ans[i]);

	int res = solution(n, q, ans);

#ifdef REPEAT_TEST
	int res2 = solution(n, q, ans);
	if (res != res2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", res, res2); return 1; }
#endif

	printf("%d\n", res);   // 원본 출력 형식 그대로

	return 0;
}
