/*
	[코드트리] 2022 하반기 오전 1번 - 싸움땅

	[프로그래머스 제출용]  원본 : swtest/코드트리_2022_하반기오전1번_싸움땅.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
	https://www.codetree.ai/training-field/frequent-problems/problems/battle-ground

	■ 문제 요약
	  N x N 격자의 각 칸에는 총이 있을 수도 있고(공격력이 적혀 있다) 없을 수도 있다(0).
	  플레이어 M명은 (행, 열, 방향, 초기 능력치)로 주어지고 처음에는 총이 없다(공격력 0).
	  방향은 0 상, 1 우, 2 하, 3 좌다.

	  K라운드 동안 매 라운드 1번부터 M번 순서로 아래를 진행한다.

	    1) 이동 : 자기 방향으로 한 칸 간다. 격자 밖이면 방향을 반대로 바꾸고 그 방향으로 간다.
	    2) 간 칸에 플레이어가 없으면
	       - 총이 있으면 그중 공격력이 가장 센 총을 집는다.
	         이미 총을 들고 있었다면 (더 센 총일 때만 바꾸고) 원래 총은 그 칸에 내려놓는다.
	    3) 간 칸에 플레이어가 있으면 싸운다
	       - (총 공격력 + 초기 능력치)가 큰 쪽이 이기고, 같으면 초기 능력치가 큰 쪽이 이긴다.
	       - 이긴 사람은 두 값의 차이만큼 점수를 얻는다.
	       - 진 사람은 들고 있던 총을 그 칸에 내려놓고,
	         자기가 가던 방향부터 시계 방향으로 돌며 "격자 안이면서 플레이어가 없는" 칸으로 한 칸 간다.
	         그 칸에 총이 있으면 가장 센 총을 집는다.
	       - 이긴 사람은 그 칸에 남아, 떨어져 있는 총(진 사람이 놓고 간 것 포함) 중
	         가장 센 것을 집고 자기 총은 내려놓는다.

	  K라운드가 끝난 뒤 각 플레이어의 점수를 1번부터 순서대로 출력한다.

	■ 풀이 방침
	  - 한 칸에 총이 여러 자루 쌓일 수 있다. 그래서 GUN[r][c][i]를 "그 칸의 i번째 총",
	    gIndex[r][c]를 "그 칸에 쌓인 총의 개수"로 둔다. 내려놓기는 gIndex를 늘려 뒤에 붙이고,
	    집기는 가장 센 자리(getMaxPowerGunIndex)와 자기 총을 맞바꾼다.
	    맞바꾸면 개수가 그대로라 gIndex를 건드릴 필요가 없다.
	  - "그 칸에 플레이어가 있는가"는 매 라운드 시작 때 tmpMAP에 번호를 찍어 두고 본다.
	    이동할 때마다 옛 칸을 0으로, 새 칸을 자기 번호로 갱신하므로 항상 최신 상태가 된다.
	  - 이동과 싸움은 1번부터 차례대로 처리한다. 같은 라운드 안에서도 앞 번호가 만든 결과를
	    뒷 번호가 그대로 본다(동시에 움직이는 문제가 아니다).

	■ 순서가 중요한 부분
	  싸움이 끝난 뒤 처리 순서가 문제 조건과 같아야 한다.

	    진 사람의 총을 그 칸에 내려놓기  ->  진 사람이 밀려나며 (총이 있으면) 집기
	                                    ->  이긴 사람이 그 칸의 총 중 가장 센 것 집기

	  이 순서라서 "진 사람이 놓고 간 총을 이긴 사람이 주워 갈 수 있다"가 성립한다.

	■ 주의할 점
	  [확인 필요] 진 사람이 네 방향 모두 막혀 밀려나지 못하면 그 칸에 이긴 사람과 함께 남는다.
	              (for (int i = 0; i < 4; i++) 루프가 그냥 끝난다)
	              문제 조건상 그런 입력이 없다고 보고 따로 처리하지 않았다.
*/
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (20+5)

int T;
int N, M, K;

// GUN[r][c][i] : (r, c)에 쌓인 i번째 총의 공격력, gIndex[r][c] : 그 칸에 쌓인 총의 개수
int GUN[MAX][MAX][MAX * MAX];
int gIndex[MAX][MAX];

struct PLAYER
{
	int r;
	int c;
	int dir;
	int s; // 초기 능력치
	int gun;
};

PLAYER player[30 + 5];

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

int SCORE[30 + 5];

void input(int k, const vector<vector<int>>& guns, const vector<vector<int>>& players)
{
	N = (int)guns.size();          // [수정] scanf("%d %d %d", &N, &M, &K) 대체
	M = (int)players.size();
	K = k;

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			gIndex[r][c] = 0;

	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			GUN[r][c][0] = guns[r - 1][c - 1];   // [수정] scanf 대체

			if (GUN[r][c][0] != 0) gIndex[r][c] = 1;
		}
	}

	for (int m = 1; m <= M; m++) // player 번호는 1번 부터
	{
		SCORE[m] = 0;

		int r = players[m - 1][0];   // [수정] scanf 대체
		int c = players[m - 1][1];
		int d = players[m - 1][2];
		int s = players[m - 1][3];

		player[m].r = r;
		player[m].c = c;
		player[m].dir = d;
		player[m].s = s;
		player[m].gun = 0; // 초기화

	}
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

// (r, c)에 쌓인 총 중 공격력이 가장 센 것의 자리 번호.
// 총이 없으면 0을 돌려주는데, 부르는 쪽에서 gIndex로 미리 걸러 준다.
int getMaxPowerGunIndex(int r, int c)
{
	int max = -1;
	int count = gIndex[r][c];
	int index = 0;
	for (int i = 0; i < count; i++)
	{
		if (max < GUN[r][c][i])
		{
			max = GUN[r][c][i];
			index = i;
		}
	}
	return index;
}

// p1이 이기면 0, p2가 이기면 1
// (총 공격력 + 초기 능력치)가 크면 승, 같으면 초기 능력치가 큰 쪽이 승
int battle(PLAYER p1, PLAYER p2)
{
	int g1 = p1.gun;
	int s1 = p1.s;
	int g2 = p2.gun;
	int s2 = p2.s;

	int sum1 = g1 + s1;
	int sum2 = g2 + s2;

	if (sum1 > sum2) return 0;
	if (sum1 < sum2) return 1;

	if (s1 > s2) return 0;
	if (s1 < s2) return 1;

	return -1; // for debug
			   // 문제 조건상 (합, 능력치)가 모두 같은 두 사람은 없다.
}

void simulate()
{
	int changeDir[4] = { 2,3,0,1 }; // 반대 방향 (상<->하, 우<->좌)

	for (int k = 0; k < K; k++)
	{
		// 이번 라운드의 "누가 어디 있는지" 지도. 이동할 때마다 같이 갱신한다.
		int tmpMAP[MAX][MAX] = { 0 };

		// 현재 player의 번호를 마킹
		for (int m = 1; m <= M; m++)
		{
			int r, c;

			r = player[m].r;
			c = player[m].c;

			tmpMAP[r][c] = m;
		}

		for (int m = 1; m <= M; m++)
		{
			// 1-1. 플레이어 순차적으로 이동
			PLAYER p = player[m];

			int nr, nc, dir;

			dir = p.dir;
			nr = p.r + dr[dir];
			nc = p.c + dc[dir];

			// 1-1. 격자를 벗어나는 경우, 반대 방향으로 변경 후 이동
			// 바뀐 방향은 계속 유지된다.
			if (nr<1 || nc<1 || nr>N || nc>N)
			{
				dir = changeDir[dir];
				player[m].dir = dir;

				nr = p.r + dr[dir];
				nc = p.c + dc[dir];
			}

			// 2-1. 이동할 방향에 플레이어가 있는지 체크,
			if (tmpMAP[nr][nc] == 0) // 없는 경우
			{
				// tmpMAP에서 이동
				tmpMAP[p.r][p.c] = 0;
				tmpMAP[nr][nc] = m;

				// player 좌표 갱신
				player[m].r = nr;
				player[m].c = nc;

				if (gIndex[nr][nc] != 0) // 총이 있는 경우
				{
					int playerGun = player[m].gun;
					int gunIndex = getMaxPowerGunIndex(nr, nc);

					// 더 센 총일 때만 바꾼다. 내 총은 그 자리에 내려놓으므로
					// 칸에 쌓인 총의 개수(gIndex)는 그대로다.
					if (playerGun < GUN[nr][nc][gunIndex])
					{
						int tmp = player[m].gun;
						player[m].gun = GUN[nr][nc][gunIndex];
						GUN[nr][nc][gunIndex] = tmp;

					}
				}
			}
			else // 2-2. player가 있는 경우
			{
				int another, winner, loser;

				another = tmpMAP[nr][nc];

				// 이동한 플레이어 0으로 갱신
				// (싸움 결과에 따라 이 사람이 그 칸에 남거나 밀려나므로, 옛 칸은 먼저 비운다)
				tmpMAP[player[m].r][player[m].c] = 0;

				if (battle(player[m], player[another]) == 0) // m이 이긴 경우
				{
					winner = m;
					loser = another;
				}
				else
				{
					winner = another;
					loser = m;
				}

				// 2-2-1. 점수 획득 : (이긴 쪽 합) - (진 쪽 합)
				SCORE[winner] += ((player[winner].gun + player[winner].s)) -
					((player[loser].gun + player[loser].s));

				// 2-2-2. 패배한 플레이어
				// 먼저 자기 총을 그 칸에 내려놓는다 (뒤에 붙이므로 개수가 하나 늘어난다).
				int loserGun = player[loser].gun;
				player[loser].gun = 0;

				GUN[nr][nc][gIndex[nr][nc]++] = loserGun;

				// 자기 방향부터 시계 방향으로 돌며 갈 수 있는 칸을 찾는다.
				// dr/dc가 상 우 하 좌 순서라 +1이 곧 시계 방향이다.
				for (int i = 0; i < 4; i++)
				{
					int lnr, lnc;

					lnr = nr + dr[player[loser].dir];
					lnc = nc + dc[player[loser].dir];

					if (lnr<1 || lnc<1 || lnr>N || lnc>N || tmpMAP[lnr][lnc] != 0)
						player[loser].dir = (player[loser].dir + 1) % 4;
					else
					{
						tmpMAP[nr][nc] = 0;
						tmpMAP[lnr][lnc] = loser;

						player[loser].r = lnr;
						player[loser].c = lnc;

						if (gIndex[lnr][lnc] != 0) // 총이 있는 경우
						{
							int playerGun = player[loser].gun;
							int gunIndex = getMaxPowerGunIndex(lnr, lnc);

							// 진 사람은 빈손(0)이라 무조건 집는다.
							// 집은 자리는 0으로 두어 "빈 자리"로 남긴다.
							if (playerGun < GUN[lnr][lnc][gunIndex])
							{
								player[loser].gun = GUN[lnr][lnc][gunIndex];
								GUN[lnr][lnc][gunIndex] = 0;
							}

						}
						break;
					}
				}

				// 2-2-3. 이긴 플레이어는 더 좋은 총으로 변경
				// 진 사람이 놓고 간 총도 후보에 들어 있다.
				int playerGun = player[winner].gun;
				int gunIndex = getMaxPowerGunIndex(nr, nc);

				if (playerGun < GUN[nr][nc][gunIndex])
				{
					int tmp = player[winner].gun;
					player[winner].gun = GUN[nr][nc][gunIndex];
					GUN[nr][nc][gunIndex] = tmp;
				}

				// 이긴 사람이 그 칸을 차지한다.
				tmpMAP[nr][nc] = winner;

				player[winner].r = nr;
				player[winner].c = nc;
			}
		}
	}

}

// [수정] main() -> solution(). 플레이어별 점수를 1번부터 순서대로 담아 반환한다.
vector<int> solution(int k, vector<vector<int>> guns, vector<vector<int>> players)
{
	input(k, guns, players);

	simulate();

	// [수정] printf -> 반환. 점수는 이미 SCORE[1..M] 에 있다.

	return vector<int>(SCORE + 1, SCORE + 1 + M);
}
