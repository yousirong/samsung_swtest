/*
	[코드트리] 2024 상반기 오전 1번 - 고대 문명 유적 탐사

	[프로그래머스 제출용]  원본 : swtest/코드트리_2024_상반기오전1번_고대문명유적탐사.cpp
	swtest 판과 같은 코드다. input()이 인자를 받고 main()이 solution()으로 바뀐 것만 다르다.
	로컬 대조는 같은 폴더의 local_test.cpp 로 한다 (제출에는 쓰지 않는다).
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
#include <stdio.h>
#include <vector>              // [추가] 함수형 인자/반환용

using namespace std;

#define MAX (5+3)
//#define DEBUG
int T;

int K, M;
int MAP[MAX][MAX];
bool visit[MAX][MAX];
int piece[300 + 50];
int pcnt;

struct RC
{
	int r;
	int c;
};

RC queue[MAX * MAX];

// 상, 우, 하, 좌
int dr[] = { -1,0,1,0 };
int dc[] = { 0,1,0,-1 };

// [수정] scanf 대신 인자로 받는다
// board = 5 x 5 유물 조각 번호, pieces = 벽면 조각(공급 순서)
void input(int k, const vector<vector<int>>& board, const vector<int>& pieces)
{
	K = k;                        // [수정] scanf("%d %d", &K, &M) 대체
	M = (int)pieces.size();

	for (int r = 1; r <= 5; r++)
		for (int c = 1; c <= 5; c++)
			MAP[r][c] = board[r - 1][c - 1];   // [수정] scanf 대체

	pcnt = 0;

	for (int p = 0; p < M; p++) piece[p] = pieces[p];   // [수정] scanf 대체
}

void printMap(int map[MAX][MAX])
{
	for (int r = 1; r <= 5; r++)
	{
		for (int c = 1; c <= 5; c++)
			printf("%d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int i = 1; i <= 5; i++)
		for (int k = 1; k <= 5; k++)
			copy[i][k] = original[i][k];
}

void rotate(int map[MAX][MAX], int sr, int sc)
{
	int tmpMAP[MAX][MAX] = { 0 };

	int size = 3;
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = map[sr + r][sc + c];

	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			map[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
	// (r, c) <- (2-c, r) 가 시계 방향 90도. 180, 270도는 이 함수를 두 번, 세 번 부른다.
}

int BFS(int map[MAX][MAX], int r, int c)
{
	int rp, wp, number;

	rp = wp = 0;

	number = map[r][c];

	queue[wp].r = r;
	queue[wp++].c = c;

	visit[r][c] = true;

	while (rp < wp)
	{
		RC out = queue[rp++];

		for (int i = 0; i < 4; i++)
		{
			int nr = out.r + dr[i];
			int nc = out.c + dc[i];

			if (map[nr][nc] != number || visit[nr][nc] == true) continue;

			queue[wp].r = nr;
			queue[wp++].c = nc;

			visit[nr][nc] = true;
		}
	}

	if (wp < 3)return 0;
	// 3개 미만이면 유물이 아니다. 이미 visit은 찍혀 있어 다시 보지 않는다.
	for (int i = 0; i < wp; i++)
	{
		int r, c;

		r = queue[i].r;
		c = queue[i].c;

		map[r][c] = 0;
	}

	return wp;
}


int getItem(int map[MAX][MAX])
{
	for (int r = 1; r <= 5; r++)
		for (int c = 1; c <= 5; c++)
			visit[r][c] = false;

	int sum = 0;
	for (int r = 1; r <= 5; r++)
	{
		for (int c = 1; c <= 5; c++)
		{
			if (visit[r][c] == true) continue;

			int count = BFS(map, r, c);

			sum += count;
		}
	}

	return sum;
}

void setItem(int map[MAX][MAX])
{
	for (int c = 1; c <= 5; c++)
		for (int r = 5; r >= 1; r--)
			// 열이 작은 것부터, 같은 열에서는 행이 큰 것(아래)부터 채운다.
			if (map[r][c] == 0) map[r][c] = piece[pcnt++];
}

int simulate()
{
	int maxItemCount = 0;
	int maxMAP[MAX][MAX] = { 0 };
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
				int tmpMAP[MAX][MAX] = { 0 };

				copyMap(tmpMAP, MAP);

				for (int rotCount = 1; rotCount <= rot; rotCount++)
					rotate(tmpMAP, r, c);

				int itemCount = getItem(tmpMAP);
				if (maxItemCount < itemCount)
				{
					maxItemCount = itemCount;
					copyMap(maxMAP, tmpMAP);
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

	int sum = maxItemCount;
	while (1)
	{
		setItem(maxMAP);

		int itemCount = getItem(maxMAP);

		if (itemCount == 0) break;

		sum += itemCount;
	}
	copyMap(MAP, maxMAP);


	return sum;
}

// [수정] main() -> solution().
// 원본은 탐사마다 값을 "%d " 로 찍었다. 같은 순서로 목록에 담아 반환한다.
// (유물을 얻지 못하는 탐사가 나오면 거기서 멈추므로 목록 길이가 K보다 짧을 수 있다)
vector<int> solution(int k, vector<vector<int>> board, vector<int> pieces)
{
	input(k, board, pieces);

	int out[300 + 50];   // [추가] 결과를 담아 둔다 (탐사는 최대 K번)
	int ocnt = 0;

	for (int i = 0; i < K; i++)
	{
		int result = simulate();

		if (result == 0)break;

		out[ocnt++] = result;   // [수정] printf -> 배열에 담기
	}

	return vector<int>(out, out + ocnt);
}
