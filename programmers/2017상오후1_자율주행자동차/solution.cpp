/*
	[코드트리] 2017 상반기 오후 1번 - 자율주행 자동차
	https://www.codetree.ai/training-field/frequent-problems/problems/autonomous-driving

	■ 문제 요약
	  N x M 격자에 자동차가 (R, C)에서 방향 D를 보고 서 있다.

	    칸  : 0 = 아직 청소하지 않은 도로, 1 = 인도(못 지나감), 2 = 이미 청소한 도로
	    방향 : 0 = 북, 1 = 동, 2 = 남, 3 = 서

	  아래 규칙을 멈출 때까지 반복하고, 청소한 칸의 개수를 출력한다.

	    1) 현재 칸을 청소한다(방문 표시).
	    2) 왼쪽 방향에 아직 청소하지 않은 도로가 있으면
	       그쪽으로 방향을 틀고 한 칸 전진한다.
	    3) 없으면 방향만 왼쪽으로 틀고 2번을 다시 본다. 네 방향을 모두 이렇게 확인한다.
	    4) 네 방향 다 안 되면 방향은 그대로 두고 한 칸 후진한다.
	    5) 후진할 칸이 인도라 후진조차 못 하면 멈춘다.

	  (백준 14503 "로봇 청소기"와 같은 문제다)

	■ 풀이 방침
	  규칙이 그대로 코드가 되는 시뮬레이션 문제다. 헷갈릴 만한 지점은 두 가지뿐이다.

	    - "왼쪽으로 회전"을 어떻게 표현하는가
	    - 4번의 후진은 "방향을 바꾸지 않는다"는 점

	■ 왼쪽 회전
	  방향이 북(0) 동(1) 남(2) 서(3) 순서, 즉 시계 방향으로 번호가 붙어 있으므로
	  왼쪽 회전은 번호를 하나 되돌리는 것이다.

	      북(0) -> 서(3) -> 남(2) -> 동(1) -> 북(0)

	  (D + 3) % 4로 계산해도 되지만, 여기서는 changeDir 표로 바로 찾는다.

	■ 후진
	  후진은 "뒤를 향해 도는 것"이 아니라 바라보는 방향을 유지한 채 뒤로 밀리는 것이다.
	  그래서 dr/dc를 더하지 않고 빼서 좌표만 옮긴다.
	  또 후진할 때는 이미 청소한 칸(2)이어도 상관없다. 인도(1)일 때만 멈춘다.

	■ 주의할 점
	  격자의 가장 바깥 줄은 문제에서 모두 인도로 주어지므로
	  자동차가 격자 밖으로 나갈 일이 없어 좌표 범위 검사를 하지 않는다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

#define MAX (50 + 5)

// 칸의 상태
#define ROAD   (0)   // 아직 청소하지 않은 도로
#define STREET (1)   // 인도, 이동 불가
#define MARK   (2)   // 이미 청소한 도로

int T;
int N, M;     // 격자 크기
int R, C;     // 자동차의 현재 위치
int D;        // 자동차가 바라보는 방향
int MAP[MAX][MAX];

// 방향 배열: 북(0), 동(1), 남(2), 서(3) - 시계 방향 순서
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0,-1 };

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다
void input(int r0, int c0, int d0, const std::vector<std::vector<int>>& board)
{
	N = (int)board.size();        // [수정] scanf 대체
	M = (int)board[0].size();     // [수정] scanf 대체
	R = r0;                       // [수정] scanf 대체
	C = c0;                       // [수정] scanf 대체
	D = d0;                       // [수정] scanf 대체

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			MAP[r][c] = board[r][c];   // [수정] scanf 대체 (원본이 0-based라 그대로)
}

// ---------------------------
// 디버그용 맵 출력
// ---------------------------
void printMap()
{
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// ---------------------------
// 자동차 시뮬레이션
// ---------------------------
void simulate()
{
	// 왼쪽 회전 표. 방향 번호가 시계 방향이므로 한 칸 되돌리면 왼쪽이다.
	//   북(0) -> 서(3), 동(1) -> 북(0), 남(2) -> 동(1), 서(3) -> 남(2)
	int changeDir[] = { 3, 0, 1, 2 };

	while (1)
	{
		// 1) 현재 칸 청소
		MAP[R][C] = MARK;

		// 왼쪽 방향과 그쪽 칸
		int nDir = changeDir[D];
		int nr = R + dr[nDir];
		int nc = C + dc[nDir];

		// 2) 왼쪽에 아직 청소하지 않은 도로가 있으면 그쪽으로 회전 + 전진
		if (MAP[nr][nc] == ROAD)
		{
			D = nDir;
			R = nr;
			C = nc;
		}
		// 3) 왼쪽이 막혔으면 방향만 계속 왼쪽으로 틀며 네 방향을 확인
		else
		{
			int i;

			for (i = 0; i < 4; i++)
			{
				// 회전만 먼저 반영한다 (전진 여부는 그다음에 판단)
				D = nDir;

				int nr = R + dr[D];
				int nc = C + dc[D];

				// 청소할 도로를 찾았으면 그쪽으로 전진하고 탈출
				if (MAP[nr][nc] == ROAD)
				{
					R = nr;
					C = nc;
					break;
				}

				// 아니면 한 번 더 왼쪽으로 틀 준비
				nDir = changeDir[D];
			}

			// 4) 네 방향 모두 청소할 곳이 없었다 (for가 끝까지 돌아 i == 4)
			if (i == 4)
			{
				// 방향은 그대로 두고 뒤로 한 칸.
				// "뒤"는 현재 방향의 반대이므로 dr/dc를 더하지 않고 뺀다.
				nr = R - dr[D];
				nc = C - dc[D];

				R = nr;
				C = nc;

				// 5) 뒤가 인도였다면 후진조차 못 하는 상황이므로 종료
				//    (이미 청소한 칸이면 계속 진행한다)
				if (MAP[R][C] == STREET)
					break;
			}
		}
	}
}

// ---------------------------
// 청소한 칸(MARK) 개수
// ---------------------------
int getAnswer()
{
	int sum = 0;

	for (int r = 0; r < N; r++)
		for (int c = 0; c < M; c++)
			if (MAP[r][c] == MARK)
				sum++;

	return sum;
}

// [수정] main() -> solution().  T 루프 껍데기는 제거했다.
int solution(int r0, int c0, int d0, std::vector<std::vector<int>> board)
{
	input(r0, c0, d0, board);

	simulate();

	return getAnswer();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
//   빌드      : g++ -O2 -DLOCAL_TEST -o run solution.cpp
//   재호출 검사 : g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n, m, r0, c0, d0;
	scanf("%d %d %d %d %d", &n, &m, &r0, &c0, &d0);   // 원본 scanf 순서 그대로

	std::vector<std::vector<int>> board(n, std::vector<int>(m));
	for (int r = 0; r < n; r++)
		for (int c = 0; c < m; c++)
			scanf("%d", &board[r][c]);

	int ans = solution(r0, c0, d0, board);
#ifdef REPEAT_TEST
	int ans2 = solution(r0, c0, d0, board);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT: %d vs %d\n", ans, ans2); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
#endif
