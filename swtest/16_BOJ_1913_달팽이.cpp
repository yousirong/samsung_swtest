// BOJ 1913 - 달팽이
//
// 문제 요약
// N x N 배열의 가운데에서 시작해서
// 1부터 N*N까지를 달팽이 모양으로 채운다.
//
// 이동 방향 순서
// 위 -> 오른쪽 -> 아래 -> 왼쪽
//
// 이동 칸 수 패턴
// 1, 1, 2, 2, 3, 3, 4, 4, ...
// 즉, 두 번 방향을 바꿀 때마다 이동 칸 수가 1씩 증가한다.
//
// 추가로 K가 배열에서 어느 좌표에 있는지도 출력해야 한다.

#include <stdio.h>

#define MAX (1000 + 100)

int N, K;              // N: 배열 크기, K: 좌표를 찾아야 하는 숫자
int MAP[MAX][MAX];     // 달팽이 모양으로 숫자를 채울 배열

// 방향 배열: 상, 우, 하, 좌
int dr[] = { -1, 0, 1, 0 };
int dc[] = {  0, 1, 0,-1 };

// ---------------------------
// 배열 출력 함수
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

// ---------------------------
// 달팽이 배열 생성 함수
// ---------------------------
void makesnail()
{
	int ansR, ansC;      // 숫자 K의 좌표를 저장할 변수
	int r, c;            // 현재 위치
	int direction;       // 현재 방향
	int index;           // 현재 채울 숫자
	int size;            // 현재 방향으로 몇 칸 이동할지

	// 시작점은 배열의 가운데
	r = c = ansR = ansC = (N + 1) / 2;

	// 처음 방향은 "위"
	direction = 0;

	// 처음 채울 숫자는 1
	index = 1;

	// 처음 이동 길이
	size = 0;

	// 가운데 칸에 1을 채움
	MAP[r][c] = index++;

	// 방향 전환을 총 2N-1번 하면
	// N x N 배열 전체를 모두 채울 수 있다.
	for (int i = 0; i < 2 * N - 1; i++)
	{
		// 두 번 방향을 바꿀 때마다 이동 길이가 1 증가
		// i = 0, 2, 4, 6 ... 일 때 size++
		if (i % 2 == 0)
			size++;

		// 현재 방향으로 size칸 이동
		for (int s = 0; s < size; s++)
		{
			int nr, nc;

			// 다음 위치 계산
			nr = r + dr[direction];
			nc = c + dc[direction];

			// 지금 채우려는 숫자가 K라면 좌표 저장
			// index는 아직 MAP에 넣기 전의 숫자
			if (index == K)
			{
				ansR = nr;
				ansC = nc;
			}

			// 다음 위치에 숫자 채우기
			MAP[nr][nc] = index++;

			// 현재 위치 갱신
			r = nr;
			c = nc;
		}

		// 방향 전환
		// 상(0) -> 우(1) -> 하(2) -> 좌(3) -> 다시 상(0)
		direction++;
		if (direction == 4)
			direction = 0;
	}

	// 완성된 배열 출력
	printMap();

	// K의 좌표 출력
	printf("%d %d", ansR, ansC);
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	scanf("%d %d", &N, &K);

	makesnail();

	return 0;
}