/*
	로컬 대조용 테스트. 제출에는 쓰지 않는다.
	solution.cpp 를 그대로 include 하므로 제출 파일은 손대지 않는다.

	  빌드       : g++ -O2 -o run local_test.cpp
	  재호출 검사 : g++ -O2 -DREPEAT_TEST -o rep local_test.cpp
	  원본 대조   : g++ -O2 -o orig ../../swtest/코드트리_2024_하반기오전1번_미지의공간탈출.cpp
	               ./orig < in.txt > a.txt; ./run < in.txt > b.txt; cmp a.txt b.txt

	원본과 똑같은 형식으로 읽고 똑같은 형식으로 출력한다.
*/
#include "solution.cpp"

int main()
{
	int n, m, f;
	scanf("%d %d %d", &n, &m, &f);   // 원본 scanf 순서 그대로

	vector<vector<int>> ground(n, vector<int>(n));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < n; c++)
			scanf("%d", &ground[r][c]);

	vector<vector<vector<int>>> faces(5, vector<vector<int>>(m, vector<int>(m)));
	for (int i = 0; i < 5; i++)
		for (int r = 0; r < m; r++)
			for (int c = 0; c < m; c++)
				scanf("%d", &faces[i][r][c]);

	vector<vector<int>> anomalies(f, vector<int>(4));
	for (int i = 0; i < f; i++)
		scanf("%d %d %d %d", &anomalies[i][0], &anomalies[i][1], &anomalies[i][2], &anomalies[i][3]);

	int ans = solution(ground, faces, anomalies);

#ifdef REPEAT_TEST
	int ans2 = solution(ground, faces, anomalies);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif

	printf("%d\n", ans);   // 원본 출력 형식 그대로

	return 0;
}
