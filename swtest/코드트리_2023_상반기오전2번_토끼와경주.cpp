/*
	[코드트리] 2023 상반기 오전 2번 - 토끼와 경주
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/rabbit-and-race/description

	■ 문제 요약
	  N x M 격자(최대 10만 x 10만), 토끼 P마리(최대 2,000). 처음엔 모두 (1, 1).
		100 N M P pid1 d1 ... : 경주 준비 (첫 명령, 출력 없음)
		200 K S : 우선순위 1등 토끼를 골라 점프시키는 것을 K번(최대 100). 명령 최대 2,000번.
				  점프한 토끼 위치가 (r, c)면 나머지 토끼 전부 r + c점.
				  K번 뒤, 이번 200에서 한 번이라도 뽑힌 토끼 중 (r + c, r, c, pid)가 가장 큰 토끼에게 S점.
		300 pid L : 그 토끼의 이동거리 x L (최대 2,000번)
		400 : 가장 높은 점수 출력 (마지막 명령)

	  입출력 예
		  예제 1 -> 151

	■ 풀이 방침
	  점프 횟수는 최대 2,000 x 100 = 20만 번이다.
	  매번 P마리를 다 훑어 1등을 찾으면 20만 x 2,000 = 4억 번이라 느리다
	  -> 최소 힙(우선순위 큐)으로 1등을 O(log P)에 뽑는다.
		  뽑을 우선순위 : (점프 횟수, r + c, r, c, pid)가 작은 토끼

	■ 이동 = 반사 (d가 최대 10^9라 한 칸씩 움직이면 안 된다)
	  한 줄(행 방향)만 보면 0 ~ N-1을 왔다 갔다 하므로 주기가 2(N-1)이다.
		  0 1 2 ... N-1 N-2 ... 1 | 0 1 2 ...    (2(N-1)칸마다 같은 자리로 돌아온다)
	  0부터 시작한 좌표 p에서
		  아래로 d : x = (p + d) % 2(N-1)
		  위로   d : 위로 가는 것은 "되돌아오는 구간"의 2(N-1) - p에서 앞으로 가는 것과 같다
					 x = (2(N-1) - p + d) % 2(N-1)
	  x > N-1이면 되돌아오는 중이므로 실제 좌표는 2(N-1) - x.
	  열 방향도 M으로 똑같이 한다.
	  네 위치 중 (r + c, r, c)가 가장 큰 칸으로 간다.

	■ 점수 = "모두에게 더하기"는 전역 값 하나로 (lazy)
	  나머지 P-1마리에게 r + c를 더하는 대신
		  전체 더하기 total += r + c,  뽑힌 토끼만 score[i] -= r + c
	  로 처리한다. 실제 점수 = score[i] + total.

	■ 주의할 점
	  - 점수는 long long. 20만 번 x (r + c 최대 20만) = 4 x 10^10.
	  - 이동거리는 300으로 바뀌어도 힙 순서에 영향이 없다 (우선순위에 d가 없다).
		pid -> 토끼 번호 찾기는 P가 2,000이라 그냥 훑는다.
	  - S점은 "이번 200에서 뽑힌 토끼" 중에서만 고른다 (picked 표시).
	  - N = 1 또는 M = 1이면 주기가 0이 되지만, 제한이 N, M >= 2라 걱정 없다.
*/
#include <stdio.h>                       // scanf, printf 사용

#define MAX_P (2000 + 10)                // 토끼 최대 2,000 + 여유

int T;                                   // 테스트 케이스 수
int Q, N, M, P;                          // 명령 수, 행 수, 열 수, 토끼 수

int pid[MAX_P];                          // 고유 번호
long long dist[MAX_P];                   // 이동거리
int R[MAX_P], C[MAX_P];                  // 현재 위치 (1부터)
int jump[MAX_P];                         // 지금까지 점프 횟수
long long score[MAX_P];                  // 저장 점수 (실제 = score + total)
int picked[MAX_P];                       // 이번 200 명령에서 뽑혔는가
long long total;                         // 모두에게 더한 점수 합

int heap[MAX_P];                         // 최소 힙 (토끼 번호 저장)
int hn;                                  // 힙 크기

// a 토끼가 b 토끼보다 먼저 뽑히는가 (점프 수, r + c, r, c, pid 작은 순)
int isFirst(int a, int b)
{
	if (jump[a] != jump[b]) return jump[a] < jump[b];                 // 점프 수 적은 쪽
	if (R[a] + C[a] != R[b] + C[b]) return R[a] + C[a] < R[b] + C[b]; // 행 + 열 작은 쪽
	if (R[a] != R[b]) return R[a] < R[b];                             // 행 작은 쪽
	if (C[a] != C[b]) return C[a] < C[b];                             // 열 작은 쪽
	return pid[a] < pid[b];                                           // 고유 번호 작은 쪽
}

void heapPush(int x)                     // 힙에 넣기
{
	int i = hn++;                        // 맨 끝 자리
	heap[i] = x;
	while (i > 0)                        // 부모보다 먼저면 위로 올린다
	{
		int p = (i - 1) / 2;             // 부모 자리
		if (!isFirst(heap[i], heap[p])) break;   // 부모가 먼저면 멈춤
		int t = heap[i]; heap[i] = heap[p]; heap[p] = t;   // 맞바꾸기
		i = p;
	}
}

int heapPop()                            // 힙에서 1등 빼기
{
	int top = heap[0];                   // 1등
	heap[0] = heap[--hn];                // 맨 끝을 맨 위로
	int i = 0;
	while (1)                            // 자식보다 늦으면 아래로 내린다
	{
		int l = i * 2 + 1, r = i * 2 + 2;   // 왼쪽, 오른쪽 자식
		int best = i;                    // 셋 중 가장 먼저인 자리
		if (l < hn && isFirst(heap[l], heap[best])) best = l;
		if (r < hn && isFirst(heap[r], heap[best])) best = r;
		if (best == i) break;            // 자기가 가장 먼저면 멈춤
		int t = heap[i]; heap[i] = heap[best]; heap[best] = t;   // 맞바꾸기
		i = best;
	}
	return top;                          // 1등 반환
}

// 길이 len인 줄에서 0부터 센 좌표 p를 d만큼 움직인 결과 (forward = 1 : 증가 방향)
int reflect(int p, long long d, int len, int forward)
{
	long long period = 2LL * (len - 1);  // 왔다 갔다 한 바퀴
	long long start = forward ? p : period - p;   // 감소 방향은 되돌아오는 구간에서 출발
	long long x = (start + d) % period;  // 한 바퀴 단위는 버린다
	if (x > len - 1) x = period - x;     // 되돌아오는 중이면 실제 좌표로
	return (int)x;
}

// (r1, c1)이 (r2, c2)보다 좋은 칸인가 (r + c, r, c 큰 순)
int isBetterCell(int r1, int c1, int r2, int c2)
{
	if (r1 + c1 != r2 + c2) return r1 + c1 > r2 + c2;   // 행 + 열 큰 쪽
	if (r1 != r2) return r1 > r2;                       // 행 큰 쪽
	return c1 > c2;                                     // 열 큰 쪽
}

// 100 : 경주 시작 준비
void ready()
{
	scanf("%d %d %d", &N, &M, &P);       // 격자 크기, 토끼 수

	hn = 0;                              // 힙 비우기
	total = 0;                           // 전체 점수 초기화
	for (int i = 0; i < P; i++)          // 토끼별
	{
		scanf("%d %lld", &pid[i], &dist[i]);   // 고유 번호, 이동거리
		R[i] = C[i] = 1;                 // (1, 1)에서 출발
		jump[i] = 0;                     // 점프 0번
		score[i] = 0;                    // 점수 0
		heapPush(i);                     // 힙에 넣기
	}
}

// 200 : 경주 진행
void race(int K, int S)
{
	for (int i = 0; i < P; i++) picked[i] = 0;   // 이번 경주 뽑힘 표시 초기화

	for (int turn = 0; turn < K; turn++) // K번 반복
	{
		int i = heapPop();               // 1등 토끼

		int r0 = R[i] - 1, c0 = C[i] - 1;   // 0부터 센 좌표
		int nr[4], nc[4];                // 네 방향 결과 (1부터)
		nr[0] = reflect(r0, dist[i], N, 0) + 1; nc[0] = C[i];   // 위
		nr[1] = reflect(r0, dist[i], N, 1) + 1; nc[1] = C[i];   // 아래
		nr[2] = R[i]; nc[2] = reflect(c0, dist[i], M, 0) + 1;   // 왼쪽
		nr[3] = R[i]; nc[3] = reflect(c0, dist[i], M, 1) + 1;   // 오른쪽

		int best = 0;                    // 가장 좋은 칸
		for (int k = 1; k < 4; k++)
			if (isBetterCell(nr[k], nc[k], nr[best], nc[best])) best = k;

		R[i] = nr[best];                 // 이동
		C[i] = nc[best];
		jump[i]++;                       // 점프 횟수 증가
		picked[i] = 1;                   // 이번 경주에 뽑혔음

		total += R[i] + C[i];            // 모두에게 r + c
		score[i] -= R[i] + C[i];         // 자기 자신은 빼기

		heapPush(i);                     // 바뀐 상태로 다시 넣기
	}

	int win = -1;                        // S점을 받을 토끼
	for (int i = 0; i < P; i++)          // 뽑혔던 토끼 중
	{
		if (picked[i] == 0) continue;    // 안 뽑혔으면 제외
		if (win == -1) { win = i; continue; }

		int si = R[i] + C[i], sw = R[win] + C[win];   // 행 + 열
		if (si != sw) { if (si > sw) win = i; }       // 행 + 열 큰 쪽
		else if (R[i] != R[win]) { if (R[i] > R[win]) win = i; }   // 행 큰 쪽
		else if (C[i] != C[win]) { if (C[i] > C[win]) win = i; }   // 열 큰 쪽
		else if (pid[i] > pid[win]) win = i;                       // 고유 번호 큰 쪽
	}
	score[win] += S;                     // K >= 1이라 반드시 있다
}

// 300 : 이동거리 변경
void changeDist(int id, int L)
{
	for (int i = 0; i < P; i++)          // 고유 번호로 찾기
	{
		if (pid[i] == id)
		{
			dist[i] *= L;                // L배
			return;
		}
	}
}

// 400 : 최고의 토끼 선정
long long best()
{
	long long ret = 0;                   // 최고 점수
	for (int i = 0; i < P; i++)
		if (score[i] + total > ret) ret = score[i] + total;   // 실제 점수 = 저장 + 전체
	return ret;
}

int main()
{
	//scanf("%d", &T);
	T = 1;                               // 테스트 케이스 1개
	for (int tc = 1; tc <= T; tc++)      // 테스트 케이스 반복
	{
		scanf("%d", &Q);                 // 명령 수 입력

		for (int q = 0; q < Q; q++)      // 명령별
		{
			int cmd;                     // 명령 번호
			scanf("%d", &cmd);

			if (cmd == 100) ready();     // 경주 준비
			else if (cmd == 200)         // 경주 진행
			{
				int K, S;
				scanf("%d %d", &K, &S);
				race(K, S);
			}
			else if (cmd == 300)         // 이동거리 변경
			{
				int id, L;
				scanf("%d %d", &id, &L);
				changeDist(id, L);
			}
			else if (cmd == 400)         // 최고 점수 출력
			{
				printf("%lld\n", best());
			}
		}
	}

	return 0;                            // 프로그램 정상 종료
}
