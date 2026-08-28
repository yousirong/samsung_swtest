/*
	[BOJ] 9012 - 괄호
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
#include <stdbool.h>

int T;   // 테스트 케이스 수

int main()
{
	scanf("%d", &T);

	// T개의 문자열을 각각 판정
	for (int tc = 0; tc < T; tc++)
	{
		char str[50 + 5];   // 괄호 문자열 (최대 길이 50 + 여유)
		int sp;             // 아직 닫히지 않은 '(' 의 개수 (스택의 높이)
		int length;         // 문자열 길이
		bool flag;          // 도중에 sp가 음수가 된 적이 있는지

		scanf("%s", str);

		sp = 0;
		flag = false;

		// 문자열 길이 구하기 : '\0'을 만날 때까지 센다 (strlen 대용)
		for (length = 0; str[length]; length++);

		// 왼쪽부터 한 글자씩 훑는다
		for (int i = 0; i < length; i++)
		{
			int ch = str[i];

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
			printf("NO\n");
		else
			printf("YES\n");
	}

	return 0;
}
