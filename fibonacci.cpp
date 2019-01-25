#include <ctime>
#include <cstdio>
typedef unsigned long long ullong;


void timeit(ullong(*func)(ullong), char const* info)
{
	clock_t start = clock();
	ullong r, s = 0ull;
	// n 纯粹为了计时故意调大，反正溢出了结果也肯定一样
	ullong n = 0xffffull;
	unsigned it = 0xffffu;
	// += 是防止直接赋值循环被优化掉；后面的输出是为了防止优化掉 s
	for (unsigned u = 0u; u < it; ++u)
		s += func(n);
	r = func(n);
	clock_t stop = clock();
	printf("%10s(%llu) = %llu (s = %llu) | clock = %d\n", \
		info, n, r, s, (int)(stop - start));
}


/* O(N) 64 位无符号也只能算到 93 项 12200160415121876738 */
ullong fibonacci(ullong n)
{
	ullong fk = 0ull, fkm1 = 1ull; // F(0) = 0, F(-1) = 1
	for (; n; --n)
	{
		ullong fkp1 = fkm1 + fk;
		fkm1 = fk; fk = fkp1;
	}
	return fk;
}


/** O(log2(N)) 因为掺杂了矩阵乘法，所以常数系数大
 * 就代码里来说，16 次乘法和 8 次加法避免不了
 * [1 1]^n = [f(n + 1) f(n)    ]
 * [1 0]     [f(n)     f(n - 1)] */
ullong fibMatrix(ullong n)
{
	ullong A[4] = { 1ull, 1ull, 1ull, 0ull }; // n = 1 底数
	ullong B[4] = { 1ull, 0ull, 0ull, 1ull }; // n = 0
	for (; n; n >>= 1)
	{
		ullong C[4];
		if (n & 1ull) // r *= x;
		{
			C[0] = B[0] * A[0] + B[1] * A[2];
			C[1] = B[0] * A[1] + B[1] * A[3];
			C[2] = B[2] * A[0] + B[3] * A[2];
			C[3] = B[2] * A[1] + B[3] * A[3];
			B[0] = C[0]; B[1] = C[1]; B[2] = C[2]; B[3] = C[3];
		}
		// x *= x，其实最后一个循环可以不算这个
		C[0] = A[0] * A[0] + A[1] * A[2];
		C[1] = A[0] * A[1] + A[1] * A[3];
		C[2] = A[2] * A[0] + A[3] * A[2];
		C[3] = A[2] * A[1] + A[3] * A[3];
		A[0] = C[0]; A[1] = C[1]; A[2] = C[2]; A[3] = C[3];
	}
	return B[1];
}


/** O(log2(N))倍数公式
 * F(2n - 1) = F(n)^2 + F(n - 1)^2
 * F(2n) = (F(n - 1) + F(n + 1)) * F(n) = (2F(n - 1) + F(n)) * F(n) */
ullong fibShift(ullong n)
{
	// 找到最左边那个 1
	ullong mask = 1ull << (sizeof(ullong) * 8 - 1);
	while (!(mask & n) && mask) mask >>= 1;

	// f(k), f(k - 1)
	ullong fk = 0ull, fkm1 = 1ull;
	for (; mask; mask >>= 1)
	{
		ullong f2km1 = fk * fk + fkm1 * fkm1;
		ullong f2k = (fkm1 + fkm1 + fk) * fk;
		if (mask & n)
		{
			fk = f2k + f2km1; // 2k + 1
			fkm1 = f2k;
		}
		else
		{
			fk = f2k; // 2k
			fkm1 = f2km1;
		}
	}
	return fk;
}


int main()
{
	/* 0 1 1 2 3 5 8 13 21 34 55 89 144 233 377 610 987 1597 2584 4181 6765... */
	timeit(fibonacci, "fibonacci");
	timeit(fibMatrix, "fibMatrix");
	timeit(fibShift, "fibShift");

	for (ullong n = 0ull; n--;)
		printf("fib(%2llu) = %llu\n", n, fibShift(n));
}
