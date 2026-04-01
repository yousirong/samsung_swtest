// 코드트리 2018 하반기 오후 1번 - 바이러스 실험
// https://www.codetree.ai/training-field/frequent-problems/problems/virus-experiment
// 문제 요약
// N x N 격자에서 K년 동안 바이러스 실험을 진행한다.
//
// 각 칸에는
// - 현재 양분(food)
// - 겨울마다 추가되는 양분(MAP)
// - 여러 개의 바이러스(각각 나이 존재)
// 가 있다.
//
// 진행 규칙 (1년)
// 1. 봄:
//    어린 바이러스부터 자기 나이만큼 양분을 먹고 나이가 1 증가한다.
//    양분이 부족하면 그 바이러스는 죽는다.
// 2. 여름:
//    봄에 죽은 바이러스는 나이 / 2 만큼 양분으로 변한다.
// 3. 가을:
//    나이가 5의 배수인 바이러스는 인접 8칸에 나이 1짜리 바이러스를 번식한다.
// 4. 겨울:
//    각 칸에 MAP[r][c]만큼 양분이 추가된다.
//
// 목표
// K년 후 살아 있는 바이러스 총 수를 구하는 문제
//
// 핵심 아이디어
// - 각 칸마다 바이러스 나이들을 "deque처럼" 관리한다.
// - 어린 바이러스부터 처리해야 하므로 앞(front)에서 순서대로 봄 처리
// - 번식으로 생기는 나이 1 바이러스는 가장 어린 바이러스이므로
//   deque의 앞(front) 쪽에 넣는다.
// - 이 코드에서는 3차원 배열 + front/back 포인터로
//   각 칸의 바이러스들을 deque처럼 구현했다.

#include <stdio.h>

#define MAX (10 + 5)
#define MAX_VIRUS (100000)
#define OFFSET (10000)

int T;

int N, M, K;
int food[MAX][MAX];   // 현재 칸의 양분
int MAP[MAX][MAX];    // 겨울마다 추가되는 양분

// virus[r][c][...] : (r,c) 칸에 있는 바이러스 나이들을 저장하는 배열
// front[r][c] ~ back[r][c]-1 구간이 실제 데이터
int virus[MAX][MAX][MAX_VIRUS];
int front[MAX][MAX];
int back[MAX][MAX];

// 8방향: 북, 북동, 동, 남동, 남, 남서, 서, 북서
int dr[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
int dc[] = { 0,  1, 1, 1, 0,-1,-1, -1 };

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d %d", &N, &M, &K);

	// 겨울에 추가될 양분 입력
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			scanf("%d", &MAP[r][c]);
		}
	}

	// 초기 양분은 모든 칸 5
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			food[r][c] = 5;
		}
	}

	// 각 칸의 deque 시작 위치를 OFFSET으로 맞춰 둔다.
	// 앞/뒤 양쪽 삽입이 가능하도록 중간에서 시작
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			front[r][c] = back[r][c] = OFFSET;
		}
	}

	// 초기 바이러스 입력
	for (int i = 0; i < M; i++)
	{
		int r, c, age;
		scanf("%d %d %d", &r, &c, &age);

		// 뒤쪽(back)에 순서대로 삽입
		int& virusCount = back[r][c];
		virus[r][c][virusCount++] = age;
	}
}

// ---------------------------
// 디버그용: 각 칸의 바이러스 출력
// ---------------------------
void printVirus()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if ((back[r][c] - front[r][c]) == 0)
				continue;

			printf("%d, %d : ", r, c);
			for (int t = front[r][c]; t < back[r][c]; t++)
				printf("%d ", virus[r][c][t]);
			putchar('\n');
		}
	}
	putchar('\n');
}

// --------------------------------------------------
// step1_2() : 봄 + 여름 처리
// --------------------------------------------------
//
// 봄:
// - 어린 바이러스부터 양분을 먹는다.
// - 자신의 나이만큼 양분을 먹을 수 있으면 살아남고 나이 +1
// - 먹지 못하면 즉시 죽음
//
// 여름:
// - 죽은 바이러스는 나이/2 만큼 양분이 된다.
//
// 구현 방식:
// - 현재 deque 구간 [start, end)를 순회
// - 살아남은 바이러스는 뒤쪽(back)에 다시 넣는다.
// - 죽은 바이러스는 양분만 추가하고 버린다.
// - front를 앞으로 밀어서 "기존 데이터는 소비"한 것으로 처리
void step1_2()
{
	// 각 칸을 독립적으로 처리
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			int start = front[r][c];
			int end = back[r][c];

			int& fr = front[r][c];
			int& bk = back[r][c];

			int t;

			// ---------------------------
			// 봄: 살아남는 바이러스 처리
			// ---------------------------
			for (t = start; t < end; t++)
			{
				// 현재 바이러스가 자기 나이만큼 양분을 먹을 수 있으면
				if (virus[r][c][t] <= food[r][c])
				{
					// 양분 소비
					food[r][c] -= virus[r][c][t];

					// 나이 1 증가
					virus[r][c][t]++;

					// 기존 front 쪽 데이터는 처리 완료
					fr++;

					// 살아남은 바이러스는 뒤쪽에 다시 저장
					virus[r][c][bk++] = virus[r][c][t];
				}
				// 양분 부족하면 이 바이러스부터 뒤는 전부 죽음
				else
				{
					break;
				}
			}

			// ---------------------------
			// 여름: 죽은 바이러스 양분 환원
			// ---------------------------
			for (; t < end; t++)
			{
				food[r][c] += (virus[r][c][t] / 2);
				fr++;
			}
		}
	}
}

// --------------------------------------------------
// step3() : 가을 처리
// --------------------------------------------------
//
// 나이가 5의 배수인 바이러스는
// 인접 8칸에 나이 1짜리 바이러스를 번식시킨다.
//
// 나이 1 바이러스는 가장 어린 바이러스이므로
// deque의 앞(front) 쪽에 삽입한다.
void step3()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			int start = front[r][c];
			int end = back[r][c];

			// 현재 칸의 살아 있는 바이러스들 확인
			for (int t = start; t < end; t++)
			{
				// 나이가 5의 배수가 아니면 번식 안 함
				if (virus[r][c][t] % 5 != 0)
					continue;

				// 8방향으로 번식
				for (int i = 0; i < 8; i++)
				{
					int nr, nc;

					nr = r + dr[i];
					nc = c + dc[i];

					// 범위 밖이면 무시
					if (nr < 1 || nc < 1 || nr > N || nc > N)
						continue;

					// front를 하나 줄이면서 앞쪽에 나이 1 삽입
					int& fr = front[nr][nc];
					virus[nr][nc][--fr] = 1;
				}
			}
		}
	}
}

// --------------------------------------------------
// step4() : 겨울 처리
// --------------------------------------------------
//
// 각 칸에 MAP[r][c] 만큼 양분 추가
void step4()
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			food[r][c] += MAP[r][c];
		}
	}
}

// ---------------------------
// 현재 살아 있는 바이러스 총 수 계산
// 각 칸의 개수 = back - front
// ---------------------------
int getAnswer()
{
	int sum = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			sum += (back[r][c] - front[r][c]);
		}
	}

	return sum;
}

int main()
{
	// 원래 테스트케이스 형식일 수도 있지만
	// 현재 코드는 1회만 수행
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// K년 동안 시뮬레이션
		for (int time = 0; time < K; time++)
		{
			step1_2(); // 봄 + 여름
			step3();   // 가을
			step4();   // 겨울
		}

		printf("%d\n", getAnswer());
	}

	// 필요한 메모리 확인용 디버그 흔적
	/*
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			printf("%d %d / %d %d\n", r, c, front[r][c], back[r][c]);
	*/

	return 0;
}