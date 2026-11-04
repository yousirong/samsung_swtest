/*
	[코드트리] 2025 상반기 오후 1번 - 미생물 연구
	https://www.codetree.ai/training-field/frequent-problems   ("미생물 연구" 검색)
	[확인 필요] 문제 개별 주소(slug)를 찾지 못해 기출 목록 주소를 달아 두었다.

	■ 문제 요약
	  N x N 배양 용기에 실험을 Q번 한다. q번째 실험에서는 q번 미생물을 넣는다.

	    1) 투입 : (r1, c1) ~ (r2, c2) 직사각형(끝 좌표는 포함하지 않음)에 q번 미생물을 채운다.
	              그 자리에 있던 미생물은 덮여 사라진다.
	              덮인 결과 어떤 미생물이 둘 이상으로 쪼개지면 그 미생물은 통째로 사라진다.
	    2) 이동 : 살아 있는 미생물을 새 용기로 옮긴다.
	              넓이가 큰 것부터(같으면 먼저 넣은 것부터) 모양을 그대로 유지한 채,
	              다른 미생물과 겹치지 않고 용기를 벗어나지 않는 자리 중
	              좌표가 가장 작은 곳에 놓는다. 놓을 곳이 없으면 그 미생물은 사라진다.
	    3) 기록 : 맞닿은 미생물 쌍마다 (넓이 x 넓이)를 더해 출력한다.

	■ 풀이 방침
	  - 용기는 MAP 한 장에 "그 칸의 미생물 번호"를 적어 관리한다 (0이면 빈칸).
	  - 쪼개짐 판정은 BFS로 한다(findLiveMicro).
	    번호별로 덩어리를 찾다가, 이미 본 번호의 덩어리가 또 나오면 쪼개진 것이므로 dead로 표시한다.
	    BFS를 돌며 덩어리의 넓이(size)와 감싸는 사각형(minR ~ maxR, minC ~ maxC)도 함께 구한다.
	  - 이동은 정렬한 순서대로 하나씩 새 용기(newMAP)에 놓는다.
	    놓을 자리는 (0, 0)부터 모든 좌상단 후보를 차례로 시도하고, 처음 들어가는 곳에 놓는다.
	    모양은 "감싸는 사각형 안에서 자기 번호인 칸"만 옮기므로 그대로 유지된다.
	  - 점수는 모든 칸의 상하좌우를 보며 서로 다른 번호가 맞닿은 쌍을 company[][]에 표시하고,
	    표시된 쌍마다 넓이를 곱해 더한다.

	■ 모양을 그대로 옮기는 방법
	  감싸는 사각형의 왼쪽 위 (sr, sc)를 새 자리 (fr, fc)에 맞춘다.
	      새 좌표 = (fr - sr + r, fc - sc + c)
	  사각형 안이라도 다른 번호이거나 빈칸이면 건너뛴다. 그래서 L자 같은 모양도 그대로 옮겨진다.

	■ 정렬
	  isPriority : 넓이가 크면 우선, 같으면 번호가 작은(먼저 넣은) 것이 우선.
	  미생물 수가 최대 50개라 선택 정렬로 충분하다.

	■ 주의할 점
	  - "[수정]" 표시가 붙은 두 곳(BFS와 getScore의 경계 검사, moveMicro의 (0, 0) 시작)은
	    원래 코드를 고친 흔적이다. 좌표가 0부터 시작하므로 경계는 0 ~ N-1 이다.
	  - dead[]와 MAP은 input()에서 초기화하지 않는다. 한 번만 실행하면 전역 0 초기화로 충분하지만
	    여러 테스트케이스를 돌리거나 함수형으로 재호출하면 이전 값이 남는다.
	  - 이동할 자리를 못 찾은 미생물은 newMAP에 안 들어가므로 자연히 사라진다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (15+5)                       // N 최대 15 + 여유
#define MAX_Q (50+10)                    // Q 최대 50 + 여유

int T;                                   // 테스트 케이스 수
int N, Q;                                // 용기 크기, 실험 수
int MAP[MAX][MAX];                       // 칸별 미생물 번호 (0 빈칸)

struct QUERY                             // 투입 영역
{
	int r1;                              // 시작 행
	int r2;                              // 끝 행 (미포함)
	int c1;                              // 시작 열
	int c2;                              // 끝 열 (미포함)
};

QUERY query[MAX_Q];                      // 실험 목록

struct MICRO                             // 미생물 정보
{
	int id;                              // 번호
	int minR;                            // 감싸는 사각형 최소 행
	int minC;                            // 최소 열
	int maxR;                            // 최대 행
	int maxC;                            // 최대 열
	int size;                            // 넓이
};

MICRO micro[MAX_Q];                      // 살아 있는 미생물 목록
int mcnt;                                // 미생물 수

bool dead[MAX_Q];                        // 번호별 사망 여부

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐
bool visit[MAX][MAX];                    // BFS 방문 표시

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };                 // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d", &N, &Q);              // 용기 크기, 실험 수 입력

	// 미생물 번호는 1번부터
	for (int q = 1; q <= Q; q++)
		scanf("%d %d %d %d", &query[q].r1, &query[q].c1, &query[q].r2, &query[q].c2);   // 투입 영역 입력
}

void printMap(int map[MAX][MAX])         // for debug
{
	for (int r = 0; r <= N; r++)
	{
		for (int c = 0; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void printMicro(MICRO m)                 // for debug : 미생물 하나 출력
{
	printf("id %d] (%d, %d) ~ (%d, %d) / size %d [dead=%d]\n",
		m.id, m.minR, m.minC, m.maxR, m.maxC, m.size, dead[m.id]);
}

void printMicroAll()                     // for debug : 전체 출력
{
	for (int i = 0; i < mcnt; i++) printMicro(micro[i]);
}

void insert(int id, int r1, int c1, int r2, int c2)   // 미생물 투입
{
	for (int r = r1; r < r2; r++)
		// 끝 좌표(r2, c2)는 포함하지 않는다. 덮이는 칸은 새 번호가 된다.
		for (int c = c1; c < c2; c++)
			MAP[r][c] = id;
}

MICRO BFS(int r, int c)                  // (r, c)의 덩어리 정보 구하기
{
	int rp, wp;                          // 큐 위치
	int minR, minC, maxR, maxC;          // 감싸는 사각형

	rp = wp = 0;                         // 큐 초기화

	queue[wp].r = r;                     // 시작 칸 넣기
	queue[wp++].c = c;

	visit[r][c] = true;                  // 방문 표시

	minR = maxR = r;                     // 사각형 초기화
	minC = maxC = c;

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			// [수정] 격자 경계 검사
			if (nr < 0 || nc < 0 || nr >= N || nc >= N) continue;

			if (MAP[out.r][out.c] != MAP[nr][nc] || visit[nr][nc] == true) continue;   // 다른 번호거나 방문함

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = true;        // 방문 표시

			if (nr < minR) minR = nr;    // 사각형 갱신
			if (nc < minC) minC = nc;
			if (nr > maxR) maxR = nr;
			if (nc > maxC) maxC = nc;
		}
	}

	MICRO ret = { 0 };                   // 결과

	ret.id = MAP[r][c];                  // 번호
	ret.minR = minR;                     // 사각형
	ret.minC = minC;
	ret.maxR = maxR;
	ret.maxC = maxC;
	ret.size = wp;                       // 넓이 = 큐에 들어간 칸 수

	return ret;                          // 반환
}

void findLiveMicro()                     // 살아 있는 미생물 찾기
{
	mcnt = 0;                            // 목록 초기화

	for (int r = 0; r < N; r++)          // 방문 초기화
		for (int c = 0; c < N; c++)
			visit[r][c] = false;

	bool check[MAX_Q] = { false };       // 번호별 덩어리 발견 여부
	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < N; c++)
		{
			int id = MAP[r][c];          // 칸의 번호

			if (id == 0 || dead[id] == true || visit[r][c] == true) continue;   // 빈칸, 죽음, 방문함

			MICRO m = BFS(r, c);         // 덩어리 정보

			// 같은 id가 두 번째 영역으로 발견 → 분리된 것이므로 사망
			if (check[id] == true)
				// 같은 번호의 덩어리가 이미 하나 있었다 -> 쪼개졌으니 사라진다.
			{
				dead[id] = true;
				continue;
			}

			check[id] = true;            // 발견 표시
			micro[mcnt++] = m;           // 목록에 추가
		}
	}

	int tcnt = mcnt;                     // 임시 개수
	mcnt = 0;
	for (int i = 0; i < tcnt; i++)       // 죽은 미생물 걸러내기
	{
		if (dead[micro[i].id] == true) continue;

		micro[mcnt++] = micro[i];
	}
}

// a가 우선순위가 더 높으면 true
bool isPriority(MICRO a, MICRO b)
{
	if (a.size != b.size) return a.size > b.size;
	// 넓이가 크면 우선, 같으면 번호(먼저 넣은 순서)가 작은 쪽

	return a.id < b.id;
}

void sort()                              // 선택 정렬
{
	for (int i = 0; i < mcnt - 1; i++)
	{
		for (int k = i + 1; k < mcnt; k++)
		{
			if (isPriority(micro[i], micro[k]) == false)   // k가 더 우선이면
			{
				MICRO tmp = micro[i];    // 교환
				micro[i] = micro[k];
				micro[k] = tmp;
			}
		}
	}
}

bool checkMove(int newMAP[MAX][MAX], MICRO m, int fr, int fc)   // (fr, fc)에 놓을 수 있는가
{
	int sr = m.minR;                     // 사각형 시작 행
	int sc = m.minC;                     // 시작 열
	int er = m.maxR;                     // 끝 행
	int ec = m.maxC;                     // 끝 열

	for (int r = sr; r <= er; r++)       // 사각형 안
	{
		for (int c = sc; c <= ec; c++)
		{
			// 미생물 사각형 범위에 다른 미생물 or 비어있는 경우
			if (MAP[r][c] != m.id || MAP[r][c] == 0) continue;

			int newR = fr - sr + r;
			// 감싸는 사각형의 왼쪽 위를 (fr, fc)에 맞춰 옮긴다.
			int newC = fc - sc + c;

			// 격자 밖을 넘어가는 경우
			if (newR >= N || newC >= N) return false;

			// 새 용기에 이미 다른 미생물이 있을 경우
			if (newMAP[newR][newC] != 0) return false;
		}
	}
	return true;                         // 놓을 수 있음
}

void move(int newMAP[MAX][MAX], MICRO m, int fr, int fc)   // (fr, fc)에 실제로 놓기
{
	int sr = m.minR;                     // 사각형 시작 행
	int sc = m.minC;                     // 시작 열
	int er = m.maxR;                     // 끝 행
	int ec = m.maxC;                     // 끝 열

	for (int r = sr; r <= er; r++)       // 사각형 안
	{
		for (int c = sc; c <= ec; c++)
		{
			// 미생물 사각형 범위에 다른 미생물 or 비어있는 경우
			if (MAP[r][c] != m.id || MAP[r][c] == 0) continue;

			int newR = fr - sr + r;      // 새 행
			int newC = fc - sc + c;      // 새 열

			newMAP[newR][newC] = m.id;   // 새 용기에 찍기
		}
	}
}

void moveMicro(int newMAP[MAX][MAX], MICRO m)   // 미생물 하나 옮기기
{
	// [수정] (0, 0)부터 탐색
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < N; c++)
		{
			if (checkMove(newMAP, m, r, c) == true)   // 놓을 수 있으면
			{
				move(newMAP, m, r, c);
				// 좌표가 작은 쪽부터 시도하므로 처음 들어가는 곳이 정답 자리다.
				return;
			}
		}
	}
}

void moveAll()                           // 전체 이동
{
	int newMAP[MAX][MAX] = { 0 }; // 새 배양 용기

	for (int i = 0; i < mcnt; i++) moveMicro(newMAP, micro[i]);   // 정렬 순서대로 옮기기

	for (int r = 0; r < N; r++)          // 새 용기를 MAP으로
		for (int c = 0; c < N; c++)
			MAP[r][c] = newMAP[r][c];
}

int getSize(int id)                      // 번호의 넓이
{
	for (int i = 0; i < mcnt; i++)
		if (micro[i].id == id) return micro[i].size;

	return -1; // for debug
}

int getScore(int maxID)                  // 맞닿은 쌍의 점수 합
{
	bool company[MAX_Q][MAX_Q] = { false };   // 맞닿음 표시
	for (int r = 0; r < N; r++)          // 모든 칸
	{
		for (int c = 0; c < N; c++)
		{
			if (MAP[r][c] == 0) continue;   // 빈칸 제외

			for (int i = 0; i < 4; i++)  // 4방향
			{
				int nr, nc;              // 이웃 칸

				nr = r + dr[i];
				nc = c + dc[i];

				// [수정] 격자 경계 검사
				if (nr < 0 || nc < 0 || nr >= N || nc >= N) continue;

				int id1 = MAP[r][c];     // 내 번호
				int id2 = MAP[nr][nc];   // 이웃 번호

				if (id1 == id2 || id2 == 0) continue;   // 같거나 빈칸이면 제외

				company[id1][id2] = true;
				// 맞닿은 쌍만 표시한다. 같은 쌍이 여러 번 맞닿아도 한 번만 센다.
				company[id2][id1] = true;
			}
		}
	}

	int score = 0;                       // 점수

	for (int i = 1; i <= maxID - 1; i++) // 모든 쌍 (i < k)
	{
		for (int k = i + 1; k <= maxID; k++)
		{
			if (company[i][k] == false) continue;   // 맞닿지 않으면 제외

			int size1 = getSize(i);      // 넓이1
			int size2 = getSize(k);      // 넓이2

			score += (size1 * size2);    // 곱 누적
		}
	}
	return score;                        // 반환
}

void simulate()
{
	for (int id = 1; id <= Q; id++)      // 실험별
	{
		int r1, c1, r2, c2;              // 투입 영역

		r1 = query[id].r1;
		c1 = query[id].c1;
		r2 = query[id].r2;
		c2 = query[id].c2;

		// 미생물 투입
		insert(id, r1, c1, r2, c2);
		// 배양 용기 이동
		findLiveMicro();
		sort();
		moveAll();

		// printMap(MAP);

		// 실험 결과 기록
		printf("%d\n", getScore(id));
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션 + 출력
	}

	return 0;                            // 프로그램 정상 종료
}
