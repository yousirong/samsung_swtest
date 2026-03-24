#include <stdio.h>

#define MAX (10000+500)

#define OFFSET (MAX/2)

int N;
int deque[MAX * 2];
int front, back;

int strcmp(const char* a, const char* b)
{
	while (*a && *a == *b) ++a, ++b;
	return *a - *b;
}

int main()
{
	front = back = OFFSET;

	scanf("%d", &N);

	for (int i = 0; i < N; i++)
	{
		char command[100];

		scanf("%s", &command);

		if (strcmp(command, "push_front") == 0)
		{
			int value;

			scanf("%d", &value);

			deque[--front] = value;
		}
		else if (strcmp(command, "push_back") == 0)
		{
			int value;

			scanf("%d", &value);

			deque[back++] = value;
		}
		else if (strcmp(command, "pop_front") == 0)
			printf("%d\n", (back == front) ? -1 : deque[front++]);
		else if (strcmp(command, "pop_back") == 0)
			printf("%d\n", (back == front) ? -1 : deque[--back]);
		else if (strcmp(command, "size") == 0)
			printf("%d\n", (back - front));
		else if (strcmp(command, "empty")==0)
			printf("%d\n", (back == front) ? 1 :0 );
		else if (strcmp(command, "front") == 0)
			printf("%d\n", (back == front) ? -1 : deque[front]);
		else if (strcmp(command, "back") == 0)
			printf("%d\n", (back == front) ? -1 : deque[back-1]);
	}
}