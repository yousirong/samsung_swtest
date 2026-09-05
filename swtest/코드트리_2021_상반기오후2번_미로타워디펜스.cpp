/*
	[코드트리] 2021 상반기 오후 2번 - 미로 타워 디펜스
	https://www.codetree.ai/training-field/frequent-problems/problems/maze-tower-defense
	(백준 21611 "마법사 상어와 블리자드"를 타워 디펜스로 바꿔 낸 문제다)

	■ 문제 요약
	  N x N 격자(N은 홀수)의 정가운데에 타워가 있고, 나머지 칸에 몬스터가 한 마리씩 있다.
	  몬스터는 가운데에서 시작하는 달팽이(나선) 순서로 늘어서 있다고 본다.

	  공격 명령 M개가 (방향 d, 거리 p)로 주어지고, 한 번의 명령은 아래 순서로 진행된다.

	    1) 공격 : 타워에서 d 방향으로 거리 1 ~ p에 있는 몬스터를 모두 없앤다.
	    2) 정렬 : 빈자리가 생겼으므로 몬스터들이 나선 순서를 따라 앞으로 당겨진다.
	    3) 폭발 : 나선 순서에서 같은 번호가 4마리 이상 연속하면 모두 사라지고 점수를 얻는다.
	             점수는 (몬스터 번호 x 마리 수)이고, 사라진 뒤 다시 당겨져
	             더 이상 터질 것이 없을 때까지 2)와 3)을 반복한다.
	    4) 재편성 : 남은 몬스터를 앞에서부터 훑으며 "연속한 묶음"을 찾아
	               (묶음의 크기, 몬스터 번호) 두 마리로 바꿔 다시 나선 순서로 깔아 놓는다.

	  모든 명령을 마쳤을 때 얻은 점수의 합을 출력한다.

	■ 핵심 아이디어 : 2차원 격자를 1차원 줄로 바꿔서 푼다
	  이 문제의 규칙(연속 4개 폭발, 앞으로 당기기, 묶음 세기)은 전부 "한 줄"에 대한 규칙이다.
	  격자 위에서 나선을 따라가며 처리하려 들면 인덱스 계산이 감당이 안 된다.

	  그래서 나선 순서를 한 번만 계산해 두고, 그 순서를 1차원 배열의 인덱스로 삼는다.

	      snail[r][c]   : (r, c)가 나선 순서로 몇 번째인지 (가운데가 0)
	      monster[i]    : 나선 i번째 자리에 있는 몬스터 번호 (0이면 빈자리)

	  이러면 폭발도, 당기기도, 재편성도 전부 1차원 배열 조작이 된다.
	  격자(MAP)가 필요한 순간은 공격받는 칸을 찾을 때뿐이고,
	  그때는 snail[nr][nc]로 1차원 인덱스를 바로 얻는다.

	    격자 -> 줄 : monster[snail[r][c]] = MAP[r][c]     (main의 초기화)
	    줄 -> 격자 : makeSnail(MAP, monster)              (재편성 후)

	  즉 makeSnail 하나가 "번호표 만들기"와 "줄을 격자에 다시 깔기" 두 가지로 쓰인다.
	  main에서는 0,1,2,... 를 넣어 번호표(snail)를 만들고,
	  makeNewTower에서는 몬스터 줄을 넣어 격자(MAP)를 다시 그린다.

	■ 나선 만들기 (makeSnail)
	  16번 파일(BOJ 1913 달팽이), 청소는 즐거워와 같은 방식이다.

	    방향 순서 : ← ↓ → ↑ 로 순환
	    구간 길이 : 1, 1, 2, 2, 3, 3, ... (두 구간마다 1씩 증가)

	  구간을 2N-1개 돌면 격자가 채워진다. 마지막 구간이 한 칸 넘치지만
	  그 칸은 0행(격자 바깥)이고 출력에 쓰이지 않아 문제가 없다.

	■ 당기기를 제자리에서 해도 되는 이유 (deleteMonster)
	  살아남는 몬스터를 monster[mcnt++]에 다시 써 넣으며 앞으로 당기는데,
	  읽는 위치(k)가 언제나 쓰는 위치(mcnt)보다 앞서 있어서 덮어쓰기 사고가 없다.
	  묶음을 유지하면 mcnt와 start가 똑같이 count만큼 늘고,
	  묶음이 터지면 start만 늘기 때문에 mcnt <= start가 계속 유지되기 때문이다.

	■ 마지막 묶음 처리
	  deleteMonster의 루프는 monster[i]와 monster[i+1]을 비교하다가 값이 바뀔 때 묶음을 마감한다.
	  i가 N*N-1일 때 monster[N*N]을 읽는데, 이 칸은 어디서도 쓰지 않아 항상 0이다.
	  덕분에 맨 끝 묶음도 "값이 바뀐다"로 판정되어 빠짐없이 마감된다.

	■ [확인 필요 1] 방향 번호가 0 ~ 3 이라고 가정하고 있다
	  dr / dc가 4칸짜리이고 d를 그대로 인덱스로 쓰므로, 이 코드는 d를 0~3으로 본다.
	  주석대로 0=→, 1=↓, 2=←, 3=↑ 이다.

	  원본인 백준 21611은 d가 1~4(1=위, 2=아래, 3=왼쪽, 4=오른쪽)다.
	  만약 코드트리도 1~4로 준다면 방향이 어긋날 뿐 아니라
	  d=4에서 dr[4]를 읽어 배열 밖을 건드리게 된다.
	  문제의 방향 정의를 한 번 확인하고, 1~4라면 아래처럼 0번을 비워 두면 된다.

	      int dr[] = { 0, -1, 1, 0, 0 };   // 1=위, 2=아래, 3=왼쪽, 4=오른쪽
	      int dc[] = { 0,  0, 0,-1, 1 };

	  (같은 폴더의 나무타이쿤, 정육면체굴리기 등이 이 방식을 쓴다)

	■ [확인 필요 2] 공격으로 없앤 몬스터도 점수에 넣고 있다
	  simulate에서 score += MAP[nr][nc] 로, 공격에 맞아 사라진 몬스터의 번호를 점수에 더한다.
	  백준 21611에서는 마법으로 파괴된 구슬은 점수가 없고 폭발만 점수를 준다.
	  타워 디펜스로 바뀌면서 "처치 점수"가 생긴 것이라면 이게 맞고,
	  원본과 같은 규칙이라면 이 줄을 빼야 한다. 문제 설명에서 확인이 필요하다.

	■ [버그] 재편성 후 뒤쪽에 옛 몬스터가 남는다  ★ 실제로 답이 달라진다
	  makeNewTower의 마지막 복사가

	      for (int i = 1; i < ncnt; i++) monster[i] = newMonster[i];

	  라서 새로 만든 길이(ncnt-1)까지만 덮어쓴다.
	  재편성은 묶음 하나를 두 마리로 바꾸므로, 크기 3짜리 묶음이 많으면
	  새 줄이 옛 줄보다 짧아지고, 그 뒤에 옛 몬스터가 그대로 살아남는다.

	    예) 줄이 [1,1,1, 2,2,2] 이면 재편성 결과는 [3,1, 3,2] 여야 하는데
	        [3,1, 3,2, 2,2] 가 되어 뒤에 2 두 마리가 유령처럼 남는다.

	  이 유령 몬스터는 다음 명령의 당기기에서 진짜 몬스터로 합류해 점수를 바꾼다.
	  실제로 크기 3짜리 묶음이 많은 입력 150개를 돌려 보니 83개에서 답이 달라졌다.
	  (랜덤 입력에서는 길이 1짜리 묶음이 많아 잘 드러나지 않는다)

	  고치려면 끝까지 덮어쓰면 된다. newMonster는 0으로 초기화되어 있으므로 이 한 줄이면 충분하다.

	      for (int i = 1; i < N * N; i++) monster[i] = newMonster[i];
*/

#include <stdio.h>

#define MAX_N (50 + 5)
#define MAX_M (100 + 10)

int T;

int N, M;
int MAP[MAX_N][MAX_N];   // 격자에 놓인 몬스터 번호
int D[MAX_M];            // m번째 명령의 공격 방향
int P[MAX_M];            // m번째 명령의 공격 거리

int snail[MAX_N][MAX_N];       // snail[r][c] : (r, c)가 나선 순서로 몇 번째인지 (가운데가 0)
int monster[MAX_N * MAX_N];    // monster[i]  : 나선 i번째 자리의 몬스터 번호 (0이면 빈자리)

// 나선을 그릴 때 쓰는 방향
// ←, ↓, →, ↑ for snail
int drs[] = { 0, 1, 0, -1 };
int dcs[] = { -1, 0, 1, 0 };

// 타워가 공격하는 방향. d를 그대로 인덱스로 쓰므로 0 ~ 3을 가정한다.
// →, ↓, ←, ↑
int dr[] = { 0, 1, 0, -1 };
int dc[] = { 1, 0, -1, 0 };

void input()
{
	scanf("%d %d", &N, &M);

	for (int r = 1; r <= N; r++)
		for (int c = 1; c <= N; c++)
			scanf("%d", &MAP[r][c]);

	for (int m = 0; m < M; m++)
		scanf("%d %d", &D[m], &P[m]);
}

void printMap(int map[MAX_N][MAX_N]) // for debug
{
	for (int r = 1; r <= N; r++)
	{
		for (int c = 1; c <= N; c++)
			printf("%2d ", map[r][c]);
		putchar('\n');
	}
	putchar('\n');
}

/*
	arr에 들어 있는 값을 가운데부터 나선 순서로 map에 깔아 놓는다

	쓰임새가 두 가지다.
	  main         : arr에 0,1,2,... 를 넣어 "나선 번호표"(snail)를 만든다
	  makeNewTower : arr에 몬스터 줄을 넣어 격자(MAP)를 다시 그린다

	구간 길이가 1,1,2,2,3,3,... 로 늘어나는 것은 달팽이 문제와 같다.
	마지막 구간에서 한 칸 넘치지만 그 칸은 격자 바깥(0행)이라 결과에 영향이 없다.
*/
void makeSnail(int map[MAX_N][MAX_N], int arr[MAX_M])
{
	int sr, sc, direction;
	int index, size;

	sr = sc = (N + 1) / 2;   // 정가운데(타워 자리)에서 시작
	direction = 0;           // 첫 방향은 왼쪽
	index = 0;
	size = 0;

	map[sr][sc] = arr[index++];
	for (int i = 0; i < 2 * N - 1; i++)
	{
		// 두 구간마다 길이가 1씩 늘어난다
		if (i % 2 == 0) size++;

		for (int s = 0; s < size; s++)
		{
			int nr, nc;

			nr = sr + drs[direction];
			nc = sc + dcs[direction];

			map[nr][nc] = arr[index++];

			sr = nr;
			sc = nc;
		}

		direction++;

		if (direction == 4) direction = 0;
	}
}

/*
	연속 4마리 이상인 묶음을 터뜨리고, 남은 몬스터를 앞으로 당긴다

	  반환값 : 이번에 터뜨려 얻은 점수 (0이면 터진 것이 없다는 뜻이므로 반복을 멈추면 된다)

	한 번 훑으면서 "터뜨리기"와 "당기기"를 동시에 처리한다.
	  count < 4 인 묶음  -> monster[mcnt++]로 옮겨 살린다
	  count >= 4 인 묶음 -> 옮기지 않고 점수만 더한다 (자연히 사라진다)

	읽는 자리(k)가 쓰는 자리(mcnt)보다 항상 앞서 있어서 제자리 당기기가 안전하다.
*/
int deleteMonster()
{
	int score, count, start;

	score = 0;
	start = count = 1;   // start : 지금 보고 있는 묶음이 시작된 위치

	int mcnt = 1;        // 살아남은 몬스터를 새로 채워 넣을 위치
	for (int i = 1; i < N * N; i++)
	{
		if (monster[i] == monster[i + 1]) count++;
		else
		{
			// 값이 바뀌었다 = [start, start+count) 묶음이 여기서 끝났다
			if (count < 4)
			{
				// 살아남는다. 앞으로 당겨 다시 채워 넣는다.
				for (int k = start; k < start + count; k++)
					monster[mcnt++] = monster[k];
			}
			else
			{
				// 터진다. 옮기지 않으므로 그대로 사라지고 점수만 얻는다.
				score += (monster[i] * count);
			}

			count = 1;
			start = i + 1;
		}
	}

	// 남은 칸을 0으로 만들기
	for (int i = mcnt; i < N * N; i++) monster[i] = 0;

	return score;
}

/*
	남은 몬스터를 (묶음 크기, 몬스터 번호) 쌍으로 바꿔 다시 격자에 깔아 놓는다

	앞에서부터 연속한 묶음을 세어 두 마리씩 만들어 낸다.
	  [1,1,1, 2,2] -> [3,1, 2,2]

	격자를 N*N칸보다 많이 채울 수는 없으므로 ncnt가 N*N에 닿으면 멈춘다.

	[버그] 아래 복사가 ncnt까지만 이루어져서, 새 줄이 옛 줄보다 짧으면
	       뒤쪽에 옛 몬스터가 남는다. 파일 상단 [버그] 항목 참고.
	       i < N * N 으로 바꾸면 해결된다.
*/
void makeNewTower()
{
	int newMonster[MAX_N * MAX_N] = { 0 };
	int ncnt, count;

	ncnt = count = 1;
	for (int i = 1; i < N * N; i++)
	{
		// 빈자리를 만나면 그 뒤로는 몬스터가 없다 (항상 앞으로 당겨져 있으므로)
		if (monster[i] == 0) break;

		if (monster[i] == monster[i + 1]) count++;
		else
		{
			// 묶음이 끝났다 -> (크기, 번호) 두 마리로 바꿔 넣는다
			newMonster[ncnt++] = count;
			newMonster[ncnt++] = monster[i];

			count = 1;
		}

		if (ncnt == N * N) break;
	}

	for (int i = 1; i < ncnt; i++) monster[i] = newMonster[i];

	// 새로 만든 줄을 다시 나선 순서로 격자에 깔아 놓는다
	makeSnail(MAP, monster);
}

int simulate()
{
	int sr, sc;

	sr = sc = (N + 1) / 2;   // 타워는 정가운데에 고정

	int score = 0;
	for (int m = 0; m < M; m++)
	{
		int d, p;

		d = D[m];
		p = P[m];

		// 1) 공격 : 타워에서 d 방향으로 거리 1 ~ p의 몬스터를 없앤다
		for (int k = 1; k <= p; k++)
		{
			int nr, nc;

			nr = sr + (dr[d]) * k;
			nc = sc + (dc[d]) * k;

			// 처치한 몬스터의 번호를 점수에 더한다 (파일 상단 [확인 필요 2] 참고)
			score += MAP[nr][nc];
			MAP[nr][nc] = 0;

			// 격자에서 지웠으면 1차원 줄에서도 같은 자리를 비워 준다
			int index = snail[nr][nc];
			monster[index] = 0;
		}

		// 2) 정렬 : 빈자리를 없애고 몬스터를 앞으로 당긴다
		int mcnt = 1;
		for (int i = 1; i < N * N; i++)
			if (monster[i] != 0) monster[mcnt++] = monster[i];

		// 남은 칸을 0으로 만들기
		for (int i = mcnt; i < N * N; i++) monster[i] = 0;

		// 3) 폭발 : 더 이상 터질 것이 없을 때까지 반복
		//    deleteMonster가 터뜨리기와 당기기를 함께 하므로 이 루프 하나로 충분하다.
		while (1)
		{
			int result = deleteMonster();

			if (result == 0) break;

			score += result;
		}

		// 4) 재편성
		makeNewTower();
	}

	return score;
}

int main()
{
	// 이 문제는 테스트 케이스가 하나다
	// scanf("%d", &T);
	T = 1;
	for (int tc = 1; tc <= T; tc++)
	{
		input();

		// init
		// 0, 1, 2, ... 를 나선 순서로 깔면 "그 칸이 몇 번째인지" 알려주는 번호표가 된다.
		// 이 번호표 덕분에 격자 좌표 <-> 1차원 줄 인덱스를 바로 오갈 수 있다.
		int snailIndex[MAX_N * MAX_N] = { 0 };
		for (int i = 0; i < N * N; i++) snailIndex[i] = i;

		makeSnail(snail, snailIndex);

		//printMap(snail);

		// 격자에 놓인 몬스터를 나선 순서의 1차원 줄로 옮겨 담는다
		for (int r = 1; r <= N; r++)
			for (int c = 1; c <= N; c++)
				monster[snail[r][c]] = MAP[r][c];

		printf("%d\n", simulate());
	}

	return 0;
}
