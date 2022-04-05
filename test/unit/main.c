#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

static const char* col_rst   = "\033[0m";
static const char* col_red   = "\033[31m";
static const char* col_green = "\033[32m";
static const char* col_mag   = "\033[35m";

static int         num_tests  = 0;
static test_def_t* first_test = NULL;

void test_add_test_def(test_def_t* test) {
	test_def_t** prev_test = &first_test;
	while (*prev_test != NULL) { prev_test = &(*prev_test)->next_test; }
	*prev_test = test;

	num_tests++;
}

// Forgive me for my crimes...
static jmp_buf test_buf;

void test_fail() { longjmp(test_buf, 1); }

static int run_tests() {
	int test_num = 1, failed_tests = 0;

	test_def_t** test = &first_test;
	while (*test != NULL) {
		printf("%s%d/%d%s Running %s:\n", col_mag, test_num, num_tests, col_rst, (*test)->name);

		if (!setjmp(test_buf)) {
			(*test)->entry();
			printf("%sPASS%s\n", col_green, col_rst);
		} else {
			printf("%sFAIL%s\n", col_red, col_rst);
			failed_tests++;
		}

		test = &(*test)->next_test;
		test_num++;
	}

	if (failed_tests) {
		fprintf(
		    stderr, "%sFAIL:%s %d/%d tests failed\n", col_red, col_rst, failed_tests, num_tests);
	} else {
		printf("%sPASS:%s All %d tests passed\n", col_green, col_rst, num_tests);
	}
	return failed_tests ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int run_one(char* name) {
	test_def_t** test = &first_test;
	while (*test != NULL) {
		if (strcmp((*test)->name, name) == 0) {
			printf("Running %s:\n", (*test)->name);

			if (!setjmp(test_buf)) {
				(*test)->entry();
				printf("%sPASS%s\n", col_green, col_rst);
				return EXIT_SUCCESS;
			} else {
				printf("%sFAIL%s\n", col_red, col_rst);
				return EXIT_FAILURE;
			}
		}

		test = &(*test)->next_test;
	}

	fprintf(stderr, "%sFAIL:%s Could not find test with name %s\n", col_red, col_rst, name);
	return EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
	if (argc == 1) { return run_tests(); }
	if (argc == 2) { return run_one(argv[1]); }
	fprintf(stderr, "%sOnly 0 or 1 commandline arguments are allowed%s\n", col_red, col_rst);
	return EXIT_FAILURE;
}
