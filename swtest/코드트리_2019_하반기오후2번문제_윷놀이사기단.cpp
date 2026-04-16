// 코드트리 2019 하반기 오후 2번 - 윷놀이 사기단
//
// 문제 요약
// 말 4개가 있고, 10번의 이동 횟수가 주어진다.
// 매 턴마다 4개의 말 중 하나를 골라 이동시킬 수 있다.
//
// 보드는 일반 경로와 파란색 지름길 경로가 있으며,
// 특정 칸(10, 20, 30)에 처음 도착하면 지름길로 진입한다.
//
// 규칙
// 1. 매 턴마다 4개의 말 중 하나를 선택해서 이동시킨다.
// 2. 이미 도착한 말은 더 이동해도 계속 도착 위치에 머문다.
// 3. 도착 칸(끝점)을 제외하고, 같은 칸에 두 말이 동시에 있을 수 없다.
// 4. 10번의 이동을 모두 수행했을 때 얻을 수 있는 점수 합의 최댓값을 구한다.
//
// 핵심 아이디어
// - next[pos][k] : pos 위치에서 k칸 이동했을 때 도착 위치를 미리 저장
// - score[pos]   : 각 위치의 점수
// - DFS로 10턴 동안 어떤 말을 움직일지 전부 탐색
// - 각 경우마다 simulate()로 실제 점수를 계산
//
// 즉,
// "복잡한 보드 이동"은 next 테이블로 처리하고,
// "말 선택의 모든 경우"는 DFS로 완전탐색하는 방식의 코드이다.

#include <stdio.h>
#include <stdbool.h>   // C에서 bool, true, false 사용하려면 필요

// 말 이동 횟수는 10번
#define MAX (10 + 3)

// 보드 칸 개수 여유 있게 선언
#define MAX_BOARD (32 + 10)

// 도착 지점 번호
#define DESTINATION (32)

int T;

// move[i] : i번째 턴의 이동 칸 수
int move[MAX];

// next[pos][k] = 현재 pos에 있는 말이 k칸 이동했을 때 도착하는 다음 위치
int next[MAX_BOARD][6];

// score[pos] = pos 위치에 도착했을 때 얻는 점수
int score[MAX_BOARD];

// num_of_cases[i] = i번째 턴에 어떤 말을 움직일지 저장 (1~4)
int num_of_cases[MAX];

// 최대 점수 정답
int maxAnswer;

/*
    입력 및 보드 정보 초기화

    move[0..9] : 10번의 이동 횟수 입력
    score[]    : 각 칸의 점수 설정
    next[][]   : 보드 경로를 반영한 이동 테이블 구성

    이 문제는 실제 보드 그림을 따라 이동 규칙이 복잡하므로,
    "현재 위치 + 이동 칸 수 -> 다음 위치"를 전부 미리 저장해둔다.
*/
void input()
{
    // 10번의 이동 횟수 입력
    for (int i = 0; i < 10; i++) scanf("%d", &move[i]);

    // -------------------------
    // 각 위치의 점수 설정
    // -------------------------
    // 바깥 기본 경로 점수
    score[1] = 2;   score[2] = 4;   score[3] = 6;   score[4] = 8;   score[5] = 10;
    score[6] = 12;  score[7] = 14;  score[8] = 16;  score[9] = 18;  score[10] = 20;
    score[11] = 22; score[12] = 24; score[13] = 26; score[14] = 28; score[15] = 30;
    score[16] = 32; score[17] = 34; score[18] = 36; score[19] = 38;

    // 지름길 및 공통 중앙 경로 점수
    score[20] = 13;
    score[21] = 16;
    score[22] = 19;
    score[23] = 25;
    score[24] = 26;
    score[25] = 27;
    score[26] = 28;
    score[27] = 24;
    score[28] = 22;
    score[29] = 30;
    score[30] = 35;
    score[31] = 40;

    // -------------------------
    // 이동 테이블 구성
    // next[pos][1~5]
    // -------------------------

    // 시작점 0에서 1~5칸 이동
    next[0][1] = 1; next[0][2] = 2; next[0][3] = 3; next[0][4] = 4; next[0][5] = 5;

    next[1][1] = 2; next[1][2] = 3; next[1][3] = 4; next[1][4] = 5; next[1][5] = 6;
    next[2][1] = 3; next[2][2] = 4; next[2][3] = 5; next[2][4] = 6; next[2][5] = 7;
    next[3][1] = 4; next[3][2] = 5; next[3][3] = 6; next[3][4] = 7; next[3][5] = 8;
    next[4][1] = 5; next[4][2] = 6; next[4][3] = 7; next[4][4] = 8; next[4][5] = 9;

    // 10점 칸(5번)에 도착한 말은 파란길로 진입
    // 10 -> 13 -> 16 -> 19 -> 25 -> 30 ...
    next[5][1] = 20; next[5][2] = 21; next[5][3] = 22; next[5][4] = 23; next[5][5] = 29;

    next[6][1] = 7;  next[6][2] = 8;  next[6][3] = 9;  next[6][4] = 10; next[6][5] = 11;
    next[7][1] = 8;  next[7][2] = 9;  next[7][3] = 10; next[7][4] = 11; next[7][5] = 12;
    next[8][1] = 9;  next[8][2] = 10; next[8][3] = 11; next[8][4] = 12; next[8][5] = 13;
    next[9][1] = 10; next[9][2] = 11; next[9][3] = 12; next[9][4] = 13; next[9][5] = 14;

    // 20점 칸(10번)에 도착한 말은 파란길로 진입
    // 20 -> 22 -> 24 -> 25 -> 30 -> 35 ...
    next[10][1] = 28; next[10][2] = 27; next[10][3] = 23; next[10][4] = 29; next[10][5] = 30;

    next[11][1] = 12; next[11][2] = 13; next[11][3] = 14; next[11][4] = 15; next[11][5] = 16;
    next[12][1] = 13; next[12][2] = 14; next[12][3] = 15; next[12][4] = 16; next[12][5] = 17;
    next[13][1] = 14; next[13][2] = 15; next[13][3] = 16; next[13][4] = 17; next[13][5] = 18;
    next[14][1] = 15; next[14][2] = 16; next[14][3] = 17; next[14][4] = 18; next[14][5] = 19;

    // 30점 칸(15번)에 도착한 말은 파란길로 진입
    // 30 -> 28 -> 27 -> 26 -> 25 -> 30 ...
    next[15][1] = 26; next[15][2] = 25; next[15][3] = 24; next[15][4] = 23; next[15][5] = 29;

    // 바깥 큰 경로의 마지막 부분
    next[16][1] = 17; next[16][2] = 18; next[16][3] = 19; next[16][4] = 31; next[16][5] = 32;
    next[17][1] = 18; next[17][2] = 19; next[17][3] = 31; next[17][4] = 32; next[17][5] = 32;
    next[18][1] = 19; next[18][2] = 31; next[18][3] = 32; next[18][4] = 32; next[18][5] = 32;
    next[19][1] = 31; next[19][2] = 32; next[19][3] = 32; next[19][4] = 32; next[19][5] = 32;

    // 10점 지름길 내부
    next[20][1] = 21; next[20][2] = 22; next[20][3] = 23; next[20][4] = 29; next[20][5] = 30;
    next[21][1] = 22; next[21][2] = 23; next[21][3] = 29; next[21][4] = 30; next[21][5] = 31;
    next[22][1] = 23; next[22][2] = 29; next[22][3] = 30; next[22][4] = 31; next[22][5] = 32;

    // 중앙 25점 이후 공통 경로
    next[23][1] = 29; next[23][2] = 30; next[23][3] = 31; next[23][4] = 32; next[23][5] = 32;

    // 30점 지름길 내부
    next[24][1] = 23; next[24][2] = 29; next[24][3] = 30; next[24][4] = 31; next[24][5] = 32;
    next[25][1] = 24; next[25][2] = 23; next[25][3] = 29; next[25][4] = 30; next[25][5] = 31;
    next[26][1] = 25; next[26][2] = 24; next[26][3] = 23; next[26][4] = 29; next[26][5] = 30;

    // 20점 지름길 내부
    next[27][1] = 23; next[27][2] = 29; next[27][3] = 30; next[27][4] = 31; next[27][5] = 32;
    next[28][1] = 27; next[28][2] = 23; next[28][3] = 29; next[28][4] = 30; next[28][5] = 31;

    // 공통 끝부분
    next[29][1] = 30; next[29][2] = 31; next[29][3] = 32; next[29][4] = 32; next[29][5] = 32;
    next[30][1] = 31; next[30][2] = 32; next[30][3] = 32; next[30][4] = 32; next[30][5] = 32;
    next[31][1] = 32; next[31][2] = 32; next[31][3] = 32; next[31][4] = 32; next[31][5] = 32;
}

/*
    현재 선택된 말 이동 순서(num_of_cases[0..9])대로
    실제 게임을 진행해서 점수를 계산한다.

    규칙:
    - 말은 4개 존재
    - horsePos[i] = i번 말의 현재 위치
    - board[pos] = 해당 칸에 말이 이미 있는지 여부
    - 도착 칸(32)은 여러 말이 동시에 있어도 허용
    - 도착 칸이 아닌데 이미 말이 있으면 그 경우는 불가능하므로 0 반환
*/
int simulate()
{
    // 말 번호는 1~4를 사용하므로 0번 인덱스는 비워둠
    int horsePos[5] = { 0 };

    // 각 위치에 말이 존재하는지 체크
    bool board[MAX_BOARD] = { 0 };

    int sum = 0;

    // 10번의 턴을 순서대로 진행
    for (int i = 0; i < 10; i++)
    {
        // 이번 턴에 선택한 말 번호
        int horseNumber = num_of_cases[i];

        // 이번 턴의 이동 칸 수
        int moveCount = move[i];

        // 현재 말의 위치
        int currentPos = horsePos[horseNumber];

        // 이동 후 위치
        int nextPos = next[currentPos][moveCount];

        // 도착점이 아닌데 이미 다른 말이 있으면 불가능한 경우
        if (nextPos != DESTINATION && board[nextPos] == true)
            return 0;

        // 말의 위치 갱신
        horsePos[horseNumber] = nextPos;

        // 이전 칸 비우기
        board[currentPos] = false;

        // 새 칸 점유 표시
        board[nextPos] = true;

        // 해당 칸 점수 누적
        sum += score[nextPos];
    }

    return sum;
}

/*
    디버깅용 함수
    10턴 동안 어떤 말을 선택했는지 출력
*/
void printCases()
{
    for (int i = 0; i < 10; i++)
        printf("%d ", num_of_cases[i]);
    putchar('\n');
}

/*
    DFS(깊이 우선 탐색)

    각 턴마다 1~4번 말 중 하나를 선택할 수 있으므로
    전체 경우의 수는 4^10개이다.

    depth = 현재 몇 번째 턴까지 선택했는지
*/
void DFS(int depth)
{
    // 10턴 선택을 모두 완료한 경우
    if (depth == 10)
    {
        //printCases();  // 필요 시 켜서 선택 경우 확인 가능

        int tmp = simulate();

        // 최대 점수 갱신
        if (maxAnswer < tmp) maxAnswer = tmp;
        return;
    }

    // 현재 턴에서 1~4번 말 중 하나 선택
    for (int i = 1; i <= 4; i++)
    {
        num_of_cases[depth] = i;
        DFS(depth + 1);
    }
}

int main()
{
    // 원래는 테스트케이스 입력 가능
    // scanf("%d", &T);

    // 현재는 테스트케이스 1개 고정
    T = 1;

    for (int tc = 1; tc <= T; tc++)
    {
        // 입력 및 보드 초기화
        input();

        // 정답 초기화
        maxAnswer = 0;

        // 모든 경우 탐색
        DFS(0);

        // 최대 점수 출력
        printf("%d\n", maxAnswer);
    }

    return 0;
}