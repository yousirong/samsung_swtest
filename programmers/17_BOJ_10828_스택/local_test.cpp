/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/17_BOJ_10828_스택.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);

	vector<string> commands;
	for (int i = 0; i < n; i++)
	{
		char buf[100];
		scanf("%s", buf);
		string line = buf;

		if (line == "push") { int v; scanf("%d", &v); char num[20]; sprintf(num, " %d", v); line += num; }
		commands.push_back(line);
	}

	vector<int> ans = solution(commands);

#ifdef REPEAT_TEST
	if (solution(commands) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본과 같은 형식

	return 0;
}
