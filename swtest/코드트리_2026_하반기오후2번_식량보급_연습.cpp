/*
	[코드트리] 2026 하반기 오후 2번 - 식량 보급 (연습)
	풀이 설명은 코드트리_2026_하반기오후2번_식량보급.cpp 참고. 본 코드와 로직이 같다.
*/
#include <stdio.h>                       // scanf, printf 사용

#define MAX (20000+1000+10)              // 처음 집 + 이주 최대 + 여유

int T;                                   // 테스트 케이스 수
int Q;                                   // 이벤트 수

long long val[MAX];                      // 저장값 (실제 - 강화 합) 오름차순
long long pre[MAX];                      // 누적합 pre[k] = val[0] + ... + val[k-1]
int hcnt;                                // 집 수
long long G;                             // 강화 합 (실제 = 저장값 + G)

long long tmp[MAX];                      // 병합 정렬용

void mergeSort(int s, int e)             // val[s] ~ val[e] 병합 정렬
{
	if (s >= e) return;                  // 원소 1개 이하면 끝

	int m = (s + e) / 2;                 // 가운데
	mergeSort(s, m);                     // 왼쪽 정렬
	mergeSort(m + 1, e);                 // 오른쪽 정렬

	int i = s, j = m + 1, k = s;         // 왼쪽, 오른쪽, 결과 위치
	while (i <= m && j <= e)             // 둘 다 남은 동안
	{
		if (val[i] <= val[j]) tmp[k++] = val[i++];   // 작은 쪽 먼저
		else tmp[k++] = val[j++];

	}
	while (i <= m) tmp[k++] = val[i++];  // 왼쪽 나머지
	while (j <= e) tmp[k++] = val[j++];  // 오른쪽 나머지

	for (int t = s; t <= e; t++) val[t] = tmp[t];   // 결과 복사

}

void makePrefix()                        // 누적합 만들기
{
	pre[0] = 0;                          // 0개 합
	for (int i = 0; i < hcnt; i++) pre[i + 1] = pre[i] + val[i];   // 누적
}

//100 겜 시작
void start()
{
	int n;                               // 처음 집 수
	scanf("%d", &n);

	hcnt = 0;                            // 집 수 초기화
	G = 0;                               // 강화 합 초기화

	for (int i = 0; i < n; i++)          // 비축량 입력
		scanf("%lld", &val[hcnt++]);

	mergeSort(0, hcnt - 1);              // 정렬
	makePrefix();                        // 누적합
}

//200 이주
void migrate(long long x)
{
	long long v = x - G;                 // 앞선 강화를 빼고 저장

	int i = hcnt - 1;                    // 맨 뒤부터
	while (i >= 0 && val[i] > v)         // 큰 값들을 뒤로 밀기
	{
		val[i + 1] = val[i];
		i--;
	}
	val[i + 1] = v;                      // 빈 자리에 넣기
	hcnt++;                              // 집 수 증가

	makePrefix();                        // 누적합 다시 만들기
}

//300
void strengthen(long long p)             // 영지 강화
{
	G += p;                              // 전체 더하기 값만 증가
}

//400
long long winter(long long p)            // 겨울나기
{
	int lo = 1, hi = hcnt;               // 이분 탐색 범위
	while (lo < hi)
	{
		int mid = (lo + hi + 1) / 2;     // 위쪽 가운데
		long long cost = (long long)mid * val[mid - 1] - pre[mid];   // mid채 맞추는 비용

		if (cost <= p) lo = mid;         // 가능하면 더 크게
		else hi = mid - 1;               // 불가능하면 더 작게

	}
	int k = lo;                          // 가장 큰 k
	long long total = pre[k] + (long long)k * G + p;   // 실제 합 + 보급량
	return total / k;                    // 평균 (내림)
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		scanf("%d", &Q);                 // 이벤트 수 입력

		for (int q = 0; q < Q; q++)      // 이벤트별
		{
			int cmd;                     // 명령 번호
			scanf("%d", &cmd);

			if (cmd == 100) start();     // 게임 시작
			else
			{
				long long p;             // 값
				scanf("%lld", &p);

				if (cmd == 200) migrate(p);                         // 이주
				else if (cmd == 300) strengthen(p);                 // 강화
				else if (cmd == 400) printf("%lld\n", winter(p));   // 겨울나기 출력
			}
		}
	}
	return 0;                            // 프로그램 정상 종료
}
