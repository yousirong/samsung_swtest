/*
	[코드트리] 2022 하반기 오전 2번 - 산타의 선물 공장
	https://www.codetree.ai/ko/frequent-problems/samsung-sw/problems/santa-gift-factory/description

	■ 문제 요약
	  벨트 m개(최대 10), 상자 n개(최대 100,000). 상자마다 고유 번호 ID(<= 10^9)와 무게 W.
	  명령 q개(최대 100,000)
		100 : 공장 설립. 상자를 순서대로 n/m개씩 1번 ~ m번 벨트에 올린다
		200 w_max : 1 ~ m번 벨트 맨 앞 상자가 w_max 이하면 하차, 아니면 맨 뒤로. 하차 무게 합 출력
		300 r_id  : 그 상자가 있으면 빼고 r_id, 없으면 -1
		400 f_id  : 그 상자가 있는 벨트 번호 출력 (없으면 -1).
					있으면 그 상자부터 끝까지를 순서 그대로 맨 앞으로 가져온다
		500 b_num : 이미 고장이면 -1. 아니면 고장 내고, 오른쪽으로 돌며 첫 정상 벨트 뒤에
					b_num의 상자를 순서대로 모두 옮긴 뒤 b_num 출력

	  입출력 예
		  예제 1 -> 35 22 -1 -1 3 3

	■ 풀이 방침 : 벨트마다 이중 연결 리스트 (배열로 구현)
	  상자를 배열에 줄 세워 두면 "중간 빼기", "뒤쪽 통째로 앞으로", "벨트 통째로 옮기기"가
	  전부 한 칸씩 당기기라 O(n)이다. 명령 10만 번 x 상자 10만 개면 시간 초과.
	  연결 리스트로 두면 모두 O(1)이다 (고장만 상자의 벨트 번호를 고치느라 O(n), 그래도 최대 10번).

		  prv[i], nxt[i] : i번 상자의 앞, 뒤 상자 (없으면 -1)
		  head[b], tail[b] : b번 벨트의 맨 앞, 맨 뒤 상자 (비면 -1)
		  beltOf[i] : i번 상자가 놓인 벨트
		  removed[i] : 하차 / 제거된 상자

	■ ID(최대 10^9)로 상자를 바로 찾기 : 정렬 + 이분 탐색
	  ID는 너무 커서 배열 칸 번호로 못 쓴다. (ID, 상자 번호) 쌍을 ID 순으로 병합 정렬해 두고
	  300 / 400 명령마다 이분 탐색으로 상자 번호를 찾는다. 한 번에 O(log n).
	  (C++ STL 없이 해시 대신 쓰는 방법이다. 정렬은 42_정리_정렬알고리즘의 병합 정렬과 같다)

	■ 연산별 연결 바꾸기
	  - 맨 앞 빼기 / 중간 빼기 (unlink) : 앞뒤 상자를 서로 이어 주고, head / tail이면 갱신
	  - 맨 뒤에 붙이기 (pushBack)
	  - 400 : 상자 i부터 끝까지를 앞으로  (i가 이미 맨 앞이면 아무 일 없음)
			  [head ... p] [i ... tail]  ->  [i ... tail] [head ... p]
			  tail 뒤에 원래 head를 잇고, p가 새 tail, i가 새 head
	  - 500 : b의 [head ... tail]을 통째로 대상 벨트 tail 뒤에 잇는다 (연결 두 개만 바꾸면 된다)

	■ 주의할 점
	  - 200의 무게 합은 최대 10 x 10^9 라 long long.
	  - 200에서 맨 앞을 뒤로 보낼 때 상자가 하나뿐이면 그대로 둔다 (빼고 다시 붙여도 같다).
	  - 고장 난 벨트는 비어 있으므로 200에서 자연히 건너뛴다.
	  - 400에서 찾은 상자가 하차 / 제거된 상자면 -1이다 (removed로 확인).
*/
#include <stdio.h>                    // scanf, printf 사용

#define MAX_BOX (100000 + 10)         // 상자 배열 크기 (최대 10만 개 + 여유 10칸)
#define MAX_BELT (10 + 2)             // 벨트 배열 크기 (1번부터 쓰므로 여유를 둠)

int T;                                // 테스트케이스 수
int Q, N, M;                          // 명령 수, 상자 수, 벨트 수

int boxId[MAX_BOX];                   // i번 상자의 ID
int boxW[MAX_BOX];                    // i번 상자의 무게
int prv[MAX_BOX], nxt[MAX_BOX];       // i번 상자의 앞, 뒤 상자 번호 (없으면 -1)
int beltOf[MAX_BOX];                  // i번 상자가 놓인 벨트 번호
int removed[MAX_BOX];                 // 1이면 하차 또는 제거된 상자

int head[MAX_BELT], tail[MAX_BELT];   // b번 벨트의 맨 앞, 맨 뒤 상자 번호 (비면 -1)
int broken[MAX_BELT];                 // 1이면 고장 난 벨트

// ID로 찾기용 : ID 오름차순으로 정렬한 (ID, 상자 번호)
int sortedId[MAX_BOX], sortedIdx[MAX_BOX];   // 정렬된 ID와 그 ID의 상자 번호
int tmpId[MAX_BOX], tmpIdx[MAX_BOX];         // 병합 정렬에 쓰는 임시 배열

// sortedId의 s ~ e 구간을 ID 오름차순으로 정렬한다
void mergeSort(int s, int e)
{
	if (s >= e) return;                  // 원소가 1개 이하면 이미 정렬된 상태

	int m = (s + e) / 2;                 // 구간의 가운데 위치
	mergeSort(s, m);                     // 왼쪽 절반 정렬
	mergeSort(m + 1, e);                 // 오른쪽 절반 정렬

	int i = s, j = m + 1, k = s;         // i: 왼쪽 위치, j: 오른쪽 위치, k: 임시 배열에 쓸 위치
	while (i <= m && j <= e)             // 양쪽 모두 원소가 남아 있는 동안
	{
		// 더 작은 쪽을 임시 배열로 옮긴다 (ID와 상자 번호를 함께 옮김)
		if (sortedId[i] <= sortedId[j]) { tmpId[k] = sortedId[i]; tmpIdx[k++] = sortedIdx[i++]; }
		else { tmpId[k] = sortedId[j]; tmpIdx[k++] = sortedIdx[j++]; }
	}
	while (i <= m) { tmpId[k] = sortedId[i]; tmpIdx[k++] = sortedIdx[i++]; }   // 왼쪽에 남은 원소를 옮김
	while (j <= e) { tmpId[k] = sortedId[j]; tmpIdx[k++] = sortedIdx[j++]; }   // 오른쪽에 남은 원소를 옮김

	for (int t = s; t <= e; t++) { sortedId[t] = tmpId[t]; sortedIdx[t] = tmpIdx[t]; }   // 정렬 결과를 원래 배열로 복사
}

// ID에 해당하는 상자 번호 (없으면 -1)
int findBox(int id)
{
	int lo = 0, hi = N - 1;              // 이분 탐색 범위
	while (lo <= hi)                     // 탐색 범위가 남아 있는 동안
	{
		int mid = (lo + hi) / 2;         // 범위의 가운데 위치

		if (sortedId[mid] == id) return sortedIdx[mid];   // 찾으면 상자 번호 반환
		if (sortedId[mid] < id) lo = mid + 1;             // 찾는 ID가 더 크면 오른쪽 탐색
		else hi = mid - 1;                                // 찾는 ID가 더 작으면 왼쪽 탐색
	}
	return -1;                           // 끝까지 못 찾으면 없는 ID
}

// b번 벨트 맨 뒤에 상자 i를 붙인다
void pushBack(int b, int i)
{
	beltOf[i] = b;                       // i는 이제 b번 벨트 소속
	prv[i] = tail[b];                    // i의 앞은 기존 맨 뒤 상자
	nxt[i] = -1;                         // i가 맨 뒤이므로 뒤 상자는 없음

	if (tail[b] == -1) head[b] = i;      // 빈 벨트였다면 i가 맨 앞도 된다
	else nxt[tail[b]] = i;               // 아니면 기존 맨 뒤 상자의 뒤를 i로 연결

	tail[b] = i;                         // 벨트의 맨 뒤를 i로 갱신
}

// 상자 i를 벨트에서 뺀다 (앞뒤를 서로 잇는다)
void unlink(int i)
{
	int b = beltOf[i];                   // i가 놓인 벨트

	if (prv[i] == -1) head[b] = nxt[i];  // i가 맨 앞이면 뒤 상자가 새 맨 앞
	else nxt[prv[i]] = nxt[i];           // 아니면 앞 상자의 뒤를 i의 뒤 상자로 연결

	if (nxt[i] == -1) tail[b] = prv[i];  // i가 맨 뒤면 앞 상자가 새 맨 뒤
	else prv[nxt[i]] = prv[i];           // 아니면 뒤 상자의 앞을 i의 앞 상자로 연결

	prv[i] = nxt[i] = -1;                // i의 연결을 지운다
}

// 100 : 공장 설립
void build()
{
	scanf("%d %d", &N, &M);              // 상자 수와 벨트 수 입력

	for (int i = 0; i < N; i++) scanf("%d", &boxId[i]);   // 상자 ID 입력
	for (int i = 0; i < N; i++) scanf("%d", &boxW[i]);    // 상자 무게 입력

	for (int b = 1; b <= M; b++)         // 모든 벨트 초기화
	{
		head[b] = tail[b] = -1;          // 빈 벨트로 설정
		broken[b] = 0;                   // 정상 벨트로 설정
	}

	int per = N / M;                     // 벨트당 상자 수
	for (int i = 0; i < N; i++)          // 상자를 입력 순서대로 벨트에 올린다
	{
		removed[i] = 0;                  // 아직 벨트 위에 있는 상자
		pushBack(i / per + 1, i);        // i번 상자를 해당 벨트 맨 뒤에 붙임
	}

	for (int i = 0; i < N; i++)          // 정렬할 (ID, 상자 번호) 준비
	{
		sortedId[i] = boxId[i];          // i번 상자의 ID
		sortedIdx[i] = i;                // i번 상자의 번호
	}
	mergeSort(0, N - 1);                 // 전체를 ID 오름차순으로 정렬
}

// 200 : 물건 하차
long long unload(int wMax)
{
	long long sum = 0;                   // 하차한 상자의 무게 합

	for (int b = 1; b <= M; b++)         // 1번부터 M번 벨트까지 차례로 확인
	{
		int i = head[b];                 // b번 벨트의 맨 앞 상자
		if (i == -1) continue;           // 빈 벨트 (고장 난 벨트 포함)

		if (boxW[i] <= wMax)             // 하차
		{
			sum += boxW[i];              // 무게 합에 더한다
			unlink(i);                   // 벨트에서 뺀다
			removed[i] = 1;              // 사라진 상자로 표시
		}
		else if (nxt[i] != -1)           // 맨 뒤로 (하나뿐이면 그대로)
		{
			unlink(i);                   // 맨 앞에서 뺀다
			pushBack(b, i);              // 같은 벨트 맨 뒤에 붙인다
		}
	}
	return sum;                          // 하차 무게 합 반환
}

// 300 : 물건 제거
int removeBox(int rId)
{
	int i = findBox(rId);                // ID로 상자 번호를 찾는다
	if (i == -1 || removed[i]) return -1;   // 없는 ID이거나 이미 사라진 상자면 -1

	unlink(i);                           // 벨트에서 뺀다
	removed[i] = 1;                      // 사라진 상자로 표시
	return rId;                          // 제거한 상자의 ID 반환
}

// 400 : 물건 확인 (+ i부터 끝까지를 앞으로)
int checkBox(int fId)
{
	int i = findBox(fId);                // ID로 상자 번호를 찾는다
	if (i == -1 || removed[i]) return -1;   // 없는 ID이거나 이미 사라진 상자면 -1

	int b = beltOf[i];                   // 상자가 놓인 벨트 번호

	if (head[b] != i)   // 이미 맨 앞이면 할 일 없음
	{
		int p = prv[i];           // i 바로 앞 -> 새 맨 뒤
		int oldHead = head[b];    // 기존 맨 앞 상자
		int oldTail = tail[b];    // 기존 맨 뒤 상자

		// [i ... oldTail] 뒤에 [oldHead ... p]를 잇는다
		nxt[oldTail] = oldHead;   // 기존 맨 뒤의 뒤를 기존 맨 앞으로 연결
		prv[oldHead] = oldTail;   // 기존 맨 앞의 앞을 기존 맨 뒤로 연결

		nxt[p] = -1;              // p는 새 맨 뒤이므로 뒤 상자가 없음
		prv[i] = -1;              // i는 새 맨 앞이므로 앞 상자가 없음

		head[b] = i;              // 벨트의 맨 앞을 i로 갱신
		tail[b] = p;              // 벨트의 맨 뒤를 p로 갱신
	}
	return b;                     // 상자가 있는 벨트 번호 반환
}

// 500 : 벨트 고장
int breakBelt(int bNum)
{
	if (broken[bNum]) return -1;         // 이미 고장 난 벨트면 -1

	broken[bNum] = 1;                    // 고장 표시

	// 오른쪽으로 돌며 첫 정상 벨트 (m 다음은 1)
	int target = bNum;                   // 상자를 옮길 벨트
	for (int k = 1; k < M; k++)          // 오른쪽으로 1칸씩 확인
	{
		int b = (bNum - 1 + k) % M + 1;  // bNum에서 k칸 오른쪽 벨트 번호
		if (broken[b] == 0) { target = b; break; }   // 첫 정상 벨트를 찾으면 멈춘다
	}

	if (head[bNum] != -1)                // 옮길 상자가 있을 때만
	{
		// 옮기는 상자들의 벨트 번호를 고친다
		for (int i = head[bNum]; i != -1; i = nxt[i]) beltOf[i] = target;

		// [head ... tail]을 통째로 target 맨 뒤에 잇는다
		if (tail[target] == -1) head[target] = head[bNum];   // target이 비었으면 맨 앞도 설정
		else
		{
			nxt[tail[target]] = head[bNum];   // target의 맨 뒤 다음을 옮겨 올 맨 앞으로 연결
			prv[head[bNum]] = tail[target];   // 옮겨 올 맨 앞의 앞을 target의 맨 뒤로 연결
		}
		tail[target] = tail[bNum];       // target의 새 맨 뒤 설정

		head[bNum] = tail[bNum] = -1;    // 고장 난 벨트를 비운다
	}

	return bNum;                         // 고장 낸 벨트 번호 반환
}

int main()
{
	//scanf("%d", &T);                   // 여러 테스트케이스를 돌릴 때 쓰는 입력
	T = 1;                               // 테스트케이스를 1개로 고정
	for (int tc = 1; tc <= T; tc++)      // 테스트케이스 반복
	{
		scanf("%d", &Q);                 // 명령 수 입력

		for (int q = 0; q < Q; q++)      // 명령을 Q번 처리
		{
			int cmd;                     // 명령 번호
			scanf("%d", &cmd);           // 명령 번호 입력

			if (cmd == 100) build();     // 100은 입력 형식이 달라 build에서 직접 읽는다
			else
			{
				int x;                   // 200 ~ 500 명령의 인자
				scanf("%d", &x);         // 인자 입력

				if (cmd == 200) printf("%lld\n", unload(x));         // 하차 무게 합 출력
				else if (cmd == 300) printf("%d\n", removeBox(x));   // 제거 결과 출력
				else if (cmd == 400) printf("%d\n", checkBox(x));    // 벨트 번호 출력
				else if (cmd == 500) printf("%d\n", breakBelt(x));   // 고장 처리 결과 출력
			}
		}
	}

	return 0;                            // 프로그램 정상 종료
}