/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/21_BOJ_10866_덱.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n;
	scanf("%d", &n);   // 원본과 같은 형식

	vector<string> commands;
	for (int i = 0; i < n; i++)
	{
		char cmd[100];
		scanf("%s", cmd);

		string line = cmd;
		if (line == "push_front" || line == "push_back")
		{
			int v;
			scanf("%d", &v);

			char buf[32];
			sprintf(buf, " %d", v);
			line += buf;
		}
		commands.push_back(line);
	}

	vector<int> ans = solution(commands);

#ifdef REPEAT_TEST
	vector<int> ans2 = solution(commands);   // 같은 인자로 한 번 더
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++)
		printf("%d\n", ans[i]);   // 원본 출력 형식 그대로 (조회 명령마다 한 줄)

	return 0;
}
