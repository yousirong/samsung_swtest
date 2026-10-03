/*
	[코드트리] 2025 하반기 오전 1번 - 택배 하차
	https://www.codetree.ai/training-field/frequent-problems   ("택배 하차" 검색)
	[확인 필요] 문제 개별 주소(slug)를 찾지 못해 기출 목록 주소를 달아 두었다.

	■ 문제 요약
	  N x N 창고에 택배 M개를 차례로 넣는다. 택배는 (번호 k, 세로 h, 가로 w, 왼쪽 열 c)로 주어진다.
	  넣은 택배는 위에서 떨어뜨려 더 내려갈 수 없을 때까지 아래로 내려간다.

	  모두 넣은 뒤에는 하차를 왼쪽 -> 오른쪽 번갈아 반복한다.
	    - 왼쪽 하차 : 왼쪽으로 그대로 빼낼 수 있는 택배(왼쪽에 아무것도 없는 것) 중 번호가 가장 작은 것을 뺀다.
	    - 오른쪽 하차 : 오른쪽으로 빼낼 수 있는 택배 중 번호가 가장 작은 것을 뺀다.
	    - 뺄 때마다 남은 택배들이 다시 아래로 떨어진다.
	  뺀 택배의 번호를 순서대로 출력한다.

	■ 풀이 방침
	  - MAP에 "그 칸을 차지한 택배 번호"를 적는다(0 빈칸). 테두리는 -1로 두어 바닥과 벽 역할을 시킨다.
	  - 떨어뜨리기(moveDown) : 지도에서 자기 칸을 지운 뒤, 바로 아래 줄이 전부 0인 동안 한 칸씩 내린다.
	    자기 칸을 먼저 지워야 자기 몸에 막혀 멈추는 일이 없다.
	  - 왼쪽/오른쪽으로 뺄 수 있는가 : 택배가 차지한 행들에서 그 바깥쪽 칸이 전부 비어 있는지 본다.
	  - 하차 후 정리(moveDownAll) : 더 떨어질 택배가 없을 때까지 "떨어질 수 있는 택배를 떨어뜨리기"를 반복한다.
	    한 번 떨어지면 그 위 택배가 새로 떨어질 수 있으므로 한 바퀴로는 부족하다.

	■ 택배 번호를 배열 인덱스로 쓴다
	  택배 번호(최대 100)를 그대로 box[] 인덱스로 쓴다. 번호가 1 ~ M이 아니라 띄엄띄엄 올 수 있어서
	  "가장 큰 번호"(maxBoxNum)까지 돌며 box[i].k == 0 인 칸은 없는 번호로 보고 건너뛴다.
	  번호 순으로 돌기 때문에 "뺄 수 있는 것 중 번호가 가장 작은 것"은 처음 찾은 것이다.

	■ 주의할 점
	  - 택배 정보는 input()이 아니라 simulate()에서 읽는다. 넣는 즉시 떨어뜨려야 하기 때문이다.
	  - 하차 루프는 M번이 아니라 2번씩(왼쪽 + 오른쪽) 묶어 돈다.
	    M이 홀수면 마지막 오른쪽 하차는 뺄 택배가 없어 아무것도 출력하지 않는다.
	  - moveDown의 b.r += 1 은 쓰이지 않는 지역 변수 갱신이다. 실제 위치는 box[index].r 이다.
*/
#include <stdio.h>

#define MAX (50+5)

int T;
int N, M, maxBoxNum;

int MAP[MAX][MAX];

struct BOX
{
	int k; // 택배 번호
	int h; // 세로크기
	int w; // 가로크기
	int r; // 행
	int c; // 열
	bool drop; // 하차 여부
};

BOX box[100 + 10];

void input()
{
	maxBoxNum = 0;

	scanf("%d %d", &N, &M);

	for (int i = 1; i <= 100; i++)
		box[i].k = 0;

	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = -1;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] = 0;
}

void printMap() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void setBox(int index)
{
	BOX b = box[index];

	for (int r = b.r; r < b.r + b.h; r++)
		for (int c = b.c; c < b.c + b.w; c++)
			MAP[r][c] = index;
}

void deleteBox(int index)
{
	BOX b = box[index];
	for (int r = b.r; r < b.r + b.h; r++)
		for (int c = b.c; c < b.c + b.w; c++)
			MAP[r][c] = 0;
}

bool checkDown(int index)
{
	BOX b = box[index];

	for (int c = b.c; c < b.c + b.w; c++)
		if (MAP[b.r + b.h][c] != 0)return false;
		// 바로 아래 줄이 전부 비어 있어야 한 칸 내려갈 수 있다.

	return true;
}

bool checkLeft(int index)
{
	BOX b = box[index];

	for (int r = b.r; r < b.r + b.h; r++)
		for (int c = 1; c < b.c; c++)
			if (MAP[r][c] != 0) return false;

	return true;
}

bool checkRight(int index)
{
	BOX b = box[index];

	for (int r = b.r; r < b.r + b.h; r++)
		for (int c = b.c + b.w; c <= N; c++)
			if (MAP[r][c] != 0) return false;
	
	return true;
}

void moveLeft()
{
	for (int i = 1; i <= maxBoxNum; i++)
	{
		if(box[i].drop == true || box[i].k == 0)continue;

		if (checkLeft(i) == true)
		{
			box[i].drop = true;
			deleteBox(i);

			printf("%d\n", i);

			return;
		}
	}
}

void moveRight()
{
	for (int i = 1; i <= maxBoxNum; i++)
	{
		if (box[i].drop == true || box[i].k == 0)continue;

		if (checkRight(i) == true)
		{
			box[i].drop = true;
			deleteBox(i);

			printf("%d\n", i);

			return;
		}
	}
}


void moveDown(int index)
{
	BOX b = box[index];

	deleteBox(index);
	// 자기 몸에 막히지 않도록 먼저 지도에서 지운다.

	while (1)
	{
		if (checkDown(index) == false) break;

		b.r += 1; // 임시 변수 수정
		box[index].r += 1; // 실제값 수정
	}
	setBox(index);
}

void moveDownAll()
{
	while (1)
	{
		bool check = true;
		for (int i = 1; i <= maxBoxNum; i++)
		{
			if (box[i].drop == true || box[i].k == 0) continue;
			if (checkDown(i) == false) continue;

			check = false;

			moveDown(i);
		}
		if (check == true) break;
		// 한 바퀴 동안 아무것도 안 떨어졌으면 정리가 끝난 것이다.
	}
}

void simulate()
{
	//1. 택배 투입
	for (int m = 0; m < M; m++)
	{
		int k, h, w, c;

		scanf("%d %d %d %d", &k, &h, &w, &c);

		box[k].k = k; // M과 최대 k가 다를 수 있음
		box[k].h = h;
		box[k].w = w;
		box[k].r = 1;
		box[k].c = c;
		box[k].drop = false;

		if (maxBoxNum < k) maxBoxNum = k;
		// 번호가 띄엄띄엄 올 수 있어 가장 큰 번호까지 돌기 위해 기억해 둔다.

		moveDown(k);
	}

	// printMap();

	for (int m = 0; m < M; m+=2)
	{
		// 2. 택배 하차 (좌측)
		moveLeft();
		moveDownAll();

		// 3. 택배 하차 (우측)
		moveRight();
		moveDownAll();
	}
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		simulate();
	}
	return 0;
}
