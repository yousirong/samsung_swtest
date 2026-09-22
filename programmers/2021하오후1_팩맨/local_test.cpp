/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2021_하반기오후1번_팩맨.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int m, t, pr, pc;

	scanf("%d %d %d %d", &m, &t, &pr, &pc);   // 원본 scanf 순서 그대로 (원본의 %D 오타는 %d로 정정)

	vector<vector<int>> monsters(m, vector<int>(3));
	for (int i = 0; i < m; i++)
		scanf("%d %d %d", &monsters[i][0], &monsters[i][1], &monsters[i][2]);

	int ans = solution(t, pr, pc, monsters);

#ifdef REPEAT_TEST
	int ans2 = solution(t, pr, pc, monsters);   // 같은 인자로 한 번 더
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로
	return 0;
}
