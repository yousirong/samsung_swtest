/*
	[SWEA] 4013 - [모의 SW 역량테스트] 특이한 자석
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWIeV9sKkcoDFAVH

	■ 문제 요약
	  톱니 8개짜리 자석 4개가 일렬로 붙어 있다. 톱니마다 N극(0) 또는 S극(1).
	  K번(1 ~ 20) 자석 하나를 시계(1) / 반시계(-1) 방향으로 1칸 돌린다.
	  돌리기 전 상태에서 맞닿은 톱니의 극이 서로 다르면, 옆 자석은 반대 방향으로 1칸 돈다 (연쇄).
	  다 돌린 뒤 빨간 화살표 위치(1번 톱니)가 S극이면 1번 자석 1점, 2번 2점, 3번 4점, 4번 8점.
	  점수 합을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : "누가 어느 방향으로 돌지"를 먼저 다 정하고, 그다음 한꺼번에 돌린다
	  1) 명령받은 자석의 방향을 list[]에 적고 DFS로 양옆에 퍼뜨린다.
	     옆 자석과 맞닿은 극이 다르고 아직 안 정해졌으면 반대 방향을 적고 그쪽으로 계속 DFS.
	  2) list[]가 0이 아닌 자석만 rotate로 돌린다.
	  3) list[]를 비운다.
	  극 비교는 "돌리기 전" 상태로 해야 하므로, 돌리면서 퍼뜨리면 안 된다. 그래서 1)과 2)를 나눴다.

	■ 톱니 번호 (1번 = 빨간 화살표, 시계 방향으로 2, 3, ...)
	  오른쪽 자석과 맞닿는 톱니 = 3번,  왼쪽 자석과 맞닿는 톱니 = 7번.
	  check(compare, number) : number의 이웃 compare와 맞닿은 극이 다르면 1
	    - 왼쪽 이웃(compare < number) : number의 7번 vs compare의 3번
	    - 오른쪽 이웃(compare > number) : number의 3번 vs compare의 7번

	■ 회전 (rotate)
	  시계(1)   : 모든 톱니가 한 칸 뒤로 밀린다. 8번 톱니가 1번 자리로 온다.
	  반시계(-1) : 한 칸 앞으로 당겨진다. 1번 톱니가 8번 자리로 간다.

	■ 주의할 점
	  - list[number - 1], list[number + 1]은 check()가 범위(1 ~ 4)를 먼저 걸러 준 뒤에만 읽는다 (&& 순서).
	  - 명령받은 자석은 DFS 전에 list에 먼저 적어서, 퍼진 쪽에서 다시 되돌아오지 않게 한다.
	  - 전역 배열 number[]와 같은 이름을 함수 매개변수 / input()의 반복 변수로도 쓴다.
	    함수 안에서는 지역 이름이 전역을 가린다. 동작은 맞지만 읽을 때 헷갈리기 쉽다.
	  - printStatus()는 디버그 출력용이다 (main()에서 주석 처리).
*/
#include <stdio.h>

#define MAX (100+20)

int T;
int wheel[5][10]; /* wheel[번호 1~4][각 톱니 1~8] */
int list[6];      // list[k] : 이번 명령에서 k번 자석이 돌 방향 (0 = 안 돎)
int number[MAX];  // 명령마다 돌릴 자석 번호
int direct[MAX];  // 명령마다 방향 (1 시계, -1 반시계)
int N;            // 명령 수 (문제의 K)

void input()
{
	scanf("%d", &N);
	for (int number = 1; number <= 4; number++)
	{
		for (int index = 1; index <= 8; index++)
		{
			scanf("%d", &wheel[number][index]);
		}
	}

	for (int i = 0; i < N; i++)
		scanf("%d %d", &number[i], &direct[i]);

	return;
}

void printStatus(int step, const char* when)   // for debug
{
	printf("=== %d번째 명령 %s ===\n", step + 1, when);

	for (int k = 1; k <= 4; k++)
	{
		printf("자석%d (회전 %2d) : ", k, list[k]);
		for (int i = 1; i <= 8; i++)
		{
			if (i == 1) printf("[%d]", wheel[k][i]);                 // 화살표 위치
			else if (i == 3 || i == 7) printf("<%d>", wheel[k][i]);  // 맞닿는 날
			else printf(" %d ", wheel[k][i]);
		}
		putchar('\n');
	}

	for (int k = 1; k <= 3; k++)
		printf("  %d-%d 맞닿은 극: %d vs %d -> %s\n",
			k, k + 1, wheel[k][3], wheel[k + 1][7],
			wheel[k][3] != wheel[k + 1][7] ? "다름(전파)" : "같음");

	putchar('\n');
}

// number번 자석을 dir 방향으로 한 칸 돌린다
void rotate(int number, int dir)
{
	int temp;

	if (dir == -1)   // 반시계 : 1번 톱니가 8번 자리로
	{
		temp = wheel[number][1];
		for (int index = 1; index <= 7; index++)
		{
			wheel[number][index] = wheel[number][index + 1];
		}
		wheel[number][8] = temp;
	}
	else             // 시계 : 8번 톱니가 1번 자리로
	{
		temp = wheel[number][8];
		for (int index = 8; index >= 2; index--)
		{
			wheel[number][index] = wheel[number][index - 1];
		}
		wheel[number][1] = temp;
	}
	return;
}

// number와 그 이웃 compare가 맞닿은 극이 다르면 1 (compare가 범위 밖이면 0)
int check(int compare, int number)
{
	if (compare < 1 || compare>4) return 0;
	if (number < 1 || number>4) return 0;

	if (compare < number && wheel[number][7] != wheel[compare][3]) return 1;   // 왼쪽 이웃
	if (compare > number && wheel[number][3] != wheel[compare][7]) return 1;   // 오른쪽 이웃

	return 0;
}

// number번이 dir 방향으로 돈다고 정해졌다 -> 양옆으로 퍼뜨린다
void DFS(int number, int dir)
{
	//printf("  DFS(%d, %d)\n", number, dir);   // for debug

	// number를 기준으로 왼쪽 바퀴가 움직여야 하는지 체크
	if (check(number - 1, number) == 1 && list[number - 1] == 0)
	{
		list[number - 1] = dir * (-1);
		DFS(number - 1, dir * (-1));
	}

	// number를 기준으로 오른쪽 바퀴가 움직여야 하는지 체크
	if (check(number + 1, number) == 1 && list[number + 1] == 0)
	{
		list[number + 1] = dir * (-1);
		DFS(number + 1, dir * (-1));
	}

	return;
}

// 화살표 위치(1번 톱니)가 S극(1)인 자석마다 1, 2, 4, 8점
int calculate()
{
	int sum, mul;

	sum = 0;
	mul = 1;
	for (int i = 1; i <= 4; i++)
	{
		sum += mul * wheel[i][1];
		mul *= 2;
	}

	return sum;
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		int ans;

		input();

		for (int i = 0; i < N; i++)
		{
			// 1) 누가 어느 방향으로 돌지 먼저 다 정한다 (돌리기 전 상태로 비교)
			list[number[i]] = direct[i];
			DFS(number[i], direct[i]);
			//printStatus(i, "회전 전");   // 어떤 자석이 어느 방향으로 돌기로 했는지 확인

			// 2) 정해진 자석만 한꺼번에 돌린다
			for (int k = 1; k <= 4; k++)
				if (list[k] != 0) rotate(k, list[k]);
			//printStatus(i, "회전 후");   // 실제 회전 결과 확인

			// 3) 다음 명령을 위해 비운다
			for (int k = 1; k <= 4; k++) list[k] = 0;
		}

		ans = calculate();

		printf("#%d %d\n", tc, ans);
	}
	return 0;
}