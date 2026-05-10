// 코드트리 2020 하반기 오전 1번 - 불안한 무빙워크
//
// 문제 요약
// 길이가 2N인 무빙워크가 원형으로 연결되어 있다.
// 사람은 1번 칸에 올라가고, N번 칸에 도착하면 즉시 내린다.
// 각 칸에는 안정성이 있으며, 사람이 올라가거나 이동해서 도착하면 안정성이 1 감소한다.
//
// 매 단계 진행 순서
// 1. 무빙워크가 한 칸 회전한다.
// 2. 무빙워크 위에 있는 사람들이 앞 칸으로 이동 가능한 경우 이동한다.
// 3. 1번 칸에 사람이 없고 안정성이 0이 아니면 새 사람을 올린다.
// 4. 안정성이 0인 칸의 개수가 K개 이상이면 종료한다.
//
// 목표
// 안정성이 0인 칸의 개수가 K개 이상이 되는 최초 단계를 출력한다.
//
// 핵심 아이디어
// - moving[i] : i번 위치의 무빙워크 칸 정보
// - peopleNumber : 해당 칸에 있는 사람 번호
// - stability : 해당 칸의 안정성
// - position[p] : p번 사람이 현재 몇 번째 위치에 있는지
// - livePeople[] : 현재 무빙워크 위에 있는 사람 번호 목록
//
// 즉,
// "무빙워크 회전 -> 사람 이동 -> 새 사람 탑승 -> 안정성 체크"
// 를 반복하는 시뮬레이션 문제이다.

#include <stdio.h>

#define MAX (100 + 20)

int T;

int N, K;

// 무빙워크 한 칸의 정보
struct MOVING
{
    int index;          // 원래 칸 번호, 디버그용
    int peopleNumber;   // 현재 이 칸 위에 있는 사람 번호, 없으면 0
    int stability;      // 현재 안정성
};

typedef struct MOVING MOVING;

// moving[i] = 현재 i번째 위치에 있는 무빙워크 칸 정보
// 실제 사용 범위는 1 ~ 2N
MOVING moving[MAX * 2];

// position[i] = i번 사람이 현재 있는 무빙워크 위치
// 사람이 내린 경우 -1
int position[200000 + 5000];

// livePeople[i] = 현재 무빙워크 위에 있는 사람 번호
int livePeople[MAX];

// 현재 무빙워크 위에 있는 사람 수
int lcnt;

/*
    입력 함수

    N : 위쪽 무빙워크 길이
    K : 안정성이 0인 칸이 K개 이상이면 종료

    총 2N개의 칸 안정성을 입력받는다.
*/
void input()
{
    scanf("%d %d", &N, &K);

    for (int i = 1; i <= 2 * N; i++)
    {
        scanf("%d", &moving[i].stability);

        // 원래 칸 번호 저장, 디버그용
        moving[i].index = i;

        // 처음에는 어떤 칸에도 사람이 없음
        moving[i].peopleNumber = 0;
    }

    lcnt = 0;
}

/*
    디버그용 출력 함수

    위쪽 N칸은 왼쪽에서 오른쪽으로 출력하고,
    아래쪽 N칸은 오른쪽에서 왼쪽으로 출력한다.
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
    무빙워크 회전 함수

    2N번 위치의 칸이 1번 위치로 오고,
    나머지 칸들은 한 칸씩 뒤로 밀린다.

    이 함수는 칸 자체를 회전시키는 것이고,
    사람의 position 배열은 simulate()에서 따로 갱신한다.
*/
void rotate()
{
    MOVING tmp = moving[2 * N];

    for (int i = 2 * N - 1; i >= 1; i--)
        moving[i + 1] = moving[i];

    moving[1] = tmp;
}

/*
    전체 시뮬레이션 함수

    매 단계마다
    1. 무빙워크 회전
    2. 회전에 따른 사람 위치 갱신
    3. 사람이 직접 한 칸 이동
    4. 새 사람 탑승
    5. 안정성 0인 칸 개수 확인

    반환값
    - 안정성이 0인 칸이 K개 이상이 되는 최초 단계
*/
int simulate()
{
    int step = 0;     // 현재 단계 수
    int crush = 0;    // 안정성이 0이 된 칸 개수
    int pIndex = 1;   // 새로 올라올 사람 번호

    while (1)
    {
        step++;

        // -------------------------
        // 1. 무빙워크 회전
        // -------------------------
        rotate();

        // -------------------------
        // 1-2. 회전에 따라 사람 위치도 한 칸 증가
        // -------------------------
        for (int i = 0; i < lcnt; i++)
        {
            int peopleNumber = livePeople[i];
            position[peopleNumber] = position[peopleNumber] + 1;
        }

        // -------------------------
        // 2. 무빙워크 위에서 사람 이동
        // -------------------------
        int nextlcnt = 0;

        for (int i = 0; i < lcnt; i++)
        {
            int peopleNumber = livePeople[i];
            int curPos = position[peopleNumber];

            // 회전만으로 N번 위치에 도착한 경우 즉시 내림
            if (curPos == N)
            {
                position[peopleNumber] = -1;
                moving[curPos].peopleNumber = 0;
                continue;
            }

            int nextPos = curPos + 1;

            // 다음 칸에 사람이 있거나 안정성이 0이면 이동 불가
            if (moving[nextPos].peopleNumber != 0 || moving[nextPos].stability == 0)
            {
                // 이동하지 못한 사람은 계속 무빙워크 위에 남음
                livePeople[nextlcnt++] = peopleNumber;
                continue;
            }

            // 현재 칸에서 사람 제거
            moving[curPos].peopleNumber = 0;

            // 다음 칸으로 사람 이동
            moving[nextPos].peopleNumber = peopleNumber;

            // 사람이 이동해서 도착한 칸의 안정성 감소
            moving[nextPos].stability--;

            // 안정성이 처음 0이 되면 crush 증가
            if (moving[nextPos].stability == 0) crush++;

            // 사람 위치 갱신
            position[peopleNumber] = nextPos;

            // 이동 후 N번 위치에 도착하면 즉시 내림
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

        // -------------------------
        // 3. 새 사람 추가
        // -------------------------
        // 1번 칸에 사람이 없고 안정성이 0이 아니면 새 사람 탑승
        if (moving[1].peopleNumber == 0 && moving[1].stability != 0)
        {
            position[pIndex] = 1;
            livePeople[nextlcnt++] = pIndex;

            moving[1].peopleNumber = pIndex++;

            // 사람이 올라간 칸의 안정성 감소
            moving[1].stability--;

            // 안정성이 처음 0이 되면 crush 증가
            if (moving[1].stability == 0) crush++;
        }

        // 현재 무빙워크 위에 남아 있는 사람 수 갱신
        lcnt = nextlcnt;

        // -------------------------
        // 4. 종료 조건 확인
        // -------------------------
        if (crush >= K) return step;
    }

    return -1; // 정상 상황에서는 도달하지 않음
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