#ifndef UNILOG_H
#define UNILOG_H

/**
 * Tóth Bálint, University of Pannonia
 * 2023.
 *
 * UNILOG universal logger macro.
 * Defines wrapper macro LOG around the printf function.
 * If global symbol LOG is defined, then expands to the Printf, 
 * otherwise will expand to NOOP.
 *
 * --------------------------------------------------------------
 * Use from code:
 * #define LOG_ENABLE 
 * #include <unilog.h>
 *
 * int main(void) 
 * {
 *   LOG("Hello World\n");
 * }
 * --------------------------------------------------------------
 * Use with gcc: 
 * $> gcc -DLOG_ENABLE (...)
 */

#include <stdio.h>

/**
 * Macro that expands either to printf, or {}.
 */
#if defined(LOG_ENABLE)
#define LOG(...) printf(__VA_ARGS__);
#else
#define LOG(...) {};
#endif

#endif //UNILOG_H
