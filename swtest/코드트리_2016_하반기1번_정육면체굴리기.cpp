/*
	[코드트리] 2016 하반기 1번 - 정육면체 굴리기
	https://www.codetree.ai/training-field/frequent-problems/problems/cube-rounding

	■ 문제 요약
	  N x M 지도 위 (R, C)에 정육면체 주사위를 올려놓는다.
	  주사위의 여섯 면은 처음에 모두 0이고, 동/서/북/남 명령이 K개 주어진다.

	  각 명령마다
	    1) 그 방향으로 한 칸 나갈 수 없으면(지도 밖) 그 명령은 무시한다
	    2) 나갈 수 있으면 주사위를 그 방향으로 한 번 굴린다
	    3) 도착한 칸의 값이 0이면  -> 주사위 바닥면의 수를 그 칸에 복사한다
	       도착한 칸의 값이 0이 아니면 -> 그 칸의 수를 주사위 바닥면에 복사하고 칸은 0으로 만든다
	    4) 이동에 성공했을 때마다 주사위 윗면의 수를 출력한다

	■ 풀이 방침
	  주사위의 여섯 면을 구조체 필드로 들고, 굴릴 때마다 값을 자리바꿈한다.
	  각 명령은 "이동 가능 판정 -> 좌표 이동 -> 주사위 회전 -> 칸/바닥면 값 교환 -> 출력"
	  순서를 그대로 따르면 된다. 순서를 바꾸면 안 되는 게 이 문제의 전부다.

	■ 여섯 면 이름 정리 (헷갈리기 쉬움)
	    top    : 윗면 (하늘을 보는 면, 출력하는 값)
	    bottom : 아랫면 (지도에 닿는 면, 칸과 값을 주고받는 면)
	    up     : 북쪽을 향한 옆면
	    down   : 남쪽을 향한 옆면
	    left   : 서쪽을 향한 옆면
	    right  : 동쪽을 향한 옆면

	  즉 up / down은 "위아래"가 아니라 지도에서 북/남쪽 옆면이다.
	  실제로 위아래인 것은 top / bottom이다.

	■ 굴리기 규칙
	  주사위를 어느 방향으로 굴리면 그 방향으로 네 면이 한 칸씩 돌아간다.
	  마주 보는 두 면(회전축)은 그대로 남는다.

	    동으로 굴림 : left -> top -> right -> bottom -> left   (up, down 고정)
	    서로 굴림   : 위의 역방향
	    북으로 굴림 : down -> top -> up -> bottom -> down      (left, right 고정)
	    남으로 굴림 : 위의 역방향

	  값을 하나씩 옮기면 아직 안 옮긴 면을 덮어쓰므로,
	  각 함수는 먼저 여섯 면을 tmp에 통째로 떠 놓고 그 스냅샷에서 읽어 대입한다.
	    tmp[0]=up, tmp[1]=left, tmp[2]=top, tmp[3]=right, tmp[4]=down, tmp[5]=bottom

	■ 주의할 점
	  이동할 수 없는 명령은 "아무 일도 일어나지 않는다"가 핵심이다.
	  주사위를 굴리지도, 값을 옮기지도, 출력하지도 않는다.
	  그래서 범위 검사를 좌표를 실제로 옮기기 전에 먼저 해야 한다.
*/

#include <stdio.h>

#define MAX (20 + 10)

// 명령 번호
#define EAST  (1)
#define WEST  (2)
#define NORTH (3)
#define SOUTH (4)

int T;

int N, M;      // 지도 크기 (N행 M열)
int R, C;      // 주사위의 현재 위치
int K;         // 명령 개수

int MAP[MAX][MAX];       // 지도
int command[1000 + 50];  // 명령 목록

// 주사위 여섯 면
struct CUBE
{
	int up;      // 북쪽을 향한 옆면
	int left;    // 서쪽을 향한 옆면
	int top;     // 윗면 (출력 대상)
	int right;   // 동쪽을 향한 옆면
	int down;    // 남쪽을 향한 옆면
	int bottom;  // 아랫면 (지도와 값을 주고받는 면)
};

typedef struct CUBE CUBE;

CUBE cube;     // 전역이라 여섯 면이 모두 0으로 시작한다 (문제 조건과 일치)

// 방향 배열. 명령 번호(1~4)를 인덱스로 그대로 쓰기 위해 0번은 더미로 비워 둔다.
//   1=동, 2=서, 3=북, 4=남
int dr[] = { 0, 0, 0, -1, 1 };
int dc[] = { 0, 1,-1,  0, 0 };

// ---------------------------
// 입력
// ---------------------------
void input()
{
	scanf("%d %d %d %d %d", &N, &M, &R, &C, &K);

	// 지도 (0-based)
	for (int r = 0; r < N; r++)
	{
		for (int c = 0; c < M; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 명령 목록
	for (int k = 0; k < K; k++)
		scanf("%d", &command[k]);
}

// ---------------------------
// 디버그용: 지도와 명령 출력
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
// 디버그용: 주사위를 전개도 모양으로 출력
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

/* --------------------------------------------------
   주사위 굴리기

   네 함수 모두 같은 방식이다.
     (1) 여섯 면을 tmp에 스냅샷으로 떠 둔다
     (2) 회전 결과대로 tmp에서 읽어 대입한다
   회전축이 되는 두 면은 손대지 않는다.

     tmp[0]=up  tmp[1]=left  tmp[2]=top
     tmp[3]=right  tmp[4]=down  tmp[5]=bottom
   -------------------------------------------------- */

// 동쪽으로 굴리기 (회전축: up, down)
void moveEast()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.top = tmp[1];    // 서쪽 옆면이 위로 올라온다
	cube.right = tmp[2];  // 윗면이 동쪽으로 넘어간다
	cube.bottom = tmp[3]; // 동쪽 옆면이 바닥으로 내려간다
	cube.left = tmp[5];   // 바닥면이 서쪽으로 올라온다
}

// 서쪽으로 굴리기 (동쪽 굴리기의 역방향, 회전축: up, down)
void moveWest()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.top = tmp[3];    // 동쪽 옆면 -> 윗면
	cube.right = tmp[5];  // 바닥면   -> 동쪽
	cube.bottom = tmp[1]; // 서쪽 옆면 -> 바닥
	cube.left = tmp[2];   // 윗면     -> 서쪽
}

// 북쪽으로 굴리기 (회전축: left, right)
void moveNorth()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.up = tmp[2];     // 윗면     -> 북쪽 옆면
	cube.top = tmp[4];    // 남쪽 옆면 -> 윗면
	cube.down = tmp[5];   // 바닥면   -> 남쪽 옆면
	cube.bottom = tmp[0]; // 북쪽 옆면 -> 바닥
}

// 남쪽으로 굴리기 (북쪽 굴리기의 역방향, 회전축: left, right)
void moveSouth()
{
	int tmp[6] = {
		cube.up, cube.left, cube.top,
		cube.right, cube.down, cube.bottom
	};

	cube.up = tmp[5];     // 바닥면   -> 북쪽 옆면
	cube.top = tmp[0];    // 북쪽 옆면 -> 윗면
	cube.down = tmp[2];   // 윗면     -> 남쪽 옆면
	cube.bottom = tmp[4]; // 남쪽 옆면 -> 바닥
}

// ---------------------------
// 명령을 순서대로 수행
// ---------------------------
void simulate()
{
	for (int k = 0; k < K; k++)
	{
		int cmd = command[k];

		// 1) 지도 밖으로 나가는 명령은 통째로 무시한다.
		//    좌표를 옮기기 전에 검사해야 하고, 굴리기와 출력도 하지 않는다.
		if (R + dr[cmd] < 0 || R + dr[cmd] > N - 1 ||
			C + dc[cmd] < 0 || C + dc[cmd] > M - 1)
			continue;

		// 2) 위치 이동
		R = R + dr[cmd];
		C = C + dc[cmd];

		// 3) 주사위 회전
		if (cmd == EAST) moveEast();
		else if (cmd == WEST) moveWest();
		else if (cmd == NORTH) moveNorth();
		else if (cmd == SOUTH) moveSouth();

		// 4) 도착한 칸과 바닥면 사이의 값 교환
		if (MAP[R][C] == 0)
		{
			// 빈 칸이면 주사위가 자기 바닥면 값을 찍고 간다
			MAP[R][C] = cube.bottom;
		}
		else
		{
			// 수가 적혀 있으면 주사위가 그 값을 가져가고 칸은 비워진다
			cube.bottom = MAP[R][C];
			MAP[R][C] = 0;
		}

		// 5) 이동에 성공한 경우에만 윗면 출력
		printf("%d\n", cube.top);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 0; tc < T; tc++)
	{
		input();
		simulate();
	}

	return 0;
}
