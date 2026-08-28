/*
	[BOJ] 1021 - 회전하는 큐
	https://www.acmicpc.net/problem/1021

	■ 문제 요약
	  1부터 N까지의 수가 순서대로 들어 있는 덱에서, 주어진 M개의 수를 순서대로 뽑아야 한다.
	  쓸 수 있는 연산은 셋뿐이다.

	    1) 맨 앞 원소를 뽑는다                       (횟수에 포함되지 않음)
	    2) 왼쪽으로 한 칸 회전 : 맨 앞 원소를 맨 뒤로  (횟수에 포함)
	    3) 오른쪽으로 한 칸 회전 : 맨 뒤 원소를 맨 앞으로 (횟수에 포함)

	  2번과 3번 연산을 최소 몇 번 써야 하는지 구한다.

	■ 풀이 방침
	  뽑아야 할 순서가 이미 정해져 있으므로, 각 목표 값마다
	  "왼쪽으로 돌려서 앞으로 데려오기"와 "오른쪽으로 돌려서 앞으로 데려오기" 중
	  더 짧은 쪽을 그때그때 고르는 그리디로 충분하다.

	  어차피 목표 값을 맨 앞으로 가져와야 한다는 사실은 고정이고,
	  그 값을 앞으로 데려오는 방법은 두 방향밖에 없다. 한 값을 처리하는 선택이
	  다른 값의 상대 순서를 바꾸지도 않으므로, 매 단계 최솟값을 고르면 전체도 최소가 된다.

	  현재 덱 크기가 size이고 목표 값이 앞에서 k번째(0-based)라면
	    왼쪽 회전 횟수 = k
	    오른쪽 회전 횟수 = size - k
	  로 둘의 합이 항상 size다. 그래서 둘 중 작은 쪽은 언제나 size/2 이하다.

	■ 자료구조
	  10866과 같은 방식의 배열 덱이다. 다만 회전 때문에 양쪽으로 계속 밀려나므로
	  시작점을 배열 한가운데(OFFSET)에 두고 배열도 넉넉하게 잡았다.

	    왼쪽 회전  : deque[back++] = deque[front++]   (앞에서 빼서 뒤에 붙임)
	    오른쪽 회전 : deque[--front] = deque[--back]   (뒤에서 빼서 앞에 붙임)

	  회전 총량은 최대 N*M(= 2500) 수준이라 준비한 여유 안에서 끝난다.
*/

#include <stdio.h>

#define MAX (50 + 5)
#define MAX_DEQUE (MAX * MAX * 2)
#define OFFSET (MAX_DEQUE / 2)

int N, M;                  // N: 덱의 초기 크기, M: 뽑아야 하는 수의 개수

int position[MAX];         // position[i] = i번째로 뽑아야 하는 값

// 배열로 구현한 덱. 회전이 반복되며 양쪽으로 계속 번져 나가므로 크게 잡는다.
int deque[MAX_DEQUE * 2];

// front: 맨 앞 원소의 인덱스, back: 뒤쪽 다음 삽입 위치
// 실제 원소 구간은 deque[front] ~ deque[back - 1]
int front, back;

// ---------------------------
// 입력 및 초기 덱 구성
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// 뽑아야 하는 순서
	for (int i = 0; i < M; i++)
		scanf("%d", &position[i]);

	// 배열 한가운데에서 시작해야 양방향 회전에 여유가 생긴다
	front = back = OFFSET;

	// 초기 덱 : 1, 2, 3, ..., N
	for (int i = 1; i <= N; i++)
		deque[back++] = i;
}

// ---------------------------
// value를 맨 앞으로 데려오는 데 필요한 "왼쪽 회전" 횟수
//
// 앞에서부터 세어 value가 몇 번째에 있는지(0-based)가 곧 답이다.
//
//   덱 [1, 2, 3, 4, 5], value = 3  ->  2번 (1과 2를 뒤로 보내면 3이 맨 앞)
// ---------------------------
int getLeftCount(int value)
{
	int ret = 0;

	for (int i = front; i < back; i++)
	{
		if (deque[i] == value)
			return ret;   // 앞에서 ret번째에 있다
		ret++;
	}

	return -1; // error : 덱에 없는 값 (정상 입력이면 오지 않는다)
}

// ---------------------------
// value를 맨 앞으로 데려오는 데 필요한 "오른쪽 회전" 횟수
//
// 뒤에서부터 세되, 맨 뒤 원소는 1번만 돌리면 앞으로 오므로 ret을 먼저 올리고 비교한다.
//
//   덱 [1, 2, 3, 4, 5], value = 5  ->  1번
//   덱 [1, 2, 3, 4, 5], value = 3  ->  3번
// ---------------------------
int getRightCount(int value)
{
	int ret = 0;

	for (int i = back - 1; i >= front; i--)
	{
		ret++;

		if (deque[i] == value)
			return ret;
	}

	return -1; // error
}

// ---------------------------
// 메인
// ---------------------------
int main()
{
	input();

	int answer = 0; // 회전 연산 총 횟수

	// 뽑아야 하는 수를 순서대로 처리
	for (int i = 0; i < M; i++)
	{
		// 이미 맨 앞에 있으면 회전 없이 바로 뽑는다 (front++ 가 곧 pop_front)
		if (deque[front] == position[i])
		{
			front++;
		}
		else
		{
			// 두 방향의 비용을 각각 구해 더 싼 쪽을 고른다 (두 값의 합은 현재 덱 크기)
			int leftCount = getLeftCount(position[i]);
			int rightCount = getRightCount(position[i]);

			if (leftCount < rightCount)
			{
				// 왼쪽 회전 : 맨 앞을 떼어 맨 뒤에 붙이기를 leftCount번
				for (int k = 0; k < leftCount; k++)
					deque[back++] = deque[front++];

				front++;              // 목표 값이 맨 앞에 왔으므로 뽑는다
				answer += leftCount;  // 회전 횟수만 정답에 누적
			}
			else
			{
				// 오른쪽 회전 : 맨 뒤를 떼어 맨 앞에 붙이기를 rightCount번
				for (int k = 0; k < rightCount; k++)
					deque[--front] = deque[--back];

				front++;               // 목표 값을 뽑는다
				answer += rightCount;
			}
		}
	}

	// 최소 회전 횟수
	printf("%d\n", answer);

	return 0;
}
