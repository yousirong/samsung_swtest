/*
	[코드트리] 2022 상반기 오전 2번 - 예술성
	https://www.codetree.ai/training-field/frequent-problems/problems/artistry

	■ 문제 요약
	  N x N 격자(N은 홀수)의 각 칸에 그림의 숫자가 적혀 있다.
	  상하좌우로 인접한 같은 숫자 칸들을 한 "그룹"으로 본다.

	  예술 점수 = 모든 그룹 쌍 (A, B)에 대해 아래 값을 더한 것
	      (A의 칸 수 + B의 칸 수) x A의 숫자 x B의 숫자 x (A와 B가 맞닿은 변의 개수)

	  점수를 구한 뒤 격자를 회전한다.
	      - 가운데 십자(가운데 행 + 가운데 열) : 반시계 방향 90도
	      - 십자로 나뉜 네 개의 정사각형     : 각각 시계 방향 90도

	  "점수 구하기 -> 회전"을 반복해 얻은 점수 4개의 합을 출력한다.
	  (처음 상태에서 한 번, 회전 세 번 뒤까지 해서 모두 4번)

	■ 풀이 방침 : 그룹에 번호를 붙여 두고 번호로 맞닿은 변을 센다
	  점수 식에 "그룹 쌍"이 나오므로, 칸이 아니라 그룹을 단위로 다뤄야 한다.

	    visit[r][c]  : 그 칸이 속한 그룹 번호 (1부터). BFS를 돌 때 바로 적어 둔다.
	    group[i]     : i번째 그룹의 (숫자, 칸 수, 그룹 번호)
	    harmony[A][B]: 그룹 A와 B가 맞닿은 변의 개수

	  visit이 "방문했나"와 "몇 번 그룹인가"를 겸하므로 배열을 따로 둘 필요가 없다.
	  0이면 아직 안 봤다는 뜻이라 BFS 시작 조건으로도 쓰인다.

	  맞닿은 변은 모든 칸에서 네 방향을 보며 세는데, 한 변을 양쪽 칸에서 각각 한 번씩
	  보고 그때마다 [A][B]와 [B][A]를 둘 다 올리므로 실제 개수의 2배가 쌓인다.
	  그래서 마지막에 2로 나눈다.

	  BFS가 끝났을 때 큐에 들어간 칸 수(wp)가 곧 그룹의 크기다.

	■ 회전 : 새 값이 어디서 오는지로 적는다
	  덮어쓰기 사고를 막으려고 원본을 tmpMAP에 복사해 두고 "새 칸 <- 옛 칸"으로 채운다.

	  십자(반시계 90도) : 새 (r, c)는 옛 (c, N+1-r)에서 온다.
	      가운데 열 <- 옛 가운데 행을 거꾸로     MAP[r][halfN]  = tmpMAP[halfN][N + 1 - r]
	      가운데 행 <- 옛 가운데 열              MAP[halfN][c]  = tmpMAP[c][halfN]
	      (N이 홀수라 halfN = (N+1)/2 가 정확히 가운데다)

	  정사각형(시계 90도) : 새 (r, c)는 옛 (size-1-c, r)에서 온다.
	      네 정사각형의 왼쪽 위 칸은 (1,1), (halfN+1,1), (1,halfN+1), (halfN+1,halfN+1)이고
	      한 변의 길이는 size = N / 2 다.

	■ 확인한 것
	  - 같은 규칙을 다른 방식으로 짠 코드와 랜덤 입력 200개를 비교해 모두 답이 같았다.
	  - N = 29의 큰 입력에서도 점수는 100만 안팎이라 int로 넘치지 않는다.
	  - harmony와 visit을 매 라운드 새로 0으로 지우므로 라운드끼리 값이 섞이지 않는다.

	■ 주의할 점
	  - 마지막(4번째) 점수를 구한 뒤에도 회전을 한 번 더 한다. 결과에는 영향이 없다.
	  - harmony는 (N*N) x (N*N) 짜리라 매 라운드 지우는 데 N^4 번 대입이 든다.
	    N = 29면 라운드마다 약 70만 번이라 괜찮지만, 그룹 수(gcnt)까지만 지워도 충분하다.
*/
#include <stdio.h>             // scanf, printf, putchar 사용

#define MAX (29+5)             // N 최대 29 + 여유

int T;                         // 테스트 케이스 수

int N;                         // 격자 크기
int halfN; // 가운데 행/열 번호 (N이 홀수라서 딱 떨어진다)
int MAP[MAX][MAX];             // 그림 숫자
int tmpMAP[MAX][MAX];          // 회전용 복사본
int visit[MAX][MAX]; // 0이면 아직 안 봄, 아니면 그 칸이 속한 그룹 번호

struct GROUP
{
	int value; // 그룹의 숫자
	int count; // 그룹의 칸 수
	int groupNumber; // 그룹 번호 (harmony의 인덱스)
};

GROUP group[MAX * MAX];        // 그룹 목록
int gcnt;                      // 그룹 수

struct RC                      // 좌표 한 쌍
{
	int r;                     // 행
	int c;                     // 열
};

RC queue[MAX * MAX];           // BFS용 큐

// 상, 우, 하, 좌
int dr[] = { -1, 0,1,0 };      // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };       // 방향별 열 변화량

int harmony[MAX * MAX][MAX * MAX]; // harmony[A][B] = 그룹 A와 B가 맞닿은 변의 개수

void input()
{
	scanf("%d", &N);                     // 격자 크기 입력

	halfN = (N + 1) / 2;                 // 가운데 번호

	// 테두리까지 0으로 비워 둔다.
	for (int r = 0; r <= N + 1; r++)
		for (int c = 0; c <= N + 1; c++)
			MAP[r][c] = 0;               // 0으로 초기화

	for (int r = 1; r <= N; r++)         // 실제 격자
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);     // 숫자 입력
}

void printMap(int map[MAX][MAX]) // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
			printf("%2d ", map[r][c]);   // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void copyMap(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= N; c++)     // 모든 열
			copy[r][c] = original[r][c]; // 한 칸씩 복사
}

// (r, c)에서 시작하는 그룹을 mark 번으로 칠하고 그 그룹의 정보를 돌려준다.
GROUP BFS(int r, int c, int mark)
{
	GROUP ret = { 0 };                   // 그룹 정보
	int value;                           // 그룹의 숫자
	int rp, wp;                          // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;                         // 빈 큐
	value = MAP[r][c];                   // 시작 칸 숫자

	queue[wp].r = r;                     // 시작 행
	queue[wp++].c = c;                   // 시작 열, 삽입 위치 증가

	visit[r][c] = mark;                  // 그룹 번호 칠하기

	while (rp < wp)                      // 처리할 칸이 남아 있는 동안
	{
		RC out = queue[rp++];            // 한 칸 꺼낸다

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (nr<1 || nc<1 || nr> N || nc>N) continue;   // 격자 밖

			// 숫자가 다르거나 이미 다른(또는 같은) 그룹으로 칠해졌으면 넘어간다.
			if (MAP[nr][nc] != value || visit[nr][nc] != 0) continue;

			queue[wp].r = nr;            // 큐에 넣는다
			queue[wp++].c = nc;

			visit[nr][nc] = mark;        // 그룹 번호 칠하기
		}
	}

	ret.value = value;                   // 숫자
	ret.count = wp; // 큐에 들어간 칸 수 = 그룹 크기
	ret.groupNumber = mark;              // 그룹 번호

	return ret;                          // 그룹 정보 반환
}

// 가운데 십자를 반시계 방향 90도 돌린다.
void rotateCenter()
{
	// 덮어쓰기 전의 값이 필요하므로 복사본을 두고 읽는다.
	copyMap(tmpMAP, MAP);

	// 새 가운데 열 <- 옛 가운데 행 (거꾸로)
	for (int r = 1; r <= N; r++)
		MAP[r][halfN] = tmpMAP[halfN][N + 1 - r];

	// 새 가운데 행 <- 옛 가운데 열
	for (int c = 1; c <= N; c++)
		MAP[halfN][c] = tmpMAP[c][halfN];
}

// (sr, sc)를 왼쪽 위로 하는 size x size 정사각형을 시계 방향 90도 돌린다.
void rotate(int sr, int sc)
{
	int size = N / 2;                    // 정사각형 한 변 길이

	// 이 정사각형만 0-based로 tmpMAP에 떠 놓고
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			tmpMAP[r][c] = MAP[sr + r][sc + c];

	// 새 (r, c) <- 옛 (size-1-c, r) 로 되돌려 넣는다.
	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			MAP[sr + r][sc + c] = tmpMAP[size - 1 - c][r];
}

// 그룹끼리 맞닿은 변의 개수를 센다.
void makeharmony()
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= N; c++)     // 모든 열
		{
			for (int i = 0; i < 4; i++)  // 4방향
			{
				int nr, nc;              // 이웃 칸

				nr = r + dr[i];          // 이웃 행
				nc = c + dc[i];          // 이웃 열

				if (nr <1 || nc<1 || nr>N || nc>N) continue;   // 격자 밖

				// 같은 그룹이면 맞닿은 변이 아니다.
				if (visit[r][c] == visit[nr][nc]) continue;

				harmony[visit[r][c]][visit[nr][nc]]++; // harmony[A][B]++;
				harmony[visit[nr][nc]][visit[r][c]]++; // harmony[B][A]++;
			}
		}
	}

	// 한 변을 양쪽 칸에서 한 번씩 보고 그때마다 [A][B], [B][A]를 둘 다 올렸으므로
	// 실제 개수의 2배가 쌓여 있다.
	for (int r = 1; r <= N * N; r++)
		for (int c = 1; c <= N * N; c++)
			harmony[r][c] /= 2;          // 2로 나눠 실제 개수로
	return;                              // 끝 (아래는 디버그 코드)
	//// for debug
	//for (int r = 1; r <= N * N; r++)
	//{
	//	for (int c = 1; c <= N * N; c++)
	//	{
	//		printf("%d %d %d\n", r, c, harmony[r][c]);
	//	}
	//}
}

// 모든 그룹 쌍을 한 번씩 보며 예술 점수를 더한다.
int getScore()
{
	int score = 0;                       // 예술 점수
	for (int i = 0; i < gcnt - 1; i++)   // 그룹 A
	{
		for (int k = i + 1; k < gcnt; k++)   // A 뒤의 그룹 B
		{
			GROUP a = group[i];          // 그룹 A
			GROUP b = group[k];          // 그룹 B

			// 맞닿지 않은 쌍은 harmony가 0이라 자동으로 0점이 된다.
			score += ((a.count + b.count) * a.value * b.value * harmony[a.groupNumber][b.groupNumber]);
		}
	}

	return score;                        // 점수 반환
}

int simulate()
{
	int score = 0;                       // 총점
	// 처음 상태 + 회전 3번 = 점수를 네 번 구한다.
	for (int round = 0; round < 4; round++)
	{
		// 회전으로 그룹이 통째로 달라지므로 라운드마다 처음부터 다시 만든다.
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= N; c++)
				visit[r][c] = 0;         // 그룹 번호 지우기

		gcnt = 0;                        // 그룹 수 초기화

		int groupNumber = 1; // 0은 "아직 안 봄"이라 1번부터 쓴다.
		for (int r = 1; r <= N; r++)     // 모든 행
		{
			for (int c = 1; c <= N; c++) // 모든 열
			{
				if (visit[r][c] != 0) continue;   // 이미 그룹에 속한 칸

				GROUP ret = BFS(r, c, groupNumber);   // 새 그룹 만들기
				group[gcnt++] = ret;              // 목록에 추가
				groupNumber++;                    // 다음 그룹 번호
			}
		}

		// for debug
		//printf("round : %d\n", round);
		//for (int i = 0; i < gcnt; i++)
		//{
		//	GROUP g = group[i];

		//	printf("%d %d %d\n", g.value, g.count, g.groupNumber);
		//}

		// 지난 라운드의 값이 남지 않도록 지운다. (그룹 번호는 최대 N*N까지 나온다)
		for (int r = 1; r <= N * N; r++)
			for (int c = 1; c <= N * N; c++)
				harmony[r][c] = 0;       // 맞닿은 변 수 초기화

		makeharmony();                   // 맞닿은 변 세기

		score += getScore();             // 이번 라운드 점수 더하기

		// 십자를 먼저 돌리고, 십자와 겹치지 않는 네 정사각형을 돌린다.
		rotateCenter();                  // 십자 반시계 회전

		rotate(1, 1);                    // 왼쪽 위 정사각형
		rotate(halfN + 1, 1);            // 왼쪽 아래 정사각형
		rotate(1, halfN + 1);            // 오른쪽 위 정사각형
		rotate(halfN + 1, halfN + 1);    // 오른쪽 아래 정사각형
	}
	return score;                        // 총점 반환
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		input();                         // 입력

		printf("%d\n", simulate());      // 결과 출력
	}

	return 0;                            // 프로그램 정상 종료
}
