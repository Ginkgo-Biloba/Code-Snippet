// timer.h
#ifndef A_TIMER_H
#define A_TIMER_H

#include <time.h>
#include <stdio.h>

#ifdef _WIN32 // Windows
#include <Windows.h>
typedef double time_type;
static _LARGE_INTEGER timeStart, timeOver;
static double dqFreq;

// 启动计时
static inline void startTiming()
{
	_LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	dqFreq = (double)(f.QuadPart);
	QueryPerformanceCounter(&timeStart);
}

// 计时，返回时间单位：毫秒
static inline time_type getTiming()
{
	QueryPerformanceCounter(&timeOver);
	return (double)(timeOver.QuadPart - timeStart.QuadPart) / dqFreq * 1000;
}

// 显示计时
static inline void showTiming()
{
	time_type t = getTiming();
	printf("\n---------- 用时 %lf 毫秒 ---------- \n", t);
}

#else // Linux
#include <unistd.h>
typedef unsigned long long time_type;
static time_type tickStart, tickOver;
#if defined(__i386__)
inline time_type getCycleCount()
{
	time_type result;
	__asm__ __volatile__("rdtsc": "=A"(result));
	return result;
}
#elif defined(__x86_64__)
inline time_type getCycleCount()
{
	time_type hi, lo;
	__asm__ __volatile__("rdtsc": "=a"(lo), "=d"(hi));
	return ((time_type)(lo)) | ((time_type)(hi) << 32);
}
#endif

// 启动计时
static inline time_type startTiming()
{ tickStart = getCycleCount() }

// 计时，返回时间单位：周期
static inline time_type getTiming()
{
	tickOver = getCycleCount();
	return (tickOver - tickStart);
}

// 显示计时
static inline void showTiming()
{
	timet_ype t = getCycleCount();
	printf("\n---------- 用时 %llu 时钟周期 ---------- \n", t);
}
#endif

// 睡眠，单位：毫秒
static inline void waitTime(unsigned long ms)
{
#ifdef _WIN32 // Windows
	Sleep(ms);
#else // Linux
	usleep(ms * 1000);
#endif
}

#endif // A_TIMER_H
