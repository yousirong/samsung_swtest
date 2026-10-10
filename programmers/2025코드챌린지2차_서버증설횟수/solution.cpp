#include <string>
#include <vector>

using namespace std;

#define HOUR (24)
#define MAX_TIME (24 + 24 + 5)   // 반납 시각은 최대 23 + 24

int USER[HOUR];          // 시간대별 이용자 수 (템플릿의 players와 이름이 겹치지 않게)
int M, K;                // 서버 한 대가 감당하는 이용자 수, 서버 운영 시간
int expire[MAX_TIME];    // expire[t] : t시에 반납되는 서버 수

void input(const vector<int>& players, int m, int k)
{
	for (int i = 0; i < HOUR; i++)
		USER[i] = players[i];     // 왼쪽 전역 USER, 오른쪽 매개변수 players → 헷갈릴 일이 없다

	M = m;
	K = k;

	for (int t = 0; t < MAX_TIME; t++)
		expire[t] = 0;
}

// 최소 증설 횟수를 돌려준다
int simulate()
{
	int count = 0;     // 증설 횟수
	int running = 0;   // 지금 돌아가는 증설 서버 수

	for (int i = 0; i < HOUR; i++)
	{
		// 1) i시에 반납되는 서버를 뺀다
		running -= expire[i];

		// 2) 이 시간대에 필요한 서버 수
		int need = USER[i] / M;

		// 3) 모자라면 모자란 만큼만 늘린다
		if (running < need)
		{
			int add = need - running;

			count += add;
			running += add;
			expire[i + K] += add;   // k시간 뒤에 반납
		}
	}

	return count;
}

int solution(vector<int> players, int m, int k) {
    int answer = 0;
    input(players, m, k);     // ← 추가
    answer = simulate();      // ← 추가
    return answer;
}
