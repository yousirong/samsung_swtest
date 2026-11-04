/*
	[SWEA] 2382 - [모의 SW 역량테스트] 미생물 격리
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AV597vbqAH0DFAVl

	■ 문제 요약
	  N x N 셀(5 <= N <= 100)의 가장자리 칸에는 약품이 칠해져 있다. 미생물 군집 K개가 있고,
	  군집마다 위치, 미생물 수, 이동 방향(1 상, 2 하, 3 좌, 4 우)이 주어진다.
	  1시간마다 모든 군집이 한 칸 움직인다.
	    - 약품 칸에 도착하면 미생물 수가 절반(나머지 버림)이 되고 방향이 반대로 바뀐다. 0이 되면 사라진다.
	    - 여러 군집이 한 칸에 모이면 합쳐진다. 수는 합, 방향은 "가장 많았던 군집"의 방향.
	  M시간 뒤 남은 미생물 수의 총합을 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 격자 두 장(MAP -> nextMAP)으로 동시에 움직이기
	  모든 군집이 "동시에" 움직이므로, 지금 상태(MAP)를 읽어 다음 상태(nextMAP)에 쓴다.
	  다 옮긴 뒤 nextMAP을 MAP으로 복사하고 nextMAP을 비운다.

	  한 칸에 여럿이 모일 때 방향을 정하려면 "이 칸에 온 군집 중 가장 컸던 수"를 알아야 한다.
	  합친 수(num)와 따로 maxNum에 그 최댓값을 들고 다닌다.
	      - 빈 칸이면 그대로 놓는다 (maxNum = 자기 수)
	      - 이미 있는 maxNum이 더 크면 수만 더한다
	      - 내가 더 크면 maxNum과 방향을 내 것으로 바꾸고 수를 더한다
	  한 시간이 끝나면 maxNum을 합친 수로 맞춰 둔다 (다음 시간에는 합쳐진 군집 하나로 본다).

	■ 주의할 점
	  - 약품 칸(가장자리)에서는 군집이 만나지 않는다. 처음엔 가장자리에 군집이 없고,
	    가장자리에 도착한 군집은 바로 방향을 바꿔 다음 시간에 안쪽으로 나간다.
	    그래서 비교에 절반으로 줄기 전 수(currentNum)를 써도 결과가 같다.
	  - 절반이 되어 0이 된 군집도 nextMAP에 복사되지만 num == 0이라 "빈 칸"으로 취급된다.
	    다른 군집이 오면 덮어쓰고, 아무도 안 오면 0이라 합계에 영향이 없다 -> 사라진 것과 같다.
	  - 같은 수의 군집이 합쳐지는 경우는 주어지지 않는다 (문제 조건). 그래서 크다/작다만 본다.
	  - changeDir[]로 반대 방향을 바로 찾는다 : 상 <-> 하, 좌 <-> 우.
	  - nextMAP은 매 시간 끝에 비우므로 다음 케이스도 깨끗한 상태로 시작한다.
*/
#include <stdio.h>              // scanf, printf 사용

#define MAX (100+10)            // N 최대 100 + 여유

int T;                  // 테스트 케이스 수
int N, M, K;            // 셀 크기, 격리 시간, 군집 수

struct BIO
{
	int dir;            // 이동 방향 (1 ~ 4)
	int num;            // 미생물 수 (합쳐졌다면 합)
	int maxNum;         // 이 칸에 모인 군집 중 가장 컸던 수 (방향을 정하는 데 쓴다)
};

BIO MAP[MAX][MAX];      // 지금 상태
BIO nextMAP[MAX][MAX];  // 한 시간 뒤 상태

// 상, 하, 좌, 우
int dr[] = {0, -1,1,0,0 };      // 방향별 행 변화량 (0번은 사용 안 함)
int dc[] = { 0,0,0,-1,1 };      // 방향별 열 변화량 (0번은 사용 안 함)
int changeDir[5] = { 0,2,1,4,3 };   // 반대 방향

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 셀 크기, 격리 시간, 군집 수 입력

	BIO init = { 0 };                    // 빈 칸 값
	for (int r = 0; r < N; r++)          // 모든 행
		for (int c = 0; c < N; c++)      // 모든 열
			MAP[r][c] = init;            // 빈 칸으로 초기화

	for (int i = 0; i < K; i++)          // 군집 K개 입력
	{
		int r, c, num, dir;              // 위치, 미생물 수, 방향

		scanf("%d %d %d %d", &r, &c, &num, &dir);   // 군집 정보 입력

		MAP[r][c].num = num;             // 미생물 수
		MAP[r][c].maxNum = num;          // 처음엔 자기 수가 최대
		MAP[r][c].dir = dir;             // 이동 방향
	}
}

int simulate()
{
	for (int m = 0; m < M; m++)          // M시간 반복
	{
		for (int r = 0; r < N; r++)      // 모든 행
		{
			for (int c = 0; c < N; c++)  // 모든 열
			{
				// 미생물이 없으면 continue
				if (MAP[r][c].num == 0) continue;

				int currentNum = MAP[r][c].num;   // 절반이 되기 전 수 (합칠 때 비교용)

				int nr, nc;              // 이동할 칸

				nr = r + dr[MAP[r][c].dir];   // 이동할 행
				nc = c + dc[MAP[r][c].dir];   // 이동할 열

				// 약품이 칠해진 구역 미생물 수 감소 및 방향 변경
				if (nr == 0 || nr == N - 1 || nc == 0 || nc == N - 1)
				{
					MAP[r][c].num /= 2;                          // 수가 절반으로
					MAP[r][c].dir = changeDir[MAP[r][c].dir];    // 방향 반대로
				}

				// 빈공간이면 이동
				if (nextMAP[nr][nc].num == 0) nextMAP[nr][nc] = MAP[r][c];
				// 이동할려는 장소의 maxNum이 더 큰경우 미생물의 수만 증가
				else if (nextMAP[nr][nc].maxNum > currentNum) nextMAP[nr][nc].num += MAP[r][c].num;
				else // 자신이 더 큰경우 max 변경 및 방향 전환 미생물추가
				{
					nextMAP[nr][nc].maxNum = currentNum;     // 가장 큰 군집 수 갱신
					nextMAP[nr][nc].dir = MAP[r][c].dir;     // 방향은 큰 군집을 따른다
					nextMAP[nr][nc].num += MAP[r][c].num;    // 수는 합친다
				}
			}
		}

		// move 종료
		// nextMAP -> MAP 복사. 합쳐진 군집은 이제 하나이므로 maxNum을 합친 수로 맞춘다.
		BIO init = { 0 };                // 빈 칸 값
		for (int r = 0; r < N; r++)
		{
			for (int c = 0; c < N; c++)
			{
				MAP[r][c] = nextMAP[r][c];           // 다음 상태를 현재로
				MAP[r][c].maxNum = MAP[r][c].num;    // 합쳐진 군집 하나로 본다
				nextMAP[r][c] = init;                // 다음 상태 판 비우기
			}
		}
	}

	// 남은 미생물 수의 합
	int sum = 0;
	for (int r = 0; r < N; r++)          // 모든 행
		for (int c = 0; c < N; c++)      // 모든 열
			sum += MAP[r][c].num;        // 미생물 수 누적

	return sum;                          // 총합 반환
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수 입력
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력


		printf("#%d %d\n", tc, simulate());   // M시간 뒤 총합 출력
	}
}
