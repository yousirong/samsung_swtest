/*
	[코드트리] 2022 하반기 오전 1번 - 싸움땅 (연습)
	풀이 설명은 코드트리_2022_하반기오전1번_싸움땅.cpp 참고.

	■ 본 코드와 다른 점 (주석으로만 표시, 코드는 그대로)
	  - input()에서 scanf("%d", &GUN[r][c])와 if (GUN[r][c] != 0)를 쓴다.
	    GUN[r][c]는 배열이라 &GUN[r][c]는 GUN[r][c][0]과 같은 주소여서 입력은 제대로 들어간다.
	    하지만 GUN[r][c] != 0은 "배열 주소 != 0"이라 항상 참이 되어,
	    총이 없는 칸(0)도 gIndex = 1이 된다.
	    공격력 0짜리 총이 한 자루 있는 셈인데, "playerGun < 0"이 될 일이 없어 결과에는 영향이 없다.
	    본 코드처럼 GUN[r][c][0]으로 쓰는 것이 정확하다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (20+5)                       // N 최대 20 + 여유

int T;                                   // 테스트 케이스 수

int N, M, K;                             // 격자 크기, 플레이어 수, 라운드 수
int GUN[MAX][MAX][MAX*MAX];              // 칸별 총 목록
int gIndex[MAX][MAX];                    // 칸별 총 개수

struct PLAYER                            // 플레이어 정보
{
	int r;                               // 행
	int c;                               // 열
	int dir;                             // 방향
	int s;                               // 초기 능력치
	int gun;                             // 들고 있는 총 공격력
};

PLAYER player[30 + 5];                   // 플레이어 배열 (1번부터)
int SCORE[30 + 5];                       // 플레이어별 점수

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };                 // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 격자 크기, 플레이어 수, 라운드 수 입력

	for (int r = 1; r <= N; r++)         // 모든 칸의
		for (int c = 1; c <= N; c++)
			gIndex[r][c] = 0;            // 총 개수 초기화

	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
		{
			scanf("%d", &GUN[r][c]);     // 총 입력 (※ GUN[r][c][0] 자리에 들어간다)

			if (GUN[r][c] != 0) gIndex[r][c] = 1;   // ※ 배열 주소 비교라 항상 참 (결과 영향 없음)
		}
	}

	for (int m = 1; m <= M; m++)         // 플레이어 1번부터
	{
		SCORE[m] = 0;                    // 점수 초기화

		int r, c, d, s;                  // 위치, 방향, 능력치

		scanf("%d %d %d %d", &r, &c, &d, &s);   // 플레이어 정보 입력

		player[m].r = r;                 // 행 저장
		player[m].c = c;                 // 열 저장
		player[m].dir = d;               // 방향 저장
		player[m].s = s;                 // 능력치 저장
		player[m].gun = 0;               // 처음엔 총 없음
	}
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%d ", map[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

int getMaxPowerGunIndex(int r, int c)    // (r, c)에서 가장 센 총의 자리 번호
{
	int max = -1;                        // 최대 공격력
	int count = gIndex[r][c];            // 그 칸의 총 개수
	int index = 0;                       // 최대 자리 번호
	for (int i = 0; i < count; i++)      // 쌓인 총 전부
	{
		if (max < GUN[r][c][i])          // 더 센 총이면
		{
			max = GUN[r][c][i];          // 최대값 갱신
			index = i;                   // 자리 갱신
		}
	}
	return index;                        // 자리 반환
}

int battle(PLAYER p1, PLAYER p2)         // p1 승 0, p2 승 1
{
	int g1 = p1.gun;                     // p1 총
	int s1 = p1.s;                       // p1 능력치
	int g2 = p2.gun;                     // p2 총
	int s2 = p2.s;                       // p2 능력치

	int sum1 = g1 + s1;                  // p1 합
	int sum2 = g2 + s2;                  // p2 합

	if (sum1 > sum2) return 0;           // 합이 크면 p1 승
	if(sum1 < sum2) return 1;            // 합이 작으면 p2 승

	if (s1 > s2) return 0;               // 합이 같으면 능력치 비교
	if (s1 < s2) return 1;

	return -1; // for debug
}

void simulate()
{
	int changeDir[4] = { 2,3,0,1 };      // 반대 방향 표

	//1-1
	for (int k = 1; k <= K; k++)         // K라운드
	{
		int tmpMAP[MAX][MAX] = { 0 };    // 플레이어 위치 지도

		// 현재 player 의 번호를 마킹
		for (int m = 1; m <= M; m++)
		{
			int r, c;                    // 플레이어 위치

			r = player[m].r;             // 행
			c = player[m].c;             // 열

			tmpMAP[r][c] = m;            // 번호 마킹
		}

		for (int m = 1; m <= M; m++)     // 1번부터 순서대로
		{
			// 1-1 순차적으로 이동
			PLAYER p = player[m];        // 현재 플레이어 복사본

			int nr, nc, dir;             // 다음 칸, 방향

			dir = p.dir;                 // 현재 방향
			nr = p.r + dr[dir];          // 다음 행
			nc = p.c + dc[dir];          // 다음 열

			//1-1 격자 밖을 벗어나면 방향 전환
			if (nr<1 || nc<1 || nr>N || nc>N)
			{
				dir = changeDir[dir];    // 반대 방향
				player[m].dir = dir;     // 방향 저장

				nr = p.r + dr[dir];      // 다시 다음 행
				nc = p.c + dc[dir];      // 다시 다음 열

			}


			// 2-1 이동할 방향에 플레이어가 있는지 체크
			if (tmpMAP[nr][nc] == 0) //  없는 경우
			{
				tmpMAP[p.r][p.c] = 0;    // 옛 칸 비우기
				tmpMAP[nr][nc] = m;      // 새 칸 마킹

				player[m].r = nr;        // 좌표 갱신
				player[m].c = nc;

				if (gIndex[nr][nc] != 0) // 총이 있으면
				{
					int playerGun = player[m].gun;                 // 내 총
					int gunIndex = getMaxPowerGunIndex(nr, nc);    // 가장 센 총 자리

					if (playerGun < GUN[nr][nc][gunIndex])         // 더 세면 교환
					{
						int tmp = player[m].gun;                   // 내 총 임시 저장
						player[m].gun = GUN[nr][nc][gunIndex];     // 센 총 집기
						GUN[nr][nc][gunIndex] = tmp;               // 내 총 내려놓기
					}
				}
			}
			else // 2-2 player가 있는경우
			{
				int another, winner, loser;  // 상대, 승자, 패자

				another = tmpMAP[nr][nc];    // 그 칸의 플레이어

				tmpMAP[player[m].r][player[m].c] = 0;   // 내 옛 칸 비우기

				if (battle(player[m], player[another]) == 0) // m이 이긴경우
				{
					winner = m;
					loser = another;
				}
				else                     // 상대가 이긴 경우
				{
					winner = another;
					loser = m;
				}

				// 2-2-1 점수 획득
				SCORE[winner] += ((player[winner].gun + player[winner].s)) - ((player[loser].gun + player[loser].s));


				// 2-2-2 패배한 플레이어
				int loserGun = player[loser].gun;   // 패자의 총
				player[loser].gun = 0;              // 빈손

				GUN[nr][nc][gIndex[nr][nc]++] = loserGun;   // 칸에 내려놓기

				for (int i = 0; i < 4; i++)  // 시계 방향으로 갈 곳 찾기
				{
					int lnr, lnc;            // 패자의 다음 칸

					lnr = nr + dr[player[loser].dir];   // 다음 행
					lnc = nc + dc[player[loser].dir];   // 다음 열

					if (lnr<1 || lnc<1 || lnr>N || lnc>N || tmpMAP[lnr][lnc] != 0)   // 막혔으면
						player[loser].dir = (player[loser].dir + 1) % 4;             // 시계 방향 회전
					else                     // 갈 수 있으면
					{
						tmpMAP[nr][nc] = 0;          // 싸운 칸에서 빠짐
						tmpMAP[lnr][lnc] = loser;    // 새 칸 마킹

						player[loser].r = lnr;       // 패자 좌표 갱신
						player[loser].c = lnc;

						if (gIndex[lnr][lnc] != 0)   // 총이 있으면
						{
							int playerGun = player[loser].gun;               // 패자 총 (0)
							int gunIndex = getMaxPowerGunIndex(lnr, lnc);    // 가장 센 총 자리

							if (playerGun < GUN[lnr][lnc][gunIndex])         // 더 세면
							{
								player[loser].gun = GUN[lnr][lnc][gunIndex];   // 총 집기
								GUN[lnr][lnc][gunIndex] = 0;                   // 자리 비우기
							}
						}
						break;               // 이동 완료
					}
				}
				// 2-2-3. 이긴 플레이어는 더 좋은 총으로 변경
				int playerGun = player[winner].gun;            // 승자 총
				int gunIndex = getMaxPowerGunIndex(nr, nc);    // 가장 센 총 자리
				if (playerGun < GUN[nr][nc][gunIndex])         // 더 세면 교환
				{
					int tmp = player[winner].gun;              // 내 총 임시 저장
					player[winner].gun = GUN[nr][nc][gunIndex];   // 센 총 집기
					GUN[nr][nc][gunIndex] = tmp;               // 내 총 내려놓기
				}

				// 이긴 플레이어 표시
				tmpMAP[nr][nc] = winner;

				player[winner].r = nr;   // 승자 좌표 갱신
				player[winner].c = nc;
			}
		}
	}
}


int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션

		for (int m = 1; m <= M; m++)     // 플레이어별
			printf("%d ", SCORE[m]);     // 점수 출력
		putchar('\n');                   // 줄바꿈
	}

	return 0;                            // 프로그램 정상 종료
}
