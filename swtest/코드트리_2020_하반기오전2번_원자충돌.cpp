/*
    [코드트리] 2020 하반기 오전 2번 - 원자 충돌
    (백준 20056 "마법사 상어와 파이어볼"과 같은 문제다  https://www.acmicpc.net/problem/20056)

    ■ 문제 요약
      N x N 격자에 원자 M개가 있고, 각 원자는 위치, 질량 m, 속력 s, 방향 d를 가진다.
      격자는 상하좌우가 이어진 토러스다.

      한 턴은 아래와 같다.

        1) 모든 원자가 동시에 자기 방향으로 속력 s칸만큼 이동한다.
        2) 이동 후 같은 칸에 2개 이상 모이면 충돌해서 하나로 합쳐졌다가 4개로 나뉜다.
             새 질량 = (합친 질량) / 5
             새 속력 = (합친 속력) / (합친 개수)
             새 방향 = 원래 방향이 모두 짝수이거나 모두 홀수면 0, 2, 4, 6
                       그렇지 않으면 1, 3, 5, 7
           새 질량이 0이면 그 원자들은 사라진다.
        3) 혼자 있는 원자는 아무 일도 일어나지 않는다.

      K번 반복한 뒤 남은 원자들의 질량 합을 출력한다.

    ■ 자료 구조 : 격자가 아니라 "원자 목록"을 들고 다닌다
      한 칸에 원자가 여러 개 겹칠 수 있고 개수도 계속 바뀌므로, 격자에 직접 담기 어렵다.
      그래서 살아 있는 원자를 배열 atom[]으로 관리하고,
      한 턴이 끝날 때마다 이 배열을 통째로 새로 만든다.

      턴 처리는 두 단계다.

        (1) 모든 원자를 이동시켜 도착 칸별로 합계를 MAP에 모은다
            (질량 합, 속력 합, 짝수/홀수 방향 개수, 원자 개수)
        (2) MAP을 훑으며 새 atom[] 배열을 만든다
            개수가 1이면 그대로, 2 이상이면 충돌 규칙을 적용

      이렇게 나누면 "모든 원자가 동시에 움직인다"는 조건이 저절로 지켜진다.
      이동 중에 원자 목록을 고치지 않기 때문이다.

    ■ MAP에 합계만 모아도 되는 이유
      충돌 결과를 계산하는 데 필요한 값이 전부 합계나 개수다.
      개별 원자의 정보를 따로 보관할 필요가 없어서 칸마다 구조체 하나면 충분하다.

      다만 원자가 하나뿐일 때는 "원래 방향"을 그대로 돌려줘야 하므로 d도 같이 담아 둔다.
      두 개 이상 모인 칸의 d는 마지막에 덮인 값이라 의미가 없지만,
      그런 칸에서는 어차피 새 방향을 다시 정하므로 문제가 없다.

    ■ 토러스 이동
      속력이 N보다 클 수 있으므로 s % N 만큼만 움직이면 결과가 같다.
      이렇게 줄여 두면 좌표 보정이 한 번으로 끝난다.

        이동 전 좌표는 1 ~ N, 변화량은 -(N-1) ~ (N-1) 이므로
        결과는 2-N ~ 2N-1 범위이고, 한쪽으로 N을 더하거나 빼면 반드시 1 ~ N에 들어온다.
        (s % N 을 빼먹으면 이 한 번의 보정이 깨진다)

    ■ 방향 번호
        0 = ↑, 1 = ↗, 2 = →, 3 = ↘, 4 = ↓, 5 = ↙, 6 = ←, 7 = ↖

      짝수는 상하좌우, 홀수는 대각선이다.
      그래서 "모두 짝수거나 모두 홀수"라는 조건이 곧 "모두 직선이거나 모두 대각선"이라는 뜻이 된다.
*/

#include <stdio.h>

#define MAX (50 + 5)

int T;
int N, M, K;

// 원자 하나
struct ATOM
{
    int r;   // 행
    int c;   // 열
    int m;   // 질량
    int s;   // 속력
    int d;   // 방향
};

typedef struct ATOM ATOM;

// 살아 있는 원자 목록. 충돌마다 칸당 4개가 생기므로 넉넉히 잡는다.
ATOM atom[100000 + 5000];
int acnt;

// 방향 배열 : ↑, ↗, →, ↘, ↓, ↙, ←, ↖
int dr[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
int dc[] = { 0,  1, 1, 1, 0,-1,-1, -1 };

// 한 칸에 모인 원자들의 합계 정보
struct INFO
{
    int m;       // 질량 합
    int s;       // 속력 합
    int d;       // 방향 (원자가 하나뿐일 때만 의미가 있다)
    int even;    // 방향이 짝수인 원자 수
    int odd;     // 방향이 홀수인 원자 수
    int count;   // 이 칸에 모인 원자 수
};

typedef struct INFO INFO;

/*
    입력
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
    디버그용: 현재 원자 목록 출력
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
    K번의 턴을 진행
*/
void simulate()
{
    for (int k = 0; k < K; k++)
    {
        // 이번 턴에 각 칸으로 모이는 원자들의 합계. 매 턴 새로 비운다.
        INFO MAP[MAX][MAX] = { 0 };

        // -------------------------
        // 1) 모든 원자를 이동시켜 도착 칸에 합산
        // -------------------------
        for (int i = 0; i < acnt; i++)
        {
            int nr, nc;
            int d, s, m;

            d = atom[i].d;
            s = atom[i].s;
            m = atom[i].m;

            // 한 바퀴(N칸)는 제자리이므로 s % N 만큼만 움직이면 된다
            nr = atom[i].r + dr[d] * (s % N);
            nc = atom[i].c + dc[d] * (s % N);

            // 토러스 보정. 위에서 이동량을 줄여 뒀기 때문에 한 번이면 충분하다.
            if (nr > N) nr -= N;
            if (nc > N) nc -= N;
            if (nr < 1) nr += N;
            if (nc < 1) nc += N;

            MAP[nr][nc].m += m;
            MAP[nr][nc].s += s;

            // 이 칸에 원자가 하나뿐이면 이 방향을 그대로 쓴다.
            // 여럿이면 어차피 새 방향을 다시 정하므로 덮여도 상관없다.
            MAP[nr][nc].d = d;

            // 짝수(직선) / 홀수(대각선) 방향 개수를 세어 둔다
            if (d % 2 == 0) MAP[nr][nc].even++;
            else MAP[nr][nc].odd++;

            MAP[nr][nc].count++;
        }

        // -------------------------
        // 2) 합계를 보고 다음 턴의 원자 목록을 새로 만든다
        // -------------------------
        acnt = 0;

        for (int r = 1; r <= N; r++)
        {
            for (int c = 1; c <= N; c++)
            {
                if (MAP[r][c].count == 0) continue;

                // 혼자 있는 원자는 아무 일 없이 그대로 살아남는다
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

                // 여기부터는 2개 이상이 모인 충돌 상황이다.

                // 나뉜 뒤의 질량이 0이면 4개를 만들지 않고 통째로 사라진다
                if ((MAP[r][c].m / 5) == 0) continue;

                int dir[4] = { 0 };

                // 모두 짝수(직선)이거나 모두 홀수(대각선)이면 새 방향은 직선 4개
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

                // 같은 자리에 네 개의 새 원자가 생긴다
                for (int i = 0; i < 4; i++)
                {
                    atom[acnt].r = r;
                    atom[acnt].c = c;

                    atom[acnt].m = MAP[r][c].m / 5;                 // 합친 질량 / 5
                    atom[acnt].s = MAP[r][c].s / MAP[r][c].count;   // 합친 속력 / 개수

                    atom[acnt].d = dir[i];
                    acnt++;
                }
            }
        }
    }
}

/*
    남아 있는 원자들의 질량 합
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
    // 이 문제는 테스트 케이스가 하나다
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
