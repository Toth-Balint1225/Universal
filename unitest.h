#ifndef UNITEST_H
#define UNITEST_H

/**
 * Tóth Bálint, University of Pannonia
 * 2023.
 *
 * UNITEST universal testing header.
 * Basic testing utilities that can be used to speed up
 * library development (not for professional use).
 * The RUN_TESTS macro creates a main function!
 * All test functions' name must start with `test_', and 
 * they must have the signature void -> int, where 
 * a return value of 0 means successful unit test.
 *
 * Use the RUN_TESTS macro in combination with TEST macro 
 * to generate the main function and execute all the test 
 * functions given with the TEST macros.
 * Important: TEST() requires the name of the test function
 * without the `test_' prefix.
 *
 * --------------------------------------------------------------
 * Use in a testing file:
 * #include <unitest.h>
 *
 * int test_load(void)
 * {
 *   return 0;
 * }
 *
 * int test_run(void)
 * {
 *   return 1;
 * }
 *
 * RUN_TESTS(
 *   TEST(load),
 *   TEST(run)
 * )
 *
 */ 

#include <stdio.h>

/**
 * Structure to store a test function and the name 
 * in a shared struct.
 */
struct test_func {
	char* name;
	int(*fun)(void);
};

/**
 * Result output of a single successful test with the name.
 */
#define SUCC(func) fprintf(stderr, "%s ... OK\n", func.name);

/**
 * Result output of a single failed test with the name and the 
 * return value of the test.
 */
#define ERR(func, res) fprintf(stderr, "%s ... FAIL (%d)\n", func.name, res);

/**
 * Macro to run a single test function.
 * Expects 0 return value for a successful test.
 */
#define RUN_TEST(func)           \
	do {                         \
		int res = func.fun();    \
		if (res == 0)  {         \
			SUCC(func)           \
		} else {                 \
			ERR(func, res)       \
		}                        \
	} while (0)

/**
 * Macro to create a single test_func struct with its initialiser
 * list.
 * Requires the function's name without the `test_' prefix.
 */
#define TEST(func) {#func,test_ ## func}

/**
 * Macro to generate the main function and run all test functions
 * given with the TEST() macro calls.
 */
#define RUN_TESTS(...)                                         \
	int main(void) {                                           \
		struct test_func funcs[] = {__VA_ARGS__};              \
		size_t len = sizeof(funcs) / sizeof(struct test_func); \
		for (size_t i=0;i<len;i++) {                           \
			RUN_TEST(funcs[i]);			                       \
		}                                                      \
		return 0;                                              \
	}

#endif // UNITEST_H
