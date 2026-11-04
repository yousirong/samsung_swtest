/*
	[SWEA] 2383 - [모의 SW 역량테스트] 점심 식사시간
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV5-BEE6AK0DFAVl

	■ 문제 요약
	  N x N 방(4 <= N <= 10)에 사람(1)이 1 ~ 10명, 계단(2 이상, 값 = 계단 길이 K)이 2개 있다.
	  사람마다 두 계단 중 하나를 골라 내려간다.
	    - 계단 입구까지는 맨해튼 거리만큼 걸린다
	    - 입구에 도착하고 1분 뒤부터 내려갈 수 있고, 내려가는 데 K분이 걸린다
	    - 계단 위에는 동시에 3명까지만 있을 수 있다. 꽉 차 있으면 입구에서 기다린다
	  모든 사람이 다 내려가는 시간의 최솟값을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 계단 배정을 DFS로 전부 해 보고, 계단마다 시뮬레이션
	  1) DFS로 사람마다 계단 0 / 1을 고른다. 최대 2^10 = 1,024가지.
	  2) 계단별로 그 계단을 고른 사람만 모아 calculate()로 다 내려가는 시각을 구한다.
	  3) 두 계단 중 늦은 쪽이 그 배정의 완료 시각. 그 최솟값이 답.

	■ 계단 하나 시뮬레이션 (calculate)
	  - 그 계단까지의 거리를 오름차순으로 정렬한다 (먼저 도착한 사람이 먼저 내려간다).
	  - 1분마다 모든 거리를 1씩 줄이고, 0이 된(입구에 도착한) 사람을 대기열 downStair에 넣는다.
	    값은 "남은 계단 칸 수" = 계단 길이.
	  - 대기열 앞의 3명(rp ~ rp + 2)만 계단 위에 있다고 보고 1씩 줄인다.
	  - 0이 된 사람은 다 내려간 것 -> rp를 민다. rp == count면 끝.
	  - 도착한 분에 바로 한 칸 내려가는 것처럼 세고, 마지막에 + 1 해서
	    "도착 1분 뒤 출발 + K분" 규칙을 맞춘다.

	■ 주의할 점
	  - 계단 번호는 1, 2 (입력 순서대로 scnt = 1부터). DFS의 list[] 값 0 / 1은 계단 1 / 2를 뜻한다.
	  - 대기열 앞 3명을 줄이는 반복문의 종료 조건이 i > wp 라서 i == wp 일 때 비어 있는 칸
	    downStair[wp]도 1 줄인다. 이 칸은 다음 사람이 들어올 때 계단 길이로 덮어쓰므로 결과는 맞지만,
	    i >= wp 로 쓰는 것이 의도에 맞다. (downStair는 지역 배열이라 처음엔 쓰레기 값이다)
	  - 아무도 고르지 않은 계단(count == 0)은 3을 돌려준다. 다른 계단은 최소
	    거리 1 + 대기 1 + 계단 2 = 4 이상이라 max에서 항상 밀려 결과에 영향이 없다.
	  - abs()를 직접 만들어 쓴다 (stdlib.h를 include하지 않았다).
*/
#include <stdio.h>                  // scanf, printf 사용

#define MAX (10+5)                  // N, 사람 수 최대 10 + 여유

int T;                      // 테스트 케이스 수
int N;                      // 방 크기
int MAP[MAX][MAX];          // 방 정보 (1 = 사람, 2 이상 = 계단 길이)
int list[MAX];              // list[p] : p번 사람이 고른 계단 (0 = 1번 계단, 1 = 2번 계단)

struct RC                   // 좌표 한 쌍
{
	int r;                  // 행
	int c;                  // 열
};

RC people[MAX];             // 사람 좌표
int pcnt;                   // 사람 수

RC stair[2 + 3];            // stair[1], stair[2] : 계단 좌표
int stairLength[2 + 3];     // 계단 길이
int distance[2 + 3][MAX];   // distance[s][p] : p번 사람에서 s번 계단 입구까지 거리

int abs(int x)
{
	return x > 0 ? x : -x;              // 절댓값
}

int getLength(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2); // 맨해튼 거리
}

void input()
{
	pcnt = 0;                            // 사람 수 초기화

	scanf("%d", &N);                     // 방 크기 입력

	int scnt = 1; // 1번 계단 부터시작
	for (int r = 1; r <= N; r++)         // 1행부터 N행까지
	{
		for (int c = 1; c <= N; c++)     // 1열부터 N열까지
		{
			int tmp;                     // 칸 값
			scanf("%d", &tmp);           // 칸 값 입력

			MAP[r][c] = tmp;             // 방에 기록
			if (tmp == 1)        // 사람
			{
				people[pcnt].r = r;      // 사람 행
				people[pcnt++].c = c;    // 사람 열, 사람 수 증가
			}
			else if (tmp >= 2)   // 계단 (값 = 길이)
			{
				stair[scnt].r = r;       // 계단 행
				stair[scnt].c = c;       // 계단 열
				stairLength[scnt++] = tmp;   // 계단 길이, 계단 번호 증가
			}
		}
	}

	// 사람마다 두 계단까지의 거리를 미리 구해 둔다
	for (int s = 1; s <= 2; s++)             // 계단 1, 2
		for (int p = 0; p < pcnt; p++)       // 모든 사람
			distance[s][p] = getLength(people[p].r, people[p].c, stair[s].r, stair[s].c);   // 거리 저장
}

// stairNumber 계단을 고른 사람 count명(people[]에 번호)이 다 내려가는 시각
int calculate(int people[MAX], int count, int stairNumber)
{
	int time;                            // 현재 시각
	int distanceArr[11] = { 0 };         // 이 계단을 고른 사람들의 남은 거리

	// 거리 저장
	for (int i = 0; i < count; i++)
		distanceArr[i] = distance[stairNumber][people[i]];   // i번째 사람의 계단까지 거리

	// 거리 오름차순 정렬 (먼저 도착한 사람이 대기열 앞에 서도록)
	for (int i = 0; i < count - 1; i++)          // 기준 위치
	{
		for (int k = i + 1; k < count; k++)      // 기준 뒤의 모든 위치
		{
			if (distanceArr[i] > distanceArr[k]) // 뒤쪽이 더 가까우면
			{
				int tmp = distanceArr[i];        // 두 값을 맞바꾼다
				distanceArr[i] = distanceArr[k];
				distanceArr[k] = tmp;
			}
		}
	}

	int downStair[MAX]; // queue  (값 = 남은 계단 칸 수)
	int wp, rp;         // rp : 아직 다 못 내려간 첫 사람, wp : 다음에 넣을 자리

	time = 1;                            // 시각 1부터
	rp = wp = 0;                         // 빈 대기열
	while (1)                            // 모두 내려갈 때까지
	{
		time++;                          // 1분 경과

		// 1분 지남 : 모두 입구에 한 걸음 가까워진다
		for (int i = 0; i < count; i++) distanceArr[i] -= 1;

		// 입구에 막 도착한 사람은 대기열 뒤에 선다
		for (int i = 0; i < count; i++)
		{
			if (distanceArr[i] == 0)     // 이번 분에 입구 도착
				downStair[wp++] = stairLength[stairNumber];   // 남은 계단 칸 수 = 계단 길이
		}

		// 대기열 앞 3명만 계단 위에서 한 칸 내려간다
		// (i > wp 이라 i == wp 인 빈 칸도 줄이지만, 그 칸은 나중에 덮어써진다)
		for (int i = rp; i < rp + 3; i++)
		{
			if (i > wp) break;           // 대기열 끝을 넘으면 중단

			downStair[i] -= 1;           // 한 칸 내려간다
		}

		// 다 내려간 사람(0)만큼 rp를 민다. 먼저 들어간 사람이 먼저 끝나므로 0은 앞쪽에 모인다
		for (int i = rp; i < wp; i++)
			if (downStair[i] == 0) rp++; // 다 내려간 사람 수만큼 전진

		if (rp == count) break;   // 이 계단을 고른 사람이 전부 내려갔다
	}

	return time + 1;   // "도착 1분 뒤에 출발" 규칙만큼 1분을 더한다
}

// 지금 배정(list[])으로 모두 내려가는 시각 = 두 계단 중 늦은 쪽
int simulate()
{
	int peopleA[MAX] = { 0 };            // 1번 계단을 고른 사람 번호
	int peopleB[MAX] = { 0 };            // 2번 계단을 고른 사람 번호
	int acnt, bcnt, timeA, timeB;        // 각 계단 인원과 완료 시각

	acnt = bcnt = 0;                     // 인원 초기화

	for (int i = 0; i < pcnt; i++)       // 모든 사람을
	{
		if (list[i] == 0) peopleA[acnt++] = i;   // 1번 계단 쪽으로
		else peopleB[bcnt++] = i;                // 2번 계단 쪽으로
	}

	timeA = calculate(peopleA, acnt, 1); // 1번 계단 완료 시각
	timeB = calculate(peopleB, bcnt, 2); // 2번 계단 완료 시각

	return (timeA > timeB) ? timeA : timeB;   // 늦은 쪽이 전체 완료 시각
}

int MINANS = 0x7fff0000;                 // 최소 완료 시각
// L번 사람의 계단을 고른다
void DFS(int L)
{
	if (L == pcnt)   // 모두 골랐다 -> 시뮬레이션
	{
		int tmp = simulate();            // 이 배정의 완료 시각
		if (tmp < MINANS) MINANS = tmp;  // 최솟값 갱신

		return;                          // 되돌아간다
	}

	list[L] = 0;     // 1번 계단
	DFS(L + 1);      // 다음 사람

	list[L] = 1;     // 2번 계단
	DFS(L + 1);      // 다음 사람
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수 입력

	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 방 입력

		MINANS = 0x7fff0000;             // 최솟값 초기화

		DFS(0);                          // 0번 사람부터 계단 배정

		printf("#%d %d\n", tc, MINANS);  // 결과 출력
	}
	return 0;                            // 프로그램 정상 종료
}
