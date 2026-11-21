/*
	[코드트리] 2019 상반기 오전 1번 - 시공의 돌풍 (풀이 2 : 테두리 좌표 배열로 회전)
	https://www.codetree.ai/training-field/frequent-problems/problems/heros-of-storm

	문제 요약과 확산 설명은 코드트리_2019_상반기오전1번_시공의돌풍.cpp 참고.
	확산(diffusion)은 그 파일과 같고, 돌풍(cleaning)의 회전 방법만 다르다.

	■ 풀이 1과 다른 점 : 회전을 "테두리 좌표 배열 + 인덱스 이동"으로 일반화
	  풀이 1 : 네 변을 정해진 순서로 한 칸씩 당긴다. 변마다 방향, 범위를 따로 맞춰야 한다.
	  풀이 2 : 직사각형 테두리 칸을 시계 방향 순서로 arr[0 ~ index-1]에 담고,
	           arr[i] 칸의 값을 arr[(i - rotateCount) mod index] 칸으로 옮긴다.
	           -> 위쪽(반시계)과 아래쪽(시계)을 같은 함수 rotateStep 하나로 처리한다.

	■ rotateStep(sr, sc, n, m, rotateCount)
	  (sr, sc)에서 시작하는 n x m 직사각형의 테두리를 rotateCount칸 회전한다.

	  1) tmpMAP에 MAP 원본을 복사한다 (옮기는 동안 원본 값이 덮이지 않도록)
	  2) 테두리 좌표를 시계 방향으로 arr[]에 담는다
	       윗줄  왼 -> 오   (sr, sc) ~ (sr, ec)
	       오른쪽 열 위 -> 아래  (sr+1, ec) ~ (er, ec)
	       아랫줄 오 -> 왼   (er, ec-1) ~ (er, sc)
	       왼쪽 열 아래 -> 위  (er-1, sc) ~ (sr+1, sc)
	     모서리는 한 번씩만 들어간다. 칸 수 index = 2(n + m) - 4.
	  3) arr[i]의 값을 arr[i - rotateCount]로 옮긴다 (원형이라 mod index)
	       rotateCount =  1 : 시계 순서에서 한 칸 앞으로 -> 반시계 회전
	       rotateCount = -1 : 시계 순서에서 한 칸 뒤로   -> 시계 회전
	     C의 % 는 음수가 나올 수 있으므로 0 미만이면 index를 더한다.

	■ cleaning()
	  위쪽 돌풍 : (1, 1)부터 up_r x M 직사각형을 반시계로 1칸  -> rotateStep(1, 1, up_r, M, 1)
	  아래쪽 돌풍 : (down_r, 1)부터 (N - up_r) x M 직사각형을 시계로 1칸 -> rotateStep(down_r, 1, N - up_r, M, -1)
	                (down_r = up_r + 1 이므로 N - up_r = N - down_r + 1 행)
	  회전하면 돌풍 칸의 -1도 함께 한 칸 밀려 (r, 2)로 간다.
	  -> 돌풍 칸을 다시 -1로 되돌리고, 바로 오른쪽 (r, 2)는 0(깨끗한 공기)으로 만든다.
	     (r, 2)로 밀려온 값이 바로 그 -1이므로, 0으로 덮으면 돌풍으로 들어간 먼지가 사라지는 효과도 난다.

	■ 주의할 점
	  - 회전 구역의 행 수가 1이면 윗줄과 아랫줄이 같은 줄이라 좌표가 중복으로 들어간다.
	    돌풍은 맨 윗줄, 맨 아랫줄과 2행 이상 떨어져 있다고 보장되므로 이 경우는 없다.
	  - tmpMAP은 확산에서도 쓰지만, 확산이 끝난 뒤에 덮어쓰므로 같은 배열을 써도 된다.
	  - 한 번 회전할 때마다 MAP 전체를 복사하므로 O(NM)이다. N, M <= 50 이라 충분하다.
*/
#include <stdio.h>                       // scanf, printf, putchar

#define MAX (50 + 10)                    // N, M 최대 50 + 여유
#define TORNADO_POSITION (-1)            // 돌풍 칸 표시

int T;                                   // 테스트 케이스 수
int N, M, time;                          // 행 수, 열 수, 시뮬레이션할 초
int MAP[MAX][MAX];                       // 현재 먼지 (-1은 돌풍)
int tmpMAP[MAX][MAX];                    // 확산 : 새로 들어온 양 / 회전 : 원본 복사본

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC tornado[2];                           // 돌풍 두 칸 ([0] 위쪽, [1] 아래쪽)
int tcnt;                                // 찾은 돌풍 칸 수

RC arr[MAX * MAX];                       // 회전할 테두리 좌표 (시계 방향 순서)

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };              // 방향별 행 변화량
int dc[] = { 0, 1, 0, -1 };              // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &time);    // 격자 크기, 시간

	tcnt = 0;                            // 돌풍 칸 수 초기화
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);     // 먼지 양

			if (MAP[r][c] == TORNADO_POSITION)   // 돌풍 위치 기억 (위쪽이 먼저)
			{
				tornado[tcnt].r = r;
				tornado[tcnt++].c = c;
			}
		}
	}
}

void printMap() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// copy <- original (전체 복사)
void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int i = 1; i <= N; i++)
		for (int k = 0; k <= M + 1; k++)
			copy[i][k] = original[i][k];
}

// 1단계 : 먼지 확산 (풀이 1과 같다)
void diffusion()
{
	for (int r = 1; r <= N; r++)         // 새로 들어올 양 초기화
		for (int c = 1; c <= M; c++)
			tmpMAP[r][c] = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			if (MAP[r][c] <= 0) continue;    // 빈 칸, 돌풍 칸은 확산 안 함

			int dust, count;                 // 한 방향 확산량, 퍼진 방향 수

			dust = count = 0;
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr < 1 || nc < 1 || nr > N || nc > M) continue;   // 격자 밖
				if (MAP[nr][nc] == TORNADO_POSITION) continue;        // 돌풍 칸

				dust = MAP[r][c] / 5;        // 이번 초 시작 값 / 5 (루프 안에서 MAP[r][c]는 안 바뀐다)

				tmpMAP[nr][nc] += dust;      // 받는 쪽은 임시 배열에 모은다
				count++;
			}

			MAP[r][c] -= (dust * count);     // 퍼진 방향 수만큼만 잃는다
		}
	}

	for (int r = 1; r <= N; r++)         // 받은 양 한꺼번에 반영
		for (int c = 1; c <= M; c++)
			MAP[r][c] += tmpMAP[r][c];
}

// (sr, sc)에서 시작하는 n x m 직사각형 테두리를 rotateCount칸 회전
// rotateCount = 1 : 반시계, -1 : 시계
void rotateStep(int sr, int sc, int n, int m, int rotateCount)
{
	int er, ec;                          // 직사각형 끝 행, 끝 열

	er = sr + n - 1;
	ec = sc + m - 1;

	copyMap(tmpMAP, MAP);                // 원본 보관 (옮기는 동안 덮이지 않도록)

	// 테두리 좌표를 시계 방향으로 담는다 (모서리는 한 번씩만)
	int index = 0;
	for (int c = sc; c <= ec; c++)       // 윗줄 : 왼 -> 오
	{
		arr[index].r = sr;
		arr[index++].c = c;
	}

	for (int r = sr + 1; r <= er; r++)   // 오른쪽 열 : 위 -> 아래
	{
		arr[index].r = r;
		arr[index++].c = ec;
	}

	for (int c = ec - 1; c >= sc; c--)   // 아랫줄 : 오 -> 왼
	{
		arr[index].r = er;
		arr[index++].c = c;
	}

	for (int r = er - 1; r >= sr + 1; r--)   // 왼쪽 열 : 아래 -> 위
	{
		arr[index].r = r;
		arr[index++].c = sc;
	}

	// arr[i]의 값을 arr[i - rotateCount]로 옮긴다
	for (int i = 0; i < index; i++)
	{
		int newIndex = (i - rotateCount) % index;   // 원형 인덱스

		newIndex = newIndex < 0 ? newIndex + index : newIndex;   // C의 음수 나머지 보정

		RC front = arr[newIndex];        // 값이 도착할 칸

		MAP[front.r][front.c] = tmpMAP[arr[i].r][arr[i].c];   // 원본 값으로 채운다
	}
}

// 2단계 : 돌풍 (위쪽 반시계, 아래쪽 시계)
void cleaning()
{
	int up_r = tornado[0].r;             // 위쪽 돌풍 행
	int down_r = tornado[1].r;           // 아래쪽 돌풍 행

	rotateStep(1, 1, up_r, M, 1);        // 위쪽 : 1행 ~ up_r행, 반시계
	MAP[up_r][1] = TORNADO_POSITION;     // 밀려난 돌풍 자리 복구
	MAP[up_r][2] = 0;                    // 바로 오른쪽은 깨끗한 공기

	rotateStep(down_r, 1, (N - up_r), M, -1);   // 아래쪽 : down_r행 ~ N행, 시계
	MAP[down_r][1] = TORNADO_POSITION;   // 돌풍 자리 복구
	MAP[down_r][2] = 0;                  // 바로 오른쪽은 깨끗한 공기
}

// 남은 먼지 총합 (돌풍의 -1은 제외)
int getAnswer()
{
	int sum = 0;
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			if (MAP[r][c] != TORNADO_POSITION) sum += MAP[r][c];

	return sum;
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)
	{
		input();                         // 입력

		for (int t = 0; t < time; t++)   // 1초 = 확산 -> 돌풍
		{
			diffusion();
			cleaning();
			// printMap();
		}

		printf("%d\n", getAnswer());     // 남은 먼지 출력
	}

	return 0;
}
