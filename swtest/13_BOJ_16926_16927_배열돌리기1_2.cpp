/*
	[BOJ] 16926 배열 돌리기 1 / 16927 배열 돌리기 2
	https://www.acmicpc.net/problem/16926
	https://www.acmicpc.net/problem/16927

	■ 문제 요약
	  N x M 크기의 배열을 R번 회전시킨 결과를 출력한다.
	  회전은 배열 전체를 돌리는 것이 아니라, 배열을 여러 개의 테두리(ring, layer)로 나눠
	  각 테두리를 "반시계 방향으로 한 칸씩" 미는 것을 1회로 센다.

	    예) 5 x 4 배열의 테두리는 2개
	        - 바깥 테두리 : (1,1) ~ (5,4)의 가장자리
	        - 안쪽 테두리 : (2,2) ~ (4,3)의 가장자리

	  두 문제의 차이는 R의 크기뿐이다. 16926은 R <= 1000, 16927은 R <= 10^9.

	■ 풀이 방침
	  테두리를 한 칸씩 R번 미는 것을 그대로 시뮬레이션하면 16927에서 시간 초과가 난다.
	  그래서 "한 칸씩 R번" 대신 "한 번에 R칸 점프"시킨다.

	    1) 테두리 위의 좌표들을 시계 방향 순서로 arr[0..index-1]에 나열한다.
	    2) arr[i]에 있던 값은 반시계로 R칸 밀리므로 arr[i - R] 자리로 간다.
	    3) 인덱스는 원형이므로 % index로 감싸고, 음수는 + index로 보정한다.

	  이러면 회전 횟수 R과 무관하게 테두리 한 개당 O(둘레)에 끝난다.

	■ 주의할 점
	  1) 값을 옮기다 보면 아직 옮기지 않은 칸을 덮어쓰게 된다.
	     그래서 회전 전 상태를 temp에 복사해 두고 "읽기는 temp, 쓰기는 MAP"을 지킨다.
	  2) C의 % 연산은 피연산자가 음수면 결과도 음수다. (-3 % 10 == -3)
	     그래서 newIndex가 음수면 반드시 + index 보정을 해줘야 한다.
	  3) 테두리 개수는 min(N, M) / 2 이다. 짧은 변 기준으로 층이 몇 겹인지 정해진다.
*/

#include <stdio.h>

#define MAX (300 + 50)   // N, M 최대보다 여유 있게 선언

int N, M, R;             // N: 행 수, M: 열 수, R: 회전 횟수
int MAP[MAX][MAX];       // 원본 배열, 1-based
int temp[MAX][MAX];      // 회전 직전 상태 스냅샷(읽기 전용으로 사용)

// 좌표 한 쌍
struct RC
{
	int r;   // row (행)
	int c;   // col (열)
};

typedef struct RC RC;

// 현재 처리 중인 테두리의 좌표를 "시계 방향 순서"로 담아 두는 배열.
// 한 테두리의 둘레는 최대 2*(N+M) 정도지만 넉넉하게 잡았다.
RC arr[MAX * MAX];

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &R);

	// N x M 배열 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}
}

// ---------------------------
// 결과 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// original -> copymap 으로 배열 전체 복사
//
// 회전 도중 덮어쓰기 사고를 막는 스냅샷용.
// ---------------------------
void copyMap(int copymap[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
		{
			copymap[r][c] = original[r][c];
		}
	}
}

// ---------------------------
// 테두리 한 겹을 rotateCount칸 반시계 회전
//
//   sr, sc      : 이 테두리의 시작(왼쪽 위) 좌표
//   n, m        : 이 테두리가 감싸는 영역의 세로/가로 길이
//   rotateCount : 회전 횟수
//
//   예) 전체가 4x4라면
//       바깥 테두리 : sr=1, sc=1, n=4, m=4
//       안쪽 테두리 : sr=2, sc=2, n=2, m=2
// ---------------------------
void rotatestep(int sr, int sc, int n, int m, int rotateCount)
{
	int er, ec;   // 끝(오른쪽 아래) 좌표

	er = sr + n - 1;
	ec = sc + m - 1;

	// 회전 전 상태 스냅샷. 아래에서 값을 읽을 때는 항상 temp를 본다.
	copyMap(temp, MAP);

	int index = 0;   // 이 테두리에 속한 칸의 개수(= 둘레)

	// 테두리 좌표를 시계 방향으로 한 바퀴 돌며 arr에 나열한다.
	// 모서리가 중복해서 들어가지 않도록 각 변의 시작점을 한 칸씩 비켜서 잡는다.

	// 1) 윗변 : (sr, sc) -> (sr, ec)
	for (int c = sc; c <= ec; c++)
	{
		arr[index].r = sr;
		arr[index++].c = c;
	}

	// 2) 오른쪽 변 : (sr+1, ec) -> (er, ec)
	//    오른쪽 위 모서리는 윗변에서 이미 넣었으므로 sr+1부터 시작
	for (int r = sr + 1; r <= er; r++)
	{
		arr[index].r = r;
		arr[index++].c = ec;
	}

	// 3) 아랫변 : (er, ec-1) -> (er, sc)
	//    오른쪽 아래 모서리는 이미 넣었으므로 ec-1부터 시작
	for (int c = ec - 1; c >= sc; c--)
	{
		arr[index].r = er;
		arr[index++].c = c;
	}

	// 4) 왼쪽 변 : (er-1, sc) -> (sr+1, sc)
	//    위/아래 왼쪽 모서리는 이미 넣었으므로 그 사이만 채운다
	for (int r = er - 1; r >= sr + 1; r--)
	{
		arr[index].r = r;
		arr[index++].c = sc;
	}

	// 이제 arr[0..index-1]에 이 테두리의 좌표가 시계 방향 순서로 들어 있다.
	// 반시계로 rotateCount칸 민다는 것은
	// "시계 방향 순서에서 인덱스를 rotateCount만큼 뒤로 당긴다"는 뜻이다.
	for (int i = 0; i < index; i++)
	{
		// arr[i]에 있던 값이 이동할 목적지 인덱스
		int newIndex = (i - rotateCount) % index;

		// C의 % 는 음수를 그대로 음수로 돌려주므로 보정이 필요하다
		newIndex = newIndex < 0 ? newIndex + index : newIndex;

		// 값이 들어갈 실제 좌표
		RC front = arr[newIndex];

		// 원래 값은 반드시 스냅샷(temp)에서 읽는다
		MAP[front.r][front.c] = temp[arr[i].r][arr[i].c];
	}
}

// ---------------------------
// 배열 전체 회전
//
// 바깥 테두리부터 안쪽으로 한 겹씩 들어가며 각각 회전시킨다.
//
//   4x4 -> 2겹, 5x4 -> 2겹, 6x6 -> 3겹
//   겹 수 = min(N, M) / 2
// ---------------------------
void rotate(int rotateCount)
{
	int sr, sc, n, m;

	// 만들 수 있는 테두리 개수는 짧은 변이 결정한다
	int step = N < M ? N / 2 : M / 2;

	// 가장 바깥 테두리에서 출발
	sr = sc = 1;
	n = N;
	m = M;

	for (int s = 0; s < step; s++)
	{
		rotatestep(sr, sc, n, m, rotateCount);

		// 한 겹 안쪽으로: 시작점은 대각선으로 한 칸, 크기는 가로세로 2씩 줄어든다
		sr++;
		sc++;
		n -= 2;
		m -= 2;
	}
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();     // N, M, R, 배열 입력

	rotate(R);   // 모든 테두리를 R칸씩 반시계 회전

	printMap();  // 결과 출력

	return 0;
}
