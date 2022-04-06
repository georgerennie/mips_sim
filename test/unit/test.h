#ifndef TEST_H
#define TEST_H

#include <stddef.h>

// This should be called if an assertion fails, causing that test to fail
void test_fail();

typedef struct test_def_t {
	const char* name;
	void (*entry)();

	struct test_def_t* next_test;
} test_def_t;

void test_add_test_def(test_def_t* test);

#define DEFINE_TEST(NAME)                                         \
	static void NAME();                                           \
                                                                  \
	static const char test_name_##NAME[] = #NAME;                 \
                                                                  \
	static test_def_t test_def_##NAME = {                         \
	    .name      = test_name_##NAME,                            \
	    .entry     = NAME,                                        \
	    .next_test = NULL,                                        \
	};                                                            \
                                                                  \
	__attribute__((constructor)) static void test_init_##NAME() { \
		test_add_test_def(&test_def_##NAME);                      \
	}                                                             \
                                                                  \
	static void NAME()

#endif
