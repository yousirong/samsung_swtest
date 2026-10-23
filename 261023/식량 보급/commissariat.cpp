#include <stdio.h>

#define MAX (20000+1000+10)

int T;
int Q;

long long val[MAX];
long long pre[MAX];
int hcnt;
long long G;

long long tmp[MAX];

void mergeSort(int s, int e)
{
    if (s >= e) return;

    int m = (s + e) / 2;
    mergeSort(s, m);
    mergeSort(m + 1, e);

    int i = s, j = m + 1, k = s;
    while (i <= m && j <= e)
    {
        if (val[i] <= val[j]) tmp[k++] = val[i++];
        else tmp[k++] = val[j++];

    }
    while (i <= m) tmp[k++] = val[i++];
    while (j <= e) tmp[k++] = val[j++];

    for (int t = s; t <= e; t++) val[t] = tmp[t];

}

void makePrefix()
{
    pre[0] = 0;
    for (int i = 0; i < hcnt; i++) pre[i + 1] = pre[i] + val[i];
}

//100 겜 시작
void start()
{
    int n;
    scanf("%d", &n);

    hcnt = 0;
    G = 0;

    for (int i = 0; i < n; i++)
        scanf("%lld", &val[hcnt++]);

    mergeSort(0, hcnt - 1);
    makePrefix();
}

//200 이주
void migrate(long long x)
{
    long long v = x - G;

    int i = hcnt - 1;
    while (i >= 0 && val[i] > v)
    {
        val[i + 1] = val[i];
        i--;
    }
    val[i + 1] = v;
    hcnt++;

    makePrefix();
}

//300
void strengthen(long long p)
{
    G += p;
}

//400 
long long winter(long long p)
{
    int lo = 1, hi = hcnt;
    while (lo < hi)
    {
        int mid = (lo + hi + 1) / 2;
        long long cost = (long long)mid * val[mid - 1] - pre[mid];

        if (cost <= p) lo = mid;
        else hi = mid - 1;

    }
    int k = lo;
    long long total = pre[k] + (long long)k * G + p;
    return total / k;
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

            if (cmd == 100) start();
            else
            {
                long long p;
                scanf("%lld", &p);

                if (cmd == 200) migrate(p);
                else if (cmd == 300) strengthen(p);
                else if (cmd == 400) printf("%lld\n", winter(p));
            }
        }
    }
    return 0;
}