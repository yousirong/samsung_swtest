/*
	[코드트리] 2019 상반기 오후 1번 - 격자 숫자 놀이
	https://www.codetree.ai/training-field/frequent-problems/problems/matrix-number-play

	■ 문제 요약
	  처음 격자는 3 x 3. 1초마다 아래 둘 중 하나를 한다.
	    - 행 개수 >= 열 개수 : 모든 행에 "행 정렬"
	    - 행 개수 <  열 개수 : 모든 열에 "열 정렬"
	  정렬 방법 (한 줄 기준)
	    1) 0을 뺀 숫자마다 나온 횟수를 센다
	    2) 횟수가 적은 순, 같으면 숫자가 작은 순으로 정렬
	    3) (숫자, 횟수) 쌍을 차례로 이어 써서 새 줄을 만든다
	  가장 긴 줄에 맞춰 나머지 줄 뒤는 0으로 채운다. 길이가 100을 넘으면 앞 100개만 남긴다.
	  (r, c) 칸이 k가 되는 최소 시간을 출력한다. 100초가 지나도 안 되면 -1.

	  (백준 17140 "이차원 배열과 연산"과 같은 문제다)

	■ 풀이 방침 : 행 정렬 하나만 구현하고, 열 정렬은 전치(transpose)로 바꿔 쓴다
	  makeArrayRow(row, col) : row행의 1 ~ col열을 정렬해 다시 쓰고, 새 길이를 돌려준다.
	  열 정렬 = 전치 -> 행 정렬 -> 다시 전치. 그래서 열용 함수를 따로 만들 필요가 없다.
	  lenR, lenC = 지금 격자의 행 수, 열 수.

	■ makeArrayRow : 한 줄 정렬
	  1) count[숫자] = 나온 횟수. 처음 나온 숫자만 number[]에 모은다 (서로 다른 숫자 목록, ncnt개)
	  2) grid[] = (숫자, 횟수) 쌍. 횟수 오름차순, 같으면 숫자 오름차순으로 정렬 (ncnt <= 100 이라 O(n^2)도 충분)
	  3) 1열부터 숫자, 횟수, 숫자, 횟수 ... 로 덮어쓴다
	  4) 새 길이(index - 1) 뒤 ~ 원래 길이 col 까지는 0으로 지운다 (원래 줄이 더 길었던 경우)

	■ transpose(row, col) : 행과 열 바꾸기
	  row x col 직사각형을 maxLength x maxLength 정사각형으로 보고 대각선 기준으로 뒤집는다.
	  직사각형 밖은 모두 0이라, 뒤집어도 0끼리 바뀌어 문제가 없다.
	  전치한 뒤에는 lenR, lenC도 서로 바꿔야 한다 (열 정렬 전후로 두 번 바꾼다).

	■ 시간 처리
	  0초에 이미 (r, c) == k 이면 0.
	  1 ~ 100초 동안 연산 후 바로 확인. 100초까지 안 되면 -1.
	  r, c는 격자 밖일 수도 있는데, 밖은 0이고 k >= 1이라 자동으로 "아님"이 된다.

	■ 주의할 점
	  - 0은 숫자로 세지 않는다 (정렬할 때 무시).
	  - 길이 100 초과 자르기는 주석 처리돼 있다. 무작위 입력 1500개로 확인했을 때 줄 길이는 최대 38이었고,
	    정답 시뮬레이션과 결과가 모두 같았다.
	  - lenR, lenC는 커지기만 하고 줄어들지 않는다. 줄어든 부분은 0으로 채워져 있으므로 값은 같다.
	    (행/열 개수 비교가 실제와 달라질 여지는 있지만, 위 무작위 테스트에서는 차이가 없었다)
	  - 새 줄 길이는 2 x (서로 다른 숫자 수)다. 자르기 없이 이 값이 120을 넘으면 MAP 밖에 쓰게 되므로,
	    불안하면 if (index == 101) break; 주석을 풀어 두면 안전하다 (문제 규칙과도 같다).
*/
#include <stdio.h>                       // scanf, printf, putchar

#define MAX (100 + 20)                   // 격자 최대 100 + 여유

int T;                                   // 테스트 케이스 수

int R, C, K;                             // 확인할 칸 (R, C), 목표 값 K
int MAP[MAX][MAX];                       // 격자 (1번부터, 밖은 0)

struct GRID                              // 정렬용 (숫자, 횟수) 쌍
{
	int value;                           // 숫자
	int count;                           // 나온 횟수
};

GRID grid[MAX];                          // 한 줄의 (숫자, 횟수) 목록

void input()
{
	scanf("%d %d %d", &R, &C, &K);       // 확인할 칸, 목표 값

	for (int r = 1; r <= 3; r++)         // 처음 3 x 3 격자
		for (int c = 1; c <= 3; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int row, int col) // for debug
{
	printf("row :%d, col : %d\n", row, col);
	for (int r = 1; r <= row; r++)
	{
		for (int c = 1; c <= col; c++)
			printf("%d ", MAP[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// row행의 1 ~ col열을 정렬해 다시 쓰고, 새 길이를 돌려준다
int makeArrayRow(int row, int col)
{
	int count[MAX] = { 0 };              // count[숫자] = 나온 횟수
	int number[MAX] = { 0 };             // 서로 다른 숫자 목록 (처음 나온 순서)
	int ncnt;                            // 서로 다른 숫자 수

	ncnt = 0;
	for (int c = 1; c <= col; c++)
	{
		if (MAP[row][c] == 0) continue;  // 0은 세지 않는다

		int value = MAP[row][c];
		if (count[value] == 0) number[ncnt++] = value;   // 처음 나온 숫자면 목록에 추가

		count[value]++;                  // 횟수 증가
	}

	for (int i = 0; i < ncnt; i++)       // (숫자, 횟수) 쌍 만들기
	{
		int value = number[i];

		grid[i].value = value;
		grid[i].count = count[value];
	}

	// 횟수 오름차순, 같으면 숫자 오름차순 (뒤 원소가 더 앞서야 하면 교환)
	for (int i = 0; i < ncnt - 1; i++)
	{
		for (int k = i + 1; k < ncnt; k++)
		{
			if ((grid[i].count > grid[k].count)
				|| ((grid[i].count == grid[k].count) && (grid[i].value > grid[k].value)))
			{
				GRID tmp = grid[i];
				grid[i] = grid[k];
				grid[k] = tmp;
			}
		}
	}

	int index = 1;                       // 쓸 열
	for (int i = 0; i < ncnt; i++)       // 숫자, 횟수 순서로 이어 쓴다
	{
		MAP[row][index++] = grid[i].value;
		MAP[row][index++] = grid[i].count;

		// 배열이 100을 넘어가는 경우
		// if (index == 101) break;
	}

	for (int i = index; i <= col; i++) MAP[row][i] = 0;   // 원래 더 길었던 뒷부분은 0으로

	return index - 1;                    // 새 길이
}

// row x col 직사각형을 전치 (정사각형으로 보고 대각선 기준 교환, 밖은 0이라 괜찮다)
void transpose(int row, int col)
{
	int maxLength = row > col ? row : col;   // 큰 쪽 기준 정사각형
	for (int r = 1; r <= maxLength - 1; r++)
	{
		for (int c = r + 1; c <= maxLength; c++)   // 대각선 위쪽만 돌며 교환
		{
			int tmp = MAP[r][c];
			MAP[r][c] = MAP[c][r];
			MAP[c][r] = tmp;
		}
	}
}

// (R, C) == K 가 되는 최소 시간, 100초 안에 안 되면 -1
int simulate()
{
	if (MAP[R][C] == K) return 0;        // 0초에 이미 만족

	int lenR, lenC;                      // 지금 행 수, 열 수

	lenR = lenC = 3;
	for (int i = 1; i <= 100; i++)       // 1 ~ 100초
	{
		if (lenR >= lenC)                // 행 정렬
		{
			for (int r = 1; r <= lenR; r++)
			{
				int length = makeArrayRow(r, lenC);
				if (lenC < length) lenC = length;   // 가장 긴 행에 맞춘다
			}
		}
		else                             // 열 정렬 = 전치 -> 행 정렬 -> 전치
		{
			int tmp;

			transpose(lenR, lenC);       // 열이 행이 된다

			tmp = lenR;                  // 크기도 바꾼다
			lenR = lenC;
			lenC = tmp;

			for (int r = 1; r <= lenR; r++)   // (원래의 열을) 행 정렬
			{
				int length = makeArrayRow(r, lenC);
				if (lenC < length) lenC = length;
			}

			tmp = lenR;                  // 크기 되돌리기
			lenR = lenC;
			lenC = tmp;

			transpose(lenR, lenC);       // 다시 전치해 원래 방향으로
		}

		if (MAP[R][C] == K) return i;    // 연산 후 확인
	}

	return -1;                           // 100초 안에 안 됨
}

int main()
{
	// scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)
	{
		input();                         // 입력

		printf("%d\n", simulate());      // 최소 시간 출력
	}

	return 0;
}
