#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <time.h>

#ifndef LOGLVL
#define LOGLVL 3
#endif

#ifndef ASSERT_CHECKS
#define ASSERT_CHECKS true
#endif

#ifndef TIMER
#define TIMER true
#endif

#ifndef TRAP
#define TRAP __builtin_trap()
#endif

#ifndef ALLOW_BLINK
#define ALLOW_BLINK true
#endif

#define C_R "\033[0m"
#define C_DIM "\033[38;5;234m"
#define B_C_DIM "\033[48;5;234m"
#define C_CYAN "\033[36m"
#define C_BLUE "\033[38;5;4m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_RED "\033[31m"
#define C_WHITE "\033[38;5;236m"
#define B_C_WHITE "\033[48;5;236m"

#if (ALLOW_BLINK)
#define BLINK "\e[5m"
#define BLINK_OFF "\e[25m"
#else
#define BLINK ""
#define BLINK_OFF ""
#endif

#define OKM C_WHITE "" B_C_WHITE C_GREEN "--" C_R C_WHITE B_C_DIM " "
#define DOTM C_WHITE "" B_C_WHITE C_BLUE "--" C_R C_WHITE B_C_DIM " "
#define ERRM                                                                   \
	C_WHITE "" B_C_WHITE C_RED "=" BLINK "×" BLINK_OFF                    \
		  "=" C_R C_WHITE B_C_DIM " "
#define END C_R C_DIM "◗\n" C_R

#define POSITION                                                               \
	"[Fn:" C_CYAN "%s" C_R B_C_DIM " Fl:" C_CYAN "%s" C_R B_C_DIM            \
	" Ln:" C_CYAN "%d" C_R B_C_DIM "]"
#define POS __func__, __FILE__, __LINE__

#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
#define UNREACHABLE()                                                          \
	do {                                                                     \
	} while (0)
#endif

#define TODO(fmt, ...)                                                         \
	do {                                                                     \
		fprintf(                                                           \
		    stderr,                                                        \
		    ERRM C_RED "TODO " C_R B_C_DIM POSITION                        \
				   " : " C_YELLOW fmt END,                             \
		    POS, ##__VA_ARGS__                                             \
		);                                                                 \
		TRAP;                                                  \
		exit(EX_SOFTWARE);                                                 \
		UNREACHABLE();                                                     \
	} while (0)

#if (LOGLVL > 0)
#define ERR(fmt, ...)                                                          \
	fprintf(                                                                 \
	    stderr, ERRM C_RED "ERR " C_R B_C_DIM POSITION " : " C_RED fmt END,  \
	    POS, ##__VA_ARGS__                                                   \
	)
#else
#define ERR(fmt, ...)                                                          \
	do {                                                                     \
	} while (0)
#endif

#if (LOGLVL > 1)
#define LOG(fmt, ...)                                                          \
	fprintf(                                                                 \
	    stdout, OKM C_BLUE "LOG " C_R B_C_DIM POSITION " : " C_BLUE fmt END, \
	    POS, ##__VA_ARGS__                                                   \
	)
#else
#define LOG(fmt, ...)                                                          \
	do {                                                                     \
	} while (0)
#endif

#if (LOGLVL > 2)
#define DBG(fmt, ...)                                                          \
	fprintf(                                                                 \
	    stdout,                                                              \
	    OKM C_YELLOW "DBG " C_R B_C_DIM POSITION " : " C_YELLOW fmt END,     \
	    POS, ##__VA_ARGS__                                                   \
	)
#else
#define DBG(fmt, ...)                                                          \
	do {                                                                     \
	} while (0)
#endif

#if (ASSERT_CHECKS)
#define ASSERT(cond, fmt, ...)                                                 \
	do {                                                                     \
		if (!(cond)) {                                                     \
			fprintf(                                                     \
			    stderr,                                                  \
			    ERRM C_RED "ASSERT " C_YELLOW "(" #cond                  \
					   ") " C_R B_C_DIM POSITION                     \
					   " : " C_RED fmt END,                          \
			    POS, ##__VA_ARGS__                                       \
			);                                                           \
			TRAP;                                            \
			exit(1);                                                     \
		} else if (LOGLVL > 2) {                                           \
			fprintf(                                                     \
			    stdout,                                                  \
			    OKM C_GREEN "ASSERT " C_YELLOW "(" #cond                 \
					    ") " C_R B_C_DIM POSITION                    \
					    " : " C_YELLOW fmt END,                      \
			    POS, ##__VA_ARGS__                                       \
			);                                                           \
		}                                                                  \
	} while (0)
#else
#define ASSERT(cond, fmt, ...)                                                 \
	do {                                                                     \
	} while (0)
#endif

#if (TIMER)
#define TIMER_CPU_START(name) clock_t _timer_##name = clock()

#define TIMER_CPU_END(name, ...)                                               \
	do {                                                                     \
		double _elapsed_##name =                                           \
		    (double)(clock() - _timer_##name) / CLOCKS_PER_SEC;            \
                                                                               \
		fprintf(                                                           \
		    stdout,                                                        \
		    DOTM C_BLUE                                                    \
		    "TIMER_CPU " C_R B_C_DIM "(" C_BLUE #name C_R B_C_DIM          \
		    ") " C_R B_C_DIM POSITION " : (" C_BLUE "%.6fs" C_R B_C_DIM    \
		    ")" __VA_OPT__(" : " C_BLUE) __VA_ARGS__ END,                  \
		    POS, _elapsed_##name, ##__VA_ARGS__                            \
		);                                                                 \
	} while (0)

#define TIMER_REAL_START(name)                                                 \
	struct timespec _timer_real_##name##_start;                              \
	clock_gettime(CLOCK_MONOTONIC, &_timer_real_##name##_start)
#define TIMER_REAL_END(name, ...)                                              \
	do {                                                                     \
		struct timespec _timer_real_##name##_end;                          \
		clock_gettime(CLOCK_MONOTONIC, &_timer_real_##name##_end);         \
		double _elapsed_real_##name =                                      \
		    (_timer_real_##name##_end.tv_sec -                             \
		     _timer_real_##name##_start.tv_sec) +                          \
		    (_timer_real_##name##_end.tv_nsec -                            \
		     _timer_real_##name##_start.tv_nsec) /                         \
			  1e9;                                                       \
		fprintf(                                                           \
		    stdout,                                                        \
		    DOTM C_BLUE                                                    \
		    "TIMER_REAL " C_R B_C_DIM "(" C_BLUE #name C_R B_C_DIM         \
		    ") " C_R B_C_DIM POSITION " : (" C_BLUE "%.6fs" C_R B_C_DIM    \
		    ")" __VA_OPT__(" : " C_BLUE) __VA_ARGS__ END,                  \
		    POS, _elapsed_real_##name, ##__VA_ARGS__                       \
		);                                                                 \
	} while (0)

#else
#define TIMER_CPU_START(name)                                                  \
	do {                                                                     \
	} while (0)
#define TIMER_CPU_END(name, ...)                                               \
	do {                                                                     \
	} while (0)
#define TIMER_REAL_START(name)                                                 \
	do {                                                                     \
	} while (0)
#define TIMER_REAL_END(name, ...)                                              \
	do {                                                                     \
	} while (0)
#endif
