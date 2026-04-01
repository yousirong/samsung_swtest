// 코드트리 2016 하반기 1번 - 정육면체 굴리기
// https://www.codetree.ai/training-field/frequent-problems/problems/cube-rounding
// 문제 요약
// 지도 위에 정육면체(주사위)를 놓고,
// 동/서/북/남 명령에 따라 굴린다.
//
// 이동 규칙
// 1. 명령 방향으로 한 칸 이동할 수 있으면 주사위를 굴린다.
// 2. 이동한 칸의 값이 0이면
//    주사위 바닥면의 숫자를 칸에 복사한다.
// 3. 이동한 칸의 값이 0이 아니면
//    그 칸의 숫자를 주사위 바닥면에 복사하고,
//    칸의 값은 0으로 만든다.
// 4. 매번 이동이 성공할 때마다 주사위 윗면 숫자를 출력한다.
//
// 핵심 아이디어
// - 주사위의 6면 값을 구조체로 관리한다.
// - 명령이 들어올 때마다 좌표를 먼저 이동 가능한지 확인한다.
// - 이동 가능하면 주사위를 해당 방향으로 회전시키고,
//   바닥면과 지도 칸 사이의 값 복사를 처리한다.

#include <stdio.h>

#define MAX (20 + 10)

// 명령 번호
#define EAST  (1)
#define WEST  (2)
#define NORTH (3)
#define SOUTH (4)

int T;

int N, M;      // 지도 크기: N행 M열
int R, C;      // 현재 주사위 위치 (R: 행, C: 열)
int K;         // 명령 개수

int MAP[MAX][MAX];       // 지도
int command[1000 + 50];  // 명령 목록

// 주사위 6면 정보
struct CUBE
{
	int up;      // 북쪽 면
	int left;    // 왼쪽 면
	int top;     // 윗면
	int right;   // 오른쪽 면
	int down;    // 남쪽 면
	int bottom;  // 아랫면
};

typedef struct CUBE CUBE;

CUBE cube;     // 실제 주사위 상태

// 방향 배열
// 인덱스: 1=동, 2=서, 3=북, 4=남
int dr[] = { 0, 0, 0, -1, 1 };
int dc[] = { 0, 1,-1,  0, 0 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d %d %d", &N, &M, &R, &C, &K);

	// 지도 입력
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 명령 입력
	for (int k = 0; k < K; k++)
		scanf("%d", &command[k]);
}

// ---------------------------
// 디버그용 지도 출력
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

	for (int k = 0; k < K; k++)
		printf("%d ", command[k]);
	putchar('\n');
}

// ---------------------------
// 디버그용 주사위 출력
//
//        up
// left  top  right
//       down
//      bottom
// ---------------------------
void printCube()
{
	printf("   %d\n", cube.up);
	printf("%d %d %d\n", cube.left, cube.top, cube.right);
	printf("   %d\n", cube.down);
	printf("   %d\n", cube.bottom);
}

// --------------------------------------------------
// 주사위 굴리기 함수들
// --------------------------------------------------
//
// 현재 구조는 다음과 같이 생각하면 된다.
//
//        up
// left  top  right
//       down
//      bottom
//
// 동쪽으로 굴리면:
// top <- left
// right <- top
// bottom <- right
// left <- bottom
//
// 나머지 up, down은 그대로
//
// 각 move 함수는 현재 면 값을 tmp에 저장한 뒤
// 회전 결과대로 다시 대입한다.

// 동쪽으로 굴리기
void moveEast()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.top = tmp[1]; // left -> top
	cube.right = tmp[2]; // top -> right
	cube.bottom = tmp[3]; // right -> bottom
	cube.left = tmp[5]; // bottom -> left
}

// 서쪽으로 굴리기
void moveWest()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.top = tmp[3]; // right -> top
	cube.right = tmp[5]; // bottom -> right
	cube.bottom = tmp[1]; // left -> bottom
	cube.left = tmp[2]; // top -> left
}

// 북쪽으로 굴리기
void moveNorth()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.up = tmp[2]; // top -> up
	cube.top = tmp[4]; // down -> top
	cube.down = tmp[5]; // bottom -> down
	cube.bottom = tmp[0]; // up -> bottom
}

// 남쪽으로 굴리기
void moveSouth()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.up = tmp[5]; // bottom -> up
	cube.top = tmp[0]; // up -> top
	cube.down = tmp[2]; // top -> down
	cube.bottom = tmp[4]; // down -> bottom
}

// ---------------------------
// 전체 시뮬레이션
// ---------------------------
void simulate()
{
	for (int k = 0; k < K; k++)
	{
		int cmd = command[k];

		// ----------------------------------------
		// 1. 먼저 이동 가능한지 확인
		// 맵 밖으로 나가면 해당 명령은 무시
		// ----------------------------------------
		if (R + dr[cmd] < 0 || R + dr[cmd] > N - 1 ||
			C + dc[cmd] < 0 || C + dc[cmd] > M - 1)
			continue;

		// ----------------------------------------
		// 2. 실제 위치 이동
		// ----------------------------------------
		R = R + dr[cmd];
		C = C + dc[cmd];

		// ----------------------------------------
		// 3. 주사위를 해당 방향으로 굴림
		// ----------------------------------------
		if (cmd == EAST) moveEast();
		else if (cmd == WEST) moveWest();
		else if (cmd == NORTH) moveNorth();
		else if (cmd == SOUTH) moveSouth();

		// ----------------------------------------
		// 4. 지도 칸과 주사위 바닥면 값 처리
		// ----------------------------------------
		if (MAP[R][C] == 0)
		{
			// 칸이 0이면 주사위 바닥면 값을 칸에 복사
			MAP[R][C] = cube.bottom;
		}
		else
		{
			// 칸이 0이 아니면 칸의 값을 주사위 바닥면에 복사
			cube.bottom = MAP[R][C];
			MAP[R][C] = 0;
		}

		// ----------------------------------------
		// 5. 매 이동 후 윗면 출력
		// ----------------------------------------
		printf("%d\n", cube.top);
	}
}

int main()
{
	// 원래 테스트케이스 형식이 있을 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 0; tc < T; tc++)
	{
		input();
		simulate();
	}

	return 0;
}