/*
	[코드트리] 2026 하반기 오전 2번 - 코디의 퍼즐 공방 (연습)
	풀이 설명은 코드트리_2026_하반기오전2번_코디의퍼즐공방.cpp 참고.

	■ 본 코드와 다른 점 / 버그 (주석으로만 표시, 코드는 그대로)
	  [버그] makeShape() : return u * 27 + r * 9 + d * 3 + 1;
	         마지막이 왼쪽 변 l(엘)이 아니라 숫자 1(일)로 적혀 있다 -> + l 이어야 한다.
	         지금은 왼쪽 변이 무엇이든 1(직선)로 저장되어 구멍 모양 / 스페어 모양 / 회전이 모두 틀어진다.
*/
#include <stdio.h>	                     // scanf, printf 사용

#define MAX (2000+5)                     // N 최대 2000 + 여유
#define SHAPE (81)	                     // 모양 가짓수 (3^4)
#define EMPTY (-1)                       // 빈칸

int T;                                   // 테스트 케이스 수
int N, Q;                                // 판 크기, 명령 수

int board[MAX][MAX];                     // 칸별 모양 번호 (EMPTY 빈칸)
long long holeCnt[SHAPE];                // 모양별 구멍 수
long long spareCnt[SHAPE];               // 모양별 스페어 수

// 위, 오른쪽, 아래, 왼쪽
int dr[] = { -1,0,1,0 };                 // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

int makeShape(int u, int r, int d, int l)   // 네 변으로 모양 번호 만들기
{
	return u * 27 + r * 9 + d * 3 + 1;   // ※ 버그 : + l 이어야 함 (지금은 숫자 1)
}

int getSide(int shape, int dir)          // dir 방향 변 꺼내기
{
	int div[] = { 27, 9,3,1 };           // 자리값
	return shape / div[dir] % 3;         // 그 자리 숫자
}

// 시계 방향 90도 : a b c d -> d a b c
int rotate(int shape)
{
	return makeShape(getSide(shape, 3), getSide(shape, 0), getSide(shape, 1), getSide(shape, 2));   // 왼쪽 변이 위로
}

int getHole(int r, int c)                // 구멍이면 필요한 모양, 아니면 -1
{
	if (r<1 || c<1 || r>N || c>N) return -1;   // 판 밖
	if (board[r][c] != EMPTY) return -1;       // 퍼즐이 있음

	int side[4];                         // 필요한 네 변
	for (int dir = 0; dir < 4; dir++)    // 네 방향
	{
		int nr = r + dr[dir];            // 이웃 행
		int nc = c + dc[dir];            // 이웃 열

		if (nr<1 || nc<1 || nr>N || nc>N) side[dir] = 1;           // 테두리 -> 직선
		else if (board[nr][nc] == EMPTY) return -1;                // 빈 이웃 -> 구멍 아님
		else side[dir] = 2 - getSide(board[nr][nc], (dir + 2) % 4);   // 맞닿은 변의 반대
	}

	return makeShape(side[0], side[1], side[2], side[3]);   // 모양 번호 반환
}

void updateAround(int y, int x, int sign)   // 다섯 칸의 구멍 기여 더하기/빼기
{
	int hole = getHole(y, x);            // 자기 칸
	if (hole != -1) holeCnt[hole] += sign;

	for (int dir = 0; dir < 4; dir++)    // 네 이웃
	{
		hole = getHole(y + dr[dir], x + dc[dir]);
		if (hole != -1) holeCnt[hole] += sign;
	}
}

//100 판 준비
void prepare(int n)
{
	N = n;                               // 판 크기

	for (int r = 1; r <= N; r++)         // 모든 칸 비우기
		for (int c = 1; c <= N; c++)
			board[r][c] = EMPTY;

	for (int s = 0; s < SHAPE; s++)      // 개수 초기화
		holeCnt[s] = spareCnt[s] = 0;
}

// 200 배치
void setCell(int y, int x, int shape)
{
	updateAround(y, x, -1);              // 바뀌기 전 기여 빼기
	board[y][x] = shape;                 // 칸 바꾸기
	updateAround(y, x, +1);              // 바뀐 뒤 기여 더하기
}

long long getScore()                     // 500 점수
{
	long long score = 0;                 // 점수

	for (int s = 0; s < SHAPE; s++)      // 모양별
	{
		if (holeCnt[s] == 0) continue;   // 구멍 없는 모양 제외

		int rot[4];                      // 회전 모양들
		long long fit = 0;               // 맞는 스페어 수

		rot[0] = s;                      // 0번 회전

		for (int i = 1; i < 4; i++) rot[i] = rotate(rot[i - 1]);   // 1~3번 회전

		for (int i = 0; i < 4; i++)
		{
			int dup = 0;                 // 중복 여부
			for (int j = 0; j < i; j++)
				if (rot[j] == rot[i]) dup = 1;   // 이미 센 모양

			if (dup == 0) fit += spareCnt[rot[i]];   // 처음 나온 모양만 더하기
		}
		score+=holeCnt[s] * fit;         // 구멍 수 x 맞는 스페어 수
	}
	return score;                        // 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		scanf("%d", &Q);                 // 명령 수 입력

		for (int q = 0; q < Q; q++)      // 명령별
		{
			int cmd;                     // 명령 번호
			scanf("%d", &cmd);

			if (cmd == 100)              // 판 준비
			{
				int n;
				scanf("%d", &n);
				prepare(n);
			}
			else if (cmd == 200)         // 배치
			{
				int y, x, u, r, d, l;
				scanf("%d %d %d %d %d %d", &y, &x, &u, &r, &d, &l);
				setCell(y, x, makeShape(u, r, d, l));
			}
			else if (cmd == 300)         // 제거
			{
				int y, x;
				scanf("%d %d", &y, &x);
				setCell(y, x, EMPTY);
			}
			else if (cmd == 400)         // 스페어 추가
			{
				int u, r, d, l;
				scanf("%d %d %d %d", &u, &r, &d, &l);
				spareCnt[makeShape(u, r, d, l)]++;
			}
			else if (cmd == 500)         // 점수 출력
			{
				printf("%lld\n", getScore());
			}
		}
	}
	return 0;                            // 프로그램 정상 종료
}
