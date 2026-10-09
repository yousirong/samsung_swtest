/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_코드챌린지2차예선_완전범죄.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int cnt, n, m;
	scanf("%d %d %d", &cnt, &n, &m);   // 원본 scanf 순서 그대로

	vector<vector<int>> info(cnt, vector<int>(2));
	for (int i = 0; i < cnt; i++)
		scanf("%d %d", &info[i][0], &info[i][1]);

	int ans = solution(info, n, m);

#ifdef REPEAT_TEST
	int ans2 = solution(info, n, m);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로

	return 0;
}
