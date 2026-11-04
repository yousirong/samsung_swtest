/*
	[SWEA] 5644 - [모의 SW 역량테스트] 무선 충전
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRDL1aeugDFAUo

	■ 문제 요약
	  10 x 10 지도에서 사용자 A는 (1, 1), B는 (10, 10)에서 출발해 M초(20 ~ 100) 동안 움직인다.
	  이동 방향 : 0 정지, 1 상, 2 우, 3 하, 4 좌.
	  충전기(BC) A개(1 ~ 8)가 있고, 각각 위치, 충전 범위 C(맨해튼 거리), 성능 P가 있다.
	  매초(출발 시점 포함) 각 사용자는 범위 안의 BC 하나에 접속해 충전한다.
	  두 사람이 같은 BC에 접속하면 성능을 반씩 나눈다. 충전량 합의 최댓값을 구한다.

	■ 풀이 방침 : 칸마다 "닿는 BC 목록"을 미리 만들고, 매초 두 사람의 BC 조합을 전부 본다
	  1) setBC : 각 BC의 마름모 범위에 속한 칸의 목록(MAP[r][c].bc[])에 BC 번호를 넣는다.
	  2) 매초 getCharge :
	     - 한쪽이라도 닿는 BC가 없으면 각자 가장 센 BC를 쓴다
	     - 둘 다 있으면 (A의 BC, B의 BC) 조합을 전부 보고 최댓값
	  3) 출발 시점(0초)에 한 번, 이동할 때마다 한 번 충전 -> 총 M + 1번.

	■ 같은 BC를 고른 경우 = 그 BC 성능을 한 번만 센다
	  반씩 나누면 두 사람 합은 P 그대로다. selectedBC[]에 고른 BC를 표시하고 표시된 BC 성능을 더하면,
	  같은 BC를 고른 경우 자연히 한 번만 더해진다.

	■ 주의할 점
	  - 입력의 BC 좌표는 (x, y) = (열, 행) 순서다. scanf에서 c, r 순으로 받는다.
	  - 사용자 이동 방향 배열도 상 = 1, 우 = 2, 하 = 3, 좌 = 4 순서다 (흔한 상하좌우 순서가 아니다).
	  - MAP[][].cnt는 케이스마다 0으로 비운다 (BC 목록을 새로 쌓는다).
	  - scanf 형식 끝의 "\n"은 공백을 건너뛰라는 뜻이라 결과에는 영향이 없다.
	  - abs()를 직접 만들어 쓴다 (stdlib.h를 include하지 않았다).
*/
#include <stdio.h>             // scanf, printf 사용

#define MAX (10+5)             // 지도 10 + 여유

int T;                 // 테스트 케이스 수
int M, A;              // 이동 시간, BC 개수

// 한 칸에 닿는 BC 목록
struct BC
{
	int bc[10];        // BC 번호들
	int cnt;           // 닿는 BC 개수
};

BC MAP[MAX][MAX];      // 칸마다 닿는 BC 목록
int power[10];         // power[i] : i번 BC 성능

struct RC              // 좌표 한 쌍
{
	int r;             // 행
	int c;             // 열
};

RC personA, personB;           // 두 사용자의 현재 위치
int directionA[100 + 20];      // A의 매초 이동 방향
int directionB[100 + 20];      // B의 매초 이동 방향

// -, 상, 우, 하, 좌
int dr[] = { 0,-1,0,1,0 };     // 방향별 행 변화량
int dc[] = { 0,0,1,0,-1 };     // 방향별 열 변화량

int abs(int x)
{
	return x > 0 ? x : -x;              // 절댓값
}

int getLength(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2); // 맨해튼 거리
}

// (r, c)에서 거리 length 이내인 칸들에 bcNum을 등록한다
void setBC(int r, int c, int length, int bcNum)
{
	int sr = r - length;                 // 범위 상자 위쪽 행
	int sc = c - length;                 // 범위 상자 왼쪽 열
	int er = r + length;                 // 범위 상자 아래쪽 행
	int ec = c + length;                 // 범위 상자 오른쪽 열

	for (int rr = sr; rr <= er; rr++)        // 상자 안 모든 행
	{
		for (int cc = sc; cc <= ec; cc++)    // 상자 안 모든 열
		{
			if (rr < 1 || cc < 1 || rr>10 || cc>10) continue;   // 지도 밖
			int len = getLength(r, c, rr, cc);                  // BC까지 거리
			if (len <= length) MAP[rr][cc].bc[MAP[rr][cc].cnt++] = bcNum;   // 마름모 안
		}
	}
}

void input()
{
	for (int r = 1; r <= 10; r++)        // 모든 칸의
		for (int c = 1; c <= 10; c++)
			MAP[r][c].cnt = 0;           // BC 목록 비우기

	scanf("%d %d", &M, &A);              // 이동 시간, BC 개수 입력

	for (int i = 0; i < M; i++) scanf("%d", &directionA[i]);   // A의 이동 방향
	for (int i = 0; i < M; i++) scanf("%d", &directionB[i]);   // B의 이동 방향

	for (int i = 1; i <= A; i++)         // BC A개
	{
		int c, r, range, p;              // 열, 행, 범위, 성능

		scanf("%d %d %d %d\n", &c, &r, &range, &p);   // (x, y) = (열, 행)

		setBC(r, c, range, i);           // 범위 안 칸에 BC 등록
		power[i] = p;                    // 성능 저장
	}
}

// 지금 위치에서 두 사람이 얻을 수 있는 충전량 합의 최댓값
int getCharge()
{
	int ar, ac, br, bc;                  // A, B의 현재 좌표
	int bcCountA, bcCountB;              // 각자 닿는 BC 개수

	ar = personA.r; ac = personA.c;      // A 좌표
	br = personB.r; bc = personB.c;      // B 좌표

	bcCountA = MAP[ar][ac].cnt;          // A가 닿는 BC 개수
	bcCountB = MAP[br][bc].cnt;          // B가 닿는 BC 개수

	// 한쪽이라도 닿는 BC가 없으면 겹칠 일이 없다 -> 각자 가장 센 BC
	if (bcCountA == 0 || bcCountB == 0)
	{
		int maxChargeA, maxChargeB;      // 각자 최대 충전량

		maxChargeA = 0;                  // A 최대 충전량 초기화

		for (int a = 0; a < bcCountA; a++)       // A가 닿는 BC 중
		{
			if (maxChargeA < power[MAP[ar][ac].bc[a]])
				maxChargeA = power[MAP[ar][ac].bc[a]];   // 가장 센 성능
		}

		maxChargeB = 0;                  // B 최대 충전량 초기화
		for (int b = 0; b < bcCountB; b++)       // B가 닿는 BC 중
		{
			if (maxChargeB < power[MAP[br][bc].bc[b]])
				maxChargeB = power[MAP[br][bc].bc[b]];   // 가장 센 성능
		}
		return maxChargeA + maxChargeB;  // 두 사람 합
	}

	int maxCharge = 0;                   // 조합 중 최대 충전량

	// (A의 BC, B의 BC) 조합을 전부 본다
	for (int a = 0; a < bcCountA; a++)
	{
		for (int b = 0; b < bcCountB; b++)
		{
			int selectedBC[10] = { 0 };          // 이번 조합에서 고른 BC 표시

			selectedBC[MAP[ar][ac].bc[a]] = 1;   // A가 고른 BC
			selectedBC[MAP[br][bc].bc[b]] = 1;   // 같은 BC면 표시가 하나뿐 -> 성능을 한 번만 센다

			int charge = 0;                      // 이번 조합의 충전량
			for (int i = 1; i <= 8; i++)
				if (selectedBC[i]) charge += power[i];   // 고른 BC 성능 합
			if (maxCharge < charge) maxCharge = charge;  // 최댓값 갱신
		}
	}

	return maxCharge;                    // 최대 충전량 반환
}

int simulate()
{
	int charge = 0;                      // 총 충전량

	personA.r = 1; personA.c = 1;        // A는 (1, 1)에서 출발
	personB.r = 10; personB.c = 10;      // B는 (10, 10)에서 출발

	for (int i = 0; i < M; i++)          // M초 동안
	{
		charge += getCharge();   // 이동 전 위치에서 충전 (0초 포함)

		personA.r = personA.r + dr[directionA[i]];   // A 행 이동
		personA.c = personA.c + dc[directionA[i]];   // A 열 이동

		personB.r = personB.r + dr[directionB[i]];   // B 행 이동
		personB.c = personB.c + dc[directionB[i]];   // B 열 이동
	}

	charge += getCharge();       // 마지막 위치에서 한 번 더 (총 M + 1번)

	return charge;               // 총 충전량 반환
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수 입력
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		printf("#%d %d\n", tc, simulate());   // 결과 출력
	}
	return 0;                            // 프로그램 정상 종료
}
