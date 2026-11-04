/*
	[SWEA] 5653 - [모의 SW 역량테스트] 줄기세포배양
	https://swexpertacademy.com/main/code/problem/problemDetail.do?contestProbId=AWXRJ8EKe48DFAUo

	■ 문제 요약
	  N x M (1 ~ 50) 초기 배양 용기에 생명력 X(1 ~ 10)인 줄기 세포들이 있다. 배양 용기는 무한히 넓다.
	    - 세포는 X시간 동안 비활성, 그다음 X시간 동안 활성, 그 뒤 죽는다 (죽어도 칸은 차지한다)
	    - 활성이 된 첫 1시간에 상하좌우 빈 칸으로 번식한다. 새 세포의 생명력도 X이고 비활성부터 시작
	    - 여러 세포가 같은 칸에 동시에 번식하려 하면 생명력이 큰 쪽이 차지한다
	  K시간(1 ~ 300) 뒤 살아 있는(비활성 + 활성) 세포 수를 구한다. 테스트 케이스가 T개.

	■ 풀이 방침 : 세포를 상태별 목록 3개로 나눠 1시간씩 시뮬레이션
	  cell[]     : 비활성 세포. time = 남은 비활성 시간
	  deadCell[] : 활성 세포 (이름은 dead지만 "곧 죽을" 활성 세포다). life = 남은 활성 시간
	  MAP        : 세포가 한 번이라도 차지한 칸 (죽은 세포 포함) -> 번식할 수 없는 칸

	  매 시간
	    1) 비활성 세포마다
	       - time > 0  : time-- 하고 그대로 비활성으로 남긴다 (liveCell)
	       - time == 0 : 이번 시간에 활성이 되어 번식한다
	                     deadCell(활성 목록)에 넣고, 상하좌우 빈 칸을 spreadCell(번식 후보)에 넣는다
	    2) 활성 세포의 life를 1 줄이고, 0이 되면 목록에서 뺀다 (죽음)
	    3) 번식 후보를 MAP에 놓는다. 같은 칸에 여럿이면 생명력이 큰 쪽만 남긴다
	       (진 후보는 time = 0으로 표시해 4에서 거른다. indexMAP으로 그 칸을 차지한 후보 번호를 기억)
	    4) 살아남은 번식 후보를 비활성 목록에 더한다
	    5) liveCell -> cell
	  끝나면 비활성(lcnt) + 활성(dcnt)이 답.

	■ 시간 맞추기 (X = 2)
	  0시 생성 -> 1, 2시간째 비활성 -> 3시간째(time == 0) 활성 + 번식 -> 4시간째 끝에 죽음.
	  1)에서 활성 목록에 넣고 같은 시간의 2)에서 바로 life를 1 줄이므로, 활성 X시간이 정확히 맞는다.

	■ 무한히 넓은 용기 = OFFSET
	  세포는 2시간에 최대 1칸씩 퍼지므로 K = 300이면 사방으로 150칸 정도 넓어진다.
	  입력을 (250, 250)에서 시작하게 놓으면 0 ~ 499 안에서 끝난다.

	■ 주의할 점
	  - [버그] dcnt(활성 세포 수)를 케이스마다 0으로 비우지 않는다.
	           앞 케이스 끝에 활성으로 남아 있던 세포가 다음 케이스 활성 목록에 그대로 섞여,
	           life가 남아 있는 동안 답에 더해진다 (번식은 하지 않는다).
	           (재현 : 1 x 1, 생명력 5, K = 6 다음에 1 x 1, 생명력 1, K = 1
	                   -> 둘째 케이스 혼자면 1, 이어서 돌리면 2)
	           -> input()에 dcnt = 0; 을 넣으면 된다.
	           생명력이 10 이하라 K가 충분히 크면 남은 세포가 그 전에 다 죽어 우연히 맞는다.
	  - [확인 필요] indexMAP은 main() 안의 지역 배열 int [500][500] = 약 1MB 다.
	           Visual Studio 기본 스택(1MB)에서는 넘칠 수 있다. 전역으로 옮기면 안전하다.
	           (전역으로 옮기면 매 시간 0으로 비우는 것도 직접 해야 한다)
	  - MAP은 죽은 세포 자리도 0이 아니므로 번식 후보에서 자동으로 빠진다.
	  - output()은 디버그 출력용이다.
*/
#include <stdio.h>             // scanf, printf, putchar 사용

#define MAX (500)              // 퍼져 나갈 공간까지 포함한 격자 크기
#define OFFSET (250)           // 초기 용기를 격자 가운데에 놓기 위한 값

int T;                 // 테스트 케이스 수
int N, M, K;           // 초기 세로, 가로, 배양 시간
int MAP[MAX][MAX];     // 세포가 차지한 칸 (값 = 생명력). 죽은 세포도 남는다

struct CELL
{
	int r;             // 행
	int c;             // 열
	int life;          // 생명력 X (활성 목록에서는 남은 활성 시간)
	int time;          // 남은 비활성 시간
};

CELL cell[MAX * MAX];          // 비활성 세포
CELL liveCell[MAX * MAX];      // 다음 시간의 비활성 세포 (임시)
CELL spreadCell[MAX * MAX];    // 이번 시간 번식 후보
CELL deadCell[MAX * MAX];      // 활성 세포 (곧 죽는다)
int ccnt, lcnt, scnt, dcnt;    // 각 목록의 개수

// 왼쪽, 위, 오른쪽, 아래
int dr[] = { 0,-1,0,1 };       // 방향별 행 변화량
int dc[] = { -1,0,1,0 };       // 방향별 열 변화량


void input()
{
	scanf("%d %d %d", &N, &M, &K);       // 초기 세로, 가로, 배양 시간 입력

	for (int r = 0; r < MAX; r++)        // 격자 전체
		for (int c = 0; c < MAX; c++)
			MAP[r][c] = 0;               // 빈 칸으로 초기화

	ccnt = 0;   // ([버그] dcnt도 여기서 0으로 비워야 한다)
	for (int r = 0; r < N; r++)          // 초기 용기의 행
	{
		for (int c = 0; c < M; c++)      // 초기 용기의 열
		{
			int life;                    // 생명력

			scanf("%d", &life);          // 생명력 입력 (0이면 빈 칸)
			MAP[r + OFFSET][c + OFFSET] = life;   // 가운데쯤에 놓는다 (사방으로 퍼질 자리)

			if (life)                    // 세포가 있으면
			{
				cell[ccnt].r = r + OFFSET;       // 행
				cell[ccnt].c = c + OFFSET;       // 열
				cell[ccnt].life = life;          // 생명력
				cell[ccnt++].time = life;   // 처음엔 X시간 비활성
			}
		}
	}
}

void output()   // for debug
{
	for (int r = OFFSET - 5; r < OFFSET + N + 5; r++)        // 초기 용기 주변 행
	{
		for (int c = OFFSET - 5; c < OFFSET + M + 5; c++)    // 초기 용기 주변 열
			printf("%d ", MAP[r][c]);                        // 값 출력
		putchar('\n');                                       // 행 끝 줄바꿈
	}
	putchar('\n');                                           // 빈 줄
}

int main()
{
	scanf("%d", &T);                     // 테스트 케이스 수 입력
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		for (int time = 0; time < K; time++)   // K시간 반복
		{
			lcnt = scnt = 0;             // 이번 시간 목록 초기화
			int indexMAP[MAX][MAX] = { 0 };   // 이번 시간에 그 칸을 차지한 번식 후보 번호 ([확인 필요] 약 1MB 지역 배열)
			for (int i = 0; i < ccnt; i++)    // 비활성 세포마다
			{
				if (cell[i].time)
				{
					// 1-1 cell을 보고 time이 0이 아닌경우, liveCell에 cell 추가
					cell[i].time--;              // 비활성 시간 1 감소
					liveCell[lcnt++] = cell[i];  // 계속 비활성
				}
				else
				{
					// 1-2 time이 0인 경우는 deadCell에 추가
					deadCell[dcnt] = cell[i];    // 활성 목록에 추가
					dcnt++;                      // 활성 세포 수 증가

					// 1-3 time이 0인경우, 4방향 탐색해서, MAP에 빈칸인 경우
					// spreadCell에 추가
					for (int dir = 0; dir < 4; dir++)
					{
						int nr, nc;              // 번식할 칸

						nr = cell[i].r + dr[dir];   // 번식할 행
						nc = cell[i].c + dc[dir];   // 번식할 열

						if (MAP[nr][nc])continue;   // 이미 누가 차지한 칸 (죽은 세포 포함)

						spreadCell[scnt].r = nr;                 // 후보 행
						spreadCell[scnt].c = nc;                 // 후보 열
						spreadCell[scnt].life = cell[i].life;    // 생명력 물려받음
						spreadCell[scnt++].time = cell[i].life;  // 비활성부터 시작
					}
				}
			}
			// 2 deadCell의 life 감소, life가 남아있으면 다시 deadCell에 추가
			int ddcnt = 0;                       // 살아남은 활성 세포 수
			for (int i = 0; i < dcnt; i++)       // 활성 세포마다
			{
				deadCell[i].life--;              // 남은 활성 시간 1 감소
				if (deadCell[i].life)            // 아직 남았으면
					deadCell[ddcnt++] = deadCell[i];   // 목록에 남긴다
			}

			dcnt = ddcnt;                        // 활성 세포 수 갱신

			for (int i = 0; i < scnt; i++)       // 번식 후보마다
			{
				int nr, nc;                      // 후보 칸

				nr = spreadCell[i].r;            // 후보 행
				nc = spreadCell[i].c;            // 후보 열

				// 3-1 MAP이 빈칸이라면, spreadCell의 life를 MAP에 표시
				if (MAP[nr][nc] == 0)
				{
					MAP[nr][nc] = spreadCell[i].life;    // 칸 차지
					indexMAP[nr][nc] = i;                // 차지한 후보 번호 기억
				}
				// 3-2 MAP이 빈칸이 아니고, 자신의 life가 더 크다면 MAP 갱신
				else if (MAP[nr][nc] < spreadCell[i].life)
				{
					spreadCell[indexMAP[nr][nc]].time = 0;   // 먼저 차지했던 후보는 진 것으로 표시

					MAP[nr][nc] = spreadCell[i].life;    // 더 큰 생명력으로 차지
					indexMAP[nr][nc] = i;                // 차지한 후보 번호 갱신
				}
				else
					spreadCell[i].time = 0;                  // 내가 진 것 (같으면 먼저 온 쪽이 남는다)
			}
			// 4) 3) 과정 종료 후, spreadCell이 0이 아닌 경우만 liveCell에 추가
			for (int i = 0; i < scnt; i++)
				if (spreadCell[i].time) liveCell[lcnt++] = spreadCell[i];   // 이긴 후보만 새 세포

			// 5) 다음 1) 과정을 다시 하기 위해 liveCell을 cell에 추가
			for (int i = 0; i < lcnt; i++) cell[i] = liveCell[i];
			ccnt = lcnt;                         // 비활성 세포 수 갱신
		}
		printf("#%d %d\n", tc, lcnt + dcnt);   // 비활성 + 활성
	}
	return 0;                            // 프로그램 정상 종료
}
