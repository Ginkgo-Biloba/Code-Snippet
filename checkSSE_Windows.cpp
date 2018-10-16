////////////////////////////////////////////////////////////
// checksimd.c : 检查 MMX 和 SSE 指令集
// Author: zyl910
// Blog: http://www.cnblogs.com/zyl910
// URL: http://www.cnblogs.com/zyl910/archive/2012/03/01/checksimd.html
// Version: V1.0
// Updata: 2012-03-21
////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <conio.h>

// SSE系列指令集的支持级别. simd_sse_level 函数的返回值。
#define SIMD_SSE_NONE	0	// 不支持
#define SIMD_SSE_1	1	// SSE
#define SIMD_SSE_2	2	// SSE2
#define SIMD_SSE_3	3	// SSE3
#define SIMD_SSE_3S	4	// SSSE3
#define SIMD_SSE_41	5	// SSE4.1
#define SIMD_SSE_42	6	// SSE4.2

const char*	simd_sse_names[] =
{
	"None",
	"SSE",
	"SSE2",
	"SSE3",
	"SSSE3",
	"SSE4.1",
	"SSE4.2",
};


// 是否支持MMX指令集
BOOL	simd_mmx()
{
	const DWORD	BIT_DX_MMX = 0x00800000;	// bit 23
	DWORD	v_edx;

	// check processor support
	__try
	{
		_asm
		{
			mov eax, 1
			cpuid
				mov v_edx, edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}
	if (v_edx & BIT_DX_MMX)
	{
		// check OS support
		__try
		{
			_asm
			{
				pxor mm0, mm0	// executing any MMX instruction
				emms
			}
			return TRUE;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{}
	}
	return FALSE;
}


// 检测SSE系列指令集的支持级别
int simd_sse_level()
{
	const DWORD	BIT_D_SSE = 0x02000000;	// bit 25
	const DWORD	BIT_D_SSE2 = 0x04000000;	// bit 26
	const DWORD	BIT_C_SSE3 = 0x00000001;	// bit 0
	const DWORD	BIT_C_SSSE3 = 0x00000100;	// bit 9
	const DWORD	BIT_C_SSE41 = 0x00080000;	// bit 19
	const DWORD	BIT_C_SSE42 = 0x00100000;	// bit 20
	BYTE	rt = SIMD_SSE_NONE;	// result
	DWORD	v_edx;
	DWORD	v_ecx;

	// check processor support
	__try
	{
		_asm
		{
			mov eax, 1
			cpuid
				mov v_edx, edx
				mov v_ecx, ecx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return SIMD_SSE_NONE;
	}
	if (v_edx & BIT_D_SSE)
	{
		rt = SIMD_SSE_1;
		if (v_edx & BIT_D_SSE2)
		{
			rt = SIMD_SSE_2;
			if (v_ecx & BIT_C_SSE3)
			{
				rt = SIMD_SSE_3;
				if (v_ecx & BIT_C_SSSE3)
				{
					rt = SIMD_SSE_3S;
					if (v_ecx & BIT_C_SSE41)
					{
						rt = SIMD_SSE_41;
						if (v_ecx & BIT_C_SSE42)
						{
							rt = SIMD_SSE_42;
						}
					}
				}
			}
		}
	}

	// check OS support
	__try
	{
		_asm
		{
			xorps xmm0, xmm0	// executing any SSE instruction
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return SIMD_SSE_NONE;
	}
	return rt;
}

int main(int argc, char* argv[])
{
	int i;
	BOOL	bmmx = simd_mmx();
	int	nsse = simd_sse_level();
	printf("MMX: %d\n", bmmx);
	printf("SSE: %d\n", nsse);
	for (i = 1; i < sizeof(simd_sse_names); ++i)
	{
		if (nsse >= i)	printf("\t%s\n", simd_sse_names[i]);
	}

	// wait
	getch();
	printf("\n");
	return 0;
}
