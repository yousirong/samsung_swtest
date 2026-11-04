/*
	[코드트리] 2023 하반기 오전 1번 - 왕실의 기사 대결
	https://www.codetree.ai/training-field/frequent-problems/problems/royal-knight-duel

	■ 문제 요약
	  L x L 체스판의 각 칸은 0(빈칸), 1(함정), 2(벽)이다.
	  기사 N명은 (좌상단 행, 좌상단 열, 높이 h, 너비 w, 체력 k)로 주어지는 직사각형이다.

	  명령 Q개가 (기사 번호 i, 방향 d)로 주어진다. 방향은 0 위, 1 오른쪽, 2 아래, 3 왼쪽이다.

	    - 이미 사라진 기사(체력이 0 이하)를 부르면 아무 일도 없다.
	    - i번 기사를 d 방향으로 한 칸 밀면, 그 방향에 닿는 다른 기사도 연쇄적으로 밀린다.
	    - 밀리는 기사 중 하나라도 벽(또는 판 밖)에 닿으면 아무도 움직이지 않는다.
	    - 움직인 뒤, 명령을 받은 기사를 뺀 나머지 밀린 기사들은
	      자기 영역에 놓인 함정 수만큼 체력이 깎인다.
	    - 체력이 0 이하가 된 기사는 판에서 사라진다.

	  명령을 모두 처리한 뒤, 살아 있는 기사들이 받은 피해량의 합을 출력한다.

	■ 풀이 방침
	  - 기사는 칸이 아니라 직사각형이므로, 매 명령마다 tmpMAP에 "이 칸은 몇 번 기사"인지 찍는다.
	    사라진 기사는 찍지 않으므로 자동으로 통행 대상에서 빠진다(setKnight).
	  - 연쇄 밀림은 BFS로 찾는다. 밀리는 기사의 칸들을 큐에 넣고,
	    각 칸의 d 방향 이웃을 보며
	        벽이면        -> 즉시 return (아무도 움직이지 않는다)
	        다른 기사면   -> 그 기사의 칸을 전부 큐에 넣는다
	    끝까지 벽을 만나지 않았다면 큐에 담긴 기사들이 모두 한 칸 움직인다.
	  - 판 테두리를 WALL로 채워 두었기 때문에 "판 밖으로 밀림"도 벽 검사 하나로 끝난다.

	■ 입력 값을 바로 의미 있는 값으로 바꿔 둔다
	  입력의 1(함정)을 TRAP(-1), 2(벽)을 WALL(-2)로 바꿔 저장한다.
	  덕분에 "기사 번호(양수)"와 "지형(음수)"이 섞이지 않고, 빈칸은 0 하나로 통일된다.

	■ 피해량 계산
	  체력이 깎이는 것은 "명령을 받은 기사를 뺀" 밀린 기사들뿐이다 (n == index는 건너뛴다).
	  움직인 뒤의 영역을 기준으로 함정 수를 센다.
	  답은 살아 있는 기사만 모아 (처음 체력 - 지금 체력)을 더한 값이다.
	  그래서 처음 체력을 init_k에 따로 보관해 둔다.

	■ 주의할 점
	  - BFS의 return은 "밀 수 없음"을 뜻한다. 이미 큐에 담아 둔 것은 버려지고 아무도 움직이지 않는다.
	  - 사라진 기사는 좌표를 그대로 들고 있지만 setKnight에서 빠지므로 다른 기사를 막지 않는다.
	  - 함정은 여러 번 밟을 수 있다. 밀릴 때마다 새 영역의 함정 수만큼 다시 깎인다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX_L (40+5)                     // L 최대 40 + 여유
#define MAX_N (30+5)                     // N 최대 30 + 여유

#define EMPTY (0)	                     // 빈칸
#define TRAP (-1)                        // 함정
#define	WALL (-2)                        // 벽

int T;                                   // 테스트 케이스 수

int L, N, Q;                             // 판 크기, 기사 수, 명령 수
int MAP[MAX_L][MAX_L];                   // 지형 (빈칸 / 함정 / 벽)
int tmpMAP[MAX_L][MAX_L];                // 기사 번호 지도

struct COMMAND                           // 명령
{
	int index;                           // 기사 번호
	int direction;                       // 방향
};

COMMAND command[100 + 10];               // 명령 목록

struct RCI                               // 큐 원소 : 좌표 + 기사 번호
{
	int r;                               // 행
	int c;                               // 열
	int index;                           // 그 칸의 기사 번호
};

RCI queue[MAX_L * MAX_L];                // BFS 큐

struct KNIGHT                            // 기사 정보
{
	int r;                               // 좌상단 행
	int c;                               // 좌상단 열
	int h;                               // 높이
	int w;                               // 너비
	int k; // 기사의 체력
	int init_k; // 기사의 원래 체력
};

KNIGHT knight[MAX_N];                    // 기사 배열 (1번부터)

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };              // 방향별 행 변화량
int dc[] = { 0, 1, 0, -1 };              // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &L, &N, &Q);       // 판 크기, 기사 수, 명령 수 입력

	for (int r = 0; r <= L+1; r++)       // 테두리 포함 전체를
		for (int c = 0; c <= L+1; c++)
			MAP[r][c] = WALL;            // 벽으로 채움

	for (int r = 1; r <= L; r++)         // 판 안쪽
	{
		for (int c = 1; c <= L; c++)
		{
			scanf("%d", &MAP[r][c]);     // 지형 입력

			if (MAP[r][c] == 1) MAP[r][c] = TRAP;
			// 입력의 1, 2를 음수로 바꿔 두면 기사 번호(양수)와 섞이지 않는다.
			else if (MAP[r][c] == 2) MAP[r][c] = WALL;
		}
	}

	for (int n = 1; n <= N; n++)         // 기사별
	{
		scanf("%d %d %d %d %d", &knight[n].r, &knight[n].c, &knight[n].h, &knight[n].w, &knight[n].k);   // 기사 정보 입력

		// 최초 체력 저장
		knight[n].init_k = knight[n].k;
	}

	for (int q = 0; q < Q; q++)          // 명령 입력
		scanf("%d %d", &command[q].index, &command[q].direction);
}

void printMap(int map[MAX_L][MAX_L]) // for debug
{
	for (int r = 1; r <= L; r++)         // 모든 행
	{
		for (int c = 1; c <= L; c++)     // 모든 열
			printf("%2d ", map[r][c]);   // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void printStatus()                       // for debug : 기사 상태 출력
{
	for (int n = 1; n <= N; n++)
	{
		KNIGHT tmp = knight[n];
		printf("%d] (%d, %d) %d, %d, : %d\n", n, tmp.r, tmp.c, tmp.h, tmp.w, tmp.k);
	}
	putchar('\n');
}

void setKnight()                         // tmpMAP에 살아 있는 기사 번호 찍기
{
	for (int r = 1; r <= L; r++)         // 지도 초기화
		for (int c = 1; c <= L; c++)
			tmpMAP[r][c] = 0;

	for (int n = 1; n <= N; n++)         // 기사별
	{
		if (knight[n].k <= 0) continue;
		// 사라진 기사는 찍지 않는다. 그래서 다른 기사를 막지도 않는다.

		int kr, kc, kh, kw;              // 기사 영역
		kr = knight[n].r;
		kc = knight[n].c;
		kh = knight[n].h;
		kw = knight[n].w;

		for (int r = kr; r < (kr + kh); r++)      // 영역 전체에
			for (int c = kc; c < (kc + kw); c++)
				tmpMAP[r][c] = n;	                // 기사 번호 찍기
	}
}

void BFS(int index, int direction)       // index번 기사를 direction으로 밀기
{
	if (knight[index].k <= 0) return;    // 사라진 기사면 무시

	int rp, wp;                          // 큐 읽기/쓰기 위치
	bool visit[MAX_L][MAX_L] = { 0 };    // 큐에 담은 칸
	int kr, kc, kh, kw;                  // 기사 영역

	kr = knight[index].r;
	kc = knight[index].c;
	kh = knight[index].h;
	kw = knight[index].w;

	rp = wp = 0;                         // 큐 초기화

	for (int r = kr; r < (kr + kh); r++) // 명령 받은 기사의 모든 칸을 큐에
	{
		for (int c = kc; c < (kc + kw); c++)
		{
			queue[wp].r = r;
			queue[wp].c = c;
			queue[wp++].index = index;

			visit[r][c] = true;          // 담음 표시
		}
	}

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RCI out = queue[rp++];           // 하나 꺼내기

		int nr, nc;                      // 밀려갈 칸

		nr = out.r + dr[direction];
		nc = out.c + dc[direction];

		if (MAP[nr][nc] == WALL) return;
		// 벽(또는 판 밖)에 닿았다. 이 명령은 아무도 움직이지 않는다.

		if (visit[nr][nc] == true || tmpMAP[nr][nc] == EMPTY) continue;
		// 빈칸이면 더 밀 대상이 없고, 이미 담은 칸은 건너뛴다.

		int another = tmpMAP[nr][nc];    // 밀리는 다른 기사
		int kr, kc, kh, kw;              // 그 기사의 영역

		kr = knight[another].r;
		kc = knight[another].c;
		kh = knight[another].h;
		kw = knight[another].w;

		for (int r = kr; r < (kr + kh); r++)   // 그 기사의 모든 칸을 큐에
		{
			for (int c = kc; c < (kc + kw); c++)
			{
				queue[wp].r = r;
				queue[wp].c = c;
				queue[wp++].index = another;

				visit[r][c] = true;      // 담음 표시
			}
		}
	}

	bool check[MAX_N] = { 0 };           // 밀리는 기사 표시
	for (int i = 0; i < wp; i++)         // 큐에 담긴 칸들의
	{
		int knightIndex = queue[i].index;   // 기사 번호

		check[knightIndex] = true;       // 밀림 표시
	}

	for (int n = 1; n <= N; n++)         // 기사별
	{
		if (check[n] == false) continue; // 안 밀린 기사 제외

		knight[n].r = knight[n].r + dr[direction];   // 한 칸 이동
		knight[n].c = knight[n].c + dc[direction];

		if (n == index) continue;
		// 명령을 받은 기사는 함정 피해를 입지 않는다.

		int kr, kc, kh, kw;              // 이동 후 영역

		kr = knight[n].r;
		kc = knight[n].c;
		kh = knight[n].h;
		kw = knight[n].w;

		for (int r = kr; r < (kr + kh); r++)
			for (int c = kc; c < (kc + kw); c++)
				if (MAP[r][c] == TRAP) knight[n].k--;   // 함정마다 체력 1 감소
	}
	//printMap(tmpMAP);


}

void simulate()
{
	for (int q = 0; q < Q; q++)          // 명령별
	{
		setKnight();                     // 기사 지도 갱신
		BFS(command[q].index, command[q].direction);   // 밀기
		//printMap(MAP);
		//printStatus();
	}

}

int getAnswer()                          // 살아 있는 기사의 피해 합
{
	int sum = 0;                         // 합
	for (int n = 1; n <= N; n++)
	{
		if (knight[n].k > 0)sum += (knight[n].init_k - knight[n].k);   // 살아 있으면 피해량 더하기
	}
	return sum;                          // 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		printf("%d\n", getAnswer());     // 결과 출력
	}
	return 0;                            // 프로그램 정상 종료
}
