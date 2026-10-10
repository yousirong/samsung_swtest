/*
	[SWEA] 2383 - [모의 SW 역량테스트] 점심 식사시간
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5-BEE6AK0DFAVl

	■ 문제 요약
	  N x N 방(4 <= N <= 10)에 사람(1)이 1 ~ 10명, 계단(2 이상, 값 = 계단 길이 K)이 2개 있다.
	  사람마다 두 계단 중 하나를 골라 내려간다.
	    - 계단 입구까지는 맨해튼 거리만큼 걸린다
	    - 입구에 도착하고 1분 뒤부터 내려갈 수 있고, 내려가는 데 K분이 걸린다
	    - 계단 위에는 동시에 3명까지만 있을 수 있다. 꽉 차 있으면 입구에서 기다린다
	  모든 사람이 다 내려가는 시간의 최솟값을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 계단 배정을 DFS로 전부 해 보고, 계단마다 시뮬레이션
	  1) DFS로 사람마다 계단 0 / 1을 고른다. 최대 2^10 = 1,024가지.
	  2) 계단별로 그 계단을 고른 사람만 모아 calculate()로 다 내려가는 시각을 구한다.
	  3) 두 계단 중 늦은 쪽이 그 배정의 완료 시각. 그 최솟값이 답.

	■ 계단 하나 시뮬레이션 (calculate)
	  - 그 계단까지의 거리를 오름차순으로 정렬한다 (먼저 도착한 사람이 먼저 내려간다).
	  - 1분마다 모든 거리를 1씩 줄이고, 0이 된(입구에 도착한) 사람을 대기열 downStair에 넣는다.
	    값은 "남은 계단 칸 수" = 계단 길이.
	  - 대기열 앞의 3명(rp ~ rp + 2)만 계단 위에 있다고 보고 1씩 줄인다.
	  - 0이 된 사람은 다 내려간 것 -> rp를 민다. rp == count면 끝.
	  - 도착한 분에 바로 한 칸 내려가는 것처럼 세고, 마지막에 + 1 해서
	    "도착 1분 뒤 출발 + K분" 규칙을 맞춘다.

	■ 주의할 점
	  - 계단 번호는 1, 2 (입력 순서대로 scnt = 1부터). DFS의 list[] 값 0 / 1은 계단 1 / 2를 뜻한다.
	  - 대기열 앞 3명을 줄이는 반복문의 종료 조건이 i > wp 라서 i == wp 일 때 비어 있는 칸
	    downStair[wp]도 1 줄인다. 이 칸은 다음 사람이 들어올 때 계단 길이로 덮어쓰므로 결과는 맞지만,
	    i >= wp 로 쓰는 것이 의도에 맞다. (downStair는 지역 배열이라 처음엔 쓰레기 값이다)
	  - 아무도 고르지 않은 계단(count == 0)은 3을 돌려준다. 다른 계단은 최소
	    거리 1 + 대기 1 + 계단 2 = 4 이상이라 max에서 항상 밀려 결과에 영향이 없다.
	  - abs()를 직접 만들어 쓴다 (stdlib.h를 include하지 않았다).
*/
#include <stdio.h>

#define MAX (10+5)

int T;
int N;
int MAP[MAX][MAX];
int list[MAX];              // list[p] : p번 사람이 고른 계단 (0 = 1번 계단, 1 = 2번 계단)

struct RC
{
	int r;
	int c;
};

RC people[MAX];             // 사람 좌표
int pcnt;

RC stair[2 + 3];            // stair[1], stair[2] : 계단 좌표
int stairLength[2 + 3];     // 계단 길이
int distance[2 + 3][MAX];   // distance[s][p] : p번 사람에서 s번 계단 입구까지 거리

int abs(int x)
{
	return x > 0 ? x : -x;
}

int getLength(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2);
}

void input()
{
	pcnt = 0;

	scanf("%d", &N);

	int scnt = 1; // 1번 계단 부터시작
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			int tmp;
			scanf("%d", &tmp);

			MAP[r][c] = tmp;
			if (tmp == 1)        // 사람
			{
				people[pcnt].r = r;
				people[pcnt++].c = c;
			}
			else if (tmp >= 2)   // 계단 (값 = 길이)
			{
				stair[scnt].r = r;
				stair[scnt].c = c;
				stairLength[scnt++] = tmp;
			}
		}
	}

	// 사람마다 두 계단까지의 거리를 미리 구해 둔다
	for (int s = 1; s <= 2; s++)
		for (int p = 0; p < pcnt; p++)
			distance[s][p] = getLength(people[p].r, people[p].c, stair[s].r, stair[s].c);
}

// stairNumber 계단을 고른 사람 count명(people[]에 번호)이 다 내려가는 시각
int calculate(int people[MAX], int count, int stairNumber)
{
	int time;
	int distanceArr[11] = { 0 };

	// 거리 저장
	for (int i = 0; i < count; i++)
		distanceArr[i] = distance[stairNumber][people[i]];

	// 거리 오름차순 정렬 (먼저 도착한 사람이 대기열 앞에 서도록)
	for (int i = 0; i < count - 1; i++)
	{
		for (int k = i + 1; k < count; k++)
		{
			if (distanceArr[i] > distanceArr[k])
			{
				int tmp = distanceArr[i];
				distanceArr[i] = distanceArr[k];
				distanceArr[k] = tmp;
			}
		}
	}

	int downStair[MAX]; // queue  (값 = 남은 계단 칸 수)
	int wp, rp;         // rp : 아직 다 못 내려간 첫 사람, wp : 다음에 넣을 자리

	time = 1;
	rp = wp = 0;
	while (1)
	{
		time++;

		// 1분 지남 : 모두 입구에 한 걸음 가까워진다
		for (int i = 0; i < count; i++) distanceArr[i] -= 1;

		// 입구에 막 도착한 사람은 대기열 뒤에 선다
		for (int i = 0; i < count; i++)
		{
			if (distanceArr[i] == 0)
				downStair[wp++] = stairLength[stairNumber];
		}

		// 대기열 앞 3명만 계단 위에서 한 칸 내려간다
		// (i > wp 이라 i == wp 인 빈 칸도 줄이지만, 그 칸은 나중에 덮어써진다)
		for (int i = rp; i < rp + 3; i++)
		{
			if (i > wp) break;

			downStair[i] -= 1;
		}

		// 다 내려간 사람(0)만큼 rp를 민다. 먼저 들어간 사람이 먼저 끝나므로 0은 앞쪽에 모인다
		for (int i = rp; i < wp; i++)
			if (downStair[i] == 0) rp++;

		if (rp == count) break;   // 이 계단을 고른 사람이 전부 내려갔다
	}

	return time + 1;   // "도착 1분 뒤에 출발" 규칙만큼 1분을 더한다
}

// 지금 배정(list[])으로 모두 내려가는 시각 = 두 계단 중 늦은 쪽
int simulate()
{
	int peopleA[MAX] = { 0 };
	int peopleB[MAX] = { 0 };
	int acnt, bcnt, timeA, timeB;

	acnt = bcnt = 0;

	for (int i = 0; i < pcnt; i++)
	{
		if (list[i] == 0) peopleA[acnt++] = i;
		else peopleB[bcnt++] = i;
	}

	timeA = calculate(peopleA, acnt, 1);
	timeB = calculate(peopleB, bcnt, 2);

	return (timeA > timeB) ? timeA : timeB;
}

int MINANS = 0x7fff0000;
// L번 사람의 계단을 고른다
void DFS(int L)
{
	if (L == pcnt)   // 모두 골랐다 -> 시뮬레이션
	{
		int tmp = simulate();
		if (tmp < MINANS) MINANS = tmp;

		return;
	}

	list[L] = 0;     // 1번 계단
	DFS(L + 1);

	list[L] = 1;     // 2번 계단
	DFS(L + 1);
}

int main()
{
	scanf("%d", &T);

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		MINANS = 0x7fff0000;

		DFS(0);

		printf("#%d %d\n", tc, MINANS);
	}
	return 0;
}
