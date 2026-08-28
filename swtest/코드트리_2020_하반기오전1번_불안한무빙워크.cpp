/*
    [코드트리] 2020 하반기 오전 1번 - 불안한 무빙워크
    (백준 20055 "컨베이어 벨트 위의 로봇"과 같은 문제다  https://www.acmicpc.net/problem/20055)

    ■ 문제 요약
      칸 2N개가 원형으로 이어진 무빙워크가 있다. 위쪽 1번 칸이 올라타는 자리, N번 칸이 내리는 자리다.
      각 칸에는 안정성(내구도)이 있고, 사람이 올라타거나 이동해 도착하면 1 줄어든다.

      한 단계는 아래 순서로 진행된다.

        1) 무빙워크가 한 칸 회전한다. 이때 N번 위치에 온 사람은 바로 내린다.
        2) 무빙워크 위의 사람들이 (내리는 쪽에 가까운 순서로) 한 칸씩 앞으로 간다.
           앞 칸에 사람이 없고 안정성이 0이 아니어야 갈 수 있다.
           이동해서 N번 위치에 닿으면 바로 내린다.
        3) 1번 칸에 사람이 없고 안정성이 0이 아니면 새 사람이 올라탄다.
        4) 안정성이 0인 칸이 K개 이상이면 종료한다.

      종료되는 단계 번호를 출력한다.

    ■ 자료 구조
        moving[i]       : 지금 i번 "위치"에 놓인 칸의 정보 (안정성, 그 위의 사람 번호)
        position[p]     : p번 사람이 있는 위치 (내렸으면 -1)
        livePeople[]    : 지금 무빙워크 위에 있는 사람 번호 목록

      여기서 "칸"과 "위치"를 구분하는 것이 중요하다.
      회전은 칸이 옮겨 다니는 것이고, 사람은 칸이 아니라 위치를 기준으로 움직인다.
      그래서 rotate()로 칸 배열을 통째로 한 칸 밀고,
      사람의 position은 따로 +1 해 주는 두 단계로 나눠 처리한다.

    ■ 사람 이동 순서가 중요하다
      앞사람이 비켜 줘야 뒷사람이 갈 수 있으므로, 반드시 "내리는 쪽에 가까운 사람부터" 움직여야 한다.
      livePeople은 새 사람을 항상 뒤에 붙이는 방식으로 관리되고,
      사람들은 서로를 추월하지 못하므로 이 배열의 순서가 언제나 "앞선 사람부터"가 된다.
      따라서 배열을 앞에서부터 훑기만 하면 순서가 저절로 맞는다.

      매 단계 새 배열(nextlcnt)로 다시 담으면서 내린 사람을 자연스럽게 제외한다.
      이때도 순서는 그대로 유지된다.

    ■ 안정성 카운트
      안정성은 계속 줄어들 수 있으므로, "0이 되는 순간"에만 crush를 올려야 한다.
      0이 된 칸을 매번 세는 방식이면 O(N) 스캔이 필요하지만
      이렇게 하면 감소 시점에 한 번만 세면 된다.

    ■ 내리는 처리가 두 군데인 이유
      회전만으로 N번 위치에 도착하는 경우(1단계)와
      스스로 한 칸 걸어가서 N번에 닿는 경우(2단계)가 따로 있기 때문이다.
      두 경우 모두 즉시 내리므로 position을 -1로 두고 칸도 비워 준다.
*/

#include <stdio.h>

#define MAX (100 + 20)

int T;

int N, K;

// 무빙워크 칸 하나
struct MOVING
{
    int index;          // 원래 칸 번호 (디버그용)
    int peopleNumber;   // 이 칸 위의 사람 번호 (0이면 비어 있음)
    int stability;      // 남은 안정성
};

typedef struct MOVING MOVING;

// moving[i] : 지금 i번 위치에 놓인 칸. 실제 사용 범위는 1 ~ 2N.
MOVING moving[MAX * 2];

// position[p] : p번 사람의 현재 위치 (내렸으면 -1)
// 사람 번호가 단계마다 계속 늘어나므로 넉넉하게 잡는다.
int position[200000 + 5000];

// 지금 무빙워크 위에 있는 사람들 (앞선 사람부터 순서대로)
int livePeople[MAX];
int lcnt;

/*
    입력
*/
void input()
{
    scanf("%d %d", &N, &K);

    for (int i = 1; i <= 2 * N; i++)
    {
        scanf("%d", &moving[i].stability);

        moving[i].index = i;        // 회전을 눈으로 따라가기 위한 표시
        moving[i].peopleNumber = 0; // 처음에는 아무도 없다
    }

    lcnt = 0;
}

/*
    디버그용 출력

    위쪽 N칸은 왼쪽에서 오른쪽으로, 아래쪽 N칸은 반대로 출력해서
    실제 무빙워크가 도는 모양처럼 보이게 했다.
*/
void printMoving()
{
    for (int i = 1; i <= N; i++)
        printf("(%d, %d, %d) ",
            moving[i].index,
            moving[i].stability,
            moving[i].peopleNumber);
    putchar('\n');

    for (int i = 2 * N; i > N; i--)
        printf("(%d, %d, %d)",
            moving[i].index,
            moving[i].stability,
            moving[i].peopleNumber);
    putchar('\n');
    putchar('\n');
}

/*
    무빙워크 회전

    마지막 위치(2N)의 칸이 1번 위치로 오고 나머지는 한 칸씩 밀린다.
    칸만 옮길 뿐 사람의 position은 건드리지 않으므로,
    호출한 쪽에서 사람 위치를 따로 +1 해 줘야 한다.
*/
void rotate()
{
    MOVING tmp = moving[2 * N];

    for (int i = 2 * N - 1; i >= 1; i--)
        moving[i + 1] = moving[i];

    moving[1] = tmp;
}

/*
    종료될 때까지 한 단계씩 진행
*/
int simulate()
{
    int step = 0;     // 단계 번호
    int crush = 0;    // 안정성이 0이 된 칸의 개수
    int pIndex = 1;   // 다음에 올라탈 사람 번호

    while (1)
    {
        step++;

        // 1) 무빙워크 회전 (칸이 이동한다)
        rotate();

        // 1-2) 회전에 따라 사람의 위치도 한 칸 뒤로 밀린다
        for (int i = 0; i < lcnt; i++)
        {
            int peopleNumber = livePeople[i];
            position[peopleNumber] = position[peopleNumber] + 1;
        }

        // 2) 사람들이 스스로 한 칸씩 이동
        //    livePeople이 "앞선 사람부터" 순서라서 앞에서부터 훑으면 된다.
        int nextlcnt = 0;

        for (int i = 0; i < lcnt; i++)
        {
            int peopleNumber = livePeople[i];
            int curPos = position[peopleNumber];

            // 회전만으로 내리는 자리에 도착한 경우 -> 바로 내린다
            if (curPos == N)
            {
                position[peopleNumber] = -1;
                moving[curPos].peopleNumber = 0;
                continue;
            }

            int nextPos = curPos + 1;

            // 앞이 막혔거나(사람) 밟을 수 없으면(안정성 0) 제자리
            if (moving[nextPos].peopleNumber != 0 || moving[nextPos].stability == 0)
            {
                livePeople[nextlcnt++] = peopleNumber;   // 그대로 남는다
                continue;
            }

            // 이동
            moving[curPos].peopleNumber = 0;
            moving[nextPos].peopleNumber = peopleNumber;

            // 도착한 칸의 안정성이 줄어든다
            moving[nextPos].stability--;

            // 0이 "되는 순간"에만 세어야 중복으로 세지 않는다
            if (moving[nextPos].stability == 0) crush++;

            position[peopleNumber] = nextPos;

            // 걸어가서 내리는 자리에 닿았으면 바로 내린다
            if (nextPos == N)
            {
                position[peopleNumber] = -1;
                moving[nextPos].peopleNumber = 0;
            }
            else
            {
                livePeople[nextlcnt++] = peopleNumber;
            }
        }

        // 3) 새 사람 탑승
        if (moving[1].peopleNumber == 0 && moving[1].stability != 0)
        {
            position[pIndex] = 1;
            livePeople[nextlcnt++] = pIndex;   // 가장 뒤에 붙는다 (순서 유지)

            moving[1].peopleNumber = pIndex++;

            moving[1].stability--;

            if (moving[1].stability == 0) crush++;
        }

        lcnt = nextlcnt;

        // 4) 종료 조건
        if (crush >= K) return step;
    }

    return -1; // 안정성은 계속 줄기만 하므로 반드시 끝난다 (도달하지 않는 줄)
}

int main()
{
    // 이 문제는 테스트 케이스가 하나다
    // scanf("%d", &T);
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        input();

        printf("%d\n", simulate());
    }

    return 0;
}
