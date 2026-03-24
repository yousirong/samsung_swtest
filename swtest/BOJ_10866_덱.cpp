// BOJ 10866 - 덱
//
// 문제 요약
// 덱(deque, double-ended queue)은
// 앞(front)과 뒤(back) 양쪽에서
// 삽입(push)과 삭제(pop)가 가능한 자료구조이다.
//
// 핵심 아이디어
// - 배열 하나로 덱을 구현한다.
// - front와 back 두 포인터를 사용한다.
// - 배열의 "중앙"에서 시작하면
//   push_front / push_back 모두 인덱스 이동만으로 처리 가능하다.
//
// 실제 원소 범위:
//   deque[front] ~ deque[back - 1]
//
// 예)
// front = 1000, back = 1000 → 비어 있음
// push_back(10) → deque[1000] = 10, back = 1001
// push_front(20) → front = 999, deque[999] = 20
// 상태: [20, 10]

#include <stdio.h>

#define MAX (10000 + 500)
#define OFFSET (MAX / 2)   // 중앙 위치

int N;                 // 수행할 명령 개수
int deque[MAX * 2];    // 배열로 구현한 덱
int front, back;       // front: 첫 원소 위치, back: 다음 삽입 위치

// ---------------------------
// 문자열 비교 함수
// strcmp를 직접 구현
// ---------------------------
int strcmp(const char* a, const char* b)
{
	while (*a && *a == *b)
	{
		++a;
		++b;
	}

	return *a - *b;
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	// 덱을 배열 중앙에서 시작
	// 이렇게 하면 양쪽으로 자유롭게 확장 가능
	front = back = OFFSET;

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		scanf("%s", command);

		// ---------------------------
		// push_front X
		// 덱 앞쪽에 삽입
		// ---------------------------
		if (strcmp(command, "push_front") == 0)
		{
			int value;
			scanf("%d", &value);

			// front를 먼저 감소시키고 값 저장
			deque[--front] = value;
		}

		// ---------------------------
		// push_back X
		// 덱 뒤쪽에 삽입
		// ---------------------------
		else if (strcmp(command, "push_back") == 0)
		{
			int value;
			scanf("%d", &value);

			// 현재 back 위치에 넣고 back 증가
			deque[back++] = value;
		}

		// ---------------------------
		// pop_front
		// 덱 앞쪽 원소 제거 + 출력
		// ---------------------------
		else if (strcmp(command, "pop_front") == 0)
		{
			// 비어 있으면 -1
			if (back == front)
				printf("-1\n");
			else
				printf("%d\n", deque[front++]);
			// 출력 후 front 증가
		}

		// ---------------------------
		// pop_back
		// 덱 뒤쪽 원소 제거 + 출력
		// ---------------------------
		else if (strcmp(command, "pop_back") == 0)
		{
			if (back == front)
				printf("-1\n");
			else
				printf("%d\n", deque[--back]);
			// back 감소 후 출력
		}

		// ---------------------------
		// size
		// 현재 덱에 들어 있는 원소 개수
		// ---------------------------
		else if (strcmp(command, "size") == 0)
		{
			printf("%d\n", back - front);
		}

		// ---------------------------
		// empty
		// 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strcmp(command, "empty") == 0)
		{
			printf("%d\n", (back == front) ? 1 : 0);
		}

		// ---------------------------
		// front
		// 맨 앞 원소 확인 (제거 X)
		// ---------------------------
		else if (strcmp(command, "front") == 0)
		{
			if (back == front)
				printf("-1\n");
			else
				printf("%d\n", deque[front]);
		}

		// ---------------------------
		// back
		// 맨 뒤 원소 확인 (제거 X)
		// ---------------------------
		else if (strcmp(command, "back") == 0)
		{
			if (back == front)
				printf("-1\n");
			else
				printf("%d\n", deque[back - 1]);
		}
	}

	return 0;
}