/*
	[BOJ] 10828 - 스택

	[프로그래머스 제출용]  원본 : swtest/17_BOJ_10828_스택.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).

	https://www.acmicpc.net/problem/10828

	■ 문제 요약
	  정수를 저장하는 스택을 구현하고 N개의 명령을 처리한다.

	    push X : 정수 X를 스택에 넣는다
	    pop    : 맨 위 정수를 빼고 출력한다. 비어 있으면 -1
	    size   : 스택에 들어 있는 정수의 개수를 출력한다
	    empty  : 비어 있으면 1, 아니면 0
	    top    : 맨 위 정수를 출력한다(빼지 않음). 비어 있으면 -1

	■ 풀이 방침
	  STL 없이 배열 하나와 정수 하나(sp)로 스택을 만든다.
	  sp는 "다음에 값이 들어갈 자리"를 가리키는 스택 포인터다.

	    sp == 0            -> 비어 있음
	    push  : stack[sp++] = value
	    pop   : stack[--sp]        (먼저 내리고 그 자리를 읽는다)
	    top   : stack[sp - 1]      (내리지 않고 바로 아래 칸을 본다)
	    size  : sp 그 자체

	  sp를 "다음 자리"로 정의했기 때문에 size가 곧 sp가 되고,
	  맨 위 원소는 항상 sp-1 번째가 된다. 이 규칙만 흔들리지 않으면 헷갈릴 일이 없다.

	    sp = 0            -> 비어 있음
	    push(10)          -> stack[0] = 10, sp = 1
	    push(20)          -> stack[1] = 20, sp = 2, top = stack[1] = 20

	■ 주의할 점
	  pop / top은 비어 있는 경우(sp == 0)를 반드시 먼저 걸러야 한다.
	  그러지 않으면 stack[-1]을 읽는 잘못된 접근이 된다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용
#include <string>              // [추가] 명령 문자열용

using namespace std;

#define MAX (10000 + 500)

int N;            // 명령의 개수

int stack[MAX];   // 배열로 구현한 스택
int sp;           // stack pointer : 다음에 값이 들어갈 위치 (= 현재 원소 개수)

// ---------------------------
// strcmp 직접 구현 (string.h 없이 명령어를 비교하기 위함)
//
// 두 문자열이 같으면 0, 다르면 처음으로 다른 문자의 차이를 반환한다.
// 여기서는 "0인지 아닌지"만 쓴다.
// ---------------------------
int strCompare(const char* a, const char* b)
{
	// a가 끝나지 않았고 두 문자가 같은 동안 계속 전진
	while (*a && *a == *b)
	{
		++a;
		++b;
	}

	// 멈춘 지점의 문자 차이. 끝까지 같았다면 둘 다 '\0'이라 0이 된다.
	return *a - *b;
}

// ---------------------------
// 디버그용: 현재 스택 상태를 위에서부터 출력
// ---------------------------
void printStack()
{
	for (int i = sp - 1; i >= 0; i--)
		printf("%d ", stack[i]);
	putchar('\n');
}

// ---------------------------
// 메인
// ---------------------------
// [수정] main() -> solution(). 명령을 문자열 목록으로 받고,
// 출력이 있는 명령(pop, size, empty, top)의 결과만 순서대로 담아 반환한다.
vector<int> solution(vector<string> commands)
{
	int out[MAX];        // [추가] 원본이 printf 하던 값들을 담아 둔다
	int ocnt = 0;        // [추가] out에 담긴 개수

	sp = 0;   // [추가] 재호출 대비 : 빈 스택으로 시작
	N = (int)commands.size();   // [수정] scanf("%d", &N) 대체

	for (int i = 0; i < N; i++)
	{
		char command[100];
		int value = 0;

		// [수정] scanf -> sscanf (읽는 대상만 바뀐다). "push 10" 이면 값까지 한 번에 읽힌다.
		sscanf(commands[i].c_str(), "%s %d", command, &value);

		// ---------------------------
		// push X : 맨 위에 X를 올린다
		// ---------------------------
		if (strCompare(command, "push") == 0)
		{

			// 현재 sp 자리에 넣고 sp를 한 칸 올린다
			stack[sp++] = value;
		}

		// ---------------------------
		// pop : 맨 위 원소를 빼서 출력
		// ---------------------------
		else if (strCompare(command, "pop") == 0)
		{
			if (sp != 0)
				// --sp로 먼저 내려간 자리가 곧 기존의 맨 위 원소다
				out[ocnt++] = stack[--sp];   // [수정] printf -> 결과 목록에 담기
			else
				out[ocnt++] = -1;            // 비어 있으면 -1
		}

		// ---------------------------
		// size : 원소 개수 = sp
		// ---------------------------
		else if (strCompare(command, "size") == 0)
		{
			out[ocnt++] = sp;
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strCompare(command, "empty") == 0)
		{
			out[ocnt++] = sp != 0 ? 0 : 1;
		}

		// ---------------------------
		// top : 맨 위 원소를 빼지 않고 출력
		// ---------------------------
		else if (strCompare(command, "top") == 0)
		{
			if (sp != 0)
				// sp는 "다음 자리"이므로 맨 위는 sp-1
				out[ocnt++] = stack[sp - 1];
			else
				out[ocnt++] = -1;
		}
	}

	return vector<int>(out, out + ocnt);
}
