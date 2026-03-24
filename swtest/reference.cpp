#include <stdio.h>

#define MAX (12+5)

int board[MAX][MAX][10 + 3];
int index[MAX][MAX];

int main(){
	int r, c, hcnt;

	hcnt = 1;
	r = c = 3;

	for (int k = 0; k < 3; k++) {
		int& position = index[r][c];

		board[r][c][position++] = hcnt++;

	}

	printf("(%d, %d) : ", r, c);
	for (int i = 0; i < index[r][c]; i++)
		printf("%d ", board[r][c][i]);
	putchar('\n');

	return 0;

}