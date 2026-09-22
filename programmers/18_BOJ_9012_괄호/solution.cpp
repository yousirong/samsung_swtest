/*
	[BOJ] 9012 - 괄호
	원본 : swtest/18_BOJ_9012_괄호.cpp
	[프로그래머스 함수형 사본]  main() 대신 solution()이 값을 받고 돌려준다.
	원본 : swtest/ 아래 같은 이름의 파일. 로직은 그대로 두고 입출력 껍데기만 바꿨다.

	https://www.acmicpc.net/problem/9012

	■ 문제 요약
	  '(' 와 ')' 로만 이루어진 문자열이 T개 주어진다.
	  각 문자열이 올바른 괄호 문자열(VPS, Valid Parenthesis String)이면 YES, 아니면 NO를 출력한다.

	■ 풀이 방침
	  괄호가 한 종류뿐이라 스택을 실제로 만들 필요가 없다.
	  스택에 쌓이는 것이 언제나 '(' 하나뿐이므로, "쌓인 개수"만 정수로 세면 스택과 동치다.

	    '(' 를 만나면 sp++   (스택에 push)
	    ')' 를 만나면 sp--   (스택에서 pop)

	  올바른 괄호 문자열의 조건은 다음 두 가지다.

	    1) 진행 도중 단 한 번도 sp가 음수가 되지 않는다
	       -> 짝이 없는 ')' 가 먼저 나온 경우. 예) ")("
	    2) 문자열이 끝났을 때 sp == 0 이다
	       -> 닫히지 않은 '(' 가 남은 경우를 걸러 준다. 예) "(()"

	  둘 중 하나라도 어기면 NO.

	■ 주의할 점
	  조건 2만 검사하면 ")(" 같은 문자열을 통과시켜 버린다.
	  개수만 같은 것으로는 부족하고, "중간에도 항상 음수가 아니어야" 한다는 점이 핵심이다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용
#include <string>              // [추가] 문자열 인자용
#include <stdbool.h>

int T;   // 테스트 케이스 수

// [수정] main() -> solution(). 여러 케이스를 돌던 T 루프는 하네스로 옮기고,
// solution()은 괄호 문자열 하나만 판정한다.
std::string solution(std::string parens)
{
	int sp;             // 아직 닫히지 않은 '(' 의 개수 (스택의 높이)
	int length;         // 문자열 길이
	bool flag;          // 도중에 sp가 음수가 된 적이 있는지

	sp = 0;
	flag = false;

	length = (int)parens.size();   // [수정] '\0'까지 세던 루프 대신 size()

	// 왼쪽부터 한 글자씩 훑는다
	for (int i = 0; i < length; i++)
	{
		int ch = parens[i];

		if (ch == '(')
			sp++;   // 열린 괄호를 하나 쌓는다
		else
			sp--;   // 닫는 괄호는 쌓여 있던 것 하나를 뺀다

		// 핵심 조건 (1)
		// sp가 음수라는 건 짝지을 '(' 가 없는데 ')' 가 나왔다는 뜻이다.
		// 뒤를 더 봐도 절대 복구되지 않으므로 즉시 중단한다.
		if (sp < 0)
		{
			flag = true;
			break;
		}
	}

	// 최종 판정
	//   flag == true : 도중에 ')' 가 앞질렀다
	//   sp != 0      : 끝났는데 닫히지 않은 '(' 가 남았다
	if (flag == true || sp != 0)
		return "NO";   // [수정] printf -> return

	return "YES";
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// 원본처럼 T개의 문자열을 읽어 한 줄씩 출력한다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	int t;
	scanf("%d", &t);

	for (int tc = 0; tc < t; tc++)
	{
		char buf[50 + 5];
		scanf("%s", buf);

		std::string ans = solution(buf);

#ifdef REPEAT_TEST
		if (solution(buf) != ans) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif

		printf("%s\n", ans.c_str());
	}

	return 0;
}
#endif
