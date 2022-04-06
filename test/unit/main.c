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

// Returns true if all characters of search match characters of name
static bool name_matches(const char* name, const char* search) {
	for (size_t i = 0;; i++) {
		if (search[i] == '\0') { return true; }
		if (name[i] == '\0') { return false; }
		if (search[i] != name[i]) { return false; }
	}
}

static int run_tests(const char* search) {
	int failed_tests = 0, tests_run = 0;

	test_def_t** test = &first_test;
	while (*test != NULL) {
		if (search == NULL || name_matches((*test)->name, search)) {
			tests_run++;

			printf("%s%d/", col_mag, tests_run);
			search == NULL ? printf("%d", num_tests) : putchar('?');
			printf("%s Running %s: ", col_rst, (*test)->name);

			if (!setjmp(test_buf)) {
				(*test)->entry();
				printf("%sPASS%s\n", col_green, col_rst);
			} else {
				printf("%sFAIL%s\n", col_red, col_rst);
				failed_tests++;
			}
		}
		test = &(*test)->next_test;
	}

	if (tests_run == 0) {
		fprintf(
		    stderr, "%sFAIL%s: Could not find any tests matching the pattern \"%s\"\n", col_red,
		    col_rst, search);
		return EXIT_FAILURE;
	}

	if (failed_tests) {
		fprintf(
		    stderr, "%sFAIL%s: %d/%d tests failed\n", col_red, col_rst, failed_tests, tests_run);
		return EXIT_FAILURE;
	}

	printf("%sPASS%s: All %d tests passed\n", col_green, col_rst, tests_run);
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	if (argc == 1) { return run_tests(NULL); }
	if (argc == 2) { return run_tests(argv[1]); }
	fprintf(stderr, "%sFAIL%s: Only 0 or 1 commandline arguments are allowed\n", col_red, col_rst);
	return EXIT_FAILURE;
}
