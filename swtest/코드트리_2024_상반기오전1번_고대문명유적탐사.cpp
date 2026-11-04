/*
	[코드트리] 2024 상반기 오전 1번 - 고대 문명 유적 탐사
	https://www.codetree.ai/training-field/frequent-problems/problems/ancient-ruin-exploration

	■ 문제 요약
	  5 x 5 격자에 유물 조각의 번호가 적혀 있다. 벽면에 새겨진 조각 M개가 순서대로 주어진다.
	  탐사를 K번 하는데, 한 번의 탐사는 아래 순서다.

	    1) 회전 : 격자 안에 들어가는 3 x 3 부분을 하나 골라 시계 방향으로 90 / 180 / 270도 돌린다.
	             고르는 기준은 "이번에 얻는 유물 가치가 가장 큰 것"이고,
	             같으면 회전 각도가 작은 것 -> 열이 작은 것 -> 행이 작은 것이다.
	    2) 유물 획득 : 같은 번호가 상하좌우로 3개 이상 이어지면 모두 사라지고, 사라진 개수만큼 가치를 얻는다.
	    3) 채우기 : 사라진 칸에 벽면 조각을 순서대로 채운다.
	               채우는 순서는 열이 작은 것부터, 같은 열에서는 행이 큰 것부터다.
	    4) 연쇄 : 채운 뒤 또 3개 이상 이어지면 다시 사라지고 가치를 얻는다. 더 없을 때까지 반복한다.

	  탐사마다 얻은 가치를 공백으로 구분해 출력한다. 유물을 하나도 얻을 수 없으면 그 탐사부터 끝난다.

	■ 풀이 방침
	  - 후보가 "3가지 각도 x 3행 x 3열 = 27가지"뿐이다. 그래서 전부 시뮬레이션해 보고 가장 좋은 것을 고른다.
	    for 순서를 (각도, 열, 행)으로 두고 "더 클 때만" 갱신하면 문제의 우선순위가 그대로 지켜진다.
	  - 고른 회전의 결과(유물이 사라진 상태)를 maxMAP에 저장해 두고, 그 상태에서 연쇄를 계속한다.
	    회전은 rotate를 각도만큼 반복해서 만든다(90도짜리 하나로 180, 270도까지 처리).
	  - 유물 찾기(getItem)는 BFS로 같은 번호 덩어리를 찾아 3개 이상이면 0으로 지우고 개수를 더한다.
	    한 번 훑는 사이에 지워진 칸은 이미 visit이 찍혀 있어 다시 보지 않는다.
	  - 채우기(setItem)는 열을 바깥, 행을 아래에서 위로 도는 이중 루프 하나로 끝난다.
	    이것이 곧 "열이 작은 것부터, 같은 열에서는 행이 큰 것부터"다.

	■ 경계 검사가 없는 이유
	  MAP을 5보다 크게(MAX = 8) 잡아 테두리가 0으로 남아 있다.
	  유물 번호는 1 이상이므로 BFS에서 0을 만나면 자연히 멈춘다. 그래서 좌표 범위 검사가 없다.

	■ 주의할 점
	  - getItem은 격자를 직접 고친다(지운 칸을 0으로 만든다). 후보를 고르는 단계에서도
	    복사본(tmpMAP)에만 쓰기 때문에 원본 MAP은 마지막에 한 번만 갱신된다.
	  - 회전만 하고 유물을 못 얻는 경우(가치 0)에는 격자를 되돌린다. 갱신한 적이 없으니 MAP은 그대로다.
	  [확인 필요] 벽면 조각을 다 쓰면(pcnt > M) 0을 채우게 된다.
	              문제에서 조각이 충분히 주어진다고 보고 검사하지 않는다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (5+3)                        // 5 x 5 + 테두리 여유
//#define DEBUG
int T;                                   // 테스트 케이스 수

int K, M;                                // 탐사 횟수, 벽면 조각 수
int MAP[MAX][MAX];                       // 유적 격자
bool visit[MAX][MAX];                    // BFS 방문 표시
int piece[300 + 50];                     // 벽면 조각 번호들
int pcnt;                                // 다음에 쓸 벽면 조각 위치

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };                 // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };                 // 방향별 열 변화량

void input()
{
	scanf("%d %d", &K, &M);              // 탐사 횟수, 조각 수 입력

	for (int r = 1; r <= 5; r++)         // 5 x 5 격자
		for (int c = 1; c <= 5; c++)
			scanf("%d", &MAP[r][c]);     // 유물 번호 입력

	pcnt = 0;                            // 벽면 조각 위치 초기화

	for (int p = 0; p < M; p++) scanf("%d", &piece[p]);   // 벽면 조각 입력
}

void printMap(int map[MAX][MAX])         // for debug
{
	for (int r = 1; r <= 5; r++)         // 모든 행
	{
		for (int c = 1; c <= 5; c++)     // 모든 열
			printf("%d ", map[r][c]);    // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])   // 격자 복사
{
	for (int i = 1; i <= 5; i++)
		for (int k = 1; k <= 5; k++)
			copy[i][k] = original[i][k];
}

void rotate(int map[MAX][MAX], int sr, int sc)   // (sr, sc)부터 3 x 3 시계 방향 90도 회전
{
	int tmpMAP[MAX][MAX] = { 0 };        // 회전 전 복사본

	int size = 3;                        // 회전 크기
	for (int r = 0; r < size; r++)       // 3 x 3 부분 복사
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = map[sr + r][sc + c];

	for (int r = 0; r < size; r++)       // 회전해서 다시 쓰기
		for (int c = 0; c < size; c++)
			map[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
	// (r, c) <- (2-c, r) 가 시계 방향 90도. 180, 270도는 이 함수를 두 번, 세 번 부른다.
}

int BFS(int map[MAX][MAX], int r, int c) // (r, c)와 같은 번호 덩어리 찾기, 3개 이상이면 지우고 개수 반환
{
	int rp, wp, number;                  // 큐 위치, 찾는 번호

	rp = wp = 0;                         // 큐 초기화

	number = map[r][c];                  // 기준 번호

	queue[wp].r = r;                     // 시작 칸 큐에 넣기
	queue[wp++].c = c;

	visit[r][c] = true;                  // 방문 표시

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr = out.r + dr[i];      // 이웃 행
			int nc = out.c + dc[i];      // 이웃 열

			if (map[nr][nc] != number || visit[nr][nc] == true) continue;   // 다른 번호거나 방문함

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = true;        // 방문 표시
		}
	}

	if (wp < 3)return 0;
	// 3개 미만이면 유물이 아니다. 이미 visit은 찍혀 있어 다시 보지 않는다.
	for (int i = 0; i < wp; i++)         // 덩어리의 모든 칸
	{
		int r, c;                        // 칸 좌표

		r = queue[i].r;
		c = queue[i].c;

		map[r][c] = 0;                   // 유물 지우기
	}

	return wp;                           // 지운 개수 반환
}


int getItem(int map[MAX][MAX])           // 격자 전체에서 유물 획득
{
	for (int r = 1; r <= 5; r++)         // 방문 초기화
		for (int c = 1; c <= 5; c++)
			visit[r][c] = false;

	int sum = 0;                         // 얻은 가치 합
	for (int r = 1; r <= 5; r++)         // 모든 칸
	{
		for (int c = 1; c <= 5; c++)
		{
			if (visit[r][c] == true) continue;   // 이미 본 칸 제외

			int count = BFS(map, r, c);  // 덩어리 확인

			sum += count;                // 가치 누적
		}
	}

	return sum;                          // 반환
}

void setItem(int map[MAX][MAX])          // 빈 칸에 벽면 조각 채우기
{
	for (int c = 1; c <= 5; c++)
		for (int r = 5; r >= 1; r--)
			// 열이 작은 것부터, 같은 열에서는 행이 큰 것(아래)부터 채운다.
			if (map[r][c] == 0) map[r][c] = piece[pcnt++];
}

int simulate()                           // 탐사 1번, 얻은 가치 반환
{
	int maxItemCount = 0;                // 최대 1차 획득 가치
	int maxMAP[MAX][MAX] = { 0 };        // 그때의 격자
	// 회전 각도가 작고
	for (int rot = 1; rot <= 3; rot++)
		// 27가지(각도 3 x 열 3 x 행 3)를 전부 해 보고 가장 좋은 것을 고른다.
	{
		// 열이 작고
		for (int c = 1; c <= 3; c++)
		{
			// 행이 작은 순서대로
			for (int r = 1; r <= 3; r++)
			{
				int tmpMAP[MAX][MAX] = { 0 };   // 시험용 격자

				copyMap(tmpMAP, MAP);           // 원본 복사

				for (int rotCount = 1; rotCount <= rot; rotCount++)   // 각도만큼
					rotate(tmpMAP, r, c);                             // 90도씩 회전

				int itemCount = getItem(tmpMAP);   // 1차 획득 가치
				if (maxItemCount < itemCount)      // 더 크면
				{
					maxItemCount = itemCount;      // 최대값 갱신
					copyMap(maxMAP, tmpMAP);       // 격자 저장
//#ifdef DEBUG
//					printf("최댓값 갱신: %d (유물 제거 후)\n", itemCount);  // ② 제거 결과 확인
//					printMap(tmpMAP);
//#endif
				}
			}
		}
	}

	if(maxItemCount == 0) return 0;
	// 어떤 회전으로도 유물을 못 얻으면 탐사가 끝난다 (격자도 그대로 둔다).

	int sum = maxItemCount;              // 총 가치
	while (1)                            // 연쇄 획득
	{
		setItem(maxMAP);                 // 빈 칸 채우기

		int itemCount = getItem(maxMAP); // 다시 획득

		if (itemCount == 0) break;       // 더 없으면 끝

		sum += itemCount;                // 누적
	}
	copyMap(MAP, maxMAP);                // 결과를 원본에 반영


	return sum;                          // 이번 탐사 가치 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		for (int k = 0; k < K; k++)      // K번 탐사
		{
			int result = simulate();     // 탐사 1번

			if (result == 0)break;       // 유물이 없으면 종료

			printf("%d ", result);       // 결과 출력
		}
	}

	return 0;                            // 프로그램 정상 종료
}
