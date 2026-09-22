/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2021_상반기오전1번_놀이기구탑승.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);

	vector<vector<int>> students(n * n, vector<int>(5));
	for (int i = 0; i < n * n; i++)
		scanf("%d %d %d %d %d", &students[i][0], &students[i][1], &students[i][2], &students[i][3], &students[i][4]);

	int ans = solution(students);

#ifdef REPEAT_TEST
	int ans2 = solution(students);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);

	return 0;
}
