#include <stdio.h>	

#define MAX (100+20)

int T;

int N, K;

struct MOVING
{
	int index; //for debug
	int peopleNumber;
	int stability;
};

MOVING moving[MAX * 2];

// position[i] = i 번째 사람이 현재 있는 belt의 번호
int position[200000 + 5000];
int livePeople[MAX]; // 현재 무빙워크에 있는 사람들의 번호
int lcnt; // 현재 무빙워크에 있는 사람들의 수 


void input()
{
	scanf("%d %d", &N, &K);

	for (int i = 1; i <= 2 * N; i++)
	{
		scanf("%d", &moving[i].stability);
		moving[i].index = i; // for debug
	}
}

void printMoving() // for debug
{
	for (int i = 1; i <= N; i++)
		printf("(%d, %d, %d) ", moving[i].index, moving[i].stability, moving[i].peopleNumber);
	putchar('\n');

	for (int i = 2 * N; i > N; i--)
		printf("(%d, %d, %d)", moving[i].index, moving[i].stability, moving[i].peopleNumber);
	putchar('\n'); putchar('\n');
}

void rotate()
{
	MOVING tmp = moving[2 * N];
	for (int i = 2 * N - 1; i >= 1; i--) moving[i + 1] = moving[i];
	moving[1] = tmp;
}

int simulate()
{
	int step, crush, pIndex;

	step = crush = 0;
	pIndex = 1; // 1번 사람부터 시작
	while (1)
	{
		step++;

		// 1-1. 무빙 워크 회전
		rotate();

		// 1-2. 무빙 워크 회전에 의한 사람 이동
		for (int i = 0; i < lcnt; i++)
		{
			int peopleNumber = livePeople[i];
			position[peopleNumber] = (position[peopleNumber] + 1);
		}

		// 2. 무빙 워크 내에서 사람을 이동
		int nextlcnt = 0;
		for (int i = 0; i < lcnt; i++)
		{
			int peopleNumber = livePeople[i];
			int curPos = position[peopleNumber];

			if (curPos == N)
			{
				position[peopleNumber] = -1;
				moving[curPos].peopleNumber = 0;

				continue;
			}

			int nextPos = curPos + 1;

			// 다음 칸에 사람이 있거나, 안정성이 0인 경우
			if (moving[nextPos].peopleNumber != 0 || moving[nextPos].stability == 0)
			{
				livePeople[nextlcnt++] = peopleNumber;
				continue;
			}

			moving[curPos].peopleNumber = 0;
			moving[nextPos].peopleNumber = peopleNumber;

			moving[nextPos].stability--;
			if (moving[nextPos].stability == 0) crush++;

			position[peopleNumber] = nextPos;

			if (nextPos == N)
			{
				position[peopleNumber] = -1;
				moving[nextPos].peopleNumber = 0;
			}
			else
			{
				livePeople[nextlcnt++] = peopleNumber;
			}

		}
		// 3. 사람추가
		if (moving[1].peopleNumber == 0 && moving[1].stability != 0)
		{
			position[pIndex] = 1;
			livePeople[nextlcnt++] = pIndex;

			moving[1].peopleNumber = pIndex++;

			moving[1].stability--;
			if (moving[1].stability == 0)	crush++;
		}

		lcnt = nextlcnt;

		if (crush >= K) return step;
	}

	return -1; // for debug;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("%d\n", simulate());
	}
	return 0;
}