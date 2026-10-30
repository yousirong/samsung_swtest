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
#include <stdio.h>

#define MAX_BOX (100000 + 10)
#define MAX_BELT (10 + 2)

int T;
int Q, N, M;

int boxId[MAX_BOX];          // i번 상자의 ID
int boxW[MAX_BOX];           // i번 상자의 무게
int prv[MAX_BOX], nxt[MAX_BOX];
int beltOf[MAX_BOX];
int removed[MAX_BOX];

int head[MAX_BELT], tail[MAX_BELT];
int broken[MAX_BELT];

// ID로 찾기용 : ID 오름차순으로 정렬한 (ID, 상자 번호)
int sortedId[MAX_BOX], sortedIdx[MAX_BOX];
int tmpId[MAX_BOX], tmpIdx[MAX_BOX];

void mergeSort(int s, int e)
{
	if (s >= e) return;

	int m = (s + e) / 2;
	mergeSort(s, m);
	mergeSort(m + 1, e);

	int i = s, j = m + 1, k = s;
	while (i <= m && j <= e)
	{
		if (sortedId[i] <= sortedId[j]) { tmpId[k] = sortedId[i]; tmpIdx[k++] = sortedIdx[i++]; }
		else { tmpId[k] = sortedId[j]; tmpIdx[k++] = sortedIdx[j++]; }
	}
	while (i <= m) { tmpId[k] = sortedId[i]; tmpIdx[k++] = sortedIdx[i++]; }
	while (j <= e) { tmpId[k] = sortedId[j]; tmpIdx[k++] = sortedIdx[j++]; }

	for (int t = s; t <= e; t++) { sortedId[t] = tmpId[t]; sortedIdx[t] = tmpIdx[t]; }
}

// ID에 해당하는 상자 번호 (없으면 -1)
int findBox(int id)
{
	int lo = 0, hi = N - 1;
	while (lo <= hi)
	{
		int mid = (lo + hi) / 2;

		if (sortedId[mid] == id) return sortedIdx[mid];
		if (sortedId[mid] < id) lo = mid + 1;
		else hi = mid - 1;
	}
	return -1;
}

// b번 벨트 맨 뒤에 상자 i를 붙인다
void pushBack(int b, int i)
{
	beltOf[i] = b;
	prv[i] = tail[b];
	nxt[i] = -1;

	if (tail[b] == -1) head[b] = i;   // 빈 벨트
	else nxt[tail[b]] = i;

	tail[b] = i;
}

// 상자 i를 벨트에서 뺀다 (앞뒤를 서로 잇는다)
void unlink(int i)
{
	int b = beltOf[i];

	if (prv[i] == -1) head[b] = nxt[i];
	else nxt[prv[i]] = nxt[i];

	if (nxt[i] == -1) tail[b] = prv[i];
	else prv[nxt[i]] = prv[i];

	prv[i] = nxt[i] = -1;
}

// 100 : 공장 설립
void build()
{
	scanf("%d %d", &N, &M);

	for (int i = 0; i < N; i++) scanf("%d", &boxId[i]);
	for (int i = 0; i < N; i++) scanf("%d", &boxW[i]);

	for (int b = 1; b <= M; b++)
	{
		head[b] = tail[b] = -1;
		broken[b] = 0;
	}

	int per = N / M;   // 벨트당 상자 수
	for (int i = 0; i < N; i++)
	{
		removed[i] = 0;
		pushBack(i / per + 1, i);
	}

	for (int i = 0; i < N; i++)
	{
		sortedId[i] = boxId[i];
		sortedIdx[i] = i;
	}
	mergeSort(0, N - 1);
}

// 200 : 물건 하차
long long unload(int wMax)
{
	long long sum = 0;

	for (int b = 1; b <= M; b++)
	{
		int i = head[b];
		if (i == -1) continue;   // 빈 벨트 (고장 난 벨트 포함)

		if (boxW[i] <= wMax)     // 하차
		{
			sum += boxW[i];
			unlink(i);
			removed[i] = 1;
		}
		else if (nxt[i] != -1)   // 맨 뒤로 (하나뿐이면 그대로)
		{
			unlink(i);
			pushBack(b, i);
		}
	}
	return sum;
}

// 300 : 물건 제거
int removeBox(int rId)
{
	int i = findBox(rId);
	if (i == -1 || removed[i]) return -1;

	unlink(i);
	removed[i] = 1;
	return rId;
}

// 400 : 물건 확인 (+ i부터 끝까지를 앞으로)
int checkBox(int fId)
{
	int i = findBox(fId);
	if (i == -1 || removed[i]) return -1;

	int b = beltOf[i];

	if (head[b] != i)   // 이미 맨 앞이면 할 일 없음
	{
		int p = prv[i];           // i 바로 앞 -> 새 맨 뒤
		int oldHead = head[b];
		int oldTail = tail[b];

		// [i ... oldTail] 뒤에 [oldHead ... p]를 잇는다
		nxt[oldTail] = oldHead;
		prv[oldHead] = oldTail;

		nxt[p] = -1;
		prv[i] = -1;

		head[b] = i;
		tail[b] = p;
	}
	return b;
}

// 500 : 벨트 고장
int breakBelt(int bNum)
{
	if (broken[bNum]) return -1;

	broken[bNum] = 1;

	// 오른쪽으로 돌며 첫 정상 벨트 (m 다음은 1)
	int target = bNum;
	for (int k = 1; k < M; k++)
	{
		int b = (bNum - 1 + k) % M + 1;
		if (broken[b] == 0) { target = b; break; }
	}

	if (head[bNum] != -1)
	{
		// 옮기는 상자들의 벨트 번호를 고친다
		for (int i = head[bNum]; i != -1; i = nxt[i]) beltOf[i] = target;

		// [head ... tail]을 통째로 target 맨 뒤에 잇는다
		if (tail[target] == -1) head[target] = head[bNum];
		else
		{
			nxt[tail[target]] = head[bNum];
			prv[head[bNum]] = tail[target];
		}
		tail[target] = tail[bNum];

		head[bNum] = tail[bNum] = -1;
	}

	return bNum;
}

int main()
{
	//scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		scanf("%d", &Q);

		for (int q = 0; q < Q; q++)
		{
			int cmd;
			scanf("%d", &cmd);

			if (cmd == 100) build();
			else
			{
				int x;
				scanf("%d", &x);

				if (cmd == 200) printf("%lld\n", unload(x));
				else if (cmd == 300) printf("%d\n", removeBox(x));
				else if (cmd == 400) printf("%d\n", checkBox(x));
				else if (cmd == 500) printf("%d\n", breakBelt(x));
			}
		}
	}

	return 0;
}
