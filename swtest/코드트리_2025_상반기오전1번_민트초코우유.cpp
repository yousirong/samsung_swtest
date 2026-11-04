/*
	[코드트리] 2025 상반기 오전 1번 - 민트 초코 우유
	https://www.codetree.ai/training-field/frequent-problems/problems/mint-choco-milk

	■ 문제 요약
	  N x N 교실의 학생마다 신봉하는 음식(T 민트, C 초코, M 우유)과 신앙심 B가 있다.
	  음식은 섞일 수 있어서 TC, TM, CM, TCM 같은 조합도 생긴다. T일 동안 하루는 아래 순서다.

	    1) 아침 : 모든 학생의 신앙심이 1 오른다.
	    2) 점심 : 상하좌우로 이어진 "음식 조합이 완전히 같은" 학생들이 한 그룹이 된다.
	       그룹마다 대표를 뽑는다 (신앙심이 가장 큰 학생 -> 행이 작은 학생 -> 열이 작은 학생).
	       대표는 신앙심이 (그룹 인원 - 1)만큼 오르고, 나머지는 1씩 내려간다.
	    3) 저녁 : 대표들이 차례로 자기 음식을 전파한다.
	       순서는 단일 음식 그룹 -> 이중 조합 -> 삼중 조합이고, 같은 단계에서는 대표 우선순위 순이다.
	       - 이미 전파를 당해 "방어 상태"가 된 대표는 전파하지 않는다.
	       - 방향은 B % 4 (0 위, 1 아래, 2 왼쪽, 3 오른쪽), 간절함 x = B - 1, 자기 신앙심은 1이 된다.
	       - 그 방향으로 한 칸씩 가며, 같은 음식인 학생은 건너뛴다.
	         다른 음식인 학생(신앙심 y)을 만나면 그 학생은 방어 상태가 되고
	            x > y  : 강한 전파. 그 학생은 전파자의 음식이 되고 신앙심 +1, x -= (y + 1)
	            x <= y : 약한 전파. 그 학생의 음식에 전파자 음식이 합쳐지고 신앙심 += x, 전파 끝
	       - 격자 밖으로 나가거나 x가 0이 되면 끝난다.

	  하루가 끝날 때마다 음식 조합별 신앙심 합을 TCM, TC, TM, CM, M, C, T 순서로 출력한다.

	■ 음식을 숫자 하나로 담는 방법
	  T = 1, C = 10, M = 100 으로 두고 조합은 더한 값으로 본다.
	      TC = 11, TM = 101, CM = 110, TCM = 111
	  십진수 각 자리가 "그 음식을 믿는가"를 뜻하므로, 음식 종류 수도 자리 개수로 바로 알 수 있다.

	  섞을 때는 mixFood에서 비트 OR(|)를 쓴다. 십진 표기인데 OR가 맞는 이유는
	  1 = 0b1, 10 = 0b1010, 100 = 0b1100100 의 켜진 비트가 하나도 겹치지 않기 때문이다.
	  그래서 OR가 곧 덧셈이고, 이미 들어 있는 음식은 두 번 더해지지 않는다.
	  (7가지 조합끼리 49가지 OR를 모두 확인해 봤고 전부 맞다)

	■ 풀이 방침
	  - 점심의 그룹은 BFS로 찾는다. "음식 값이 완전히 같은 이웃"만 따라간다.
	    한 그룹을 다 모은 뒤 isPriority로 대표를 고르고 신앙심을 조정한다.
	  - 저녁은 대표만 골라 그룹(1, 2, 3)별로 나눈 뒤 각 그룹 안에서 정렬한다.
	    정렬 기준이 대표 선정 기준과 같으므로 isPriority를 그대로 다시 쓴다.
	  - 방어 상태는 defense 값으로 표시하고 저녁이 끝나면 모두 지운다.

	■ 주의할 점
	  - 대표 정렬은 저녁이 "시작될 때"의 값으로 한 번만 한다.
	    전파 도중 신앙심이 바뀌어도 순서는 다시 정하지 않는다.
	    대신 전파할 때는 student[][]에서 지금 값을 다시 읽어 쓴다(spreader).
	  - 전파를 당해 음식이 바뀐 대표도, 방어 상태라서 그날은 전파하지 않는다.
	  - 전역 이름 index 는 리눅스 <strings.h>의 index() 함수와 겹칠 수 있다.
	    stdio.h만 쓰는 지금은 괜찮지만 다른 헤더를 붙일 때는 이름을 바꾸는 편이 안전하다.
*/
#include <stdio.h>                       // scanf, printf, putchar 사용

#define MAX (50 + 5)                     // N 최대 50 + 여유

#define TMINT_CHOKO_MILK (111)
// T = 1, C = 10, M = 100 을 더한 값으로 조합을 나타낸다.
#define TMINT_CHOKO (1 + 10)             // TC
#define TMINT_MILK (1 + 100)             // TM
#define CHOKO_MILK (100 + 10)            // CM
#define MILK (100)                       // M
#define CHOKO (10)                       // C
#define TMINT (1)                        // T

#define UP (0)                           // 위
#define DOWN (1)                         // 아래
#define LEFT (2)                         // 왼쪽
#define RIGHT (3)                        // 오른쪽

int TestCase;                            // 테스트 케이스 수
int N, T;                                // 교실 크기, 날 수
char F[MAX][MAX];                        // 입력 음식 문자
int B[MAX][MAX];                         // 입력 신앙심

struct STUDENT                           // 학생 정보
{
	int food; // 신봉하는 음식
	int believe; // 신앙심
	bool isLeader; // 대표
	int defense; // 방어 여부
	int row; // 대표자 선정을 위한 값
	int col;                             // 열
};

STUDENT student[MAX][MAX];               // 학생 격자
STUDENT candidates[MAX * MAX];           // 한 그룹의 학생들
int ccnt;                                // 그룹 인원 수

STUDENT group[3 + 1][MAX * MAX];         // 단계별 대표 목록 (1, 2, 3)
int index[3 + 1];                        // 단계별 대표 수

struct RC                                // 좌표
{
	int r;                               // 행
	int c;                               // 열
};

RC queue[MAX * MAX];                     // BFS 큐
bool visit[MAX][MAX];                    // BFS 방문 표시

// 위, 아래, 왼쪽, 오른쪽
int dr[4] = { -1, 1, 0, 0 };             // 방향별 행 변화량
int dc[4] = { 0, 0, -1, 1 };             // 방향별 열 변화량

void input()
{
	scanf("%d %d", &N, &T);              // 교실 크기, 날 수 입력

	for (int r = 1; r <= N; r++)         // 음식 입력
		for (int c = 1; c <= N; c++)
			scanf(" %c", &F[r][c]);

	for (int r = 1; r <= N; r++)         // 신앙심 입력
		for (int c = 1; c <= N; c++)
			scanf("%d", &B[r][c]);

	for (int r = 1; r <= N; r++)         // 학생 정보 구성
	{
		for (int c = 1; c <= N; c++)
		{
			student[r][c].believe = B[r][c];   // 신앙심
			student[r][c].isLeader = false; // 초기화
			student[r][c].defense = 0; // 초기화
			student[r][c].row = r;       // 행
			student[r][c].col = c;       // 열

			if (F[r][c] == 'T') student[r][c].food = TMINT;        // 민트
			else if (F[r][c] == 'C') student[r][c].food = CHOKO;   // 초코
			else if (F[r][c] == 'M') student[r][c].food = MILK;    // 우유
		}
	}
}

void printBelieve() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			printf("%d ", student[r][c].believe);   // 신앙심 출력
		}
		putchar('\n');
	}
	putchar('\n');
}

void printFood() // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			printf("%3d ", student[r][c].food);     // 음식 값 출력
		}
		putchar('\n');
	}
	putchar('\n');
}

void printGroup() // for debug
{
	for (int g = 1; g <= 3; g++)         // 단계별
	{
		int gIndex = index[g];           // 대표 수
		printf("group %d ", g);

		for (int i = 0; i < gIndex; i++) // 대표 좌표 출력
			printf("(%d, %d) ", group[g][i].row, group[g][i].col);

		putchar('\n');
	}
}

void morning()                           // 아침 : 신앙심 +1
{
	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			student[r][c].believe++;
}

// a가 우선순위가 더 높으면 true
bool isPriority(STUDENT a, STUDENT b)
{
	if (a.believe != b.believe) return a.believe > b.believe;
	// 신앙심이 크면 우선, 같으면 행 -> 열이 작은 쪽
	if (a.row != b.row) return a.row < b.row;

	return a.col < b.col;                // 열이 작은 쪽
}

void BFS(int r, int c)                   // (r, c)가 속한 그룹 찾고 대표 처리
{
	int rp, wp;                          // 큐 위치

	rp = wp = 0;                         // 큐 초기화

	queue[wp].r = r;                     // 시작 칸 넣기
	queue[wp++].c = c;

	visit[r][c] = true;                  // 방문 표시

	ccnt = 0;                            // 그룹 인원 초기화
	candidates[ccnt++] = student[r][c];  // 시작 학생 추가

	while (rp < wp)                      // 큐가 빌 때까지
	{
		RC out = queue[rp++];            // 하나 꺼내기

		for (int i = 0; i < 4; i++)      // 4방향
		{
			int nr, nc;                  // 이웃 칸

			nr = out.r + dr[i];
			nc = out.c + dc[i];

			if (nr < 1 || nc < 1 || nr > N || nc > N) continue;   // 격자 밖
			if (visit[nr][nc] == true) continue;                 // 방문함

			// 인접한 학생들과 신봉 음식이 완전히 같은 경우에만 그룹을 형성
			if (student[out.r][out.c].food != student[nr][nc].food) continue;
			// (조합까지 완전히 같아야 한다. TC와 T는 다른 그룹이다)

			queue[wp].r = nr;            // 큐에 넣기
			queue[wp++].c = nc;

			visit[nr][nc] = true;        // 방문 표시

			candidates[ccnt++] = student[nr][nc];   // 그룹에 추가
		}
	}

	STUDENT leader = { 0 };              // 대표 (신앙심 0으로 시작해 누구든 이김)
	for (int i = 0; i < ccnt; i++)       // 그룹원 중
		if (isPriority(candidates[i], leader) == true)   // 더 우선이면
			leader = candidates[i];      // 대표 갱신

	student[leader.row][leader.col].isLeader = true;   // 대표 표시
	student[leader.row][leader.col].believe += (ccnt - 1);
	// 대표는 (인원 - 1)만큼 오르고, 아래에서 나머지는 1씩 내린다.

	for (int i = 0; i < ccnt; i++)       // 그룹원 중
	{
		STUDENT c = candidates[i];

		if (c.row == leader.row && c.col == leader.col) continue;   // 대표 제외

		student[c.row][c.col].believe--; // 신앙심 -1
	}
}

void lunch()                             // 점심 : 그룹 만들고 대표 뽑기
{
	for (int r = 1; r <= N; r++)         // 방문, 대표 표시 초기화
		for (int c = 1; c <= N; c++)
			visit[r][c] = student[r][c].isLeader = false;

	for (int r = 1; r <= N; r++)         // 모든 칸
	{
		for (int c = 1; c <= N; c++)
		{
			if (visit[r][c] == true) continue;   // 이미 그룹에 속함

			BFS(r, c);                   // 새 그룹 처리
		}
	}
}

int getGroupNumber(int food)             // 음식 조합의 단계 (1, 2, 3)
{
	if (food == TMINT_CHOKO_MILK) return 3;
	// 단일 음식 1단계, 두 가지 조합 2단계, 세 가지 조합 3단계
	if (food == TMINT || food == CHOKO || food == MILK) return 1;

	return 2;                            // 두 가지 조합
}

int mixFood(int sFood, int nFood)        // 음식 합치기
{
	return sFood | nFood;
	// 1, 10, 100의 비트가 겹치지 않아 OR가 곧 "음식 합치기"다.
}

void dinner()                            // 저녁 : 전파
{
	for (int g = 1; g <= 3; g++) index[g] = 0;   // 단계별 대표 수 초기화

	// 그룹 별 분리
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
		{
			STUDENT s = student[r][c];

			if (s.isLeader == false) continue;   // 대표만

			int groupNumber = getGroupNumber(s.food);   // 단계

			group[groupNumber][index[groupNumber]++] = s;   // 단계별 목록에 추가
		}
	}

	// 그룹 내 정렬
	for (int g = 1; g <= 3; g++)
	{
		int gIndex = index[g];           // 대표 수
		for (int i = 0; i < gIndex - 1; i++)       // 선택 정렬
		{
			for (int k = i + 1; k < gIndex; k++)
			{
				STUDENT a = group[g][i];
				STUDENT b = group[g][k];

				if (isPriority(a, b) == false)   // b가 더 우선이면
				{
					group[g][i] = b;     // 교환
					group[g][k] = a;
				}
			}
		}
	}

	// printGroup();

	// 전파 시작
	for (int g = 1; g <= 3; g++)         // 단계 순서대로
	{
		int gIndex = index[g];
		for (int i = 0; i < gIndex; i++) // 대표 순서대로
		{
			int gr, gc;                  // 대표 좌표

			gr = group[g][i].row;
			gc = group[g][i].col;

			STUDENT spreader = student[gr][gc];   // 지금 값으로 다시 읽기

			// 방어 상태에서는 대표자가 되어도 전파를 하지 않음.
			if (spreader.defense != 0) continue;

			int sr, sc;                  // 전파 현재 위치

			sr = spreader.row;
			sc = spreader.col;

			// 원본의 신앙심을 1로 변경
			student[sr][sc].believe = 1;

			int x = spreader.believe - 1; // 간절함
			int dir = spreader.believe % 4;   // 전파 방향

			while (1)
			{
				int nr, nc;              // 다음 칸

				nr = sr + dr[dir];
				nc = sc + dc[dir];

				// 격자 밖으로 나가거나 간절함이 0이 되면 전파 종료
				if (nr < 1 || nc < 1 || nr > N || nc > N || x == 0) break;

				// 전파 대상
				STUDENT next = student[nr][nc];

				// 전파 대상이 전파자와 신봉 음식이 완전히 같은 경우
				if (next.food == spreader.food)
				{
					// 전파를 하지 않고 바로 다음으로 진행
					sr = nr;
					sc = nc;

					continue;
				}

				// 전파 대상이 전파자와 신봉 음식이 다른 경우, 전파 시도
				int y = next.believe; // 신앙심

				// 대표자에게 전파가 시도된 학생은 방어 상태
				student[next.row][next.col].defense = 1;
				// 전파를 당한 학생은 오늘 대표여도 전파하지 못한다.

				if (x > y) // 강한 전파 성공
				{
					x -= (y + 1);        // 간절함 감소
					if (x < 0) x = 0;    // 음수 방지

					student[next.row][next.col].believe++;              // 신앙심 +1
					student[next.row][next.col].food = spreader.food;   // 음식 교체
				}
				else // x <= y, 약한 전파 성공
				{
					student[next.row][next.col].believe += x;           // 신앙심 += x
					student[next.row][next.col].food = mixFood(spreader.food, next.food);   // 음식 합치기

					x = 0;               // 간절함 소진

					break;               // 전파 끝
				}

				sr = nr;                 // 한 칸 진행
				sc = nc;
			}
		}
	}

	for (int r = 1; r <= N; r++)         // 방어 상태 해제
		for (int c = 1; c <= N; c++)
			student[r][c].defense = 0;
}

void printAnswer()                       // 조합별 신앙심 합 출력
{
	int sum[TMINT_CHOKO_MILK + 1] = { 0 };   // 음식 값을 인덱스로 쓰는 합 배열

	for (int r = 1; r <= N; r++)         // 모든 학생
		for (int c = 1; c <= N; c++)
			sum[student[r][c].food] += student[r][c].believe;   // 합 누적

	int outputIndex[7]
		= { TMINT_CHOKO_MILK, TMINT_CHOKO, TMINT_MILK, CHOKO_MILK, MILK, CHOKO, TMINT };   // 출력 순서

	for (int i = 0; i < 7; i++)          // 순서대로 출력
		printf("%d ", sum[outputIndex[i]]);
	putchar('\n');                       // 줄바꿈
}

void simulate()
{
	for (int t = 0; t < T; t++)          // T일
	{
		morning();                       // 아침
		lunch();                         // 점심
		dinner();                        // 저녁

		printAnswer();                   // 결과 출력
	}
}

int main()
{
	// scanf("%d", &TestCase);
	TestCase = 1;                        // 테스트 케이스 1개
	for (int tc = 1; tc <= TestCase; tc++)   // 테스트 케이스 반복
	{
		input();                         // 입력

		simulate();                      // 시뮬레이션
	}

	return 0;                            // 프로그램 정상 종료
}
