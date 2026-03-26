// BOJ 10828 - 스택
//
// 문제 요약
// 스택(stack)을 직접 구현하고,
// 주어진 명령(push, pop, size, empty, top)을 처리하는 문제
//
// 핵심 아이디어
// - 배열을 이용해서 스택을 구현한다.
// - sp(stack pointer)를 "다음에 값이 들어갈 위치"로 둔다.
// - 따라서 현재 스택의 맨 위(top)는 stack[sp - 1]이다.
//
// 예)
// sp = 0 → 비어 있음
// push(10) → stack[0] = 10, sp = 1
// push(20) → stack[1] = 20, sp = 2
// 현재 top → stack[1] = 20

#include <stdio.h>

#define MAX (10000 + 500)

int N;            // 명령의 개수

int stack[MAX];   // 배열로 구현한 스택
int sp;           // stack pointer (다음에 들어갈 위치)

// ---------------------------
// 문자열 비교 함수
// strcmp를 직접 구현한 것
//
// 두 문자열이 같으면 0 반환
// 다르면 첫 번째 다른 문자 차이를 반환
// ---------------------------
int strCompare(const char* a, const char* b)
{
	// 두 문자열이 같을 때까지 계속 비교
	while (*a && *a == *b)
	{
		++a;
		++b;
	}

	// 다른 문자가 나오면 그 차이를 반환
	return *a - *b;
}

// ---------------------------
// 디버그용: 현재 스택 상태 출력
// ---------------------------
void printStack()
{
	for (int i = sp - 1; i >= 0; i--)
		printf("%d ", stack[i]);
	putchar('\n');
}

// ---------------------------
// 메인 함수
// ---------------------------
int main()
{
	sp = 0;   // 처음에는 스택이 비어 있음

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		// 명령어 입력
		scanf("%s", command);

		// ---------------------------
		// push X
		// 스택 맨 위에 X 추가
		// ---------------------------
		if (strCompare(command, "push") == 0)
		{
			int value;
			scanf("%d", &value);

			stack[sp++] = value;
			// stack[sp]에 넣고 sp 증가
		}

		// ---------------------------
		// pop
		// 스택 맨 위 원소 제거 + 출력
		// ---------------------------
		else if (strCompare(command, "pop") == 0)
		{
			// 스택이 비어있지 않으면
			if (sp != 0)
				printf("%d\n", stack[--sp]);
			// sp를 먼저 감소시키고 해당 위치 출력
			else
				printf("-1\n");  // 비어 있으면 -1
		}

		// ---------------------------
		// size
		// 현재 스택에 들어 있는 원소 개수
		// ---------------------------
		else if (strCompare(command, "size") == 0)
		{
			printf("%d\n", sp);
		}

		// ---------------------------
		// empty
		// 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strCompare(command, "empty") == 0)
		{
			printf("%d\n", sp != 0 ? 0 : 1);
		}

		// ---------------------------
		// top
		// 맨 위 원소를 제거하지 않고 출력
		// ---------------------------
		else if (strCompare(command, "top") == 0)
		{
			// 스택이 비어있지 않으면 top 출력
			if (sp != 0)
				printf("%d\n", stack[sp - 1]);
			else
				printf("-1\n");  // 비어 있으면 -1
		}
	}

	return 0;
}