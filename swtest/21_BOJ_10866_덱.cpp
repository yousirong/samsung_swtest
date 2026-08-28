/*
	[BOJ] 10866 - 덱
	https://www.acmicpc.net/problem/10866

	■ 문제 요약
	  덱(deque, 양쪽 끝에서 넣고 뺄 수 있는 큐)을 구현하고 N개의 명령을 처리한다.

	    push_front X / push_back X : 앞 / 뒤에 X를 넣는다
	    pop_front    / pop_back    : 앞 / 뒤 원소를 빼고 출력. 비어 있으면 -1
	    size                       : 원소 개수
	    empty                      : 비어 있으면 1, 아니면 0
	    front        / back        : 앞 / 뒤 원소를 출력(빼지 않음). 비어 있으면 -1

	■ 풀이 방침
	  큐와 마찬가지로 배열 위에서 포인터 두 개를 미는 방식인데,
	  덱은 앞쪽으로도 자라야 하므로 시작 위치를 배열 맨 앞이 아니라 "중앙"으로 잡는다.

	    front : 맨 앞 원소의 위치
	    back  : 다음에 뒤로 넣을 위치

	  실제 원소 구간은 deque[front] ~ deque[back - 1] 이다.

	    비어 있음   : front == back
	    push_front  : deque[--front] = value   (먼저 왼쪽으로 한 칸 물러난 뒤 쓴다)
	    push_back   : deque[back++]  = value   (현재 자리에 쓰고 오른쪽으로 한 칸)
	    pop_front   : deque[front++]
	    pop_back    : deque[--back]
	    size        : back - front

	  중앙에서 시작했기 때문에 front는 왼쪽으로, back은 오른쪽으로 각각 자유롭게 자란다.

	    front = back = OFFSET      -> 비어 있음
	    push_back(10)              -> deque[OFFSET] = 10, back = OFFSET+1
	    push_front(20)             -> front = OFFSET-1, deque[OFFSET-1] = 20
	    상태 : [20, 10]

	■ 주의할 점
	  1) push_front는 "--front 후 쓰기", push_back은 "쓰고 back++" 로 전위/후위가 서로 다르다.
	     front는 실제 원소를 가리키고 back은 빈 다음 자리를 가리키기 때문이다.
	     같은 이유로 뒤쪽 조회는 deque[back - 1], 앞쪽 조회는 deque[front]다.
	  2) 아래 strcmp는 표준 라이브러리의 strcmp와 이름이 같다.
	     여기서는 <string.h>를 포함하지 않아 충돌하지 않지만,
	     다른 헤더를 추가하면 중복 정의로 깨질 수 있으니 이름을 바꿔 두는 편이 안전하다.
	  3) OFFSET이 MAX/2(= 5250)인데 N은 최대 10,000이다.
	     push_front만 계속 들어오는 입력이라면 front가 0 아래로 내려가 배열 밖을 건드린다.
	     안전하게 하려면 OFFSET을 MAX(= 10,500)로 두면 된다.
	     (배열 자체는 MAX*2 크기라 그렇게 잡아도 양쪽 모두 여유가 있다)
*/

#include <stdio.h>

#define MAX (10000 + 500)
#define OFFSET (MAX / 2)   // 덱이 시작하는 배열 중앙 위치

int N;                 // 명령 개수
int deque[MAX * 2];    // 배열로 구현한 덱
int front, back;       // front: 맨 앞 원소 위치, back: 뒤쪽 다음 삽입 위치

// ---------------------------
// strcmp 직접 구현 (string.h 없이 명령어를 비교하기 위함)
// 같으면 0을 반환한다.
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
// 메인
// ---------------------------
int main()
{
	// 배열 중앙에서 시작해야 앞/뒤 어느 쪽으로도 늘어날 수 있다
	front = back = OFFSET;

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		scanf("%s", command);

		// ---------------------------
		// push_front X : 앞쪽에 삽입
		// ---------------------------
		if (strcmp(command, "push_front") == 0)
		{
			int value;
			scanf("%d", &value);

			// front는 실제 원소를 가리키므로, 먼저 한 칸 물러난 뒤 그 자리에 쓴다
			deque[--front] = value;
		}

		// ---------------------------
		// push_back X : 뒤쪽에 삽입
		// ---------------------------
		else if (strcmp(command, "push_back") == 0)
		{
			int value;
			scanf("%d", &value);

			// back은 빈 다음 자리를 가리키므로, 그 자리에 쓰고 한 칸 전진
			deque[back++] = value;
		}

		// ---------------------------
		// pop_front : 앞쪽 원소를 빼서 출력
		// ---------------------------
		else if (strcmp(command, "pop_front") == 0)
		{
			if (back == front)
				printf("-1\n");   // 비어 있음
			else
				// 현재 front를 출력하고 한 칸 전진
				printf("%d\n", deque[front++]);
		}

		// ---------------------------
		// pop_back : 뒤쪽 원소를 빼서 출력
		// ---------------------------
		else if (strcmp(command, "pop_back") == 0)
		{
			if (back == front)
				printf("-1\n");
			else
				// 한 칸 물러난 자리가 곧 마지막 원소다
				printf("%d\n", deque[--back]);
		}

		// ---------------------------
		// size : 원소 개수 = back - front
		// ---------------------------
		else if (strcmp(command, "size") == 0)
		{
			printf("%d\n", back - front);
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strcmp(command, "empty") == 0)
		{
			printf("%d\n", (back == front) ? 1 : 0);
		}

		// ---------------------------
		// front : 맨 앞 원소 (제거하지 않음)
		// ---------------------------
		else if (strcmp(command, "front") == 0)
		{
			if (back == front)
				printf("-1\n");
			else
				printf("%d\n", deque[front]);
		}

		// ---------------------------
		// back : 맨 뒤 원소 (제거하지 않음)
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
