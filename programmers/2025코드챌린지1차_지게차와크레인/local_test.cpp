/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_코드챌린지1차예선_지게차와크레인.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m;
	scanf("%d %d", &n, &m);   // 원본 scanf 순서 그대로

	vector<string> storage(n);
	for (int i = 0; i < n; i++)
	{
		char line[64];
		scanf("%s", line);
		storage[i] = line;
	}

	int q;
	scanf("%d", &q);

	vector<string> requests(q);
	for (int i = 0; i < q; i++)
	{
		char req[8];
		scanf("%s", req);
		requests[i] = req;
	}

	int ans = solution(storage, requests);

#ifdef REPEAT_TEST
	int ans2 = solution(storage, requests);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로

	return 0;
}
