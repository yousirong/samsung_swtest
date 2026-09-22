/*
	[코드트리] 2022 상반기 오후 2번 - 나무박멸
	https://www.codetree.ai/training-field/frequent-problems/problems/tree-kill-all

	■ 문제 요약
	  N x N 격자의 각 칸은 나무 그루 수(양수), 빈칸(0), 벽(-1) 중 하나다.
	  M년 동안 매년 아래를 진행한다.

	    1) 성장 : 나무가 있는 칸은 상하좌우 네 칸 중 나무가 있는 칸 수만큼 그루 수가 늘어난다.
	    2) 번식 : 나무가 있는 칸은 상하좌우 중 "벽, 나무, 제초제가 없는 칸"에
	             (그루 수 / 번식 가능한 칸 수)만큼 번식한다. 모든 나무가 동시에 번식한다.
	    3) 제초제 : 박멸되는 나무가 가장 많은 칸에 제초제를 뿌린다.
	               (같으면 행이 작은 칸, 행도 같으면 열이 작은 칸)
	               제초제는 그 칸과 네 대각선 방향으로 K칸까지 퍼진다.
	               퍼지다가 벽이나 나무가 없는 칸을 만나면 그 칸까지만 뿌려지고 멈춘다.
	               뿌려진 칸의 나무는 모두 없어지고, 제초제는 C년 동안 남아 번식을 막는다.
	               이미 제초제가 있는 칸에 다시 뿌리면 C년으로 새로 갱신된다.

	  M년 동안 박멸한 나무 그루 수의 합을 출력한다.

	■ 풀이 방침
	  - 성장은 제자리에서 바로 더해도 된다.
	    "이웃에 나무가 있는가"만 보므로, 먼저 자란 칸의 값이 바뀌어도
	    나무가 있다/없다는 바뀌지 않아 뒤 칸 계산에 영향이 없다.
	  - 번식은 "동시에" 일어나므로 tmpMAP에 모았다가 한꺼번에 더한다.
	    제자리에서 더하면 방금 번식한 칸이 같은 해에 또 번식한다.
	  - 제초제는 herbicide[r][c]에 "남은 해"를 적는다. 0이 아니면 번식을 막는다.
	  - 박멸 수를 세는 함수(getDeleteTreeCount)와 실제로 뿌리는 함수(weeding)를 나눴다.
	    세는 쪽은 MAP을 건드리면 안 되기 때문이다.

	■ 제초제 남은 해의 흐름
	  weeding에서 C를 넣고, 다음 해부터 "번식 -> 1 감소" 순서로 진행한다.
	  그래서 C, C-1, ..., 1 인 동안 번식을 막아 정확히 C년 동안 남는다.
	      y년 : weeding -> herbicide = C
	      y+1년 : 번식 때 C (막음)   -> 감소 -> C-1
	      ...
	      y+C년 : 번식 때 1 (막음)   -> 감소 -> 0 (사라짐)

	■ 주의할 점
	  - 나무가 하나도 없으면 findMaxDeleteTree가 (0, 0)을 돌려주고 weeding(0, 0)이 불린다.
	    MAX에 여유가 있어 배열 밖으로 나가지는 않고, 나무가 없으니 번식도 없어 답에 영향이 없다.
	  - 벽을 만나면 그 칸에는 제초제를 적지 않고 멈춘다. 문제는 "그 칸까지 뿌려진다"고 하지만
	    벽에는 나무가 생길 수 없어 결과는 같다.
	  - herbicide는 input()에서 초기화하지 않는다. 한 번만 실행하면 전역 0 초기화로 충분하지만
	    여러 테스트케이스를 돌리거나 함수형으로 재호출하면 이전 값이 남는다.
*/
#include <stdio.h>

#define MAX (20 + 5)
#define WALL (-1)

int T;

int N, M, K, C; // 격자 크기, 햇수, 제초제 확산 범위, 제초제 유지 햇수
int MAP[MAX][MAX]; // 양수 : 나무 그루 수, 0 : 빈칸, -1 : 벽
int herbicide[MAX][MAX]; // 제초제 (남은 햇수, 0이면 없음)

struct INFO
{
	int r;
	int c;
	int count;
};

// ↑, →, ↓, ←
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };

// ↖, ↗, ↘, ↙
int dr2[] = { -1, -1, 1, 1 };
int dc2[] = { -1, 1, 1, -1 };

void input()
{
	scanf("%d %d %d %d", &N, &M, &K, &C);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

void spreadTree()
{
	// 1. 인접한 칸 만큼 나무가 성장
	// 제자리에서 더해도 된다 : 이웃은 "나무가 있는가"만 보므로 값이 바뀌어도 결과가 같다.
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == WALL) continue;

			int count = 0; // 주변 나무의 개수
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr < 1 || nc < 1 || nr > N || nc > N) continue;

				if (MAP[nr][nc] > 0) count++;
			}

			MAP[r][c] += count;
		}
	}

	// 2. 인접한 칸에 동시에 번식
	// 동시에 일어나므로 tmpMAP에 모았다가 마지막에 더한다.
	int tmpMAP[MAX][MAX] = { 0 };
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			if (MAP[r][c] == 0 || MAP[r][c] == WALL) continue;

			// 먼저 번식 가능한 칸 수를 센다 (나눗셈의 분모).
			int count = 0; // 번식 가능한 칸
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr < 1 || nc < 1 || nr > N || nc > N) continue;

				if (MAP[nr][nc] == WALL) continue;

				// 빈칸이면서 제초제가 없어야 한다.
				if (MAP[nr][nc] == 0 && herbicide[nr][nc] == 0) count++;
			}

			// 같은 조건의 칸에 몫만큼 나눠 준다.
			// 가능한 칸이 없으면(count == 0) 아래 if에 한 번도 들어가지 않아 0으로 나누지 않는다.
			for (int i = 0; i < 4; i++)
			{
				int nr, nc;

				nr = r + dr[i];
				nc = c + dc[i];

				if (nr < 1 || nc < 1 || nr > N || nc > N) continue;

				if (MAP[nr][nc] == WALL) continue;

				if (MAP[nr][nc] == 0 && herbicide[nr][nc] == 0)
					tmpMAP[nr][nc] += (MAP[r][c] / count);
			}
		}
	}

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			MAP[r][c] += tmpMAP[r][c];
}

// (r, c)에 제초제를 나뒀을 때, 사라지는 나무의 수
// MAP은 건드리지 않고 세기만 한다.
int getDeleteTreeCount(int r, int c)
{
	int sum = 0;

	sum += MAP[r][c];

	for (int i = 0; i < 4; i++)
	{
		for (int k = 1; k <= K; k++)
		{
			int nr, nc;

			nr = r + dr2[i] * k;
			nc = c + dc2[i] * k;

			// 격자 밖, 빈칸, 벽을 만나면 그 방향은 더 퍼지지 않는다.
			if (nr < 1 || nc < 1 || nr > N || nc > N) break;
			if (MAP[nr][nc] == 0 || MAP[nr][nc] == WALL) break;

			sum += MAP[nr][nc];
		}
	}

	return sum;
}

// 박멸 수가 가장 큰 칸을 고른다.
INFO findMaxDeleteTree()
{
	INFO result;
	int maxR, maxC, maxTree;

	maxR = maxC = maxTree = 0;
	// 행, 열이 작은 순서로 보면서 "더 클 때만" 갱신 -> 동점이면 앞선 칸이 남는다.
	for (int r = 1; r <= N; r++) // 행이 작은 순서대로
	{
		for (int c = 1; c <= N; c++) // 열이 작은 순서대로
		{
			// 나무가 있는 칸만 후보로 본다. (나무가 있으면 박멸 수는 최소 1이라 빈칸보다 항상 낫다)
			if (MAP[r][c] == 0 || MAP[r][c] == WALL) continue;

			int deleteTreeCount = getDeleteTreeCount(r, c);
			if (maxTree < deleteTreeCount)
			{
				maxR = r;
				maxC = c;
				maxTree = deleteTreeCount;
			}

		}
	}

	// 나무가 하나도 없으면 (0, 0, 0)이 반환된다.
	result.r = maxR;
	result.c = maxC;
	result.count = maxTree;

	return result;
}

// (r, c)에 제초제를 실제로 뿌린다.
void weeding(int r, int c)
{
	MAP[r][c] = 0;
	herbicide[r][c] = C;
	for (int i = 0; i < 4; i++)
	{
		for (int k = 1; k <= K; k++)
		{
			int nr, nc;

			nr = r + dr2[i] * k;
			nc = c + dc2[i] * k;

			if (nr < 1 || nc < 1 || nr > N || nc > N) break;
			// 벽에는 적지 않고 멈춘다. (벽엔 나무가 생기지 않으니 적어도 안 적어도 같다)
			if (MAP[nr][nc] == WALL) break;

			// 나무가 없는 칸 : 그 칸까지는 뿌리고 멈춘다.
			if (MAP[nr][nc] == 0)
			{
				herbicide[nr][nc] = C;
				break;
			}

			MAP[nr][nc] = 0;
			herbicide[nr][nc] = C; // 이미 제초제가 있어도 C로 갱신
		}
	}
}

int simulate()
{
	int sum = 0;
	for (int m = 0; m < M; m++)
	{
		spreadTree(); // 나무의 성장, 번식

		// 번식이 끝난 뒤 1년씩 줄인다. 그래서 뿌린 뒤 정확히 C년 동안 번식을 막는다.
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= N; c++)
				if (herbicide[r][c] != 0) herbicide[r][c]--;

		INFO target = findMaxDeleteTree(); // 제초제를 뿌릴 위치 선정

		weeding(target.r, target.c); // 제초제를 뿌리는 작업 진행

		sum += target.count; // 총 박멸한 나무의 그루 수
	}

	return sum;
}

int main()
{
	// scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		printf("%d\n", simulate());
	}

	return 0;
}
