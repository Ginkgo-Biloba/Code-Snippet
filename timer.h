#ifndef TIMER_H
#define TIMER_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996) // strcpy
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
#define INLINE_KEYWORD inline
#else
#define INLINE_KEYWORD
#endif


#if (defined _WIN32) || (defined _WIN64) || (defined _WINDOWS) || (defined _MSC_VER)
/* Windows */
#include <Windows.h>
static INLINE_KEYWORD void timer_sleep(unsigned long ms)
{
	Sleep(ms);
}
#elif (defined __unix) || (defined __unix__) || (defined __linux) || (defined __linux__) && \
	(defined(__i386__) || defined(__x86_64__))
/* Linux 或 Unix */
#include <unistd.h>
typedef unsigned long long timer_type;
static INLINE_KEYWORD void timer_sleep(timer_type ms)
{
	usleep(ms * 1000u);
}
# if defined(__i386__)
static INLINE_KEYWORD timer_type getCycleCount()
{
	timer_type result;
	__asm__ __volatile__("rdtsc": "=A"(result));
	return result;
}
# elif defined(__x86_64__)
static INLINE_KEYWORD timer_type getCycleCount()
{
	timer_type hi, lo;
	__asm__ __volatile__("rdtsc": "=a"(lo), "=d"(hi));
	return lo | (hi << 32);
}
# else
#  error should not parse here.
# endif
#else
/* 其他，用 clock_t 模拟 */
#include <time.h>
typedef clock_t timer_type;
static INLINE_KEYWORD timer_type getCycleCount()
{
	return clock();
}
#endif // OS

/// Timer 类或结构体

#ifdef __cplusplus
# if (defined _WIN32) || (defined _WIN64) || (defined _WINDOWS) || (defined _MSC_VER)
class Timer
{
public:
	// 初始化
	void initialize(char const* d)
	{
		QueryPerformanceFrequency(&stop);
		cycle = 1000.0 / static_cast<double>(stop.QuadPart);
		if (d)
		{
			size_t const len = strlen(d) + 1u;
			desp = static_cast<char*>(malloc(sizeof(char)* len));
			strcpy(desp, d);
		}
		else
			desp = nullptr;
		QueryPerformanceCounter(&start);
	}

	// 不增加深度
	Timer(char const* d = nullptr)
	{
		initialize(d);
	}

	// 增加深度
	Timer(int, char const* d = nullptr)
	{
		initialize(d);
		depth++;
	}

	// 步计
	void stepTime(char const* d = nullptr)
	{
		QueryPerformanceCounter(&stop);
		double rst = (stop.QuadPart - start.QuadPart) * cycle;
		printf("%*s%s; %f milliseconds (depth %u)\n", depth, "", d, rst, depth);
	}

	// 圈计
	void cycleTime(char const* d = nullptr)
	{
		stepTime(d);
		start = stop;
	}

	// 重置
	void resetTime()
	{
		QueryPerformanceCounter(&start);
	}

	~Timer()
	{
		if (desp)
		{
			stepTime(desp);
			free(desp);
		}
		depth--;
	}

private:
	static unsigned int depth;

	double cycle;
	LARGE_INTEGER start, stop;
	char* desp;

	Timer(Timer const&) {}
	Timer& operator=(Timer const&) { return *this; }
};

# else
class Timer
{
public:
	// 初始化
	void initialize(char const* d)
	{
		if (d)
		{
			size_t const len = strlen(d) + 1u;
			desp = static_cast<char*>(malloc(sizeof(char)* len));
			strcpy(desp, d);
		}
		else
			desp = nullptr;
		start = getCycleCount();
	}

	// 不增加深度
	Timer(char const* d = nullptr)
	{
		initialize(d);
	}

	// 增加深度
	Timer(int, char const* d = nullptr)
	{
		initialize(d);
		depth++;
	}

	// 步计
	void stepTime(char const* d = nullptr)
	{
		stop = getCycleCount();
		unsigned long long rst = static_cast<unsigned long long>(stop - start);
		printf("%*s%s; %llu clocks (depth %u)\n", depth, "", d, rst, depth);
	}

	// 圈计
	void cycleTime(char const* d = nullptr)
	{
		stepTime(d);
		start = stop;
	}

	// 重置
	void resetTime()
	{
		start = getCycleCount();
	}

	~Timer()
	{
		if (desp)
		{
			stepTime(desp);
			free(desp);
		}
		depth--;
	}

private:
	static unsigned int depth;

	unsigned long long start, stop;
	char* desp;

	Timer(Timer const&) {}
	Timer& operator=(Timer const&) { return *this; }
};

# endif // OS

#define TIMER_DECLARE_DEPTH \
	unsigned int Timer::depth = 0u

#define TIMER_ME \
	Timer timer_dumb_class_(__FUNCTION__)
#define TIMER_ME_DEEPER \
	Timer timer_dumb_class_(0, __FUNCTION__)

# define TIMER_DECLARE \
	Timer timer_dumb_var_
# define TIMER_DECLARE_DEEPER \
	Timer timer_dumb_var_(0)
# define TIMER_STEP(desp) \
	timer_dumb_var_.stepTime(desp)
# define TIMER_CYCLE(desp) \
	timer_dumb_var_.cycleTime(desp)
# define TIMER_RESET \
	timer_dumb_var_.resetTime()
// 只在析构函数中递减，因为只有构造函数增加缩进
# define TIMER_DECREASE_DEPTH

#else // c
extern unsigned int timer_depth;
# if (defined _WIN32) || (defined _WIN64) || (defined _WINDOWS) || (defined _MSC_VER)
/* Windows */
typedef struct timer_struct
{
	LARGE_INTEGER start, stop;
	double cycle;
} Timer;
static void timer_init(Timer* timer)
{
	QueryPerformanceFrequency(&(timer->stop));
	timer->cycle = 1000.0 / (double)(timer->stop.QuadPart);
	QueryPerformanceCounter(&(timer->start));
}
static void timer_init_depth(Timer* timer)
{
	timer_init(timer);
	timer_depth++;
}
static void timer_step_time(Timer* timer, char const* d)
{
	QueryPerformanceCounter(&(timer->stop));
	double rst = (timer->stop.QuadPart - timer->start.QuadPart) * timer->cycle;
	printf("%*s%s; %f milliseconds (depth %u)\n", timer_depth, "", d, rst, timer_depth);
}
static void timer_cycle_time(Timer* timer, char const* d)
{
	timer_step_time(timer, d);
	timer->start = timer->stop;
}
static void timer_reset_time(Timer* timer)
{
	QueryPerformanceCounter(&(timer->start));
}

# else
/* LInux、Unix 或其他 */
typedef struct timer_struct
{
	timer_type start, stop;
} Timer;
static void timer_init(Timer* timer)
{
	timer->start = getCycleCount();
}
static void timer_init_depth(Timer* timer)
{
	timer_init(timer);
	timer_depth++;
}
static void timer_step_time(Timer* timer, char const* d)
{
	timer->stop = getCycleCount();
	unsigned long long rst = (unsigned long long)(timer->stop - timer->start);
	printf("%*s%s; %llu clocks (depth %u)\n", timer_depth, "", d, rst, timer_depth);
}
static void timer_cycle_time(Timer* timer, char const* d)
{
	timer_step_time(timer, d);
	timer->start = timer->stop;
}
static void timer_reset_time(Timer* timer)
{
	timer->start = getCycleCount();
}
# endif

# define TIMER_DECLARE_DEPTH \
	unsigned int timer_depth = 0u
/* 没有析构函数，需要手动递减 */
# define TIMER_DECREASE_DEPTH \
	timer_depth--
/* 存在下面两个是为了保持兼容 */
#define TIMER_ME 
#define TIMER_ME_DEEPER

# define TIMER_DECLARE \
	Timer timer_dumb_var_; \
	timer_init(&timer_dumb_var_)
# define TIMER_DECLARE_DEEPER \
	Timer timer_dumb_var_; \
	timer_init_depth(&timer_dumb_var_)
# define TIMER_STEP(desp) \
	timer_step_time(&timer_dumb_var_, desp)
# define TIMER_CYCLE(desp) \
	timer_cycle_time(&timer_dumb_var_, desp)
# define TIMER_RESET \
	timer_reset_time(&timer_dumb_var_)

#endif // __cplusplus

#ifdef _MSC_VER
#pragma warning(pop) // strcpy
#endif

#endif // TIMER_HPP
