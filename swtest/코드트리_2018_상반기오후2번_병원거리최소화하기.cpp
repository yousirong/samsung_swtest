/*
	[코드트리] 2018 상반기 오후 2번 - 병원 거리 최소화하기
	https://www.codetree.ai/training-field/frequent-problems/problems/min-of-hospital-distance

	■ 문제 요약
	  N x N 격자에 사람(1)과 병원(2)이 놓여 있다.
	  병원 중 정확히 M개만 남기고 나머지는 폐업시킨다.

	  "도시의 병원 거리"는 각 사람마다 가장 가까운 살아남은 병원까지의 거리를 구해 모두 더한 값이고,
	  거리는 맨해튼 거리 |r1-r2| + |c1-c2| 다.
	  도시의 병원 거리를 최소로 만들었을 때 그 값을 출력한다.

	  (백준 15686 "치킨 배달"과 같은 문제다)

	■ 풀이 방침 : 병원 조합 완전탐색
	  병원은 최대 13개뿐이라 M개를 고르는 조합이 많아야 13C6 = 1,716가지다.
	  전부 시도해도 전혀 부담이 없다.

	    1) 입력을 받으며 사람과 병원의 좌표를 각각 목록으로 모아 둔다
	    2) DFS로 병원 목록에서 M개를 고르는 조합을 만든다
	    3) 각 조합마다 모든 사람에 대해 "선택된 병원 중 최소 거리"를 구해 더한다
	    4) 그 합의 최솟값을 갱신한다

	■ 구현 포인트
	  1) 격자를 매번 훑을 필요가 없다. 필요한 것은 좌표뿐이므로
	     입력 단계에서 person[], hospital[] 목록으로 펴 두면
	     이후 계산이 "좌표 목록 두 개의 거리 계산"으로 단순해진다.
	  2) 이동 경로가 아니라 맨해튼 거리를 쓰므로 BFS가 필요 없다.
	     벽이나 장애물 개념이 없어서 두 점 사이 거리가 공식 하나로 나온다.
	  3) 조합이므로 DFS의 다음 시작값을 i + 1로 넘긴다.
	  4) 사람마다 최솟값을 구해야 하므로 minDistance는 INF에서 시작한다.
	     (문제 조건상 병원이 최소 M개는 있으므로 갱신되지 않는 경우는 없다)
*/

#include <stdio.h>                 // scanf, printf, putchar 사용

#define MAX (50 + 10)              // N 최대 50 + 여유
#define MAX_HOSPITAL (13 + 5)      // 병원 최대 13개 + 여유
#define INF (0x7fff0000)           // 아주 큰 값

// 칸의 의미
#define PERSON   (1)               // 사람
#define HOSPITAL (2)               // 병원

int T;                     // 테스트 케이스 수

int N, M;                  // N: 격자 크기, M: 남길 병원 수
int MAP[MAX][MAX];         // 격자 (좌표 수집이 끝나면 사실상 쓰지 않는다)

int num_of_cases[MAX];     // 고른 병원의 인덱스들

// 좌표 한 쌍
struct RC
{
	int r;                 // 행
	int c;                 // 열
};

typedef struct RC RC;      // struct RC를 RC로 줄여 쓰기

RC person[MAX * 2];               // 사람 좌표 목록
RC hospital[MAX_HOSPITAL];        // 병원 좌표 목록
int pcnt, hcnt;                   // 사람 수, 병원 수

int minAnswer;                    // 도시의 병원 거리 최솟값

// ---------------------------
// 입력 (읽으면서 좌표 목록도 함께 만든다)
// ---------------------------
void input()
{
	scanf("%d %d", &N, &M);              // 격자 크기, 남길 병원 수 입력

	pcnt = hcnt = 0;                     // 개수 초기화

	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
		{
			scanf("%d", &MAP[r][c]);     // 칸 입력

			if (MAP[r][c] == PERSON)     // 사람이면
			{
				person[pcnt].r = r;      // 사람 목록에 행
				person[pcnt++].c = c;    // 사람 목록에 열, 개수 증가
			}
			else if (MAP[r][c] == HOSPITAL)   // 병원이면
			{
				hospital[hcnt].r = r;    // 병원 목록에 행
				hospital[hcnt++].c = c;  // 병원 목록에 열, 개수 증가
			}
		}
	}
}

// ---------------------------
// 디버그용 격자 출력
// ---------------------------
void printMap()
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%d ", MAP[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

// ---------------------------
// 디버그용: 고른 병원 인덱스 출력
// ---------------------------
void printCases()
{
	for (int i = 0; i < M; i++)          // 고른 M개
		printf("%d ", num_of_cases[i]);  // 인덱스 출력
	putchar('\n');                       // 줄바꿈
}

// ---------------------------
// 절댓값 (stdlib.h 없이 직접 구현)
// ---------------------------
int abs(int x)
{
	return (x > 0) ? x : -x;             // 양수면 그대로, 아니면 부호 반전
}

// ---------------------------
// 맨해튼 거리 : |r1-r2| + |c1-c2|
// ---------------------------
int getDistance(int r1, int c1, int r2, int c2)
{
	return abs(r1 - r2) + abs(c1 - c2);  // 행 차이 + 열 차이
}

// ---------------------------
// 현재 조합에서의 도시의 병원 거리
//
//   1) 고른 병원에 표시
//   2) 사람마다 표시된 병원까지의 거리 중 최솟값을 찾는다
//   3) 그 최솟값들을 모두 더한다
// ---------------------------
int calculate()
{
	bool selectedHospital[MAX_HOSPITAL] = { 0 };   // 이번 조합에서 남긴 병원 표시

	for (int i = 0; i < M; i++)          // 고른 M개
		selectedHospital[num_of_cases[i]] = true;   // 표시

	int sum = 0;                         // 도시의 병원 거리

	for (int i = 0; i < pcnt; i++)       // 사람마다
	{
		int minDistance = INF;           // 가장 가까운 병원까지 거리

		// 선택된 병원만 후보로 본다
		for (int k = 0; k < hcnt; k++)
		{
			if (selectedHospital[k] == false)   // 폐업한 병원은
				continue;                       // 건너뛴다

			int distance = getDistance(         // 사람과 병원 사이 거리
				person[i].r, person[i].c,
				hospital[k].r, hospital[k].c
			);

			minDistance = (distance < minDistance) ? distance : minDistance;   // 최솟값 갱신
		}

		sum += minDistance;              // 이 사람의 병원 거리 더하기
	}

	return sum;                          // 합 반환
}

// ---------------------------
// DFS : 병원 목록에서 M개를 고르는 조합 생성
//
//   depth : 지금까지 고른 개수
//   start : 이번에 고를 수 있는 최소 인덱스
// ---------------------------
void DFS(int depth, int start)
{
	// M개를 다 골랐으면 그 조합의 값을 계산
	if (depth == M)
	{
		// printCases();

		int tmp = calculate();           // 이 조합의 병원 거리
		if (tmp < minAnswer)             // 더 작으면
			minAnswer = tmp;             // 최솟값 갱신

		return;                          // 되돌아간다
	}

	for (int i = start; i < hcnt; i++)   // start부터 병원 후보
	{
		num_of_cases[depth] = i;         // i번 병원을 남긴다

		// 조합이므로 다음은 i보다 뒤에서만 고른다
		DFS(depth + 1, i + 1);
	}
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;

	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		minAnswer = INF;                 // 최솟값 초기화

		DFS(0, 0);                       // 조합 탐색 시작

		printf("%d\n", minAnswer);       // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
