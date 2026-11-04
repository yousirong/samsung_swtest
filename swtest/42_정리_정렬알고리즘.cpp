/*
	[정리] 정렬 알고리즘 7종 + 구조체 우선순위 정렬

	■ 이 파일의 목적
	  백준 / 코드트리 풀이가 아니라, 시험에서 직접 짜야 하는 정렬들을 지금까지 써 온
	  C 스타일(전역 배열, <algorithm> 없음)로 한 파일에 모아 비교하는 예제다.
	  main()은 같은 입력을 7가지 정렬로 각각 정렬해 출력한다. 결과가 모두 같아야 한다.

	    입력 예                출력 예
	      1                    #1
	      6                    selection : 1 2 3 4 5 9
	      5 2 9 1 4 3          bubble    : 1 2 3 4 5 9
	                           ...

	■ 한눈에 비교
	                 시간 (평균)     시간 (최악)     추가 메모리   안정   언제 쓰나
	    선택 정렬     N^2            N^2            없음          X     N이 작을 때 (~ 수천). 제일 짧다
	    버블 정렬     N^2            N^2            없음          O     원리 이해용. 실전에선 선택/삽입이 낫다
	    삽입 정렬     N^2            N^2            없음          O     거의 정렬된 배열, "하나 끼워 넣기"
	    병합 정렬     N log N        N log N        tmp[N]        O     N이 클 때 기본값. 안정해서 우선순위 정렬에 안전
	    퀵 정렬       N log N        N^2            재귀 스택     X     평균은 가장 빠르지만 최악이 있다
	    힙 정렬       N log N        N log N        없음          X     우선순위 큐(힙)를 이해하는 용도
	    계수 정렬     N + 값 범위    N + 값 범위    cnt[값 범위]  O     값이 작은 정수일 때 (예: 0 ~ 1,000)

	  "안정(stable)" = 같은 값끼리는 원래 순서가 유지된다.
	  숫자만 정렬할 때는 상관없지만, 구조체를 정렬할 때 비교에 안 쓰는 칸의 순서가 바뀔 수 있다.

	■ 시험에서 고르는 기준
	  - N <= 1,000 정도         : 선택 정렬로 충분하다 (100만 번). 지금까지 풀이 대부분이 이 경우였다
	  - N이 수만 이상            : 병합 정렬 (식량 보급에서 2만 개를 선택 정렬하면 4억 번이라 느렸다)
	  - 정렬된 배열에 하나 추가   : 삽입 정렬 한 번 (뒤에서부터 한 칸씩 밀기). 식량 보급의 이주가 이 경우
	  - 값 범위가 작은 정수      : 계수 정렬
	  - 여러 기준으로 정렬       : 우선순위 함수 isPriority(a, b)를 만들고 그 함수로 비교 (아래 8번)

	■ 공통 규칙
	  - 정렬할 배열은 전역 arr[0] ~ arr[N-1], 오름차순.
	  - 내림차순이 필요하면 비교 부호(>, <)만 뒤집는다.
	  - 두 칸 바꾸기는 swap()으로 통일했다.
*/
#include <stdio.h>                  // scanf, printf, putchar 사용

#define MAX_N (100000 + 10)         // 원소 최대 10만 개 + 여유
#define MAX_VALUE (1000)        // 계수 정렬이 다룰 수 있는 최댓값 (0 ~ MAX_VALUE)

int T;                          // 테스트케이스 수
int N;                          // 원소 개수
int origin[MAX_N];              // 입력 원본 (정렬마다 여기서 복사해 온다)
int arr[MAX_N];                 // 정렬할 배열
int tmp[MAX_N];                 // 병합 정렬용 임시 배열
int cnt[MAX_VALUE + 1];         // 계수 정렬용

// arr[i]와 arr[j]를 맞바꾼다
void swap(int i, int j)
{
	int t = arr[i];                 // arr[i]를 잠시 보관
	arr[i] = arr[j];                // arr[i] 자리에 arr[j]
	arr[j] = t;                     // arr[j] 자리에 보관한 값
}

/* ─────────────────────────────────────────────
	1. 선택 정렬 (Selection Sort)  O(N^2)
	   i번 자리에 "남은 것 중 가장 작은 값"을 가져다 놓는다.
	   지금까지 풀이에서 쓴 모양이 이것이다 (i, k 이중 for문 + 바로 swap).

	     for i : 0 ~ N-2
	       for k : i+1 ~ N-1
	         arr[i] > arr[k] 이면 바꾼다   -> i번 자리에 최솟값이 남는다
   ───────────────────────────────────────────── */
void selectionSort()
{
	for (int i = 0; i < N - 1; i++)              // 채울 자리 i
		for (int k = i + 1; k < N; k++)          // i 뒤의 모든 원소와 비교
			if (arr[i] > arr[k]) swap(i, k);     // 더 작은 값이 있으면 i 자리로 가져온다
}

/* ─────────────────────────────────────────────
	2. 버블 정렬 (Bubble Sort)  O(N^2)
	   이웃한 두 칸을 비교해 큰 값을 뒤로 보낸다. 한 바퀴 돌면 가장 큰 값이 맨 뒤로 간다.
	   한 바퀴 동안 한 번도 안 바꿨으면 이미 정렬된 것이라 멈춘다 (changed).
   ───────────────────────────────────────────── */
void bubbleSort()
{
	for (int end = N - 1; end > 0; end--)        // 이번 바퀴에서 확정될 맨 뒤 위치
	{
		int changed = 0;                         // 이번 바퀴에 바꾼 적이 있는지

		for (int i = 0; i < end; i++)            // 앞에서부터 이웃한 두 칸 비교
		{
			if (arr[i] > arr[i + 1])             // 앞이 더 크면
			{
				swap(i, i + 1);                  // 큰 값을 뒤로 보낸다
				changed = 1;                     // 바꿨다고 표시
			}
		}

		if (changed == 0) break;   // 이미 정렬됨
	}
}

/* ─────────────────────────────────────────────
	3. 삽입 정렬 (Insertion Sort)  O(N^2), 거의 정렬돼 있으면 O(N)
	   앞쪽 arr[0 ~ i-1]은 이미 정렬돼 있다. arr[i]를 꺼내 들고,
	   자기보다 큰 값들을 한 칸씩 뒤로 밀다가 빈 자리에 넣는다.

	   "정렬된 배열에 하나 끼워 넣기"가 바로 이 안쪽 while 한 번이다.
	   (식량 보급의 이주 처리, 고대 문명 유적 탐사의 정렬 유지 등)
   ───────────────────────────────────────────── */
void insertionSort()
{
	for (int i = 1; i < N; i++)                  // 1번부터 하나씩 끼워 넣는다
	{
		int value = arr[i];   // 끼워 넣을 값
		int k = i - 1;        // 정렬된 구간의 맨 뒤부터 비교

		while (k >= 0 && arr[k] > value)   // 큰 값을 뒤로 민다
		{
			arr[k + 1] = arr[k];           // 한 칸 뒤로 밀기
			k--;                           // 한 칸 앞으로 이동
		}
		arr[k + 1] = value;                // 빈 자리에 값을 넣는다
	}
}

/* ─────────────────────────────────────────────
	4. 병합 정렬 (Merge Sort)  O(N log N), 안정
	   반으로 나눠 각각 정렬(재귀)한 뒤, 정렬된 두 줄을 앞에서부터 비교하며 tmp에 합친다.

	     mergeSort(s, e)
	       1) 가운데 m으로 나눠 mergeSort(s, m), mergeSort(m+1, e)
	       2) 두 줄의 맨 앞끼리 비교해 작은 쪽을 tmp에 옮기기를 반복
	       3) 남은 쪽을 마저 옮기고 tmp를 arr로 복사

	   2)에서 같으면 왼쪽 것을 먼저(<=) 옮기므로 같은 값의 원래 순서가 유지된다 = 안정.
	   N이 크면 이것을 기본으로 쓰면 된다. 최악도 N log N이다.
   ───────────────────────────────────────────── */
void mergeSort(int s, int e)
{
	if (s >= e) return;   // 한 칸 이하는 이미 정렬

	int m = (s + e) / 2;                         // 구간의 가운데
	mergeSort(s, m);                             // 왼쪽 절반 정렬
	mergeSort(m + 1, e);                         // 오른쪽 절반 정렬

	int i = s, j = m + 1, k = s;                 // i: 왼쪽 위치, j: 오른쪽 위치, k: tmp에 쓸 위치
	while (i <= m && j <= e)                     // 양쪽에 원소가 남아 있는 동안
	{
		if (arr[i] <= arr[j]) tmp[k++] = arr[i++];   // 같으면 왼쪽 먼저 -> 안정
		else tmp[k++] = arr[j++];                    // 오른쪽이 더 작으면 오른쪽을 옮긴다
	}
	while (i <= m) tmp[k++] = arr[i++];          // 왼쪽에 남은 원소를 옮긴다
	while (j <= e) tmp[k++] = arr[j++];          // 오른쪽에 남은 원소를 옮긴다

	for (int t = s; t <= e; t++) arr[t] = tmp[t];   // 합친 결과를 arr로 복사
}

/* ─────────────────────────────────────────────
	5. 퀵 정렬 (Quick Sort)  평균 O(N log N), 최악 O(N^2)
	   기준값(pivot)을 하나 골라, 기준보다 작은 값은 왼쪽, 큰 값은 오른쪽으로 모은 뒤
	   양쪽을 각각 재귀로 정렬한다.

	   기준을 맨 앞으로 고르면 이미 정렬된 입력에서 한쪽만 계속 남아 N^2이 된다.
	   그래서 가운데 값을 기준으로 쓴다. (그래도 최악은 있다. 걱정되면 병합 정렬)

	   나누기(Lomuto 방식)
	     1) 가운데 값을 맨 끝(e)으로 보낸다
	     2) store = s. i가 s ~ e-1을 돌며 기준보다 작은 값을 store 자리로 모은다
	     3) 기준을 store 자리로 돌려놓는다 -> 기준은 최종 위치에 도착
   ───────────────────────────────────────────── */
void quickSort(int s, int e)
{
	if (s >= e) return;                          // 한 칸 이하는 이미 정렬

	swap((s + e) / 2, e);   // 가운데 값을 기준으로 (맨 끝에 둔다)
	int pivot = arr[e];     // 기준값

	int store = s;                               // 기준보다 작은 값을 모을 다음 자리
	for (int i = s; i < e; i++)                  // 기준을 뺀 구간을 훑는다
	{
		if (arr[i] < pivot)                      // 기준보다 작으면
		{
			swap(i, store);                      // 왼쪽 모음 자리로 보낸다
			store++;                             // 다음 모음 자리
		}
	}
	swap(store, e);   // 기준을 제자리에

	quickSort(s, store - 1);                     // 기준 왼쪽 정렬
	quickSort(store + 1, e);                     // 기준 오른쪽 정렬
}

/* ─────────────────────────────────────────────
	6. 힙 정렬 (Heap Sort)  O(N log N)
	   배열을 "최대 힙"(부모 >= 자식)으로 만든 뒤, 맨 앞(최댓값)을 맨 뒤로 보내고
	   남은 범위를 다시 힙으로 고치기를 반복한다.

	   배열로 보는 완전 이진 트리 (0번부터)
	     i의 왼쪽 자식 = 2i + 1,  오른쪽 자식 = 2i + 2,  부모 = (i - 1) / 2

	   heapify(i, size) : i에서 시작해 자식 중 큰 쪽과 바꾸며 아래로 내려간다 (size 미만 범위만)
	   우선순위 큐를 직접 만들 때도 이 heapify(아래로 내리기)를 그대로 쓴다.
   ───────────────────────────────────────────── */
void heapify(int i, int size)
{
	while (1)                                    // 더 내려갈 수 없을 때까지
	{
		int largest = i;                         // 부모와 자식 중 가장 큰 위치
		int l = 2 * i + 1;                       // 왼쪽 자식
		int r = 2 * i + 2;                       // 오른쪽 자식

		if (l < size && arr[l] > arr[largest]) largest = l;   // 왼쪽 자식이 더 크면
		if (r < size && arr[r] > arr[largest]) largest = r;   // 오른쪽 자식이 더 크면

		if (largest == i) break;   // 부모가 가장 크다 -> 끝

		swap(i, largest);          // 큰 자식과 자리를 바꾼다
		i = largest;               // 바뀐 자식 자리에서 계속 내려간다
	}
}

void heapSort()
{
	// 1) 최대 힙 만들기 : 자식이 있는 마지막 노드부터 거꾸로 heapify
	for (int i = N / 2 - 1; i >= 0; i--) heapify(i, N);

	// 2) 최댓값(맨 앞)을 뒤로 보내고, 남은 범위를 다시 힙으로
	for (int end = N - 1; end > 0; end--)
	{
		swap(0, end);                            // 최댓값을 맨 뒤로
		heapify(0, end);                         // 남은 범위를 다시 최대 힙으로
	}
}

/* ─────────────────────────────────────────────
	7. 계수 정렬 (Counting Sort)  O(N + 값 범위)
	   값이 0 ~ MAX_VALUE 인 정수일 때만 쓴다.
	   값마다 몇 번 나왔는지 세고(cnt[값]++), 작은 값부터 센 만큼 다시 써 넣는다.
	   비교를 아예 하지 않는다. 값 범위가 작으면 가장 빠르다.
	   (음수가 있으면 OFFSET을 더해 0 이상으로 옮겨서 센다)
   ───────────────────────────────────────────── */
void countingSort()
{
	for (int v = 0; v <= MAX_VALUE; v++) cnt[v] = 0;   // 개수 초기화

	for (int i = 0; i < N; i++) cnt[arr[i]]++;         // 값마다 등장 횟수 세기

	int k = 0;                                         // arr에 다시 쓸 위치
	for (int v = 0; v <= MAX_VALUE; v++)               // 작은 값부터
		while (cnt[v]-- > 0) arr[k++] = v;             // 센 개수만큼 써 넣는다
}

/* ─────────────────────────────────────────────
	8. 구조체 우선순위 정렬 (삼성 기출에서 가장 많이 쓰는 형태)
	   "점수 높은 순, 같으면 나이 적은 순, 같으면 번호 작은 순" 같은 여러 기준은
	   비교를 함수 하나(isPriority)로 모아 두고, 정렬은 그 함수로만 비교한다.
	   기준이 바뀌면 isPriority만 고치면 된다.
	   (루돌프의 반란, 포탑 부수기, 메이즈 러너 등의 우선순위 고르기와 같은 함수다)

	   여기서는 안정한 병합 정렬에 isPriority를 끼웠다. N이 작으면 선택 정렬에 끼워도 된다.
   ───────────────────────────────────────────── */
struct STUDENT
{
	int id;                     // 번호
	int score;                  // 점수
	int age;                    // 나이
};

STUDENT student[MAX_N];         // 정렬할 학생 배열
STUDENT stmp[MAX_N];            // 병합 정렬용 임시 배열
int scnt;                       // 학생 수

// a가 b보다 앞에 와야 하면 1
int isPriority(STUDENT a, STUDENT b)
{
	if (a.score != b.score) return a.score > b.score;   // 1) 점수 높은 순
	if (a.age != b.age) return a.age < b.age;           // 2) 나이 적은 순
	return a.id < b.id;                                 // 3) 번호 작은 순
}

void mergeSortStudent(int s, int e)
{
	if (s >= e) return;                          // 한 명 이하는 이미 정렬

	int m = (s + e) / 2;                         // 구간의 가운데
	mergeSortStudent(s, m);                      // 왼쪽 절반 정렬
	mergeSortStudent(m + 1, e);                  // 오른쪽 절반 정렬

	int i = s, j = m + 1, k = s;                 // i: 왼쪽 위치, j: 오른쪽 위치, k: stmp에 쓸 위치
	while (i <= m && j <= e)                     // 양쪽에 원소가 남아 있는 동안
	{
		// 오른쪽이 "더 우선"일 때만 오른쪽을 먼저 -> 같으면 왼쪽 먼저 (안정)
		if (isPriority(student[j], student[i])) stmp[k++] = student[j++];
		else stmp[k++] = student[i++];           // 아니면 왼쪽을 옮긴다
	}
	while (i <= m) stmp[k++] = student[i++];     // 왼쪽에 남은 원소를 옮긴다
	while (j <= e) stmp[k++] = student[j++];     // 오른쪽에 남은 원소를 옮긴다

	for (int t = s; t <= e; t++) student[t] = stmp[t];   // 합친 결과를 student로 복사
}

void demoStudent()
{
	// id, score, age
	STUDENT data[] = {
		{ 1, 80, 20 }, { 2, 95, 22 }, { 3, 80, 19 },
		{ 4, 95, 22 }, { 5, 70, 21 }, { 6, 80, 19 },
	};
	scnt = 6;                                    // 예제 학생 수

	for (int i = 0; i < scnt; i++) student[i] = data[i];   // 예제 데이터 복사

	mergeSortStudent(0, scnt - 1);               // 우선순위대로 정렬

	// 기대 : 2(95,22) 4(95,22) 3(80,19) 6(80,19) 1(80,20) 5(70,21)
	printf("student   :");                       // 결과 제목
	for (int i = 0; i < scnt; i++)               // 정렬된 순서대로
		printf(" %d(%d,%d)", student[i].id, student[i].score, student[i].age);   // 번호(점수,나이) 출력
	putchar('\n');                               // 줄바꿈
}

/* ───────────────────────────────────────────── */

void input()
{
	scanf("%d", &N);                             // 원소 개수 입력
	for (int i = 0; i < N; i++)                  // N개의 값을
		scanf("%d", &origin[i]);                 // 원본 배열에 입력
}

void copyInput()
{
	for (int i = 0; i < N; i++) arr[i] = origin[i];   // 원본을 정렬용 배열로 복사
}

void printArr(const char* name)
{
	printf("%-10s:", name);                      // 정렬 이름 출력 (왼쪽 정렬 10칸)
	for (int i = 0; i < N; i++) printf(" %d", arr[i]);   // 정렬 결과 출력
	putchar('\n');                               // 줄바꿈
}

int main()
{
	scanf("%d", &T);                             // 테스트케이스 수 입력
	for (int tc = 1; tc <= T; tc++)              // 테스트케이스 반복
	{
		input();                                 // 원소 입력

		printf("#%d\n", tc);                     // 케이스 번호 출력

		copyInput(); selectionSort();      printArr("selection");   // 선택 정렬
		copyInput(); bubbleSort();         printArr("bubble");      // 버블 정렬
		copyInput(); insertionSort();      printArr("insertion");   // 삽입 정렬
		copyInput(); mergeSort(0, N - 1);  printArr("merge");       // 병합 정렬
		copyInput(); quickSort(0, N - 1);  printArr("quick");       // 퀵 정렬
		copyInput(); heapSort();           printArr("heap");        // 힙 정렬
		copyInput(); countingSort();       printArr("counting");   // 값이 0 ~ MAX_VALUE 일 때만
	}

	demoStudent();                               // 구조체 우선순위 정렬 예제

	return 0;                                    // 프로그램 정상 종료
}
