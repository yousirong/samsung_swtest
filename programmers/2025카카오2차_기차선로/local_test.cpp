/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/프로그래머스_2025_하반기2차_기차선로.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

#include <vector>

using namespace std;

static int run(vector<vector<int>> g)
{
	int rows = (int)g.size(), cols = (int)g[0].size();
	vector<int*> p(rows);
	for (int i = 0; i < rows; i++) p[i] = &g[i][0];

	return solution(&p[0], rows, cols);
}

int main()
{
	int expected[6] = { 2, 2, 4, 644, 1, 0 };
	int got[6] = {
		run({ {1,0,-1}, {0,0,7}, {0,0,2} }),
		run({ {1,0,0,0,0,-1,-1}, {-1,0,0,1,0,0,1} }),
		run({ {1,0,0,0,0}, {0,0,3,0,2}, {0,0,0,0,2} }),
		run({ {1,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1} }),
		run({ {1,7}, {0,2} }),
		run({ {1,-1,0,0}, {-1,0,0,0}, {0,0,0,-1}, {0,0,-1,1} })
	};

	for (int i = 0; i < 6; i++)
		printf("예제%d 기대 %d / 결과 %d %s\n", i + 1, expected[i], got[i], expected[i] == got[i] ? "O" : "X");

	return 0;
}
