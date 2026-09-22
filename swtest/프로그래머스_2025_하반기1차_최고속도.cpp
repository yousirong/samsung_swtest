/*
	[프로그래머스] 2025 카카오 하반기 1차 - 최고 속도 (Lv. 3)
	https://school.programmers.co.kr/learn/courses/30/lessons/468376

	■ 문제 요약
	  2차원 평면에 도시 n개(점)와 도로 m개(x축 또는 y축에 평행한 선분)가 있다.
	  도로끼리 만나는 지점에서는 서로 갈아탈 수 있고, 모든 도시는 도로 위에 있다.

	  모든 도로의 "정중앙"에는 과속 단속 카메라가 있고 제한 속도 limit이 정해져 있다.
	  그 지점을 지날 때는 제한 속도 이하로 달려야 한다.
	  한 지점에 카메라가 여러 개면 그중 가장 낮은 제한을 따른다.

	  1번 도시에서 출발하기 전에 속도 v를 하나 정하고 끝까지 그 속도로 달린다.
	  2 ~ n번 도시로 갈 때 각각 낼 수 있는 최고 속도를 도시 번호 순서대로 구한다.
	  카메라를 하나도 지나지 않고 갈 수 있으면 0을 담는다.

	  제한 : 2 <= n <= 100, 1 <= m <= 1,000, 좌표 절댓값 10^9 이하,
	         1 <= limit <= 1,000,000, 도로 길이는 짝수(그래서 중앙이 항상 정수 좌표),
	         서로 다른 두 도로는 최대 한 점에서만 만난다, 카메라 위치와 도시 위치는 겹치지 않는다.

	  입출력 예
	      city [[-1,3],[7,3],[1,-1],[-2,6]]
	      road [[-1,7,7,7,80],[-3,3,9,3,45],[-2,-4,-2,6,60],[1,-4,1,8,50],[5,1,5,7,70]]
	      -> [70, 50, 0]

	      city [[3,5],[3,3],[2,1],[9,1],[7,-1]]
	      road [[3,-2,3,4,30],[5,1,9,1,29],[3,4,3,8,99],[1,1,5,1,99],[7,-3,7,5,99]]
	      -> [0, 30, 29, 29]

	■ 풀이 방침 : 제한 속도는 "도로"가 아니라 "점"에 걸린다
	  도로 단위로 그래프를 만들면 틀린다. 예제 2를 보면 도로 [5,1,9,1,29]의 카메라 (7,1)이
	  다른 도로 [7,-3,7,5,99] 위에도 놓여 있어서, 그 도로로 지나가도 29에 묶인다.

	  그래서 점을 노드로 삼는다.

	    1) 도로마다 그 위에 놓인 점을 모은다.
	       - 자기 카메라(중점)
	       - 도시
	       - 다른 도로와 만나는 점 (가로 도로의 y와 세로 도로의 x가 만나는 자리)
	       - 도로의 양 끝점 (같은 방향 도로끼리 끝에서 닿는 경우를 덮는다)
	    2) 카메라가 있는 점에 제한 속도를 적는다. 한 점에 여러 개면 가장 낮은 값.
	    3) 도로 위에서 이웃한 점끼리 잇는다. 이동 자체에는 비용이 없고, 점을 밟을 때만 제한이 걸린다.

	  이제 구하려는 값은 "경로에서 만난 카메라 제한의 최솟값을 최대로" 하는 최대 병목 경로다.

	■ 최대 병목을 유니온 파인드로 푸는 법 (다익스트라 없이)
	  제한이 높은 카메라부터 하나씩 "열어" 가며 연결해 나간다.

	    - 카메라가 없는 점은 처음부터 전부 열려 있다.
	      먼저 양쪽이 다 열린 간선을 전부 합쳐 둔다.
	      이 상태에서 1번 도시와 이어져 있는 도시는 카메라를 지나지 않고 갈 수 있으므로 답이 0이다.
	    - 그다음 카메라를 제한이 큰 것부터 연다. 열자마자 이웃 중 열린 점들과 합친다.
	      이때 1번 도시와 처음 이어지는 도시의 답이 곧 "방금 연 카메라의 제한"이다.
	      그 경로에 쓰인 카메라는 모두 지금까지 연 것들(제한이 이 값 이상)이고,
	      마지막으로 연 카메라가 병목이기 때문이다.

	  카메라 수가 m(<= 1,000)개뿐이라 "열 때마다 도시 n개를 확인"해도 10만 번이면 끝난다.

	■ 구현 메모
	  - 한 도로 위의 점은 x나 y 중 한 축만 변하므로, key = x + y 가 그 축을 따라 단조 증가한다.
	    정렬과 이분 탐색을 이 key 하나로 처리한다.
	  - 좌표가 10^9까지라 더하면 int를 넘긴다. 좌표와 key는 long long으로 둔다.
	  - 점은 도로마다 따로 모아 한 배열에 이어 붙이고, 도로 r의 시작 위치만 base[r]에 적어 둔다.
	    같은 좌표라도 도로가 다르면 다른 노드지만, 두 도로가 만나는 점은 간선으로 이어 준다.
	    (좌표를 전역으로 합쳐 번호를 매기지 않아도 되므로 정렬을 도로마다 작게 나눠 할 수 있다)
	  - 점 개수 상한 : 교차점은 두 도로당 최대 한 개라 전체 교차 쌍이 25만 개를 넘지 않고,
	    도로마다 도시 100개와 끝점/중점이 더해져도 60만 개 안쪽이다.
*/
#include <stdio.h>
#include <stdbool.h>

#define MAX_CITY (100 + 5)
#define MAX_ROAD (1000 + 5)
#define MAX_POINT (800000)   // 점 개수 상한 : 교차점 50만 + 도시/끝점 여유
#define MAX_EDGE (1200000)  // 간선 개수 상한 (도로 위 이웃 + 교차 연결)
#define MAX_TMP (4000)
#define INF (0x7fffffff)

int T;

int N, M;   // 도시 수, 도로 수

// [주의] y1, y0, j1 은 <math.h>가 쓰는 이름이다. 전역으로 두면 충돌하므로 이름을 피한다.
long long cityX[MAX_CITY], cityY[MAX_CITY];
long long roadSX[MAX_ROAD], roadSY[MAX_ROAD];   // 도로 시작점
long long roadEX[MAX_ROAD], roadEY[MAX_ROAD];   // 도로 끝점
int roadLimit[MAX_ROAD];                        // 그 도로 중앙 카메라의 제한 속도

// 도로별 점 목록을 한 배열에 이어 붙인다
int base[MAX_ROAD];          // 도로 r의 점들이 시작하는 위치
int pointCount[MAX_ROAD];    // 도로 r의 점 개수
long long px[MAX_POINT], py[MAX_POINT];
long long pkey[MAX_POINT];   // 정렬/이분 탐색용 키 (x + y : 도로 위에서 단조 증가)
int pcnt;                    // 전체 점 개수

int nodeLimit[MAX_POINT];    // 그 점의 카메라 제한 (카메라가 없으면 INF)
bool opened[MAX_POINT];      // 열린 점인가 (카메라가 없거나, 이미 연 카메라)

// 인접 리스트
int head[MAX_POINT];
int nxt[MAX_EDGE * 2], dest[MAX_EDGE * 2];
int ecnt;

int parent[MAX_POINT];       // 유니온 파인드

int cityNode[MAX_CITY];      // 도시가 놓인 점의 번호
int answer[MAX_CITY];

// 카메라가 있는 점 목록 (제한 내림차순으로 정렬해서 쓴다)
int camNode[MAX_ROAD];
int camLimit[MAX_ROAD];
int camCount;

// 도로 하나를 만드는 동안 쓰는 임시 배열
long long tmpX[MAX_TMP], tmpY[MAX_TMP], tmpKey[MAX_TMP];
int tcnt;

void input()
{
	scanf("%d", &N);

	for (int i = 0; i < N; i++)
		scanf("%lld %lld", &cityX[i], &cityY[i]);

	scanf("%d", &M);

	for (int i = 0; i < M; i++)
		scanf("%lld %lld %lld %lld %d",
			&roadSX[i], &roadSY[i], &roadEX[i], &roadEY[i], &roadLimit[i]);
}

// (x, y)가 도로 r 위에 있는가.
// 입력이 x1 <= x2, y1 <= y2 로 주어지므로 범위 검사만 하면 된다.
bool onRoad(int r, long long x, long long y)
{
	return (roadSX[r] <= x && x <= roadEX[r] && roadSY[r] <= y && y <= roadEY[r]);
}

// 도로 r 위에 있으면 임시 목록에 넣는다
void addTmp(int r, long long x, long long y)
{
	if (onRoad(r, x, y) == false) return;

	tmpX[tcnt] = x;
	tmpY[tcnt] = y;
	tmpKey[tcnt] = x + y;
	tcnt++;
}

// 임시 목록을 key 기준으로 정렬 (퀵 정렬)
void sortTmp(int lo, int hi)
{
	if (lo >= hi) return;

	long long pivot = tmpKey[(lo + hi) / 2];
	int i = lo, k = hi;

	while (i <= k)
	{
		while (tmpKey[i] < pivot) i++;
		while (tmpKey[k] > pivot) k--;

		if (i <= k)
		{
			long long t;

			t = tmpKey[i]; tmpKey[i] = tmpKey[k]; tmpKey[k] = t;
			t = tmpX[i];   tmpX[i] = tmpX[k];     tmpX[k] = t;
			t = tmpY[i];   tmpY[i] = tmpY[k];     tmpY[k] = t;

			i++;
			k--;
		}
	}

	sortTmp(lo, k);
	sortTmp(i, hi);
}

// 도로 r에서 key가 k인 점의 번호. 없으면 -1.
int findNode(int r, long long k)
{
	int lo = 0, hi = pointCount[r] - 1;

	while (lo <= hi)
	{
		int mid = (lo + hi) / 2;
		long long v = pkey[base[r] + mid];

		if (v == k) return base[r] + mid;

		if (v < k) lo = mid + 1;
		else hi = mid - 1;
	}

	return -1;
}

void addEdge(int a, int b)
{
	if (a < 0 || b < 0) return;

	dest[ecnt] = b; nxt[ecnt] = head[a]; head[a] = ecnt++;
	dest[ecnt] = a; nxt[ecnt] = head[b]; head[b] = ecnt++;
}

void buildGraph()
{
	pcnt = 0;
	ecnt = 0;

	// 1) 도로마다 그 위에 있는 점을 모아 정렬하고 중복을 없앤다
	for (int r = 0; r < M; r++)
	{
		tcnt = 0;

		// 카메라(중점). 도로 길이가 짝수라 나눠떨어진다.
		addTmp(r, (roadSX[r] + roadEX[r]) / 2, (roadSY[r] + roadEY[r]) / 2);

		// 양 끝점
		addTmp(r, roadSX[r], roadSY[r]);
		addTmp(r, roadEX[r], roadEY[r]);

		// 도시
		for (int i = 0; i < N; i++)
			addTmp(r, cityX[i], cityY[i]);

		// 다른 도로와 만나는 점
		for (int s = 0; s < M; s++)
		{
			if (s == r) continue;

			// 가로 x 세로 : 세로 도로의 x와 가로 도로의 y가 만나는 자리
			// (두 조합을 다 넣고, 실제로 도로 위인지는 addTmp가 걸러 준다)
			addTmp(r, roadSX[s], roadSY[r]);
			addTmp(r, roadSX[r], roadSY[s]);

			// 같은 방향 도로끼리 끝에서 닿는 경우
			addTmp(r, roadSX[s], roadSY[s]);
			addTmp(r, roadEX[s], roadEY[s]);
		}

		sortTmp(0, tcnt - 1);

		base[r] = pcnt;

		int k = 0;
		for (int i = 0; i < tcnt; i++)
		{
			// 같은 key는 같은 점이다 (도로 위에서는 key가 한 축을 따라 단조 증가)
			if (i > 0 && tmpKey[i] == tmpKey[i - 1]) continue;

			px[pcnt] = tmpX[i];
			py[pcnt] = tmpY[i];
			pkey[pcnt] = tmpKey[i];
			nodeLimit[pcnt] = INF;
			head[pcnt] = -1;

			pcnt++;
			k++;
		}

		pointCount[r] = k;
	}

	// 2) 카메라 제한을 점에 적는다 (한 점에 여러 개면 가장 낮은 것)
	for (int r = 0; r < M; r++)
	{
		long long mx = (roadSX[r] + roadEX[r]) / 2;
		long long my = (roadSY[r] + roadEY[r]) / 2;

		int id = findNode(r, mx + my);

		if (nodeLimit[id] > roadLimit[r]) nodeLimit[id] = roadLimit[r];
	}

	// 다른 도로 위에 놓인 카메라도 같은 제한을 받는다.
	// (그 점은 두 도로가 만나는 점이므로 양쪽 도로의 점 목록에 모두 들어 있다)
	for (int r = 0; r < M; r++)
	{
		for (int s = 0; s < M; s++)
		{
			if (s == r) continue;

			long long mx = (roadSX[s] + roadEX[s]) / 2;
			long long my = (roadSY[s] + roadEY[s]) / 2;

			if (onRoad(r, mx, my) == false) continue;

			int id = findNode(r, mx + my);

			if (id >= 0 && nodeLimit[id] > roadLimit[s]) nodeLimit[id] = roadLimit[s];
		}
	}

	// 3) 도로 위에서 이웃한 점끼리 잇는다
	for (int r = 0; r < M; r++)
		for (int i = 0; i + 1 < pointCount[r]; i++)
			addEdge(base[r] + i, base[r] + i + 1);

	// 4) 두 도로가 만나는 점은 서로 다른 노드이므로 이어 준다
	for (int r = 0; r < M; r++)
	{
		for (int s = r + 1; s < M; s++)
		{
			long long cand[4][2];

			cand[0][0] = roadSX[s]; cand[0][1] = roadSY[r];
			cand[1][0] = roadSX[r]; cand[1][1] = roadSY[s];
			cand[2][0] = roadSX[s]; cand[2][1] = roadSY[s];
			cand[3][0] = roadEX[s]; cand[3][1] = roadEY[s];

			for (int i = 0; i < 4; i++)
			{
				long long x = cand[i][0];
				long long y = cand[i][1];

				if (onRoad(r, x, y) == false) continue;
				if (onRoad(s, x, y) == false) continue;

				addEdge(findNode(r, x + y), findNode(s, x + y));
			}
		}
	}

	// 카메라가 있는 점 목록 만들기
	camCount = 0;
	for (int i = 0; i < pcnt; i++)
	{
		if (nodeLimit[i] == INF) continue;

		camNode[camCount] = i;
		camLimit[camCount] = nodeLimit[i];
		camCount++;
	}
}

int find(int x)
{
	while (parent[x] != x)
	{
		parent[x] = parent[parent[x]];   // 경로 절반 압축
		x = parent[x];
	}

	return x;
}

void unite(int a, int b)
{
	a = find(a);
	b = find(b);

	if (a != b) parent[a] = b;
}

// 열린 점 v를 이웃 중 열린 점들과 합친다
void uniteOpenedNeighbor(int v)
{
	for (int e = head[v]; e != -1; e = nxt[e])
	{
		int u = dest[e];

		if (opened[u] == true) unite(v, u);
	}
}

void solve()
{
	// 도시가 놓인 점 찾기
	for (int i = 0; i < N; i++)
	{
		cityNode[i] = -1;

		for (int r = 0; r < M && cityNode[i] < 0; r++)
		{
			if (onRoad(r, cityX[i], cityY[i]) == false) continue;

			cityNode[i] = findNode(r, cityX[i] + cityY[i]);
		}
	}

	for (int i = 0; i < pcnt; i++)
	{
		parent[i] = i;
		opened[i] = (nodeLimit[i] == INF);   // 카메라가 없는 점은 처음부터 열려 있다
	}

	// 1) 카메라를 하나도 지나지 않고 갈 수 있는 범위를 먼저 합친다
	for (int v = 0; v < pcnt; v++)
	{
		if (opened[v] == false) continue;

		uniteOpenedNeighbor(v);
	}

	int start = cityNode[0];

	for (int i = 1; i < N; i++)
		answer[i] = (find(cityNode[i]) == find(start)) ? 0 : -1;   // 0 = 카메라 없이 도달

	// 2) 카메라를 제한이 큰 것부터 연다 (선택 정렬 : 카메라는 최대 1,000개)
	for (int i = 0; i < camCount - 1; i++)
	{
		int best = i;

		for (int k = i + 1; k < camCount; k++)
			if (camLimit[k] > camLimit[best]) best = k;

		int t;
		t = camLimit[i]; camLimit[i] = camLimit[best]; camLimit[best] = t;
		t = camNode[i];  camNode[i] = camNode[best];   camNode[best] = t;
	}

	for (int i = 0; i < camCount; i++)
	{
		int v = camNode[i];

		opened[v] = true;
		uniteOpenedNeighbor(v);

		// 이번에 연 카메라 덕분에 처음 이어진 도시는 이 제한이 곧 최고 속도다
		for (int c = 1; c < N; c++)
		{
			if (answer[c] != -1) continue;

			if (find(cityNode[c]) == find(start)) answer[c] = camLimit[i];
		}
	}

	// 문제에서 모든 도시와 도로가 연결돼 있다고 했으므로 -1은 남지 않는다
	for (int c = 1; c < N; c++)
		if (answer[c] == -1) answer[c] = 0;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		buildGraph();

		solve();

		for (int i = 1; i < N; i++)
			printf("%d ", answer[i]);
		putchar('\n');
	}

	return 0;
}
