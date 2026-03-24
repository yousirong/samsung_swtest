// BOJ 10845 / 18258 - 큐 / 큐 2
//
// 문제 요약
// 큐(queue)를 직접 구현하고,
// push / pop / size / empty / front / back 명령을 처리하는 문제
//
// 핵심 아이디어
// - 배열 하나로 큐를 구현한다.
// - 두 개의 포인터를 사용한다:
//     rp (read pointer)  → 현재 맨 앞 원소 위치
//     wp (write pointer) → 다음 원소가 들어갈 위치
//
// 큐의 실제 데이터 범위:
//     queue[rp] ~ queue[wp - 1]
//
// 예)
// 초기: rp = 0, wp = 0 → 비어 있음
//
// push(10): queue[0] = 10, wp = 1
// push(20): queue[1] = 20, wp = 2
//
// 상태:
// rp = 0, wp = 2 → [10, 20]
//
// pop(): queue[rp++] → 10 제거 → rp = 1

#include <stdio.h>

#define MAX (10000 + 500)

int N;            // 명령 개수

int queue[MAX];   // 배열로 구현한 큐
int rp, wp;       // rp: 읽기 위치, wp: 쓰기 위치

// ---------------------------
// 문자열 비교 함수
// strcmp를 직접 구현한 것
// ---------------------------
int strCompare(const char* a, const char* b)
{
	while (*a && *a == *b)
	{
		++a;
		++b;
	}

	return *a - *b;
}

// ---------------------------
// 디버그용: 큐 상태 출력
// ---------------------------
void printQueue()
{
	for (int i = rp; i < wp; i++)
		printf("%d ", queue[i]);
	putchar('\n');
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	// 처음에는 큐가 비어 있음
	rp = wp = 0;

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		// 명령 입력
		scanf("%s", command);

		// ---------------------------
		// push X
		// 맨 뒤에 원소 추가
		// ---------------------------
		if (strCompare(command, "push") == 0)
		{
			int value;

			scanf("%d", &value);

			queue[wp++] = value;
			// 현재 wp 위치에 넣고 wp 증가
		}

		// ---------------------------
		// pop
		// 맨 앞 원소 제거 + 출력
		// ---------------------------
		else if (strCompare(command, "pop") == 0)
		{
			// 큐가 비어 있으면 -1
			if (wp == rp)
				printf("-1\n");
			else
				printf("%d\n", queue[rp++]);
			// queue[rp] 출력 후 rp 증가
		}

		// ---------------------------
		// size
		// 현재 큐에 들어 있는 원소 개수
		// ---------------------------
		else if (strCompare(command, "size") == 0)
		{
			printf("%d\n", wp - rp);
		}

		// ---------------------------
		// empty
		// 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strCompare(command, "empty") == 0)
		{
			printf("%d\n", (wp == rp) ? 1 : 0);
		}

		// ---------------------------
		// front
		// 맨 앞 원소 확인 (제거 X)
		// ---------------------------
		else if (strCompare(command, "front") == 0)
		{
			if (wp == rp)
				printf("-1\n");
			else
				printf("%d\n", queue[rp]);
		}

		// ---------------------------
		// back
		// 맨 뒤 원소 확인 (제거 X)
		// ---------------------------
		else if (strCompare(command, "back") == 0)
		{
			if (wp == rp)
				printf("-1\n");
			else
				printf("%d\n", queue[wp - 1]);
		}
	}

	return 0;
}