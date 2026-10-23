/*
	[코드트리] 2026 하반기 - 식량 보급

	■ 문제 요약
	  집마다 식량 비축량이 있다. 이벤트 Q개(최대 200,000)가 순서대로 일어난다.
	    100 N a1 ... aN : 처음 집 N개(최대 20,000)
	    200 x           : 비축량 x인 집이 새로 들어온다 (최대 1,000번)
	    300 p           : 지금 있는 모든 집의 비축량 +p (영구)
	    400 p           : 식량 p를 마음대로 나눠 줘서 "가장 적은 비축량"을 최대로 만든 값을 출력 (일시적)
	  값이 크다 : a, x <= 10^9,  강화 p <= 10^6,  보급 p <= 10^12

	  입출력 예
	      예제 1 -> 3 3 7 4

	■ 400(겨울나기) 한 번 = "물 채우기"
	  비축량을 오름차순 v[0] <= v[1] <= ... 로 놓고, 가장 낮은 집들부터 물을 붓듯이 같이 올린다.
	  아래쪽 k채를 v[k-1] 높이까지 올리는 비용 = k x v[k-1] - (v[0] + ... + v[k-1]).
	  이 비용이 p 이하인 가장 큰 k를 찾으면, 그 k채를 똑같이 올린 높이
	      (v[0] + ... + v[k-1] + p) / k   (내림)
	  가 답이다. (k가 최대라서 이 높이는 v[k]에 못 미친다 -> k + 1번째 집은 신경 쓸 필요가 없다)
	  비용은 k가 커질수록 커지므로 k를 이분 탐색한다. 누적합 pre[]가 있으면 한 번에 O(log N).

	■ 300(영지 강화)는 "전체 더하기 값" 하나로 처리 (lazy)
	  모든 집에 p를 더하는 대신 전역 G에 p를 더해 둔다. 실제 비축량 = 저장값 + G.
	  나중에 들어오는 집은 그 전 강화를 받지 않아야 하므로, 저장할 때 x - G 로 넣는다.
	  모든 집에 같은 값을 더하는 것이라 저장값끼리의 순서는 절대 바뀌지 않는다.
	  -> 정렬과 누적합을 300 때문에 다시 만들 필요가 없다.

	■ 200(이주)만 정렬 배열을 고친다
	  이주는 최대 1,000번이다. 들어갈 자리를 찾아 뒤를 한 칸씩 밀고(삽입 정렬 한 번), 누적합을 다시 만든다.
	  한 번에 O(집 수) = 최대 21,000 -> 1,000번 해도 2,100만 번이라 충분하다.
	  처음 N개는 병합 정렬로 한 번 정렬한다 (선택 정렬은 20,000^2 = 4억이라 느리다).

	■ 주의할 점
	  - 전부 long long. 비축량은 10^9 + 강화 20만 번 x 10^6 ~ 2 x 10^11, 집 2만 채를 곱하면 ~10^16 근처.
	  - 저장값(x - G)은 음수가 될 수 있다. 답을 나눌 때는 실제값 합(pre[k] + k x G + p)으로 나눠야
	    양수끼리 나눗셈이 되어 내림이 맞다 (C의 음수 나눗셈은 0 쪽으로 버린다).
	  - p = 0이면 k = 1에서 비용 0이라 답은 가장 작은 비축량 그대로다.
*/
#include <stdio.h>

#define MAX_HOUSE (20000 + 1000 + 10)

int T;
int Q;

long long val[MAX_HOUSE];   // 저장값 (실제 비축량 - 들어올 때까지의 강화) 오름차순
long long pre[MAX_HOUSE];   // pre[k] = val[0] + ... + val[k-1]
int hcnt;                   // 집 수
long long G;                // 지금까지 강화 합 (실제 = 저장값 + G)

long long tmp[MAX_HOUSE];   // 병합 정렬용

void mergeSort(int s, int e)   // val[s] ~ val[e]
{
	if (s >= e) return;

	int m = (s + e) / 2;
	mergeSort(s, m);
	mergeSort(m + 1, e);

	int i = s, j = m + 1, k = s;
	while (i <= m && j <= e)
	{
		if (val[i] <= val[j]) tmp[k++] = val[i++];
		else tmp[k++] = val[j++];
	}
	while (i <= m) tmp[k++] = val[i++];
	while (j <= e) tmp[k++] = val[j++];

	for (int t = s; t <= e; t++) val[t] = tmp[t];
}

void makePrefix()
{
	pre[0] = 0;
	for (int i = 0; i < hcnt; i++) pre[i + 1] = pre[i] + val[i];
}

// 100 : 게임 시작
void start()
{
	int n;
	scanf("%d", &n);

	hcnt = 0;
	G = 0;

	for (int i = 0; i < n; i++)
		scanf("%lld", &val[hcnt++]);

	mergeSort(0, hcnt - 1);
	makePrefix();
}

// 200 : 이주 (정렬 위치에 끼워 넣기)
void migrate(long long x)
{
	long long v = x - G;   // 앞선 강화를 받지 않은 것으로 저장

	int i = hcnt - 1;
	while (i >= 0 && val[i] > v)   // 큰 값들을 한 칸씩 뒤로 민다
	{
		val[i + 1] = val[i];
		i--;
	}
	val[i + 1] = v;
	hcnt++;

	makePrefix();
}

// 300 : 영지 강화
void strengthen(long long p)
{
	G += p;   // 저장값은 그대로, 순서도 그대로
}

// 400 : 겨울나기
long long winter(long long p)
{
	// 아래쪽 k채를 val[k-1] 높이까지 올리는 비용 <= p 인 가장 큰 k (1 ~ hcnt)
	// 비용 = k x val[k-1] - pre[k]  (G는 양쪽에서 지워진다)
	int lo = 1, hi = hcnt;
	while (lo < hi)
	{
		int mid = (lo + hi + 1) / 2;
		long long cost = (long long)mid * val[mid - 1] - pre[mid];

		if (cost <= p) lo = mid;
		else hi = mid - 1;
	}

	int k = lo;
	long long total = pre[k] + (long long)k * G + p;   // 실제 비축량 합 + 보급량 (양수)
	return total / k;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		scanf("%d", &Q);

		for (int q = 0; q < Q; q++)
		{
			int cmd;
			scanf("%d", &cmd);

			if (cmd == 100) start();
			else
			{
				long long p;
				scanf("%lld", &p);

				if (cmd == 200) migrate(p);
				else if (cmd == 300) strengthen(p);
				else if (cmd == 400) printf("%lld\n", winter(p));
			}
		}
	}

	return 0;
}
