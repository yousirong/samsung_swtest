/*
	[코드트리] 2017 하반기 오후 1번 - 돌아가는 팔각의자
	https://www.codetree.ai/training-field/frequent-problems/problems/rounding-eight-angle

	■ 문제 요약
	  톱니가 8개인 팔각의자 4개가 1번부터 4번까지 일렬로 맞물려 있다.
	  각 톱니는 0 또는 1의 상태를 가진다.

	  회전 명령 K개가 주어지고, 각 명령은 (의자 번호, 방향)이다. 방향은 1이 시계, -1이 반시계다.
	  한 의자를 회전시키면 맞닿은 옆 의자로 회전이 전파된다.

	    맞닿은 두 톱니의 상태가 서로 다르면 -> 옆 의자가 "반대 방향"으로 함께 회전한다
	    같으면 -> 그 방향으로는 더 이상 전파되지 않는다 (거기서 멈춘다)

	  명령을 모두 수행한 뒤 점수를 출력한다.
	  점수는 12시 방향 톱니가 1인 의자마다 1, 2, 4, 8점을 더한 값이다.

	  (백준 14891 "톱니바퀴"와 같은 문제다)

	■ 톱니 인덱스 약속
	  각 의자의 톱니를 12시부터 시계 방향으로 1 ~ 8번으로 둔다.

	      chair[n][1] : 12시 방향 (점수 계산에 쓰는 톱니)
	      chair[n][3] : 3시 방향  = 오른쪽 의자와 맞닿는 톱니
	      chair[n][7] : 9시 방향  = 왼쪽 의자와 맞닿는 톱니

	  그래서 n번과 n+1번이 맞물린 상태를 비교할 때는
	  chair[n][3] 과 chair[n+1][7] 을 본다.

	■ 회전
	  번호가 시계 방향으로 매겨져 있으므로

	    시계 방향   : 각 톱니가 인덱스 +1 자리로 간다 (8번은 1번으로)
	    반시계 방향 : 각 톱니가 인덱스 -1 자리로 간다 (1번은 8번으로)

	■ 이 문제의 핵심 함정 : "먼저 다 판정하고, 나중에 한꺼번에 돌린다"
	  전파를 판정하면서 곧바로 회전시키면, 그다음 비교가 이미 돌아간 상태를 보게 되어 틀린다.
	  그래서 두 단계로 나눈다.

	    1) 회전 전 상태를 기준으로 각 의자가 어느 방향으로 돌지 check[]에 기록만 한다
	       (target에서 오른쪽으로, 다시 target에서 왼쪽으로 각각 끊길 때까지 훑는다)
	    2) 기록이 끝난 뒤에 check[]를 보고 실제 회전을 수행한다

	  전파는 target에서 바깥으로 퍼지므로, check[right]는 check[right+1]을 정하기 전에
	  이미 확정되어 있다. 그래서 한 번의 훑기로 방향을 이어 붙일 수 있다.
*/

#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용
#include <string>              // [추가] 톱니 문자열용

#define MAX (100 + 10)

#define CLOCKWISE (1)
#define COUTNER_CLOCKWISE (-1)

int T;

int K;                     // 회전 명령 개수
int chair[5][10];          // chair[의자 번호 1~4][톱니 1~8]
int chairNumber[MAX];      // k번째 명령에서 직접 돌릴 의자 번호
int directions[MAX];       // k번째 명령의 회전 방향 (1 또는 -1)

// ---------------------------
// 입력
// ---------------------------
// [수정] scanf 대신 인자로 받는다.
//        chairs[i] 는 "10001111" 같은 8자리 문자열, rotations[i] = {의자 번호, 방향}
void input(const std::vector<std::string>& chairs, const std::vector<std::vector<int>>& rotations)
{
	// 의자 4개의 톱니 상태. 원본은 %1d 로 한 자리씩 읽었다.
	for (int number = 1; number <= 4; number++)
	{
		for (int index = 1; index <= 8; index++)
		{
			chair[number][index] = chairs[number - 1][index - 1] - '0';   // [수정] scanf 대체
		}
	}

	K = (int)rotations.size();   // [수정] scanf 대체

	for (int i = 0; i < K; i++)
	{
		chairNumber[i] = rotations[i][0];   // [수정] scanf 대체
		directions[i] = rotations[i][1];    // [수정] scanf 대체
	}
}

// ---------------------------
// 의자 하나를 실제로 회전
//
//   반시계(-1) : [1][2][3]...[8]  ->  [2][3][4]...[1]
//   시계(1)    : [1][2][3]...[8]  ->  [8][1][2]...[7]
// ---------------------------
void rotate(int number, int direction)
{
	int tmp;

	if (direction == COUTNER_CLOCKWISE)
	{
		// 앞에서부터 한 칸씩 당기고, 맨 앞 값을 뒤로 보낸다
		tmp = chair[number][1];

		for (int index = 1; index <= 7; index++)
			chair[number][index] = chair[number][index + 1];

		chair[number][8] = tmp;
	}
	else
	{
		// 뒤에서부터 한 칸씩 밀고, 맨 뒤 값을 앞으로 보낸다
		tmp = chair[number][8];

		for (int index = 8; index >= 2; index--)
			chair[number][index] = chair[number][index - 1];

		chair[number][1] = tmp;
	}
}

// ---------------------------
// K개의 회전 명령을 순서대로 수행
// ---------------------------
void simulate()
{
	for (int k = 0; k < K; k++)
	{
		// check[i] : 이번 명령에서 i번 의자가 돌 방향
		//            0이면 돌지 않음, 1이면 시계, -1이면 반시계
		int check[5 + 1] = { 0 };

		int target = chairNumber[k];
		int direction = directions[k];

		// 직접 지목된 의자는 무조건 그 방향으로 돈다
		check[target] = direction;

		// ----- 오른쪽으로 전파 -----
		// 내 3시 톱니와 오른쪽 의자의 9시 톱니를 비교.
		// 다르면 오른쪽 의자는 나와 반대 방향으로 돌고, 같으면 거기서 멈춘다.
		for (int right = target; right <= 3; right++)
		{
			if (chair[right][3] != chair[right + 1][7])
				check[right + 1] = check[right] * (-1);
			else
				break;
		}

		// ----- 왼쪽으로 전파 -----
		// 내 9시 톱니와 왼쪽 의자의 3시 톱니를 비교.
		for (int left = target; left >= 2; left--)
		{
			if (chair[left][7] != chair[left - 1][3])
				check[left - 1] = check[left] * (-1);
			else
				break;
		}

		// ----- 판정이 모두 끝난 뒤에 한꺼번에 회전 -----
		//
		// 위의 두 전파 검사는 반드시 "회전 전 상태"를 봐야 한다.
		// 중간에 돌려 버리면 다음 비교가 이미 바뀐 톱니를 보게 되어 결과가 틀어진다.
		for (int i = 1; i <= 4; i++)
		{
			if (check[i] != 0)
				rotate(i, check[i]);
		}
	}
}

// ---------------------------
// 최종 점수
//
//   1번 의자 12시가 1이면 1점, 2번은 2점, 3번은 4점, 4번은 8점
//   (즉 4자리 이진수를 읽는 것과 같다)
// ---------------------------
int getScore()
{
	// 의자 개수가 늘어난다면 아래처럼 일반화할 수 있다
	//
	// int sum, mul;
	// sum = 0;
	// mul = 1;
	// for (int i = 1; i <= 4; i++)
	// {
	//     sum += mul * chair[i][1];
	//     mul *= 2;
	// }

	return (chair[1][1] * 1) +
		(chair[2][1] * 2) +
		(chair[3][1] * 4) +
		(chair[4][1] * 8);
}

// [수정] main() -> solution().  T 루프 껍데기는 제거했다.
int solution(std::vector<std::string> chairs, std::vector<std::vector<int>> rotations)
{
	input(chairs, rotations);

	simulate();

	return getScore();   // [수정] printf -> return
}

// ==========================================================
// [추가] 로컬 대조용 하네스. 제출할 때는 이 블록 전체를 지운다.
// ==========================================================
#ifdef LOCAL_TEST
int main()
{
	std::vector<std::string> chairs(4);
	for (int i = 0; i < 4; i++)
	{
		char buf[32];
		scanf("%s", buf);            // 원본은 %1d 로 한 자리씩 읽지만 결과는 같다
		chairs[i] = buf;
	}

	int k;
	scanf("%d", &k);
	std::vector<std::vector<int>> rotations(k, std::vector<int>(2));
	for (int i = 0; i < k; i++) scanf("%d %d", &rotations[i][0], &rotations[i][1]);

	int ans = solution(chairs, rotations);
#ifdef REPEAT_TEST
	int ans2 = solution(chairs, rotations);
	if (ans != ans2) { printf("!! NOT RE-ENTRANT\n"); return 1; }
#endif
	printf("%d\n", ans);
	return 0;
}
#endif
