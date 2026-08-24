// 코드트리 2020 하반기 오전 2번 - 원자 충돌
//
// 문제 요약
// N x N 격자 위에 M개의 원자가 있다.
// 각 원자는 위치(r, c), 질량(m), 속력(s), 방향(d)을 가진다.
// 격자는 위아래, 좌우가 연결된 원형 격자이다.
//
// 매 턴 진행 규칙
// 1. 모든 원자는 자신의 방향으로 속력 s만큼 이동한다.
// 2. 이동 후 같은 칸에 원자가 2개 이상 모이면 충돌한다.
// 3. 충돌한 원자들은 하나로 합쳐진 뒤 다시 4개로 분리된다.
//    - 새 질량 = 합쳐진 질량 / 5
//    - 새 속력 = 합쳐진 속력 / 합쳐진 원자 개수
//    - 원래 방향들이 모두 짝수이거나 모두 홀수이면 새 방향은 0, 2, 4, 6
//    - 그렇지 않으면 새 방향은 1, 3, 5, 7
// 4. 새 질량이 0이면 원자는 소멸한다.
//
// 목표
// K번의 이동과 충돌이 끝난 뒤 남아 있는 원자들의 질량 합을 구한다.
//
// 핵심 아이디어
// - atom[] : 현재 살아 있는 원자 목록
// - MAP[r][c] : 한 턴 동안 이동한 원자들을 칸별로 합산하는 임시 배열
// - MAP[r][c].count == 1이면 충돌 없이 그대로 유지
// - MAP[r][c].count >= 2이면 충돌 후 4개로 분리하거나 소멸
//
// 즉,
// "원자 이동 -> 같은 칸 원자 합치기 -> 분리/소멸 처리"
// 를 K번 반복하는 시뮬레이션 문제이다.

#include <stdio.h>

#define MAX (50 + 5)

int T;
int N, M, K;

// 원자 정보 구조체
struct ATOM
{
    int r;   // 현재 행
    int c;   // 현재 열
    int m;   // 질량
    int s;   // 속력
    int d;   // 방향
};

typedef struct ATOM ATOM;

// 현재 살아 있는 원자 배열
ATOM atom[100000 + 5000];

// 현재 살아 있는 원자 개수
int acnt;

// 방향 번호
// ↑, ↗, →, ↘, ↓, ↙, ←, ↖
int dr[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
int dc[] = { 0,  1, 1, 1, 0,-1,-1, -1 };

// 한 칸에 모인 원자들의 합산 정보를 저장하는 구조체
struct INFO
{
    int m;       // 질량 합
    int s;       // 속력 합
    int d;       // 방향, 원자가 1개인 경우 기존 방향 복원용
    int even;    // 짝수 방향 원자 개수
    int odd;     // 홀수 방향 원자 개수
    int count;   // 해당 칸에 모인 원자 개수
};

typedef struct INFO INFO;

/*
    입력 함수

    N : 격자 크기
    M : 초기 원자 개수
    K : 시뮬레이션 반복 횟수

    각 원자의 위치, 질량, 속력, 방향을 입력받는다.
*/
void input()
{
    scanf("%d %d %d", &N, &M, &K);

    acnt = 0;

    for (int i = 0; i < M; i++)
    {
        scanf("%d %d %d %d %d",
            &atom[acnt].r,
            &atom[acnt].c,
            &atom[acnt].m,
            &atom[acnt].s,
            &atom[acnt].d);

        acnt++;
    }
}

/*
    디버그용 원자 출력 함수

    현재 atom 배열에 저장된 모든 원자의 상태를 출력한다.
*/
void printAtom()
{
    for (int i = 0; i < acnt; i++)
    {
        printf("%d %d %d %d %d\n",
            atom[i].r,
            atom[i].c,
            atom[i].m,
            atom[i].s,
            atom[i].d);
    }
    putchar('\n');
}

/*
    전체 시뮬레이션 함수

    K번 반복하면서
    1. 모든 원자 이동
    2. 같은 칸에 도착한 원자들의 질량/속력/방향 정보를 합산
    3. MAP을 확인하면서 다음 턴의 atom 배열을 새로 구성

    매 턴마다 atom 배열을 새로 만드는 방식이므로,
    이전 턴의 원자 목록은 MAP에 합산된 뒤 사라지고
    새로 생성된 원자들만 atom[]에 다시 저장된다.
*/
void simulate()
{
    for (int k = 0; k < K; k++)
    {
        // MAP[r][c] = 이번 턴 이동 후 (r,c)에 모인 원자들의 합산 정보
        INFO MAP[MAX][MAX] = { 0 };

        // -------------------------
        // 1. 모든 원자 이동
        // -------------------------
        for (int i = 0; i < acnt; i++)
        {
            int nr, nc;
            int d, s, m;

            d = atom[i].d;
            s = atom[i].s;
            m = atom[i].m;

            // 속력이 N보다 클 수 있으므로 s % N만큼만 이동해도 결과가 같다.
            nr = atom[i].r + dr[d] * (s % N);
            nc = atom[i].c + dc[d] * (s % N);

            // 격자는 원형으로 연결되어 있으므로 범위를 벗어나면 반대편으로 이동
            if (nr > N) nr -= N;
            if (nc > N) nc -= N;
            if (nr < 1) nr += N;
            if (nc < 1) nc += N;

            // -------------------------
            // 2. 이동한 칸에 원자 정보 합산
            // -------------------------
            MAP[nr][nc].m += m;
            MAP[nr][nc].s += s;

            // 원자가 1개인 경우에만 유효한 방향 정보
            // count == 1이면 이 값 그대로 원자의 방향이 된다.
            MAP[nr][nc].d = d;

            // 방향이 짝수인지 홀수인지 개수 저장
            if (d % 2 == 0) MAP[nr][nc].even++;
            else MAP[nr][nc].odd++;

            // 해당 칸에 모인 원자 개수 증가
            MAP[nr][nc].count++;
        }

        // -------------------------
        // 3. MAP을 보고 다음 atom 배열 생성
        // -------------------------
        acnt = 0;

        for (int r = 1; r <= N; r++)
        {
            for (int c = 1; c <= N; c++)
            {
                // 원자가 없는 칸은 무시
                if (MAP[r][c].count == 0) continue;

                // -------------------------
                // 3-1. 원자가 1개만 있는 경우
                // -------------------------
                // 충돌이 없으므로 그대로 atom 배열에 다시 넣는다.
                if (MAP[r][c].count == 1)
                {
                    atom[acnt].r = r;
                    atom[acnt].c = c;
                    atom[acnt].m = MAP[r][c].m;
                    atom[acnt].s = MAP[r][c].s;
                    atom[acnt].d = MAP[r][c].d;
                    acnt++;

                    continue;
                }

                // -------------------------
                // 3-2. 원자가 2개 이상인 경우: 충돌 처리
                // -------------------------

                // 새 질량은 합쳐진 질량 / 5
                // 새 질량이 0이면 분리되지 않고 원자는 소멸한다.
                if ((MAP[r][c].m / 5) == 0) continue;

                // 새 방향 결정
                int dir[4] = { 0 };

                // 모든 원자의 방향이 짝수이거나 모든 원자의 방향이 홀수인 경우
                if (MAP[r][c].even == MAP[r][c].count
                    || MAP[r][c].odd == MAP[r][c].count)
                {
                    dir[0] = 0;
                    dir[1] = 2;
                    dir[2] = 4;
                    dir[3] = 6;
                }
                else
                {
                    dir[0] = 1;
                    dir[1] = 3;
                    dir[2] = 5;
                    dir[3] = 7;
                }

                // 4개의 새 원자로 분리
                for (int i = 0; i < 4; i++)
                {
                    atom[acnt].r = r;
                    atom[acnt].c = c;

                    // 새 질량 = 합쳐진 질량 / 5
                    atom[acnt].m = MAP[r][c].m / 5;

                    // 새 속력 = 합쳐진 속력 / 합쳐진 원자 개수
                    atom[acnt].s = MAP[r][c].s / MAP[r][c].count;

                    atom[acnt].d = dir[i];
                    acnt++;
                }
            }
        }
    }
}

/*
    정답 계산 함수

    현재 살아 있는 모든 원자의 질량 합을 반환한다.
*/
int getAnswer()
{
    int sum = 0;

    for (int i = 0; i < acnt; i++)
        sum += atom[i].m;

    return sum;
}

int main()
{
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        simulate();

        printf("%d\n", getAnswer());
    }

    return 0;
} 