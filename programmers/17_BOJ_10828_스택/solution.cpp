/*
	[BOJ] 10828 - 스택
	원본 : swtest/17_BOJ_10828_스택.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.

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
#include <stdlib.h>            // [추가] atoi

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
	vector<int> answer;

	sp = 0;   // [추가] 재호출 대비 : 빈 스택으로 시작
	N = (int)commands.size();   // [수정] scanf("%d", &N) 대체

	for (int i = 0; i < N; i++)
	{
		// [수정] scanf("%s", command) 대신 목록에서 꺼낸다.
		// "push X"처럼 값이 붙어 오므로 공백 앞까지가 명령어다.
		const string& line = commands[i];
		char command[100] = { 0 };

		size_t pos = line.find(' ');
		string head = (pos == string::npos) ? line : line.substr(0, pos);
		for (size_t k = 0; k < head.size() && k < 99; k++) command[k] = head[k];

		// ---------------------------
		// push X : 맨 위에 X를 올린다
		// ---------------------------
		if (strCompare(command, "push") == 0)
		{
			int value = atoi(line.c_str() + pos + 1);   // [수정] scanf("%d", &value) 대체

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
				answer.push_back(stack[--sp]);   // [수정] printf -> 결과 목록에 담기
			else
				answer.push_back(-1);            // 비어 있으면 -1
		}

		// ---------------------------
		// size : 원소 개수 = sp
		// ---------------------------
		else if (strCompare(command, "size") == 0)
		{
			answer.push_back(sp);
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strCompare(command, "empty") == 0)
		{
			answer.push_back(sp != 0 ? 0 : 1);
		}

		// ---------------------------
		// top : 맨 위 원소를 빼지 않고 출력
		// ---------------------------
		else if (strCompare(command, "top") == 0)
		{
			if (sp != 0)
				// sp는 "다음 자리"이므로 맨 위는 sp-1
				answer.push_back(stack[sp - 1]);
			else
				answer.push_back(-1);
		}
	}

	return answer;
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n;
	scanf("%d", &n);

	vector<string> commands;
	for (int i = 0; i < n; i++)
	{
		char buf[100];
		scanf("%s", buf);
		string line = buf;

		if (line == "push") { int v; scanf("%d", &v); char num[20]; sprintf(num, " %d", v); line += num; }
		commands.push_back(line);
	}

	vector<int> ans = solution(commands);

#ifdef REPEAT_TEST
	if (solution(commands) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++) printf("%d\n", ans[i]);   // 원본과 같은 형식

	return 0;
}
#endif
