/*
	[프로그래머스 함수형] BOJ 10866 - 덱
	원본 : swtest/21_BOJ_10866_덱.cpp
	복습 노트 : docs/review/G02/21_BOJ_10866_덱.md

	원본은 main 안에서 scanf로 명령을 하나씩 읽어 그 자리에서 printf 했다.
	함수형에서는 명령 목록을 인자로 받고, 출력하던 값들을 vector에 모아 반환한다.

	[핵심] 원본의 명령 분기(strcmp 사슬)는 한 글자도 바꾸지 않았다.
	       scanf 를 sscanf 로 바꿔 "읽는 곳"만 stdin에서 문자열로 옮겼기 때문이다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 반환용
#include <string>              // [추가] 명령 문자열용

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
std::vector<int> solution(std::vector<std::string> commands)
{
	std::vector<int> answer;   // [추가] 원본이 printf 하던 값들을 모은다

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
				answer.push_back(-1);   // [수정] printf("-1\n") -> push_back
			else
				// 현재 front를 출력하고 한 칸 전진
				answer.push_back(deque[front++]);   // [수정] printf -> push_back
		}

		// ---------------------------
		// pop_back : 뒤쪽 원소를 빼서 출력
		// ---------------------------
		else if (strcmp(command, "pop_back") == 0)
		{
			if (back == front)
				answer.push_back(-1);   // [수정] printf -> push_back
			else
				// 한 칸 물러난 자리가 곧 마지막 원소다
				answer.push_back(deque[--back]);   // [수정] printf -> push_back
		}

		// ---------------------------
		// size : 원소 개수 = back - front
		// ---------------------------
		else if (strcmp(command, "size") == 0)
		{
			answer.push_back(back - front);   // [수정] printf -> push_back
		}

		// ---------------------------
		// empty : 비어 있으면 1, 아니면 0
		// ---------------------------
		else if (strcmp(command, "empty") == 0)
		{
			answer.push_back((back == front) ? 1 : 0);   // [수정] printf -> push_back
		}

		// ---------------------------
		// front : 맨 앞 원소 (제거하지 않음)
		// ---------------------------
		else if (strcmp(command, "front") == 0)
		{
			if (back == front)
				answer.push_back(-1);   // [수정] printf -> push_back
			else
				answer.push_back(deque[front]);   // [수정] printf -> push_back
		}

		// ---------------------------
		// back : 맨 뒤 원소 (제거하지 않음)
		// ---------------------------
		else if (strcmp(command, "back") == 0)
		{
			if (back == front)
				answer.push_back(-1);   // [수정] printf -> push_back
			else
				answer.push_back(deque[back - 1]);   // [수정] printf -> push_back
		}
	}

	return answer;   // [수정] return 0 -> 결과 배열 반환
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
//   빌드      : g++ -O2 -DLOCAL_TEST -o run solution.cpp
//   재호출 검사 : g++ -O2 -DLOCAL_TEST -DREPEAT_TEST -o rep solution.cpp
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int n;
	scanf("%d", &n);   // 원본과 같은 형식

	std::vector<std::string> commands;
	for (int i = 0; i < n; i++)
	{
		char cmd[100];
		scanf("%s", cmd);

		std::string line = cmd;
		if (line == "push_front" || line == "push_back")
		{
			int v;
			scanf("%d", &v);

			char buf[32];
			sprintf(buf, " %d", v);
			line += buf;
		}
		commands.push_back(line);
	}

	std::vector<int> ans = solution(commands);

#ifdef REPEAT_TEST
	std::vector<int> ans2 = solution(commands);   // 같은 인자로 한 번 더
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

	for (size_t i = 0; i < ans.size(); i++)
		printf("%d\n", ans[i]);   // 원본 출력 형식 그대로 (조회 명령마다 한 줄)

	return 0;
}
#endif
