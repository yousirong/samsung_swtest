#include <stdio.h>

#define MAX (100+10)

#define FILP_UPDOWN (1)
#define FILP_LEFTRIGHT (2)
#define CLOCKWISE (3)
#define COUNTER_CLOCKWISE (4)
#define SPILT_CLOCKWISE (5)
#define SPILT_COUNTERWISE (6)

int N, M, R;
int MAP[MAX][MAX];
int temp[MAX][MAX];


void input()
{
	scanf("%d %d %d", &N, &M, &R);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			scanf("%d", &MAP[r][c]);
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			copy[r][c] = original[r][c];
}

void printMap()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= M; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
}

void filp_UpDown()
{
	copyMap(temp, MAP);
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			MAP[r][c] = temp[N + 1 - r][c];

}
void filp_LeftRight()
{
	copyMap(temp, MAP);
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			MAP[r][c] = temp[r][M+1-c];
}
void Clockwise()
{
	// MAP[1][4] = temp[1][1]
	// MAP[1][6] = temp[1][6]
	// MAP[6][1] = temp[4][6]
	// MAP[1][1] = temp[4][1]
	copyMap(temp, MAP);
	int t = N;
	N = M;
	M = t;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			MAP[r][c] = temp[M + 1 - c][r];

}
void counter_Clockwise()
{
	// MAP[1][1] = temp[1][N]
	// MAP[1][M] = temp[M][N]
	// MAP[N][M] = temp[M][1]
	// MAP[N][1] = temp[1][1]
	copyMap(temp, MAP);
	int t = N;
	N = M;
	M = t;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= M; c++)
			MAP[r][c] = temp[c][N + 1 - r];
}
void split_Clockwise()
{
	copyMap(temp, MAP);

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;
	int halfN, halfM;

	halfN = (N / 2);
	halfM = (M / 2);

	sr1 = sc1 = 1;
	sr2 = 1; sc2 = halfM + 1;
	sr3 = halfN + 1; sc3 = halfM + 1;
	sr4 = halfN + 1; sc4 = 1;

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr2 + r][sc2 + c] = temp[sr1 + r][sc1 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr3 + r][sc3 + c] = temp[sr2 + r][sc2 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr4 + r][sc4 + c] = temp[sr3 + r][sc3 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr1 + r][sc1 + c] = temp[sr4 + r][sc4 + c];
}
void split_counterClockwise()
{
	copyMap(temp, MAP);

	int sr1, sc1, sr2, sc2, sr3, sc3, sr4, sc4;
	int halfN, halfM;

	halfN = (N / 2);
	halfM = (M / 2);

	sr1 = sc1 = 1;
	sr2 = 1; sc2 = halfM + 1;
	sr3 = halfN + 1; sc3 = halfM + 1;
	sr4 = halfN + 1; sc4 = 1;

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr1 + r][sc1 + c] = temp[sr2 + r][sc2 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr2 + r][sc2 + c] = temp[sr3 + r][sc3 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr3 + r][sc3 + c] = temp[sr4 + r][sc4 + c];

	for (int r = 0; r < halfN; r++)
		for (int c = 0; c < halfM; c++)
			MAP[sr4 + r][sc4 + c] = temp[sr1 + r][sc1 + c];
}

int main()
{
	input();

	for (int r = 0; r < R; r++)
	{
		int COMMAND;

		scanf("%d", &COMMAND);

		if (COMMAND == FILP_UPDOWN) filp_UpDown();
		else if (COMMAND == FILP_LEFTRIGHT) filp_LeftRight();
		else if (COMMAND == CLOCKWISE) Clockwise();
		else if (COMMAND == COUNTER_CLOCKWISE) counter_Clockwise();
		else if (COMMAND == SPILT_CLOCKWISE) split_Clockwise();
		else if (COMMAND == SPILT_COUNTERWISE) split_counterClockwise();
	}

	printMap();

	return 0;

}