/*
	[코드트리] 2017 상반기 오후 2번 - 방화벽 설치하기 (연습 버전)

	■ 요약
	  빈 칸 목록에서 3개를 조합으로 고르고, 복사본에 벽을 세운 뒤 불을 멀티 소스 BFS로 퍼뜨려
	  남은 빈 칸 수의 최댓값을 구한다.
	  자세한 설명은 코드트리_2017_상반기오후2번_방화벽설치하기.cpp 참고.
*/
#include <stdio.h>              // scanf, printf, putchar 사용

#define MAX (8+3)	            // N, M 최대 8 + 여유

#define EMPTY (0)               // 빈 칸
#define WALL (1)                // 벽
#define FIRE (2)                // 불

int T;                          // 테스트 케이스 수

int N, M;                       // 격자 크기
int MAP[MAX][MAX];              // 원본 격자
int tmpMAP[MAX][MAX];           // 작업용 격자

int num_of_cases[5];            // 고른 빈 칸 인덱스 3개

int maxAnswer;                  // 안전 영역 최댓값

struct RC                       // 좌표 한 쌍
{
	int r;                      // 행
	int c;                      // 열
};

RC queue[MAX * MAX];            // BFS용 큐

RC emptyRoom[MAX * MAX];        // 빈 칸 목록
int emptyCount;                 // 빈 칸 수

// 상 우 하 좌
int dr[] = { -1, 0, 1, 0 };     // 방향별 행 변화량
int dc[] = { 0,1,0,-1 };        // 방향별 열 변화량

void input()
{
	scanf("%d %d", &N, &M);              // 격자 크기 입력

	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= M; c++)     // 모든 열
			scanf("%d", &MAP[r][c]);     // 칸 상태 입력
}

void printMap() // for debug
{
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= M; c++)     // 모든 열
			printf("%d ", tmpMAP[r][c]); // 값 출력
		putchar('\n');                   // 행 끝 줄바꿈
	}
	putchar('\n');                       // 빈 줄
}

void copyMAP(int copy[MAX][MAX], int original[MAX][MAX])
{
	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= M; c++)     // 모든 열
			copy[r][c] = original[r][c]; // 한 칸씩 복사
}

void printCases()
{
	for (int i = 0; i < 3; i++) printf("%d ", num_of_cases[i]);   // 고른 인덱스 출력
	putchar('\n');                       // 줄바꿈
}

void BFS()
{
	copyMAP(tmpMAP, MAP);                // 원본 복사

	for (int i = 0; i < 3; i++)          // 고른 3곳에
	{
		int wr, wc;                      // 벽 좌표
		int index = num_of_cases[i];     // 빈 칸 목록 인덱스

		wr = emptyRoom[index].r;         // 행
		wc = emptyRoom[index].c;         // 열

		tmpMAP[wr][wc] = WALL;           // 벽 세우기
	}

	int rp, wp;                          // rp: 꺼낼 위치, wp: 넣을 위치

	rp = wp = 0;                         // 빈 큐

	for (int r = 1; r <= N; r++)         // 모든 칸에서
	{
		for (int c = 1; c <= M; c++)
		{
			if (tmpMAP[r][c] == FIRE)    // 불이면
			{
				queue[wp].r = r;         // 큐에 넣는다
				queue[wp++].c = c;
			}
		}
	}

	while (rp < wp)                      // 처리할 칸이 남아 있는 동안
	{
		RC out = queue[rp++];            // 한 칸 꺼낸다

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];          // 이웃 행
			nc = out.c + dc[i];          // 이웃 열

			if (nr<1 || nc<1 || nr>N || nc>M) continue;   // 격자 밖
			if (tmpMAP[nr][nc] != 0)continue;             // 빈 칸이 아니면 건너뜀

			queue[wp].r = nr;            // 큐에 넣는다
			queue[wp++].c = nc;

			tmpMAP[nr][nc] = FIRE;       // 불이 번진다
		}
	}
}

int countEmpty()
{
	int sum = 0;                         // 안전 영역 수
	for (int r = 1; r <= N; r++)         // 모든 행
		for (int c = 1; c <= M; c++)     // 모든 열
			sum += !tmpMAP[r][c];        // 0인 칸만 센다

	return sum;                          // 개수 반환
}

void DFS(int depth, int start)
{
	if (depth == 3)                      // 3곳을 다 골랐으면
	{
		//printCases();

		BFS();                           // 실험

		int tmp = countEmpty();          // 안전 영역 수
		if (maxAnswer < tmp) maxAnswer = tmp;   // 최댓값 갱신

		return;                          // 되돌아간다
	}

	for (int i = start; i < emptyCount; i++)   // start부터 후보
	{
		num_of_cases[depth] = i;         // i번 빈 칸을 고른다

		DFS(depth + 1, i + 1);           // 다음은 i 뒤에서
	}
}

void setEmptyCount()
{
	emptyCount = 0;                      // 빈 칸 수 초기화
	for (int r = 1; r <= N; r++)         // 모든 행
	{
		for (int c = 1; c <= M; c++)     // 모든 열
		{
			if (MAP[r][c] == EMPTY)      // 빈 칸이면
			{
				emptyRoom[emptyCount].r = r;     // 목록에 추가
				emptyRoom[emptyCount++].c = c;
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

		setEmptyCount();                 // 빈 칸 목록 만들기

		maxAnswer = 0;                   // 답 초기화

		DFS(0, 0);                       // 조합 탐색

		printf("%d\n", maxAnswer);       // 결과 출력
	}
	return 0;                            // 프로그램 정상 종료
}
