// BOJ 1021 - 회전하는 큐
//
// 문제 요약
// 1부터 N까지 들어 있는 덱(deque)에서
// 주어진 M개의 수를 순서대로 꺼내야 한다.
//
// 가능한 연산
// 1) 맨 앞 원소를 꺼낸다.
// 2) 왼쪽으로 한 칸 회전한다.
//    (맨 앞 원소를 맨 뒤로 보냄)
// 3) 오른쪽으로 한 칸 회전한다.
//    (맨 뒤 원소를 맨 앞으로 보냄)
//
// 목표
// 주어진 수들을 순서대로 꺼낼 때
// 2번, 3번 회전 연산의 최소 횟수를 구한다.

#include <stdio.h>

#define MAX (50 + 5)
#define MAX_DEQUE (MAX * MAX * 2)
#define OFFSET (MAX_DEQUE / 2)

// N: 덱의 초기 크기
// M: 뽑아야 하는 수의 개수
int N, M;

// position[i] = i번째로 뽑아야 하는 값
int position[MAX];

// 배열로 덱 구현
// 가운데에서 시작해서 왼쪽/오른쪽으로 여유 있게 확장 가능하게 사용
int deque[MAX_DEQUE * 2];

// front: 현재 덱의 맨 앞 인덱스
// back : 현재 덱의 맨 뒤 다음 인덱스
// 즉 실제 원소는 deque[front] ~ deque[back-1]
int front, back;

// ---------------------------
// 입력 함수
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);

	// 뽑아야 하는 순서 입력
	for (int i = 0; i < M; i++)
		scanf("%d", &position[i]);

	// 덱 초기화
	// 가운데에서 시작하면 양쪽으로 회전할 때 인덱스 여유가 많음
	front = back = OFFSET;

	// 초기 덱 상태: 1, 2, 3, ..., N
	for (int i = 1; i <= N; i++)
		deque[back++] = i;
}

// ---------------------------
// 현재 덱에서 value를 찾을 때
// 왼쪽 회전(앞에서 뒤로 보내기)을 몇 번 해야
// 맨 앞으로 올 수 있는지 계산
//
// 예)
// 덱: [1, 2, 3, 4, 5]
// value = 3
// 왼쪽 회전 2번 하면 3이 맨 앞에 온다
// -> 반환값 2
// ---------------------------
int getLeftCount(int value)
{
	int ret = 0;

	for (int i = front; i < back; i++)
	{
		if (deque[i] == value)
			return ret;
		ret++;
	}

	return -1; // error
}

// ---------------------------
// 현재 덱에서 value를 찾을 때
// 오른쪽 회전(뒤에서 앞으로 보내기)을 몇 번 해야
// 맨 앞으로 올 수 있는지 계산
//
// 예)
// 덱: [1, 2, 3, 4, 5]
// value = 5
// 오른쪽 회전 1번 하면 5가 맨 앞
// -> 반환값 1
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
// 메인 함수
// ---------------------------
int main()
{
	input();

	int answer = 0; // 회전 연산 총 횟수

	// 뽑아야 하는 수를 순서대로 처리
	for (int i = 0; i < M; i++)
	{
		// 이미 맨 앞에 있다면 그냥 꺼내면 됨
		// 회전 연산 필요 없음
		if (deque[front] == position[i])
		{
			front++;   // 맨 앞 원소 제거
		}
		else
		{
			// 왼쪽 회전 횟수와 오른쪽 회전 횟수 계산
			int leftCount = getLeftCount(position[i]);
			int rightCount = getRightCount(position[i]);

			// 왼쪽 회전이 더 적게 드는 경우
			if (leftCount < rightCount)
			{
				// 왼쪽 회전 leftCount번 수행
				// 맨 앞 원소를 맨 뒤로 보냄
				for (int k = 0; k < leftCount; k++)
					deque[back++] = deque[front++];

				// 이제 원하는 값이 맨 앞에 왔으므로 제거
				front++;

				// 회전 횟수 누적
				answer += leftCount;
			}
			// 오른쪽 회전이 더 적게 들거나 같은 경우
			else
			{
				// 오른쪽 회전 rightCount번 수행
				// 맨 뒤 원소를 맨 앞으로 보냄
				for (int k = 0; k < rightCount; k++)
					deque[--front] = deque[--back];

				// 이제 원하는 값이 맨 앞에 왔으므로 제거
				front++;

				// 회전 횟수 누적
				answer += rightCount;
			}
		}
	}

	// 최소 회전 횟수 출력
	printf("%d\n", answer);

	return 0;
}