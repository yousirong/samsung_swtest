/*
	[프로그래머스 함수형] BOJ 10866 - 덱

	[프로그래머스 제출용]  원본 : swtest/21_BOJ_10866_덱.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).

	복습 노트 : docs/review/G02/21_BOJ_10866_덱.md

	원본은 main 안에서 scanf로 명령을 하나씩 읽어 그 자리에서 printf 했다.
	함수형에서는 명령 목록을 인자로 받고, 출력하던 값들을 vector에 모아 반환한다.

	[핵심] 원본의 명령 분기(strcmp 사슬)는 한 글자도 바꾸지 않았다.
	       scanf 를 sscanf 로 바꿔 "읽는 곳"만 stdin에서 문자열로 옮겼기 때문이다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 반환용
#include <string>              // [추가] 명령 문자열용

using namespace std;

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
// [수정] main() -> solution()
//        원본은 명령마다 printf 했지만, 여기서는 answer에 담아 한꺼번에 반환한다.
// ---------------------------
vector<int> solution(vector<string> commands)
{
	int out[MAX];        // [추가] 원본이 printf 하던 값들을 담아 둔다
	int ocnt = 0;        // [추가] out에 담긴 개수

	// 배열 중앙에서 시작해야 앞/뒤 어느 쪽으로도 늘어날 수 있다
	// (원본에서 main 첫 줄에 있던 초기화. 재호출 대비 역할도 겸한다)
	front = back = OFFSET;

	N = (int)commands.size();   // [수정] scanf("%d", &N) 대체 - 명령 개수는 배열 길이로 알 수 있다

	for (int i = 0; i < N; i++)
	{
		char command[100];

		sscanf(commands[i].c_str(), "%s", command);   // [수정] scanf -> sscanf (읽는 대상만 바뀜)

		// ---------------------------
		// push_front X : 앞쪽에 삽입
		// ---------------------------
		if (strcmp(command, "push_front") == 0)
		{
			int value;
			sscanf(commands[i].c_str(), "%*s %d", &value);   // [수정] scanf -> sscanf

			// front는 실제 원소를 가리키므로, 먼저 한 칸 물러난 뒤 그 자리에 쓴다
			deque[--front] = value;
		}

		// ---------------------------
		// push_back X : 뒤쪽에 삽입
		// ---------------------------
		else if (strcmp(command, "push_back") == 0)
		{
			int value;
			sscanf(commands[i].c_str(), "%*s %d", &value);   // [수정] scanf -> sscanf

			// back은 빈 다음 자리를 가리키므로, 그 자리에 쓰고 한 칸 전진
			deque[back++] = value;
		}

		// ---------------------------
		// pop_front : 앞쪽 원소를 빼서 출력
		// ---------------------------
		else if (strcmp(command, "pop_front") == 0)
		{
			if (back == front)
				out[ocnt++] = -1;   // [수정] printf("-1\n") -> 결과 배열에 담기
			else
				// 현재 front를 출력하고 한 칸 전진
				out[ocnt++] = deque[front++];   // [수정] printf -> 결과 배열에 담기
		}

		// ---------------------------
		// pop_back : 뒤쪽 원소를 빼서 출력
		// ---------------------------
		else if (strcmp(command, "pop_back") == 0)
		{
			if (back == front)
				out[ocnt++] = -1;   // [수정] printf -> 결과 배열에 담기
			else
				// 한 칸 물러난 자리가 곧 마지막 원소다
				out[ocnt++] = deque[--back];   // [수정] printf -> 결과 배열에 담기
		}

		// ---------------------------
		// size : 원소 개수 = back - front
		// ---------------------------
		else if (strcmp(command, "size") == 0)
		{
			out[ocnt++] = back - front;   // [수정] printf -> 결과 배열에 담기
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strcmp(command, "empty") == 0)
		{
			out[ocnt++] = (back == front) ? 1 : 0;   // [수정] printf -> 결과 배열에 담기
		}

		// ---------------------------
		// front : 맨 앞 원소 (제거하지 않음)
		// ---------------------------
		else if (strcmp(command, "front") == 0)
		{
			if (back == front)
				out[ocnt++] = -1;   // [수정] printf -> 결과 배열에 담기
			else
				out[ocnt++] = deque[front];   // [수정] printf -> 결과 배열에 담기
		}

		// ---------------------------
		// back : 맨 뒤 원소 (제거하지 않음)
		// ---------------------------
		else if (strcmp(command, "back") == 0)
		{
			if (back == front)
				out[ocnt++] = -1;   // [수정] printf -> 결과 배열에 담기
			else
				out[ocnt++] = deque[back - 1];   // [수정] printf -> 결과 배열에 담기
		}
	}

	return vector<int>(out, out + ocnt);
}
