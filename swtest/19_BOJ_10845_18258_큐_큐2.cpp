/*
	[BOJ] 10845 큐 / 18258 큐 2
	https://www.acmicpc.net/problem/10845
	https://www.acmicpc.net/problem/18258

	■ 문제 요약
	  정수를 저장하는 큐를 구현하고 N개의 명령을 처리한다.

	    push X : 정수 X를 큐의 뒤에 넣는다
	    pop    : 맨 앞 정수를 빼고 출력한다. 비어 있으면 -1
	    size   : 큐에 들어 있는 정수의 개수를 출력한다
	    empty  : 비어 있으면 1, 아니면 0
	    front  : 맨 앞 정수를 출력한다(빼지 않음). 비어 있으면 -1
	    back   : 맨 뒤 정수를 출력한다(빼지 않음). 비어 있으면 -1

	  두 문제는 명령이 같고 N의 크기만 다르다. 10845는 N <= 10,000, 18258은 N <= 2,000,000.

	■ 풀이 방침
	  원형 큐를 쓰지 않고, 긴 배열 위에서 포인터 두 개만 앞으로 미는 "선형 큐"로 만든다.

	    rp (read pointer)  : 맨 앞 원소의 위치
	    wp (write pointer) : 다음 원소가 들어갈 위치

	  큐에 실제로 들어 있는 구간은 queue[rp] ~ queue[wp - 1] 이다.

	    비어 있음 : rp == wp
	    push      : queue[wp++] = value
	    pop       : queue[rp++]        (읽고 나서 rp를 올린다)
	    size      : wp - rp
	    front     : queue[rp]
	    back      : queue[wp - 1]

	  pop을 해도 rp만 올라갈 뿐 앞칸을 재사용하지 않기 때문에,
	  배열 크기는 "push 명령의 최대 개수"만큼 필요하다. 대신 매 연산이 O(1)로 아주 단순해진다.

	    rp = 0, wp = 0            -> 비어 있음
	    push(10)                  -> queue[0] = 10, wp = 1
	    push(20)                  -> queue[1] = 20, wp = 2   =>  [10, 20]
	    pop()                     -> 10 출력, rp = 1          =>  [20]

	■ 주의할 점
	  1) 모든 조회/삭제 명령은 비어 있는 경우(rp == wp)를 먼저 걸러야 한다.
	  2) 이 코드의 MAX는 10,500이라 10845(N <= 10,000) 기준이다.
	     18258(N <= 2,000,000)에 그대로 내면 배열이 모자라 터진다.
	     18258을 풀려면 MAX를 2,000,000 이상으로 키우고,
	     출력이 많으므로 입출력 속도(버퍼 출력 등)도 함께 신경 써야 한다.
*/

#include <stdio.h>

#define MAX (10000 + 500)

int N;            // 명령 개수

int queue[MAX];   // 배열로 구현한 큐
int rp, wp;       // rp: 맨 앞 원소 위치, wp: 다음에 넣을 위치

// ---------------------------
// strcmp 직접 구현 (string.h 없이 명령어를 비교하기 위함)
// 같으면 0을 반환한다.
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
// 디버그용: 큐에 남아 있는 구간을 앞에서부터 출력
// ---------------------------
void printQueue()
{
	for (int i = rp; i < wp; i++)
		printf("%d ", queue[i]);
	putchar('\n');
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	// 빈 큐로 시작 (rp == wp 이면 비어 있음)
	rp = wp = 0;

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		scanf("%s", command);

		// ---------------------------
		// push X : 맨 뒤에 추가
		// ---------------------------
		if (strCompare(command, "push") == 0)
		{
			int value;

			scanf("%d", &value);

			// 현재 wp 자리에 넣고 wp를 한 칸 앞으로
			queue[wp++] = value;
		}

		// ---------------------------
		// pop : 맨 앞 원소를 빼서 출력
		// ---------------------------
		else if (strCompare(command, "pop") == 0)
		{
			if (wp == rp)
				printf("-1\n");   // 비어 있음
			else
				// queue[rp]를 출력한 뒤 rp를 한 칸 앞으로 민다
				printf("%d\n", queue[rp++]);
		}

		// ---------------------------
		// size : 원소 개수 = wp - rp
		// ---------------------------
		else if (strCompare(command, "size") == 0)
		{
			printf("%d\n", wp - rp);
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strCompare(command, "empty") == 0)
		{
			printf("%d\n", (wp == rp) ? 1 : 0);
		}

		// ---------------------------
		// front : 맨 앞 원소 (제거하지 않음)
		// ---------------------------
		else if (strCompare(command, "front") == 0)
		{
			if (wp == rp)
				printf("-1\n");
			else
				printf("%d\n", queue[rp]);
		}

		// ---------------------------
		// back : 맨 뒤 원소 (제거하지 않음)
		//        wp는 "다음 자리"이므로 맨 뒤는 wp-1
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
