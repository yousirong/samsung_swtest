/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2017_하반기오후1번_돌아가는팔각의자.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	vector<string> chairs(4);
	for (int i = 0; i < 4; i++)
	{
		char buf[32];
		scanf("%s", buf);            // 원본은 %1d 로 한 자리씩 읽지만 결과는 같다
		chairs[i] = buf;
	}

	int k;
	scanf("%d", &k);
	vector<vector<int>> rotations(k, vector<int>(2));
	for (int i = 0; i < k; i++) scanf("%d %d", &rotations[i][0], &rotations[i][1]);

	int ans = solution(chairs, rotations);
#ifdef REPEAT_TEST
	int ans2 = solution(chairs, rotations);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
