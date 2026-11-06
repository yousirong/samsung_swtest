#include <stdio.h>                    // scanf, printf 사용

#define MAX (100000 + 10)             // 벨트 / 선물 최대 10만 개 + 여유

int T;                                // 테스트케이스 수
int Q, N, M;                          // 명령 수, 벨트 수, 선물 수

int prv[MAX], nxt[MAX];               // i번 선물의 앞, 뒤 선물 번호 (없으면 0)
int head[MAX], tail[MAX];             // b번 벨트의 맨 앞, 맨 뒤 선물 번호 (비면 0)
int cnt[MAX];                         // b번 벨트의 선물 수

// b번 벨트 맨 뒤에 선물 x를 붙인다 (공장 설립용)
void pushBack(int b, int x)
{
    prv[x] = tail[b];                    // x의 앞은 원래 맨 뒤 선물
    nxt[x] = 0;                          // x 뒤에는 아무것도 없다
    if (tail[b] != 0) nxt[tail[b]] = x;  // 원래 맨 뒤 선물 뒤에 x를 잇는다
    else head[b] = x;                    // 벨트가 비어 있었으면 x가 맨 앞도 된다
    tail[b] = x;                         // x가 새 맨 뒤
    cnt[b]++;                            // 선물 수 증가
}

// b번 벨트 맨 앞 선물을 빼서 돌려준다 (비었으면 0)
int popFront(int b)
{
    int x = head[b];                     // 맨 앞 선물
    if (x == 0) return 0;                // 빈 벨트면 뺄 것이 없다

    head[b] = nxt[x];                    // 다음 선물이 새 맨 앞
    if (head[b] != 0) prv[head[b]] = 0;  // 새 맨 앞의 앞은 없다
    else tail[b] = 0;                    // 하나뿐이었으면 벨트가 빈다
    nxt[x] = 0;                          // 뺀 선물의 연결을 끊는다
    cnt[b]--;                            // 선물 수 감소
    return x;                            // 뺀 선물 반환
}

// [s ... e] 덩어리(k개)를 dst 벨트 맨 앞에 붙인다
void attachFront(int dst, int s, int e, int k)
{
    nxt[e] = head[dst];                  // 덩어리 끝 뒤에 원래 맨 앞을 잇는다
    if (head[dst] != 0) prv[head[dst]] = e;   // 원래 맨 앞의 앞은 덩어리 끝
    else tail[dst] = e;                  // dst가 비어 있었으면 덩어리 끝이 맨 뒤
    prv[s] = 0;                          // 덩어리 시작의 앞은 없다
    head[dst] = s;                       // 덩어리 시작이 새 맨 앞
    cnt[dst] += k;                       // 선물 수 증가
}

// 100 : 공장 설립
void build()
{
    scanf("%d %d", &N, &M);              // 벨트 수, 선물 수 입력

    for (int b = 1; b <= N; b++)         // 모든 벨트를
        head[b] = tail[b] = cnt[b] = 0;  // 빈 상태로 초기화

    for (int i = 1; i <= M; i++)         // 선물 번호 오름차순으로
    {
        int b;                           // i번 선물이 올라갈 벨트
        scanf("%d", &b);
        pushBack(b, i);                  // 그 벨트 맨 뒤에 붙인다
    }
}

// 200 : 물건 모두 옮기기
int moveAll(int src, int dst)
{
    if (cnt[src] == 0) return cnt[dst];  // 옮길 선물이 없다

    attachFront(dst, head[src], tail[src], cnt[src]);   // src 전체를 dst 앞에 붙인다

    head[src] = tail[src] = cnt[src] = 0;   // src는 빈 벨트가 된다
    return cnt[dst];                     // dst 선물 수
}

// 300 : 앞 물건만 교체하기
int swapFront(int src, int dst)
{
    int a = popFront(src);               // src 맨 앞 (먼저 둘 다 뺀다)
    int b = popFront(dst);               // dst 맨 앞

    if (a != 0) attachFront(dst, a, a, 1);   // src에서 뺀 선물을 dst 앞에
    if (b != 0) attachFront(src, b, b, 1);   // dst에서 뺀 선물을 src 앞에

    return cnt[dst];                     // dst 선물 수
}

// 400 : 물건 나누기
int divide(int src, int dst)
{
    int k = cnt[src] / 2;                // 옮길 개수 floor(n / 2)
    if (k == 0) return cnt[dst];         // 0개 또는 1개면 옮기지 않는다

    int s = head[src];                   // 덩어리 시작 = src 맨 앞
    int e = s;                           // 덩어리 끝 (앞에서 k번째)
    for (int i = 1; i < k; i++) e = nxt[e];   // k - 1번 뒤로 이동

    head[src] = nxt[e];                  // 남은 첫 선물이 src의 새 맨 앞
    prv[head[src]] = 0;                  // k < n이라 남은 선물이 반드시 있다
    cnt[src] -= k;                       // src 선물 수 감소

    attachFront(dst, s, e, k);           // 떼어 낸 덩어리를 dst 앞에 붙인다
    return cnt[dst];                     // dst 선물 수
}

// 500 : 선물 정보 얻기
int giftInfo(int p)
{
    int a = (prv[p] != 0) ? prv[p] : -1; // 앞 선물 (없으면 -1)
    int b = (nxt[p] != 0) ? nxt[p] : -1; // 뒤 선물 (없으면 -1)
    return a + 2 * b;                    // a + 2b
}

// 600 : 벨트 정보 얻기
int beltInfo(int bnum)
{
    int a = (head[bnum] != 0) ? head[bnum] : -1;   // 맨 앞 선물 (없으면 -1)
    int b = (tail[bnum] != 0) ? tail[bnum] : -1;   // 맨 뒤 선물 (없으면 -1)
    int c = cnt[bnum];                   // 선물 수
    return a + 2 * b + 3 * c;            // a + 2b + 3c
}

int main()
{
    //scanf("%d", &T);
    T = 1;                               // 테스트케이스 1개
    for (int tc = 1; tc <= T; tc++)      // 테스트케이스 반복
    {
        scanf("%d", &Q);                 // 명령 수 입력

        for (int q = 0; q < Q; q++)      // 명령마다
        {
            int cmd;                     // 명령 번호
            scanf("%d", &cmd);

            if (cmd == 100) build();     // 공장 설립 (출력 없음)
            else if (cmd == 200 || cmd == 300 || cmd == 400)   // 두 벨트를 쓰는 명령
            {
                int src, dst;            // 보내는 벨트, 받는 벨트
                scanf("%d %d", &src, &dst);

                if (cmd == 200) printf("%d\n", moveAll(src, dst));        // 모두 옮기기
                else if (cmd == 300) printf("%d\n", swapFront(src, dst)); // 앞 물건 교체
                else printf("%d\n", divide(src, dst));                    // 물건 나누기
            }
            else if (cmd == 500)         // 선물 정보
            {
                int p;
                scanf("%d", &p);
                printf("%d\n", giftInfo(p));
            }
            else if (cmd == 600)         // 벨트 정보
            {
                int b;
                scanf("%d", &b);
                printf("%d\n", beltInfo(b));
            }
        }
    }

    return 0;                            // 프로그램 정상 종료
}
